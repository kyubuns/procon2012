#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "kyuri.hpp"

//設定
int const max_thresh=255;
cv::Mat src_img;
cv::Mat gray_img;
cv::RNG rng(255);

cv::Mat loadImage(const std::string &filename) {
  cv::Mat tmp = cv::imread(filename);
  if(tmp.data == NULL) throw std::runtime_error("no imagefile");
  return tmp;
}

cv::Mat transform(cv::Mat src_img, double rev){
  cv::Point2f pts1[] = {
    cv::Point2f(100+rev,100),
    cv::Point2f(100,380),
    cv::Point2f(540,380),
    cv::Point2f(540-rev,100)
  };
  cv::Point2f pts2[] = {
    cv::Point2f(100,100),
    cv::Point2f(100,380),
    cv::Point2f(540,380),
    cv::Point2f(540,100)
  };

  // 透視変換行列を計算
  cv::Mat perspective_matrix = cv::getPerspectiveTransform(pts1, pts2);
  cv::Mat dst_img;

  // 変換
  cv::warpPerspective(src_img, dst_img, perspective_matrix, src_img.size(), cv::INTER_LINEAR);

  return dst_img;
}

int main(int argc,char** argv) {
  using namespace std;
  using namespace cv;
  src_img = transform(loadImage(argv[1]), 50.0);

  cvtColor(src_img,gray_img,CV_BGR2GRAY);
  blur(gray_img,gray_img,Size(3,3));

  namedWindow("Contours",CV_WINDOW_AUTOSIZE);
  imshow("Contours",src_img);
  createTrackbar("Canny thresh", "Contours", 0, max_thresh, [](int thresh,void*){
    Mat canny_img;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;

    Canny(gray_img,canny_img,thresh,thresh*2,3);
    findContours(canny_img,contours,hierarchy,CV_RETR_TREE,CV_CHAIN_APPROX_SIMPLE);

    Mat draw_img = Mat::zeros(canny_img.size(),CV_8UC1);
    for(int i=0;i<contours.size();i++){
      Scalar color=Scalar(255, 255, 255);
      drawContours(draw_img,contours,i,color,1,8,hierarchy,0,Point());
    }
    kyuri::main(src_img, draw_img);
  });
  setTrackbarPos("Canny thresh", "Contours", 100);

  waitKey();
  return 0;
}

