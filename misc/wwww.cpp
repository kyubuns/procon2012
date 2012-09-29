#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <cstdlib>
#include <list>
#include <cmath>
#include <queue>



#include <vector>
#include <cmath>
#include <queue>
#include <cassert>
#define ___ROUND(x) ((int)(x+0.5))

/*
	Canny wo fix suru
*/
void CannyFix(cv::Mat &src){
	static const double PI = acos(-1);
	using namespace std;
	const int dx[] = {-1,0,1,-1,1,-1,0,1};
	const int dy[] = {-1,-1,-1,0,0,1,1,1};
	cv::Mat cpy = src.clone();
	for(int i = 0 ; i < src.rows ; i++){
		for(int j = 0 ; j < src.cols ; j++){
			if( src.data[i*src.step+j] > 0 ){
				vector<int> u;
				queue<int> Q;
				Q.push(i*src.step+j);
				src.data[i*src.step+j] = 0;

				while(Q.size()){
					int q = Q.front(); Q.pop();
					int y = q / src.step;
					int x = q % src.step;
					//cout << x << "," << y << " " << src.cols << " " << src.rows << endl;
					u.push_back(q);
					for(int d = 0 ; d < 8 ; d++){
						int tx = x + dx[d];
						int ty = y + dy[d];
						if( tx >= 0 && tx < src.step && ty >= 0 && ty < src.rows && src.data[ ty * src.step + tx ] > 0 ){
							src.data[ ty * src.step + tx ] = 0;
							Q.push(ty*src.step+tx);
						}
					}
				}
				if( u.size() <= 20 ){
					for(int k = 0 ; k < u.size() ; k++){
						cpy.data[u[k]] = 0;
					}
					continue;
				}

				int	sumx = 0 , sumy = 0;
				for(int k = 0 ; k < u.size() ; k++){
					int y = u[k] / src.step;
					int x = u[k] % src.step;
					sumx += x;
					sumy += y;
				}
				sumx /= u.size();
				sumy /= u.size();
				
				int maxdist = 0;
				for(int k = 0 ; k < u.size() ; k++){
					int y = u[k] / src.step;
					int x = u[k] % src.step;
					maxdist = max( maxdist, (x-sumx)*(x-sumx)+(y-sumy)*(y-sumy) );
				}
				double len = 2 * sqrt(1.0*maxdist) * PI;
				//cout << u.size() << " " << len << " " <<  ( len - u.size() )  / len << endl;
				
				if( fabs( len - u.size() )  / len < 0.4	 ){
					for(int k = 0 ; k < u.size() ; k++){
						cpy.data[u[k]] = 0;
					}
				}
			}

		}
	}
	src = cpy.clone();
}

bool isProbableConnecting(const cv::Mat &edgeData,const cv::Point &a,const cv::Point &b,const double &ra,const double &rb){
	if( (a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y) < 1e-3 ) return true; // for same petals
	if( fabs(ra-rb) / (rb) > 0.1) return false;
	//std::cout <<  sqrt( (a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y) - ra - rb  << std::endl;
	if( sqrt( (a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y) ) - ra - rb > 3*std::min(ra,rb) ) return false;
	double dx = ( b.x - a.x );
	double dy = ( b.y - a.y );
	int loopCounter;
	if( abs(dy) < abs(dx) ){
		loopCounter = abs(dx);
		dy /= abs(dx);
		dx /= abs(dx);
	}else{
		loopCounter = abs(dy);
		dx /= abs(dy);
		dy /= abs(dy);
	}
	

	double cx = a.x;
	double cy = a.y;
	
	int cx_int = ___ROUND(cx);
	int cy_int = ___ROUND(cy);
	int A = a.y < b.y ? a.y : b.y;
	int B = a.y > b.y ? a.y : b.y;
	int C = a.x < b.x ? a.y : b.x;
	int D = a.x > b.x ? a.y : b.x;
	
	int k = 2;
	int p = 0;
	//std::cout << std::endl;
	while(loopCounter--){
		cx += dx;
		cy += dy;
		//std::cout <<  ___ROUND(cy) << " " <<  ___ROUND(cx) << "[" << ___ROUND(cy)*edgeData.cols+___ROUND(cx) << "]" << std::endl;
		if( edgeData.data[___ROUND(cy)*edgeData.cols+___ROUND(cx)] ){
			if( p == 0 ) k--;
			if(!k)return false;
			p = 1;
		}else{
			p = 0;
		}
	}
	/*std::cout << "(" << a.x << "," << a.y << ") - ("<< b.x << "," << b.y << ")" << std::endl;
	for(int i = A ; i <= B ; i++){
		for(int j = C ; j <= D ; j++){
			std::cout << (cpy.data[___ROUND(i)*edgeData.cols+___ROUND(j)]!=255?cpy.data[___ROUND(i)*edgeData.cols+___ROUND(j)]:2);
		}
		std::cout << std::endl;
	}*/
	return true;
}
// O(n^2) n := number of petals.
std::vector< std::vector<cv::Point> > clustering(const cv::Mat &edgeData,const std::vector<cv::Point> &petals,const std::vector<double> &radiuses){
	std::vector< std::vector<cv::Point> > answer;
	std::vector<bool> used(petals.size(),false);
	for(int i = 0 ; i < petals.size() ; i++){
		if( used[i] ) continue;
		used[i] = true;
		answer.push_back(std::vector<cv::Point>());
		answer.back().push_back(petals[i]);
		for(int j = i + 1 ; j < petals.size() ; j++){
			if( used[j] ) continue;
			if( isProbableConnecting(edgeData,petals[i],petals[j],radiuses[i],radiuses[j]) ){
				answer.back().push_back(petals[j]);
				used[j] = true;
				if( answer.back().size() >= 6 ) break;
			}
		}
		//std::cout << i << std::endl;
	}
	return answer;
}


