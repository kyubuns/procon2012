#include <cv.h>
#include <highgui.h>

using namespace cv;
inline int GetMatIdx(cv::Mat img, int y, int x) {return y*img.step+x*img.channels();}

int main (int argc, char **argv) {
  Mat src_img_ = imread(argv[1]);
  if(!src_img_.data) return -1;

  //=======================================
  cv::Mat src_img = src_img_.clone();
  for(int i = 0 ; i < src_img.rows ; i++){
    for(int j = 0 ; j < src_img.cols ; j++){
      int id = GetMatIdx(src_img,i,j);
      int total = src_img.data[id+0] + src_img.data[id+1] + src_img.data[id+2];
      int r = src_img.data[id+2];
      int g = src_img.data[id+1];
      int b = src_img.data[id+0];
      if( r > 100 && r > g*1.5 && r > b*1.5 ) {
        src_img.data[id+0] = 255;
        src_img.data[id+1] = 255;
        src_img.data[id+2] = 255;
        src_img_.data[id+0] = 255;
        src_img_.data[id+1] = 0;
        src_img_.data[id+2] = 0;
      }
      else {
        src_img.data[id+0] = 0;
        src_img.data[id+1] = 0;
        src_img.data[id+2] = 0;
      }
    }
  }
  //=======================================

  Mat canny_img;
  Canny(src_img, canny_img, 50, 200);

  namedWindow("Original", CV_WINDOW_AUTOSIZE);
  namedWindow("Canny", CV_WINDOW_AUTOSIZE);
  imshow("Original", src_img);
  imshow("hoge", src_img_);
  imshow("Canny", canny_img);
  waitKey(0);

  return 0;
}
