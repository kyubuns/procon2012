#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <queue>
#include <vector>
#include <cassert>
#include <cmath>
#define GetMatIdx(img,y,x) ((y)*img.step()+(x)*img.channels())
using namespace std;
const double PI = acos(-1.0);
#ifdef _DEBUG
    //Debugモードの場合
    #pragma comment(lib,"C:\\opencv\\lib\\opencv_core242d.lib")
    #pragma comment(lib,"C:\\opencv\\lib\\opencv_imgproc242d.lib")
    #pragma comment(lib,"C:\\opencv\\lib\\opencv_highgui242d.lib")
    #pragma comment(lib,"C:\\opencv\\lib\\opencv_objdetect242d.lib")
    //以下、必要に応じて追加
    //#pragma comment(lib,"C:\\opencv\\lib\\opencv_ml242d.lib")
    //#pragma comment(lib,"C:\\opencv\\lib\\opencv_features2d242d.lib")
    //#pragma comment(lib,"C:\\opencv\\lib\\opencv_video242d.lib")
    //#pragma comment(lib,"C:\\opencv\\lib\\opencv_calib3d242d.lib")
    //#pragma comment(lib,"C:\\opencv\\lib\\opencv_flann242d.lib")
    //#pragma comment(lib,"C:\\opencv\\lib\\opencv_contrib242d.lib")
    //#pragma comment(lib,"C:\\opencv\\lib\\opencv_legacy242d.lib")
    //#pragma comment(lib,"C:\\opencv\\lib\\opencv_gpu242d.lib")
#else
    //Releaseモードの場合
    #pragma comment(lib,"C:\\opencv\\lib\\opencv_core242.lib")
    #pragma comment(lib,"C:\\opencv\\lib\\opencv_imgproc242.lib")
    #pragma comment(lib,"C:\\opencv\\lib\\opencv_highgui242.lib")
    #pragma comment(lib,"C:\\opencv\\lib\\opencv_objdetect242.lib")
    //以下、必要に応じて追加
    //#pragma comment(lib,"C:\\opencv\\lib\\opencv_ml242.lib")
    //#pragma comment(lib,"C:\\opencv\\lib\\opencv_features2d242.lib")
    //#pragma comment(lib,"C:\\opencv\\lib\\opencv_video242.lib")
    //#pragma comment(lib,"C:\\opencv\\lib\\opencv_calib3d242.lib")
    //#pragma comment(lib,"C:\\opencv\\lib\\opencv_flann242.lib")
    //#pragma comment(lib,"C:\\opencv\\lib\\opencv_contrib242.lib")
    //#pragma comment(lib,"C:\\opencv\\lib\\opencv_legacy242.lib")
    //#pragma comment(lib,"C:\\opencv\\lib\\opencv_gpu242.lib")
#endif

long long sum[3000][3000][3];


std::vector<cv::Point> myMatch(const cv::Mat src,const cv::Mat pattern){
	cv::imshow("pattern", pattern);
	if( src.channels() != pattern.channels() ) assert(0);

	long long psum[3] = {0,0,0};
	for(int i = 0 ; i < pattern.rows ; i++){
		for(int j = 0 ; j < pattern.cols ; j++){
			for(int k = 0 ; k < pattern.channels() ; k++){
				psum[k] += pattern.data[GetMatIdx(pattern,i,j)+k];
			}
		}
	}
	vector< vector<bool> > done(src.rows,vector<bool>(src.cols));
	cout << "(" << pattern.rows << "," << pattern.cols << "[" << pattern.step << "]) : " << psum[0] << " " << psum[1] << " " << psum[2] << endl;
	std::vector<cv::Point> p;
	for(int i = 0 ; i < src.rows - pattern.rows + 1 ; i++){
		for(int j = 0 ; j < src.cols - pattern.cols + 1 ; j++){
			if( done[i][j] ) continue;
			long long cut = 0;
			for(int c = 0 ; c < pattern.channels() ; c++){
				long long d = sum[i+pattern.rows][j+pattern.cols][c] - sum[i+pattern.rows][j][c] - sum[i][j+pattern.cols][c] + sum[i][j][c];
				//cout << d << endl;
				cut += ( psum[c] - d ) * ( psum[c] - d ) ;
			}
			
			if( cut > 5000000 ){ continue; }
			//cout << cut << endl;
			double calc = 0;
			for(int k = 0 ; k < pattern.rows ; k++){
				for(int l = 0 ; l < pattern.cols ; l++){
					if( done[i+k][j+l] ) calc += 1e9;
					int sum = 0;
					for(int c = 0 ; c < pattern.channels() ; c++){
						int d = src.data[GetMatIdx(src,i+k,j+l)+c] - pattern.data[GetMatIdx(pattern,k,l)+c];
						sum += d * d;
					}
					calc += sum;
					if( calc / (pattern.cols*pattern.rows) > 2000 ) break;
				}
			}
			if( calc / (pattern.cols*pattern.rows) < 2000){
				p.push_back(cv::Point(j,i));
				for(int k = 0 ; k < pattern.rows ; k++){
					for(int l = 0 ; l < pattern.cols ; l++){
						done[i+k][j+l] = true;
					}
				}
				
			}
		}
	}
	return p;	
}

