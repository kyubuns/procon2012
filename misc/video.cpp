#include "cv.h"
#include "highgui.h"

using namespace cv;

int main(int, char**) {
  VideoCapture cap(0); // デフォルトカメラをオープン
  if(!cap.isOpened())  // 成功したかどうかをチェック
    return -1;

  Mat edges;
  namedWindow("edges",1);
  for(;;)
  {
    Mat frame;
    cap >> frame;
    Mat dst_img(frame.rows/3, frame.cols/3, frame.type());
    resize(frame, dst_img, dst_img.size(), cv::INTER_LANCZOS4);
    imwrite("test.png", dst_img);
    break;
  }
  return 0;
}
