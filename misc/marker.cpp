#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <list>
#include <cmath>
using namespace std;
using namespace cv;

cv::Mat src_img,gray_img,dst_img,detected_edges;
int lowThreshold,ratio=3,karnel_size=3;
int const max_lowThreshold=100;
string window_name="Window";

void CannyThreshold(int, void*){
	blur(gray_img,detected_edges,Size(3,3));
	Canny(detected_edges,detected_edges,lowThreshold,lowThreshold*ratio,karnel_size);
	dst_img=Scalar::all(0);
	src_img.copyTo(dst_img,detected_edges);
	imshow(window_name,dst_img);

}

int main(int argc, char *argv[])
{
	src_img=cv::imread(argv[1]);
	if(!src_img.data)return -1;

	dst_img.create(src_img.size(),src_img.type());

	cvtColor(src_img,gray_img,CV_BGR2GRAY);

	namedWindow(window_name,CV_WINDOW_AUTOSIZE);

	createTrackbar("Min Threshold:",window_name,&lowThreshold,max_lowThreshold,CannyThreshold);
	CannyThreshold(0,0);

	waitKey(0);
}
