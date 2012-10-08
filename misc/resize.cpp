#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

cv::Mat resize(cv::Mat &src_img, double a) {
  cv::Mat dst_img(src_img.rows*a, src_img.cols*a, src_img.type());
  cv::resize(src_img, dst_img, dst_img.size(), cv::INTER_LANCZOS4);
  return dst_img;
}

/*
int main(int argc, char *argv[]) {
	cv::Mat src_img = cv::imread(argv[1]);
	if(!src_img.data)return -1;

  cv::imshow("resize image1", resize(src_img, 0.1));
  cv::waitKey(0);
}
*/
