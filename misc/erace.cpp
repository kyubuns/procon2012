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
