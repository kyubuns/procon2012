#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace std;
using namespace cv;

Mat src_img,gray_img;
int const max_thresh=255;
RNG rng(255);

int main(int argc,char** argv){
  src_img = imread(argv[1]);
  cvtColor(src_img,gray_img,CV_BGR2GRAY);
  blur(gray_img,gray_img,Size(3,3));

  namedWindow("Contours",CV_WINDOW_AUTOSIZE);
  imshow("Contours",src_img);
  createTrackbar("Canny thresh;", "Contours", 0, max_thresh, [](int thresh,void*){
    Mat canny_img;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;

    Canny(gray_img,canny_img,thresh,thresh*2,3);
    findContours(canny_img,contours,hierarchy,CV_RETR_TREE,CV_CHAIN_APPROX_SIMPLE);

    Mat draw_img = Mat::zeros(canny_img.size(),CV_8UC3);
    for(int i=0;i<contours.size();i++){
      Scalar color=Scalar(rng.uniform(0,255),rng.uniform(0,255),rng.uniform(0,255));
      drawContours(draw_img,contours,i,color,2,8,hierarchy,0,Point());
    }
    imshow("Contours",draw_img);
  });

  setTrackbarPos("Canny thresh;", "Contours", 100);
  waitKey();
}
