#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <list>
#include <cmath>

int main(int argc, char *argv[])
{
  cv::Mat src_img = cv::imread("img/sample.jpg", 1);
  if(!src_img.data) return -1;

  cv::Mat gray_img, bin_img;
  cv::cvtColor(src_img, gray_img, CV_BGR2GRAY);

  std::vector<std::vector<cv::Point> > contours;
  // 画像の二値化
  cv::threshold(gray_img, bin_img, 0, 255, cv::THRESH_BINARY|cv::THRESH_OTSU);
  // 輪郭の検出
  cv::findContours(bin_img, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
  
  std::vector<cv::RotatedRect> rrects;
  for(int i = 0; i < contours.size(); ++i) {
    size_t count = contours[i].size();
    if(count < 10 || count > 100) continue; // （小さすぎる|大きすぎる）輪郭を除外

    cv::Mat pointsf;
    cv::Mat(contours[i]).convertTo(pointsf, CV_32F);
    // 楕円フィッティング
    cv::RotatedRect box = cv::fitEllipse(pointsf);
    // 楕円の描画
    cv::ellipse(src_img, box, cv::Scalar(255,0,count*2), 1, CV_AA);
    rrects.push_back(box);
    std::cout << i << " : " << box.center.x << " " << box.center.y << " | " << box.size.width << " " << box.size.height << " | " << box.angle << std::endl;
  }
  std::cout << "=============" << std::endl;
  for(int i=0;i<rrects.size();++i) {
    auto &rect = rrects[i];
    int index = 0;
    int index2 = 0;
    int index3 = 0;
    cv::Point point = rect.center;
    double b = INT_MAX;
    double b2 = INT_MAX;
    double b3 = INT_MAX;
    bool flag = false;
    for(int j=0;j<rrects.size();++j) {
      if(i == j) continue;
      auto &r = rrects[j];
      cv::Point c = r.center;
      double tmp = (point.x-c.x)*(point.x-c.x)+(point.y-c.y)*(point.y-c.y);
      if(b > tmp) {
        index = j;
        b = tmp;
        continue;
      }
      if(b2 > tmp) {
        index2 = j;
        b2 = tmp;
        continue;
      }
      if(b3 > tmp) {
        index3 = j;
        b3 = tmp;
        continue;
      }
    }

    static double wall = 2.0;
    static double wall2 = 300.0;
    std::cout << rrects[i].size.width << ", " << rrects[i].size.height << " : " << rrects[i].angle << std::endl;
    std::cout << rrects[index].size.width << ", " << rrects[index].size.height << " : " << rrects[index].angle << std::endl;
    double tmp = fabs(rrects[i].size.width - rrects[index].size.width) + fabs(rrects[i].size.height - rrects[index].size.height)/* + fabs(rrects[i].angle - rrects[index].angle)*/;
    std::cout << tmp << std::endl;
    if(tmp < wall) flag = true;
    else flag = false;


    point = rrects[i].center;
    cv::Point c = rrects[index].center;
    tmp = (point.x-c.x)*(point.x-c.x)+(point.y-c.y)*(point.y-c.y);
    if(flag) std::cout << "-" << tmp << std::endl;
    if(tmp < wall2) {
      if(flag) cv::line(src_img, point, rrects[index].center, cv::Scalar(255, 0, 255), 2);
      else cv::line(src_img, point, rrects[index].center, cv::Scalar(255, 0, 0), 1);
    }
    std::cout << "-" << std::endl;








    std::cout << rrects[i].size.width << ", " << rrects[i].size.height << " : " << rrects[i].angle << std::endl;
    std::cout << rrects[index2].size.width << ", " << rrects[index2].size.height << " : " << rrects[index2].angle << std::endl;
    tmp = fabs(rrects[i].size.width - rrects[index2].size.width) + fabs(rrects[i].size.height - rrects[index2].size.height)/* + fabs(rrects[i].angle - rrects[index2].angle)*/;
    std::cout << tmp << std::endl;
    if(tmp < wall) flag = true;
    else flag = false;

    point = rrects[i].center;
    c = rrects[index2].center;
    tmp = (point.x-c.x)*(point.x-c.x)+(point.y-c.y)*(point.y-c.y);
    if(flag) std::cout << "-" << tmp << std::endl;
    if(tmp < wall2) {
      if(flag) cv::line(src_img, point, rrects[index2].center, cv::Scalar(255, 0, 255), 2);
      else cv::line(src_img, point, rrects[index2].center, cv::Scalar(0, 255, 0), 1);
    }
    std::cout << "-" << std::endl;










    std::cout << rrects[i].size.width << ", " << rrects[i].size.height << " : " << rrects[i].angle << std::endl;
    std::cout << rrects[index3].size.width << ", " << rrects[index3].size.height << " : " << rrects[index3].angle << std::endl;
    tmp = fabs(rrects[i].size.width - rrects[index3].size.width) + fabs(rrects[i].size.height - rrects[index3].size.height)/* + fabs(rrects[i].angle - rrects[index3].angle)*/;
    std::cout << tmp << std::endl;
    if(tmp < wall) flag = true;
    else flag = false;


    point = rrects[i].center;
    c = rrects[index3].center;
    tmp = (point.x-c.x)*(point.x-c.x)+(point.y-c.y)*(point.y-c.y);
    if(flag) std::cout << "-" << tmp << std::endl;
    if(tmp < wall2) {
      if(flag) cv::line(src_img, point, rrects[index3].center, cv::Scalar(255, 0, 255), 2);
      else cv::line(src_img, point, rrects[index3].center, cv::Scalar(0, 0, 255),1);
    }
    std::cout << "----" << std::endl;
  }

  cv::namedWindow("fit ellipse", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
  cv::imshow("fit ellipse", src_img);
  cv::waitKey(0);
}
