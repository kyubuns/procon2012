#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <queue>
#include <algorithm>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace std;

inline int GetMatIdx(cv::Mat img, int y, int x) {return y*img.cols*img.channels()+x*img.channels();}

long long sum[3000][3000][3];
long long bin_sum[3000][3000];

double edakari=5000000; //画素値の二乗和の差
double ok_avgDiff=2000; // 平均二乗画素差分和の許容差 (画素毎に差分とって二乗取ったやつの総和を面積で割ってる)
double ok_WBratio=0.05;  // 白黒画素面積比率の許容差
double BonWratio=0.5; // 白の中にいる黒の面積比率の許容差
double BnumOnW=2.0;  // 白の中にいる黒の個数(4近傍)の許容差
double gDiff=100;    // 黒画素のみの重心位置の差の二乗の許容差;
					 // 黒かたまり
	


std::vector<cv::Point> myMatch(const cv::Mat src,const cv::Mat pattern,const cv::Mat bin){
	cv::imshow("pattern", pattern);
	cv::imshow("pattern2", bin);
	if( src.channels() != pattern.channels() ) assert(0);

	long long psum[3] = {0,0,0};
	long long bsum = 0;
	for(int i = 0 ; i < pattern.rows ; i++){
		for(int j = 0 ; j < pattern.cols ; j++){
			for(int k = 0 ; k < pattern.channels() ; k++){
				psum[k] += pattern.data[GetMatIdx(pattern,i,j)+k];
			}
			bsum += (bin.data[GetMatIdx(bin,i,j)]>0?1:0);
		}
	}
	double pattern_WBratio = 1.0 * bsum / (pattern.cols*pattern.rows);



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
			if( cut > edakari ){ continue; }

			long long bin_d = bin_sum[i+pattern.rows][j+pattern.cols] - bin_sum[i+pattern.rows][j] - bin_sum[i][j+pattern.cols] + bin_sum[i][j];
			double WBratio_calc = 1.0* bin_d / (pattern.cols*pattern.rows);
			if( fabs(WBratio_calc-pattern_WBratio) > ok_WBratio ){ continue; }
			
			//cout << WBratio_calc << " " << pattern_WBratio << " " << ok_WBratio << endl;
			

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
					if( calc / (pattern.cols*pattern.rows) > ok_avgDiff ) break;
				}
			}
			if( calc / (pattern.cols*pattern.rows) < ok_avgDiff){
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
	while(Q.size()){
		int q=Q.front();Q.pop();
		int g=input.data[q],b=input.data[q+1],r=input.data[q+2];
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
			if(!done[nq]&&dif<1750){
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
                        sx = x , sy = y;
						//sx = gx = x;
						//sy = gy = y;
						//petal_detect(x,y);
						//cout << sx << " " << gx << " - " << sy << " " << gy << endl;
                break;
				case CV_EVENT_RBUTTONDOWN:
						gx = x , gy = y;
                break;
				case CV_EVENT_MBUTTONDOWN:
						sx = gx = x;
						sy = gy = y;
						petal_detect(x,y);
                default:
                break;
        }

}

void cv_ScaleShift(cv::Mat &src, double scale, double offset){

    double val;
    
	for(int i = 0 ; i < src.rows ; i++){
		for(int j = 0 ; j < src.step ; j++){
			val = scale * src.data[i*src.step+j] + offset;
            val = (0 > val) ? 0 : val;
            val = (val < 255) ? val : 255;
			src.data[i*src.step+j] = (unsigned char)val;
		}
	}

}


cv::Mat topFaceFix(const cv::Mat &src,std::vector<cv::Point> pixels){
	cv::Mat ans = src.clone();

	vector< vector<bool> > face(src.rows,vector<bool>(src.cols));
	vector< vector<bool> > done(src.rows,vector<bool>(src.cols));
	int border = 2;
	for(int i = 0 ; i < pixels.size() ; i++){
		for(int j = -border ; j <= border ; j++){
			for(int k = -border ; k <= border ; k++){
				if( pixels[i].y+j < 0 || pixels[i].x+k < 0 || pixels[i].y+j >= src.rows || pixels[i].x+k >= src.cols ) continue; 
				face[pixels[i].y+j][pixels[i].x+k] = true;
			}
		}
	}

	vector<cv::Point> tot;

	const int dx[] = {-1,0,1,0};
	const int dy[] = {0,-1,0,1};

	for(int y = 0 ; y < src.rows ; y++){
		for(int x = 0 ; x < src.cols ; x++){
			if( done[y][x] || face[y][x] ) continue;
			queue<cv::Point> Q;
			Q.push(cv::Point(x,y));
			done[y][x] = true;
			int badAreaFlag = false;
			int perimeter = 0;
			vector<cv::Point> connected;
			while(Q.size()){
				cv::Point q = Q.front(); Q.pop();
				connected.push_back(q);
				for(int i = 0 ; i < 4 ; i++){
					int tx = q.x + dx[i];
					int ty = q.y + dy[i];
					if( tx < 0 || ty < 0 || tx >= src.cols || ty >= src.rows){
						badAreaFlag = true; //重要
						continue;
					}
					if( face[ty][tx] ){perimeter++; continue;}

					if( done[ty][tx] == false ){
						done[ty][tx] = true;
						Q.push(cv::Point(tx,ty));
					}
				}
			}
			//周長と面積で
			if( connected.size() > 200 ) badAreaFlag = true;
			//3色じゃないとエラー出ます。
			
			if( badAreaFlag ){
				for(int i = 0 ; i < connected.size() ; i++){
					ans.data[GetMatIdx(ans,connected[i].y,connected[i].x)+0] = 255;
					ans.data[GetMatIdx(ans,connected[i].y,connected[i].x)+1] = 0;
					ans.data[GetMatIdx(ans,connected[i].y,connected[i].x)+2] = 0;
				}
			}
		}
	}
	return ans;
	
}


int main(int argc, char *argv[]){

	using namespace std;
	// 探索画像
	
	cv::Mat search_img = cv::imread(argv[1], 1);
	//cv_ScaleShift(search_img,0.5,-50);
	//cv_ScaleShift(search_img,2,0);
	//medianBlur(search_img, search_img, 1);
	cv::Mat bin_img;
	cv::cvtColor(search_img, bin_img, CV_BGR2GRAY);
	cv::threshold(bin_img,bin_img, 0, 255, cv::THRESH_BINARY|cv::THRESH_OTSU);
	input = search_img.clone();
	
	//累積和
	for(int i = 1 ; i <= search_img.rows ; i++){
		for(int j = 1 ; j <= search_img.cols ; j++){
			for(int k = 0 ; k < search_img.channels() ; k++){
				sum[i][j][k] = sum[i][j-1][k] + search_img.data[GetMatIdx(search_img,i-1,j-1)+k];
			}
			bin_sum[i][j] = bin_sum[i][j-1] + (bin_img.data[GetMatIdx(bin_img,i-1,j-1)]>0?1:0);
		}
	}
	for(int i = 1 ; i <= search_img.rows ; i++){
		for(int j = 1 ; j <= search_img.cols ; j++){
			for(int k = 0 ; k < search_img.channels() ; k++){
				sum[i][j][k] += sum[i-1][j][k];
			}
			bin_sum[i][j] += bin_sum[i-1][j];
		}
	}
	///
	for(int i = 0 ; i < 10 ; i++){
		for(int j = 0 ; j < 10 ; j++){
			cout << sum[i][j][2] << " ";
		}
		cout << endl;
	}


	if(search_img.empty()) return -1; 
	cv::namedWindow("control", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
	cvSetMouseCallback("control", Mouse);  //コールバック関数の登録
	
	cv::Rect rect(sx,sy,gx-sx+1,gy-sy+1);

	vector<int> history;
	vector<cv::Point> tot,tot2;

	while(1){
		cv::Mat output = search_img.clone();
		cv::imshow("control", output);

		for(int i = 0 ; i < tot.size() ; i++){
			//search_img.data[GetMatIdx(search_img,res[i].y,res[i].x)+0]=255;
			//search_img.data[GetMatIdx(search_img,res[i].y,res[i].x)+1]=0;
			//search_img.data[GetMatIdx(search_img,res[i].y,res[i].x)+2]=255;
			cv::rectangle(output, tot[i], cv::Point(tot[i].x+tot2[i].x,tot[i].y+tot2[i].y), cv::Scalar(0,0,200), 1, 4);
		}
		sx = sy = gx = gy = -1;
		while(sx == -1 || gx == -1){
			if( cv::waitKey(33) == 'z' ){
				if( history.size() ){
					for(int i = 0 ; i < history.back() ; i++)
						tot.pop_back(),tot2.pop_back();
					history.pop_back();
					output = search_img.clone();
					for(int i = 0 ; i < tot.size() ; i++)
						cv::rectangle(output, tot[i], cv::Point(tot[i].x+tot2[i].x,tot[i].y+tot2[i].y), cv::Scalar(0,0,200), 1, 4);
				}
			}
		
			cv::Mat view = output.clone();
			if( sx != -1 ){
				cv::circle(view,cv::Point(sx,sy),1,cv::Scalar(0,255,0),1,1);
			}
			cv::imshow("control", view);
			cv::waitKey(30);
		};
		
		if(sx>gx)swap(sx,gx);
        if(sy>gy)swap(sy,gy);
		cout << sx << " " << sy << " " << gx << " " << gy << endl;
		cout << input.cols << " " << input.rows << endl;
		cv::Mat tmp_img(input,cv::Rect(sx,sy,gx-sx+1,gy-sy+1));
		cv::Mat bin(bin_img,cv::Rect(sx,sy,gx-sx+1,gy-sy+1));
		


		std::vector<cv::Point> res = myMatch(input,tmp_img,bin);
		cout << res.size() << endl;

		for(int i = 0 ; i < res.size() ; i++)
			tot.push_back(res[i]),tot2.push_back(cv::Point(tmp_img.cols,tmp_img.rows));
		history.push_back(res.size());

		if( gx == sx && gy == sy ){
			cv::Mat ans = topFaceFix(input,res);
			cv::imshow("test",ans);
			input = ans.clone();
			search_img = ans.clone();
		}
	}
	
	
}
