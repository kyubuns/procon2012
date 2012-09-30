#include<iostream>
#include <cstdio>
#include <cstdlib>
#include <queue>
#include <algorithm>
#include <map>
#include <vector>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#define GetMatIdx(img,y,x) (y*img.cols*img.channels()+x*img.channels())
#define GetMatMaxSize(img) (img.cols*img.rows*img.channels())

using namespace std;
using namespace cv;

typedef pair<int,int> P;

Mat src_img;
int blur_size,AcDif;
int const max_blur=20,max_diff=20;


#define haveColor(src,i) (src.data[i+0]>0||src.data[i+1]>0||src.data[i+2]>0)
void bigFix(cv::Mat &src){
    
	const int dx[] = {-1,0,1,-1,1,-1,0,1,0};
	const int dy[] = {-1,-1,-1,0,0,1,1,1,0};
	cv::Mat cpy = src.clone();
    int step = src.channels() * src.cols;
    int ch = src.channels();
    cout << ch << endl;
	for(int i = 0 ; i < src.rows ; i++){
		for(int j = 0 ; j < src.cols ; j++){
			if( haveColor(src,i*step+j*ch) ){
				vector<int> u;
				queue<int> Q;
				Q.push(i*step+ch*j);
				src.data[i*step+ch*j+0] = 0;
                src.data[i*step+ch*j+1] = 0;
                src.data[i*step+ch*j+2] = 0;
                
				while(Q.size()){
					int q = Q.front(); Q.pop();
					int y = q / step;
					int x = q % step / 3;
					//cout << x << "," << y << " " << src.cols << " " << src.rows << endl;
					u.push_back(q);
					for(int d = 0 ; d < 8 ; d++){
						int tx = x + dx[d];
						int ty = y + dy[d];
						if( tx >= 0 && tx < src.cols && ty >= 0 && ty < src.rows && haveColor(src,ty*step+tx*ch) ){
							src.data[ty*step+tx*ch+0] = 0;
							src.data[ty*step+tx*ch+1] = 0;
							src.data[ty*step+tx*ch+2] = 0;
                            Q.push(ty*src.step+tx*ch);
						}
					}
				}
				//cout << u.size() << endl;
				if( u.size() < 100 ){
					
					for(int k = 0 ; k < u.size() ; k++){
						int y = u[k] / step;
						int x = u[k] % step / 3;
						for(int d = 0 ; d < 9 ; d++){
							int tx = x + dx[d];
							int ty = y + dy[d];
                            if( tx >= 0 && tx < src.cols && ty >= 0 && ty < src.rows ){
                                cpy.data[ ty * step + tx*ch+0 ] = 0;
                                cpy.data[ ty * step + tx*ch+1 ] = 0;
                                cpy.data[ ty * step + tx*ch+2 ] = 0;
                            }

						}
					}
				}
			}
            
		}
	}
	src = cpy.clone();
}

void Erace(int,void*){
	Mat blur_img;
	blur(src_img,blur_img,Size(blur_size+1,blur_size+1));
	queue<int> Q;
	vector<bool> done(GetMatMaxSize(blur_img));
	//memset(done,0,sizeof(done));
	//int step = blur_img.cols * blur_img.channels();
	Q.push(GetMatIdx(blur_img,300,10));done[GetMatIdx(blur_img,300,10)]=1;
	int step=blur_img.cols*blur_img.channels();
	int dq[]={-blur_img.channels(),blur_img.channels(),step,-step},cnt=0;
	while(Q.size()){
		int q=Q.front();Q.pop();
		//cout << q << endl;
		cnt++;
		int g=blur_img.data[q],b=blur_img.data[q+1],r=blur_img.data[q+2];
		blur_img.data[q]=blur_img.data[q+1]=blur_img.data[q+2]=0;
		for(int i=0;i<4;i++){
			int nq=q+dq[i];
			int nx=nq%step,ny=nq/step;
			if(nq<0||nq>=GetMatMaxSize(blur_img))continue;
			int ng=blur_img.data[nq],nb=blur_img.data[nq+1],nr=blur_img.data[nq+2];
			int dif=max(abs(r-nr),max(abs(g-ng),abs(b-nb)));
			if(!done[nq]&&dif<AcDif){
				Q.push(nq);
				done[nq]=1;
			}
		}
	}
	cout<<cnt<<endl;
    bigFix(blur_img);
	imshow("dst_img",blur_img);
}

int main(int argc,char** argv){
	src_img = imread(argv[1]);
	if(!src_img.data)return -1;
	namedWindow("src_img",WINDOW_AUTOSIZE);
	namedWindow("dst_img",WINDOW_AUTOSIZE);
	imshow("src_img",src_img);
	createTrackbar("Size-1;","src_img",&blur_size,max_blur,Erace);
	createTrackbar("Near:","src_img",&AcDif,max_diff,Erace);
	Erace(0,0);
	waitKey();
}