/*
つかいかた

./ぷろぐらむ　元がぞう　パターンファイル1 パターンファイル2 ... パターンファイルn

しきい値はうえの関数の1000とかいうところをいじってください

差分の二乗和の総和を評価値に計算してます．かなり遅いです．

*/

int sx,sy,gx,gy;

cv::Mat input;

void petal_detect(int x,int y){
	queue<int> Q;
	vector<bool> done(input.step*input.rows);
	Q.push(GetMatIdx(input,y,x));done[GetMatIdx(input,y,x)]=1;//初期位置。クリックで決定？
	int step=input.step;
	int dq[]={-input.channels(),input.channels(),step,-step},cnt=0;
	int g=input.data[GetMatIdx(input,y,x)],b=input.data[GetMatIdx(input,y,x)+1],r=input.data[GetMatIdx(input,y,x)+2];
		
	while(Q.size()){
		int q=Q.front();Q.pop();
		//int g=input.data[q],b=input.data[q+1],r=input.data[q+2];
		sx = min(sx,q%step/input.channels());
		gx = max(gx,q%step/input.channels());
		sy = min(sy,q/step);
		gy = max(gy,q/step);

		//input.data[q]=input.data[q+1]=input.data[q+2]=0;
		for(int i = 0 ; i < 4 ; i++){
			int nq=q+dq[i];
			if(nq<0||nq>=input.step*input.rows)continue;
			int ng=input.data[nq],nb=input.data[nq+1],nr=input.data[nq+2];
			int dif=(r-nr)*(r-nr)+(g-ng)*(g-ng)+(b-nb)*(b-nb);
			if(!done[nq]&&dif<1000){
				Q.push(nq);
				done[nq]=1;
			}
		}
	}
	sx = max(0,sx-1);
	gx = min(input.cols-1,gx+1);
	sy = max(0,sy-1);
	gy = min(input.rows-1,gy+1);


}
void Mouse( int event, int x, int y, int flags ,void *param = NULL) // コールバック関数
{
        switch(event)
        {
                case CV_EVENT_LBUTTONDOWN:
						sx = x;
						sy = y;
                break;
				case CV_EVENT_RBUTTONDOWN:
						gx = x;
						gy = y;
				break;
				case CV_EVENT_MBUTTONDOWN:
                        sx = gx = x;
						sy = gy = y;
						petal_detect(x,y);
						//cout << sx << " " << gx << " - " << sy << " " << gy << endl;
				break;
                default:
                break;
        }

}


int main(int argc, char *argv[]){
	const double EPS = 0.1;
	double weight[] = {5.902,1.35,0.274};
	
	double total_weight;
	cin >> total_weight;
	int a,b,c;
	cin >> a >> b >> c;
	
	double ra = 1.0;
	double rb = 1.0 * b / a;
	double rc = 1.0 * c / a;
	
	vector< pair< pair<double,int> , pair<int,int> > > v;
	for(int i = 1 ; i < 10000 ; i++){
		if( i * weight[0] > total_weight + EPS ) continue;
		for(int j = 0 ; j < 10000 ; j++){
			int kc = (int)(( total_weight - i * weight[0] - j * weight[1] ) / weight[2]);
			
			if( kc < 0 ) break;
			if( kc >= 10000) continue;
			for(int k = max(0,kc-1) ; k <= min(10000,kc+1) ; k++){
				if( fabs( total_weight - i * weight[0] - j * weight[1] - k * weight[2] ) < EPS ){
					double a = 1.0;
					double b = 1.0 * j / i;
					double c = 1.0 * k / i;
					//if( fabs(rb-b) + fabs(rc-c) ){
						v.push_back(make_pair(make_pair(fabs(rb-b) + fabs(rc-c),i),make_pair(j,k)));
					//}
				}
			}
		}
	}
	sort(v.begin(),v.end());
	for(int i =0 ; i <v.size() ; i++)
		cout <<v[i].first.first << "("<< v[i].first.second << "," << v[i].second.first <<"," << v[i].second.second <<")" << endl;
}



