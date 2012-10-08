#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

const double src_to_roi = 4.0;    //src_img -> roi_imtの拡大率
const double roi_to_cnt = 5.0;    //roi_img -> cnt_imtの拡大率
const int div_x = 5;               //count_imgの分割数
const int div_y = 2;               //count_imgの分割数

Mat src_img;
Mat roi_img;
Point begin_point;

void show_count_window(int count) {
  const int x = count%div_x;
  const int y = count/div_x;
  const int width = roi_img.cols/div_x;
  const int height = roi_img.rows/div_y;
  Mat show_img = roi_img.clone();
  cv::rectangle(show_img, cv::Point(x*width,y*height), cv::Point((x+1)*width,(y+1)*height), cv::Scalar(0,0,200), 2, 4);
  imshow("roi", show_img);

  /*
  cv::Mat cnt_img(roi_img, cv::Rect(x*width, y*height, width, height));
  cv::Mat dst_img(width*roi_to_cnt, height*roi_to_cnt, roi_img.type());
  cv::resize(cnt_img, dst_img, dst_img.size(), cv::INTER_LANCZOS4);
  imshow("cnt", dst_img);
  */
}

void src_mouse_callback(int event, int x, int y, int flag ,void*) {
  std::string desc;
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
    cv::Mat dst_img(tmp_img.rows*src_to_roi, tmp_img.cols*src_to_roi, tmp_img.type());
    cv::resize(tmp_img, dst_img, dst_img.size(), cv::INTER_LANCZOS4);
    roi_img = dst_img.clone();
    show_count_window(0);
  }
}

enum size {
  small,
  medium,
  large
};

void calc(const int s_, const int m_, const int l_, const int total_weight) {
  const double weight[3] = { 0.274, 1.35, 5.902 };
  int raito[3] = { s_, m_, l_ };
  double raito_total = raito[small] * weight[small] + raito[medium] * weight[medium] + raito[large] * weight[large];
  if(raito_total == 0.0) {
    std::cout << "raito_total == 0" << std::endl;
    return;
  }
  int s = static_cast<int>(raito[small]*(total_weight/raito_total)+0.5) ;
  int m = static_cast<int>(raito[medium]*(total_weight/raito_total)+0.5);
  int l = static_cast<int>(raito[large]*(total_weight/raito_total)+0.5) ;
  double t = s * weight[small] + m * weight[medium] + l * weight[large];
  if(total_weight > t) {
    s += (total_weight - t)/weight[0];
    if(s<0) s=0;
    if(m<0) m=0;
    if(l<0) l=0;
  }
  t = s * weight[small] + m * weight[medium] + l * weight[large];
  std::cout << "---result---" << std::endl;
  std::cout << "small:  " << static_cast<int>(s) << " (" << s_ << ")" << std::endl;
  std::cout << "medium: " << static_cast<int>(m) << " (" << m_ << ")" << std::endl;
  std::cout << "large:  " << static_cast<int>(l) << " (" << l_ << ")" << std::endl;
  std::cout << "  total:" << t << "g" << " (" << total_weight << ")" << std::endl;
  std::cout << std::endl;
}

int main(int argc, char *argv[]) {
  src_img = imread(argv[1]);
  if(src_img.empty()) return -1;
  namedWindow("source",WINDOW_AUTOSIZE);
  imshow("source", src_img);
  setMouseCallback("source", src_mouse_callback, 0);

  int i = 0;
  int a[3] = {0};
  int weight = 100;
  while(1) {
    int key = cv::waitKey(0);
    if(key == 'z') a[small]++;
    else if(key == 'a') a[small]--;
    else if(key == 'x') a[medium]++;
    else if(key == 's') a[medium]--;
    else if(key == 'c') a[large]++;
    else if(key == 'd') a[large]--;
    else if(key == 'v') i++;
    else if(key == 'f') i--;
    /*
    else if(key == 'v') weight+=10;
    else if(key == 'f') weight-=10;
    else if(key == 'b') weight+=1;
    else if(key == 'g') weight-=1;
    */
    else if(key == 27/*esc*/) break;

    //画面更新
    if(i>=div_x*div_y)i=0;
    if(i<0)i=div_x*div_y-1;
    show_count_window(i);

    calc(a[small], a[medium], a[large], weight);
  }

  return 0;
}
