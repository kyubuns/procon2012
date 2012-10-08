#include <iostream>
#include <cstdio>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace std;
using namespace cv;

Mat src_img,tmp_img,result_img;

int match_method;
const int max_method=5;

void MatchingMethod(int,void*){
	Mat display_img;
	src_img.copyTo(display_img);

	int result_cols=src_img.cols-tmp_img.cols+1;
	int result_rows=src_img.rows-tmp_img.rows+1;

	result_img.create(result_cols,result_rows,CV_32FC1);

	matchTemplate(src_img,tmp_img,result_img,match_method);
	normalize(result_img,result_img,0,1,NORM_MINMAX,-1,Mat());

	double minVal,maxVal;
	Point minLoc,maxLoc,matchLoc;

	minMaxLoc(result_img,&minVal,&maxVal,&minLoc,&maxLoc,Mat());
	
	if(match_method == CV_TM_SQDIFF || match_method == CV_TM_SQDIFF_NORMED)
		matchLoc=minLoc;
	else matchLoc=maxLoc;

	rectangle(display_img,matchLoc,Point(matchLoc.x+tmp_img.cols,matchLoc.y+tmp_img.rows),Scalar::all(0),2,8,0);
	rectangle(result_img,matchLoc,Point(matchLoc.x+tmp_img.cols,matchLoc.y+tmp_img.rows),Scalar::all(0),2,8,0);

	imshow("image_window",display_img);
	imshow("result_window",result_img);
}

int main(int argc,char** argv){
	src_img=imread(argv[1]);
	tmp_img=imread(argv[2]);
	if(!src_img.data||!tmp_img.data)return -1;

	namedWindow("image_window",CV_WINDOW_AUTOSIZE);
	namedWindow("result_window",CV_WINDOW_AUTOSIZE);

	string bar_name="Method: \n 0: SQDIFF \n 1: SQDIFF NORMED \n 2: TM CCORR \n 3: TM CCORR NORMED \n 4: TM COEFF \n 5: TM COEFF NORMED";
	createTrackbar(bar_name,"image_window",&match_method,max_method,MatchingMethod);

	MatchingMethod(0,0);

	waitKey(0);
}
