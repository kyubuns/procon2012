#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <sstream>
#include <string>

using namespace std;
using namespace cv;

const int src_width = 1280;
int div_x = 5;            //count_imgの分割数
int div_y = 4;            //count_imgの分割数
int weight = 0;

const std::string msg_small  = "s";
const std::string msg_medium = "m";
const std::string msg_large  = "l";

enum size {
  small,
  medium,
  large
};

int a[3] = {0};
double speed = 0;
Mat src_img;
Mat roi_img;
Mat game_img;
Point begin_point;

inline int GetMatIdx(Mat img, int y, int x) {return y*img.cols*img.channels()+x*img.channels();}
std::vector<std::pair<int, std::string>> memo;

string itos(int i) {
  ostringstream s;
  s << i ;
  return s.str();
}

string dtos(double i) {
  char a[256];
  sprintf(a, "%.2f", i);
  return std::string(a);
}

void show_count_window(int &count, double &line_) {
  const int x = count%div_x;
  const int y = count/div_x;
  const int width = roi_img.cols/div_x;
  const int height = roi_img.rows/div_y;
  if(line_>width) {count++; line_ = 0; memo.clear();}
  int line = static_cast<int>(line_);
  Mat show_img = roi_img.clone();
  cv::rectangle(show_img, cv::Point(x*width, y*height), cv::Point((x+1)*width, (y+1)*height), cv::Scalar(0,0,200), 2, 4);
  cv::line(show_img, cv::Point(x*width+line, y*height), cv::Point(x*width+line, (y+1)*height), cv::Scalar(200,0,0), 2, 4);

  //score
  cv::Scalar score_color(255,0,0);
  double score_size = 0.5;
  cv::putText(show_img, "HI-SPEED "+dtos(speed), cv::Point(50,30), cv::FONT_HERSHEY_SIMPLEX, 0.6, score_color, 1, CV_AA);
  cv::putText(show_img, "large:"+itos(a[large]), cv::Point(50,50), cv::FONT_HERSHEY_SIMPLEX, score_size, score_color, 1, CV_AA);
  cv::putText(show_img, "medium:"+itos(a[medium]), cv::Point(50,70), cv::FONT_HERSHEY_SIMPLEX, score_size, score_color, 1, CV_AA);
  cv::putText(show_img, "small:"+itos(a[small]), cv::Point(50,90), cv::FONT_HERSHEY_SIMPLEX, score_size, score_color, 1, CV_AA);

  //memo
  int cnt = 1;
  for(auto tmp : memo) {
    int pos = tmp.first;
    std::string &msg = tmp.second;
    cv::Scalar color;
    if(msg==msg_small) color = cv::Scalar(50,150,50);
    if(msg==msg_medium) color = cv::Scalar(150,50,50);
    if(msg==msg_large) color = cv::Scalar(50,50,150);
    cv::putText(show_img, msg, cv::Point(x*width+pos, y*height+10*cnt), cv::FONT_HERSHEY_SIMPLEX, 0.3, color, 1, CV_AA);
    if(cnt>10) cnt=1;
  }

  namedWindow("roi",CV_WINDOW_NORMAL|CV_GUI_EXPANDED);
  imshow("roi", show_img);


  //=============================================
  /*
  Point p1(428, 52);
  Point p2(1201, 633);
  Mat tmp_img(p2.y-p1.y, p2.x-p1.x, show_img.type());
  cv::resize(show_img, tmp_img, tmp_img.size(), cv::INTER_LANCZOS4);
  imshow("tmp", tmp_img);

  for( int y = 0 ; y < tmp_img.rows; ++y ) {
    for( int x = 0 ; x < tmp_img.cols; ++x ) {
      game_img.data[GetMatIdx(game_img,y+p1.y,x+p1.x)] = tmp_img.data[GetMatIdx(tmp_img,y,x)];
      game_img.data[GetMatIdx(game_img,y+p1.y,x+p1.x)+1] = tmp_img.data[GetMatIdx(tmp_img,y,x)+1];
      game_img.data[GetMatIdx(game_img,y+p1.y,x+p1.x)+2] = tmp_img.data[GetMatIdx(tmp_img,y,x)+2];
    }
  }
  imshow("nidera", game_img);
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
    double src_to_roi = static_cast<double>(src_width)/tmp_img.cols;
    cv::Mat dst_img(tmp_img.rows*src_to_roi, tmp_img.cols*src_to_roi, tmp_img.type());
    cv::resize(tmp_img, dst_img, dst_img.size(), cv::INTER_LANCZOS4);
    roi_img = dst_img.clone();
  }
}

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
  if(argc!=3) {
    std::cout << "./torikoro [weight] [imgfile]" << std::endl;
    return 0;
  }
  Mat tmp_img = imread(argv[2]);
  if(tmp_img.empty()) return -1;
  weight = atoi(argv[1]);

  //いい感じに拡大/縮小する
  double t = static_cast<double>(src_width)/tmp_img.cols;
  cout << t << endl;
  src_img = Mat(tmp_img.rows*t, tmp_img.cols*t, tmp_img.type());
  cv::resize(tmp_img, src_img, src_img.size(), cv::INTER_LANCZOS4);
  //game_img = imread("misc/img/nidera.jpg");
  namedWindow("source",CV_WINDOW_NORMAL|CV_WINDOW_KEEPRATIO|CV_GUI_EXPANDED);
  imshow("source", src_img);
  setMouseCallback("source", src_mouse_callback, 0);

  double line = 0;
  int i = 0;
  while(1) {
    int key = cv::waitKey(30);
    if(roi_img.empty()) continue;
    if(key == 'b') { speed+=0.1; }
    if(key == 'g') { speed-=0.1; }
    if(speed == 0) {
      if(key == 'z') div_y--;
      if(key == 'a') div_y++;
      if(key == 'x') div_x--;
      if(key == 's') div_x++;
      if(key == 'r') break;
      if(key > 0) {
        if(div_x < 1) div_x = 1;
        if(div_y < 1) div_y = 1;
        i=0; line = 0; memo.clear(); a[0] = a[1] = a[2] = 0;
      }
    } else {
      if(key == 'z') { a[small]++; memo.push_back(make_pair(line, msg_small)); }
      if(key == 'a') a[small]--;
      if(key == 'x') { a[medium]++; memo.push_back(make_pair(line, msg_medium)); }
      if(key == 's') a[medium]--;
      if(key == 'c') { a[large]++; memo.push_back(make_pair(line, msg_large)); }
      if(key == 'd') a[large]--;
      if(key == 'v') { i++; line = -20; memo.clear(); }
      if(key == 'f') { i--; line = -20; memo.clear(); }
      if(key == 'r') { i=0; line = 0; memo.clear(); a[0] = a[1] = a[2] = 0; }
      if(key == 27/*esc*/) break;
    }

    //画面更新
    if(i>=div_x*div_y)i=0;
    if(i<0)i=div_x*div_y-1;
    if(speed<0) speed=0;
    line+=speed;
    show_count_window(i,line);

    calc(a[small], a[medium], a[large], weight);
  }

  return 0;
}

