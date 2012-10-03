#include<iostream>
#include <cstdio>
#include <cstdlib>
#include <queue>
#include <algorithm>
#include <map>
#include <vector>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#define REP(i,j,k) for(int i=j;i<k;i++)
#define rep(i,j) REP(i,0,j)
#define GetMatIdx(img,y,x) (y*img.cols*img.channels()+x*img.channels())
#define GetMatMaxSize(img) (img.cols*img.rows*img.channels())
#define haveColor(src,i) (src.data[i+0]>0||src.data[i+1]>0||src.data[i+2]>0)

using namespace std;
using namespace cv;

typedef pair<int,int> P;

cv::Mat src_img, erasebg_img;
int blur_size,AcDif;
int const max_blur=20,max_diff=20;

void bigFix(cv::Mat &src){

	const int dx[] = {-1,0,1,-1,1,-1,0,1,0};
	const int dy[] = {-1,-1,-1,0,0,1,1,1,0};
	cv::Mat cpy = src.clone();
	int ch = src.channels();
	int step = ch * src.cols;
	cout << ch << endl;
	rep(i,src.rows)rep(j,src.cols){
		int idx=i*step+j*ch;
		if( haveColor(src,idx) ){
			vector<int> u;
			queue<int> Q;
			Q.push(idx);
			src.data[idx+0] = src.data[idx+1] = src.data[idx+2] = 0;
			while(Q.size()){
				int q = Q.front(); Q.pop();
				int y = q / step;
				int x = q % (step / 3);
				u.push_back(q);
				rep(d,8){
					int tx = x + dx[d];
					int ty = y + dy[d];
					int nidx=ty*step+tx*ch;
					if( tx >= 0 && tx < src.cols && ty >= 0 && ty < src.rows && haveColor(src,nidx) ){
						src.data[nidx+0] = src.data[nidx+1] = src.data[nidx+2] = 0;
						Q.push(nidx);
					}
				}
			}
			if( u.size() < 100 ){ //要パラメータ化？
				rep(k,u.size()){
					int y = u[k] / step;
					int x = u[k] % step / 3;
					rep(d,9){
						int tx = x + dx[d];
						int ty = y + dy[d];
						int nidx=ty*step+tx*ch;
						if( tx >= 0 && tx < src.cols && ty >= 0 && ty < src.rows )
							cpy.data[ nidx+0 ] = cpy.data[ nidx+1 ] = cpy.data[ nidx+2 ] = 0;
					}
				}
			}
		}
	}
	src = cpy.clone();
}

int _gl_clicked_background_x;  // Erase() de tukau
int _gl_clicked_background_y;  // Erase() de tukau

void Erase(int,void*){
	Mat dst_img;
	blur(src_img,dst_img,Size(blur_size+1,blur_size+1));
	queue<int> Q;
	vector<bool> done(GetMatMaxSize(dst_img));
	int sIdx = GetMatIdx(dst_img,_gl_clicked_background_y,_gl_clicked_background_x);
	//cout << _gl_clicked_background_x <<","<< _gl_clicked_background_y<< ": " << sIdx << endl;
	
	Q.push(sIdx);done[sIdx]=1;//初期位置。クリックで決定？
	int step=dst_img.cols*dst_img.channels();
	int dq[]={-dst_img.channels(),dst_img.channels(),step,-step},cnt=0;
	while(Q.size()){
		int q=Q.front();Q.pop();
		int g=dst_img.data[q],b=dst_img.data[q+1],r=dst_img.data[q+2];
		dst_img.data[q]=255;dst_img.data[q+1]=dst_img.data[q+2]=0;
		rep(i,4){
			int nq=q+dq[i];
			int nx=nq%step,ny=nq/step;
			if(nq<0||nq>=GetMatMaxSize(dst_img))continue;
			int ng=dst_img.data[nq],nb=dst_img.data[nq+1],nr=dst_img.data[nq+2];
			int dif=max(abs(r-nr),max(abs(g-ng),abs(b-nb)));
			if(!done[nq]&&dif<AcDif){
				Q.push(nq);
				done[nq]=1;
			}
		}
	}
	bigFix(dst_img);
	erasebg_img = dst_img;
	cv::circle(erasebg_img,cv::Point(_gl_clicked_background_x,_gl_clicked_background_y),5,cv::Scalar(0,255,0),1);
	imshow("dst_img",erasebg_img);
}

void MouseClick( int event, int x, int y, int flags ,void *param = NULL) // コールバック関数
{
        switch(event)
        {
                case CV_EVENT_LBUTTONDOWN:
					_gl_clicked_background_x = x;
					_gl_clicked_background_y = y;
					Erase(0,0);
                break;

                default:
                break;
        }

}

int erase_main(){
	namedWindow("dst_img",WINDOW_AUTOSIZE);
	cvSetMouseCallback("dst_img", MouseClick);
	createTrackbar("Size-1;","dst_img",&blur_size,max_blur,Erase);
	createTrackbar("Near:","dst_img",&AcDif,max_diff,Erase);
	Erase(0,0);
	waitKey();
}
