#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

int
main(int argc, char *argv[])
{
	cv::Mat src_img = cv::imread(argv[1]);
	if(!src_img.data)return -1;

  if(src_img.empty()) return -1; 
  cv::Mat dst_img1(src_img.rows*10.0, src_img.cols*10.0, src_img.type());
  cv::resize(src_img, dst_img1, dst_img1.size(), cv::INTER_LANCZOS4);

  cv::namedWindow("resize image1", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
  cv::imwrite("kyuri.png", dst_img1);
  cv::imshow("resize image1", dst_img1);
  cv::waitKey(0);
}
