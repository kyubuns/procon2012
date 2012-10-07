#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

Mat src_img;
Point begin_point;

void mouse_callback( int event, int x, int y, int flag ,void*) {
  std::string desc;
  if(event==EVENT_LBUTTONDOWN) {
    std::cout << x << "," << y << ": down" << std::endl;
    begin_point.x = x;
    begin_point.y = y;
  }
  else if(event==EVENT_LBUTTONUP) {
    std::cout << x << "," << y << ": up" << std::endl;
    if(x == begin_point.x) return;
    if(y == begin_point.y) return;
    if(x < begin_point.x) swap(x, begin_point.x);
    if(y < begin_point.y) swap(y, begin_point.y);

    // (x,y), (width,height)
    cv::Mat roi_img(src_img, cv::Rect(begin_point.x, begin_point.y, x-begin_point.x, y-begin_point.y));
    cv::Mat dst_img(roi_img.rows*5.0, roi_img.cols*5.0, roi_img.type());
    cv::resize(roi_img, dst_img, dst_img.size(), cv::INTER_LANCZOS4);
    imshow("dst", dst_img);
  }
}

int main (int argc, char *argv[]) {
  src_img = imread(argv[1]);
  if(src_img.empty()) return -1;
  namedWindow("source",WINDOW_AUTOSIZE);
  imshow("source", src_img);
  setMouseCallback("source", mouse_callback, 0);
  waitKey();

  return 0;
}