int main(int argc, char *argv[])
{
	srand(100);
	cv::Mat src_img = cv::imread("img/sample.jpg", 1);
	if(!src_img.data) return -1;

	cv::Mat gray_img, bin_img,lap_bin_img,tmp_img;
	
	cv::cvtColor(src_img, gray_img, CV_BGR2GRAY);

	std::vector<std::vector<cv::Point> > contours;
	
	/*cv::Mat laplacian_img;
	cv::Laplacian(gray_img, tmp_img, CV_32F, 3);
	cv::convertScaleAbs(tmp_img, laplacian_img, 1, 1);
	cv::threshold(laplacian_img, lap_bin_img, 0, 255, cv::THRESH_BINARY|cv::THRESH_OTSU);
	cv::imshow("lap", lap_bin_img);
	cv::waitKey(0);
	*/
	
	// 画像の二値化
	
	cv::threshold(gray_img, bin_img, 0, 255, cv::THRESH_BINARY|cv::THRESH_OTSU);
	// 輪郭の検出
	cv::findContours(bin_img, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
	 
	// Canny
	cv::Mat canny_img;
	cv::Canny(src_img, canny_img, 50, 200);
	CannyFix(canny_img);
	/*for(int i = 0; i < contours.size(); ++i) {
		size_t count = contours[i].size();
		if(count < 10 || count > 100) continue; // （小さすぎる|大きすぎる）輪郭を除外

		cv::Mat pointsf;
		cv::Mat(contours[i]).convertTo(pointsf, CV_32F);
		// 楕円フィッティング
		cv::RotatedRect box = cv::fitEllipse(pointsf);
		// 楕円の描画
		cv::ellipse(canny_img, box, cv::Scalar(0,0,0), 4, 2);
		//rrects.push_back(box);
		//std::cout << i << " : " << box.center.x << " " << box.center.y << " | " << box.size.width << " " << box.size.height << " | " << box.angle << std::endl;	
	}*/

	cv::vector< cv::Point > middles;
	std::vector<double> radiuses;
	for(int i = 0 ; i < contours.size() ; i++){
		size_t count = contours[i].size();
		if(count < 10 || count > 100) continue; // （小さすぎる|大きすぎる）輪郭を除外

		double x = 0;
		double y = 0;
		for(int j = 0 ; j < contours[i].size() ; j++){
			x += contours[i][j].x;
			y += contours[i][j].y;
			//middles.push_back(cv::Point(x,y));
		}
		x /= contours[i].size();
		y /= contours[i].size();
		
		double rad = 0;
		for(int j = 0 ; j < contours[i].size() ; j++){
			rad = std::max( (x-contours[i][j].x)*(x-contours[i][j].x)+(y-contours[i][j].y)*(y-contours[i][j].y) , rad); 
			//middles.push_back(cv::Point(x,y));
		}
		radiuses.push_back(sqrt(rad));
		middles.push_back(cv::Point(x,y));
	
	}
	std::cout << middles.size() << std::endl;
	//std::cout << dx << " " << dy << " " << " " << loopCounter << " [" << edgeData.channels() << std::endl;
	std::vector< std::vector<cv::Point> > vp = clustering(canny_img,middles,radiuses);
	
	std::cout << middles.size() << std::endl;
	
	cv::namedWindow("Original(Grayscale)", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
	cv::namedWindow("Canny", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
	cv::imshow("Original(Grayscale)", src_img);
	cv::imshow("Canny", canny_img);
	cv::waitKey(0);
	
	for(int i = 0 ; i < vp.size() ; i++){
		cv::Scalar s = cv::Scalar(rand()%256,rand()%256,rand()%256);
		for(int j = 0 ; j < vp[i].size() ; j++){
			cv::circle(src_img, vp[i][j], 2, s, 3, 4);
		}
		
	}
  cv::imshow("fit ellipse", src_img);
  cv::waitKey(0);
	
  std::vector<cv::RotatedRect> rrects;
  for(int i = 0; i < contours.size(); ++i) {
    size_t count = contours[i].size();
    if(count < 10 || count > 100) continue; // （小さすぎる|大きすぎる）輪郭を除外

    cv::Mat pointsf;
    cv::Mat(contours[i]).convertTo(pointsf, CV_32F);
    // 楕円フィッティング
    cv::RotatedRect box = cv::fitEllipse(pointsf);
    // 楕円の描画
    cv::ellipse(src_img, box, cv::Scalar(255,0,count*2), 1, CV_AA);
    rrects.push_back(box);
    std::cout << i << " : " << box.center.x << " " << box.center.y << " | " << box.size.width << " " << box.size.height << " | " << box.angle << std::endl;
  }
  std::cout << "=============" << std::endl;
  for(int i=0;i<rrects.size();++i) {
    cv::RotatedRect &rect = rrects[i];
    int index = 0;
    int index2 = 0;
    int index3 = 0;
    cv::Point point = rect.center;
    double b = INT_MAX;
    double b2 = INT_MAX;
    double b3 = INT_MAX;
    bool flag = false;
    for(int j=0;j<rrects.size();++j) {
      if(i == j) continue;
      cv::RotatedRect &r = rrects[j];
      cv::Point c = r.center;
      double tmp = (point.x-c.x)*(point.x-c.x)+(point.y-c.y)*(point.y-c.y);
      if(b > tmp) {
        index = j;
        b = tmp;
        continue;
      }
      if(b2 > tmp) {
        index2 = j;
        b2 = tmp;
        continue;
      }
      if(b3 > tmp) {
        index3 = j;
        b3 = tmp;
        continue;
      }
    }

    static double wall = 2.0;
    static double wall2 = 300.0;
    std::cout << rrects[i].size.width << ", " << rrects[i].size.height << " : " << rrects[i].angle << std::endl;
    std::cout << rrects[index].size.width << ", " << rrects[index].size.height << " : " << rrects[index].angle << std::endl;
    double tmp = fabs(rrects[i].size.width - rrects[index].size.width) + fabs(rrects[i].size.height - rrects[index].size.height)/* + fabs(rrects[i].angle - rrects[index].angle)*/;
    std::cout << tmp << std::endl;
    if(tmp < wall) flag = true;
    else flag = false;


    point = rrects[i].center;
    cv::Point c = rrects[index].center;
    tmp = (point.x-c.x)*(point.x-c.x)+(point.y-c.y)*(point.y-c.y);
    if(flag) std::cout << "-" << tmp << std::endl;
    if(tmp < wall2) {
      if(flag) cv::line(src_img, point, rrects[index].center, cv::Scalar(255, 0, 255), 2);
      else cv::line(src_img, point, rrects[index].center, cv::Scalar(255, 0, 0), 1);
    }
    std::cout << "-" << std::endl;








    std::cout << rrects[i].size.width << ", " << rrects[i].size.height << " : " << rrects[i].angle << std::endl;
    std::cout << rrects[index2].size.width << ", " << rrects[index2].size.height << " : " << rrects[index2].angle << std::endl;
    tmp = fabs(rrects[i].size.width - rrects[index2].size.width) + fabs(rrects[i].size.height - rrects[index2].size.height)/* + fabs(rrects[i].angle - rrects[index2].angle)*/;
    std::cout << tmp << std::endl;
    if(tmp < wall) flag = true;
    else flag = false;

    point = rrects[i].center;
    c = rrects[index2].center;
    tmp = (point.x-c.x)*(point.x-c.x)+(point.y-c.y)*(point.y-c.y);
    if(flag) std::cout << "-" << tmp << std::endl;
    if(tmp < wall2) {
      if(flag) cv::line(src_img, point, rrects[index2].center, cv::Scalar(255, 0, 255), 2);
      else cv::line(src_img, point, rrects[index2].center, cv::Scalar(0, 255, 0), 1);
    }
    std::cout << "-" << std::endl;










    std::cout << rrects[i].size.width << ", " << rrects[i].size.height << " : " << rrects[i].angle << std::endl;
    std::cout << rrects[index3].size.width << ", " << rrects[index3].size.height << " : " << rrects[index3].angle << std::endl;
    tmp = fabs(rrects[i].size.width - rrects[index3].size.width) + fabs(rrects[i].size.height - rrects[index3].size.height)/* + fabs(rrects[i].angle - rrects[index3].angle)*/;
    std::cout << tmp << std::endl;
    if(tmp < wall) flag = true;
    else flag = false;


    point = rrects[i].center;
    c = rrects[index3].center;
    tmp = (point.x-c.x)*(point.x-c.x)+(point.y-c.y)*(point.y-c.y);
    if(flag) std::cout << "-" << tmp << std::endl;
    if(tmp < wall2) {
      if(flag) cv::line(src_img, point, rrects[index3].center, cv::Scalar(255, 0, 255), 2);
      else cv::line(src_img, point, rrects[index3].center, cv::Scalar(0, 0, 255),1);
    }
    std::cout << "----" << std::endl;
  }

  cv::namedWindow("fit ellipse", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
  cv::imshow("fit ellipse", src_img);
  cv::waitKey(0);
}
