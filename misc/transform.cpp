#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

const static int rev = 50;

int main(int argc, char *argv[]) {
  cv::Mat src_img = cv::imread("img/sample.jpg", 1);
  if(src_img.empty()) return -1;

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

  // 変換前後の座標を描画
  cv::line(src_img, pts1[0], pts1[1], cv::Scalar(255,255,0), 2, CV_AA);
  cv::line(src_img, pts1[1], pts1[2], cv::Scalar(255,255,0), 2, CV_AA);
  cv::line(src_img, pts1[2], pts1[3], cv::Scalar(255,255,0), 2, CV_AA);
  cv::line(src_img, pts1[3], pts1[0], cv::Scalar(255,255,0), 2, CV_AA);

  cv::line(src_img, pts2[0], pts2[1], cv::Scalar(255,0,255), 2, CV_AA);
  cv::line(src_img, pts2[1], pts2[2], cv::Scalar(255,0,255), 2, CV_AA);
  cv::line(src_img, pts2[2], pts2[3], cv::Scalar(255,0,255), 2, CV_AA);
  cv::line(src_img, pts2[3], pts2[0], cv::Scalar(255,0,255), 2, CV_AA);

  cv::namedWindow("src", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
  cv::namedWindow("dst", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
  cv::imshow("src", src_img);
  cv::imshow("dst", dst_img);
  cv::waitKey(0);
}
