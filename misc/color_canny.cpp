#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace std;
using namespace cv;

Mat src_img;
int val,ratio=3,karnel_size=3;
int const max_val=100;
string window_name="window";

void canny_bar(int,void*){
	Mat mid_img = src_img.clone(),canny_img;
	blur(src_img,mid_img,Size(3,3));
	Canny(mid_img,mid_img,val,val*ratio,karnel_size);
	canny_img=Scalar::all(0);
	src_img.copyTo(canny_img,mid_img);
	imshow(window_name,canny_img);
}

int main(int argc,char** argv){
	src_img = imread(argv[1]);
	if(!src_img.data)return -1;

	namedWindow("source",WINDOW_AUTOSIZE);
	namedWindow(window_name,CV_WINDOW_AUTOSIZE);

	createTrackbar("Canny thresh:",window_name,&val,max_val,canny_bar);
	canny_bar(0,0);
	imshow("source",src_img);
	waitKey();
}
