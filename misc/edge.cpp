#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

int
main (int argc, char *argv[])
{
  // グレースケール画像として読み込む
  cv::Mat src_img = cv::imread("img/sample.jpg", 0);
  if(src_img.empty()) return -1; 
  
  // Canny
  cv::Mat canny_img;
  cv::Canny(src_img, canny_img, 50, 200);

  cv::namedWindow("Original(Grayscale)", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
  cv::namedWindow("Canny", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
  cv::imshow("Original(Grayscale)", src_img);
  cv::imshow("Canny", canny_img);
  cv::waitKey(0);
}
