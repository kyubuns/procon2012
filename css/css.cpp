#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <queue>
#include <algorithm>
#include <ctime>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <sstream>
#include <string>
//ToDo: ポイント保持

using namespace std;
using namespace cv;
string itos(int i) {
  ostringstream s;
  s << i;
  return s.str();
}

const int window_width = 1280;
const int window_height = 700;
int image_index = 0;
cv::Mat src_img;
cv::Mat con_img;
enum size {
  small,
  medium,
  large
};
int weapon = large;
std::vector<std::pair<cv::Point,int>> point;
//std::map<int,std::vector<std::pair<cv::Point,int>>> memo;
int a[3] = {0};
time_t start_time;

//数えるだけ
void calc(const int s_, const int m_, const int l_) {
  std::cout << "---result---" << std::endl;
  std::cout << "small:  " << s_ << std::endl;
  std::cout << "medium: " << m_ << std::endl;
  std::cout << "large:  " << l_ << std::endl;
  std::cout << std::endl;
}

void draw() {
  //con
  if(con_img.data == NULL) return;
  cv::Mat copy = con_img.clone();
  for(auto &tmp : point) {
    cv::Point p = tmp.first;
    int k = tmp.second;
    cv::Scalar color(0,0,0);
    if(k==small) color = cv::Scalar(255,0,0);
    if(k==medium) color = cv::Scalar(0,255,0);
    if(k==large) color = cv::Scalar(0,0,255);
    cv::circle(copy, p, 6, color, 2);
  }
  imshow("con", copy);
  calc(a[small], a[medium], a[large]);

  //status
  cv::Mat status = cv::Mat::zeros(cv::Size(200, 100), CV_8UC3);
  cv::rectangle(status, cv::Point(0,0), cv::Point(200,100), cv::Scalar(255, 255, 255), -1);
  std::string w;
  if(weapon==small) w = "small";
  if(weapon==medium) w = "medium";
  if(weapon==large) w = "large";
  cv::Scalar color(0,0,0);
  if(weapon==small) color = cv::Scalar(255,0,0);
  if(weapon==medium) color = cv::Scalar(0,255,0);
  if(weapon==large) color = cv::Scalar(0,0,255);
  cv::putText(status, w.c_str(), cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 0.8, color, 2, CV_AA);

  cv::putText(status, "time: ", cv::Point(10, 80), cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(0,0,0), 1, CV_AA);
  time_t end_time;
  std::time(&end_time);
  time_t sec_time = std::difftime(end_time, start_time);
  std::string mes = itos(sec_time);
  cv::putText(status, mes.c_str(), cv::Point(50, 80), cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0,0,0), 1, CV_AA);
  imshow("status", status);
}

void con_mouse_callback(int event, int x, int y, int flag ,void*) {
  bool f = false;
  if(event==EVENT_LBUTTONDOWN||event==EVENT_MBUTTONDOWN||event==EVENT_RBUTTONDOWN) {
    point.push_back(std::make_pair(cv::Point(x,y), weapon));
    a[weapon]++;
    f = true;
  }

  if(f) draw();
}

void src_mouse_callback(int event, int x, int y, int flag ,void*) {
  std::string desc;
  static Point begin_point;
  if(event==EVENT_LBUTTONDOWN) {
    begin_point.x = x;
    begin_point.y = y;
  }
  else if(event==EVENT_LBUTTONUP) {
    if(x == begin_point.x) return;
    if(y == begin_point.y) return;
    if(x < begin_point.x) swap(x, begin_point.x);
    if(y < begin_point.y) swap(y, begin_point.y);

    // (x,y), (width,height)
    cv::Mat tmp_img(src_img, cv::Rect(begin_point.x, begin_point.y, x-begin_point.x, y-begin_point.y));
    double src_to_roi = std::min(static_cast<double>(window_width)/tmp_img.cols, static_cast<double>(window_height)/tmp_img.rows);
    con_img = cv::Mat(tmp_img.rows*src_to_roi, tmp_img.cols*src_to_roi, tmp_img.type());
    cv::resize(tmp_img, con_img, con_img.size(), cv::INTER_LANCZOS4);
    imshow("con", con_img);
    setMouseCallback("con", con_mouse_callback, 0);
    point.clear();
    std::time(&start_time);
  }
}

bool load_image(const std::string &path) {
  char a[128];
  sprintf(a, "%04d", image_index);
  Mat tmp_img = imread(path + "DSC_"+std::string(a)+".jpg");
  std::cout << path + "DSC_"+std::string(a)+".jpg" << std::endl;
  if(tmp_img.empty()) {
    std::cout << "load image error" << std::endl;
    return false;
  }

  //サイズ合わせる
  double t = std::min(static_cast<double>(window_width)/tmp_img.cols, static_cast<double>(window_height)/tmp_img.rows);
  src_img = Mat(tmp_img.rows*t, tmp_img.cols*t, tmp_img.type());
  cv::resize(tmp_img, src_img, src_img.size(), cv::INTER_LANCZOS4);
  //namedWindow("source",CV_WINDOW_NORMAL|CV_WINDOW_KEEPRATIO|CV_GUI_EXPANDED);
  //imshow("source", src_img);
  con_img = src_img;
  imshow("con", con_img);
  setMouseCallback("con", con_mouse_callback, 0);
  return true;
}

int main(int argc, char *argv[]) {
  if(argc!=3) {
    std::cout << "./css [loaddir] [no]" << std::endl;
    return 0;
  }
  image_index = atoi(argv[2]);
  if(load_image(argv[1]) == false) return -1;
  //setMouseCallback("source", src_mouse_callback, 0);


  while(1){
    int key = cv::waitKey(100);
    if(key == 'z' && point.empty() == false) {
      auto &tmp = *point.rbegin();
      cv::Point p = tmp.first;
      int k = tmp.second;
      point.pop_back();
      a[k]--;
    }
    if(key == 'v') {
      for(int i=0;i<1000;++i) {
        image_index++;
        if(image_index > 999) image_index = 0;
        if(load_image(argv[1]) == true) break;
      }
      point.clear();
    }
    if(key == 'c') {
      for(int i=0;i<1000;++i) {
        image_index--;
        if(image_index < 0) image_index = 999;
        if(load_image(argv[1]) == true) break;
      }
      point.clear();
    }
    if(key == '1') { weapon = large; }
    if(key == '2') { weapon = medium; }
    if(key == '3') { weapon = small; }
    draw();
  }

  return 0;
}
