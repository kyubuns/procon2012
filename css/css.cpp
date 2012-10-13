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
std::map<int,std::vector<std::pair<cv::Point,int>>> memo;
int aaaa[3] = {0};
time_t start_time;

//数えるだけ
void calc(const int s_, const int m_, const int l_, bool force = false) {
  static int bs = -1;
  static int bm = -1;
  static int bl = -1;
  if(!force&&bs==s_&&bm==m_&&bl==l_) return;
  std::cout << "---result---" << std::endl;
  std::cout << "large:  " << l_ << std::endl;
  std::cout << "medium: " << m_ << std::endl;
  std::cout << "small:  " << s_ << std::endl;
  bs = s_;
  bm = m_;
  bl = l_;
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
    if(k!=weapon) continue;
    cv::circle(copy, p, 6, color, 2);
  }
  imshow("con", copy);
  calc(aaaa[small], aaaa[medium], aaaa[large]);

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

void hoge() {
  const double EPS = 10;
  double weight[] = {5.710,1.328,0.277};

  double total_weight;
  cout << "total weight? :";
  cin >> total_weight;
  int a,b,c;
  //cout << "Large Medium Small:";
  //cin >> a >> b >> c;
  //cout << endl;
  a = aaaa[large];
  b = aaaa[medium];
  c = aaaa[small];

  double ra = 1.0 * a / (a+b+c);
  double rb = 1.0 * b / (a+b+c);
  double rc = 1.0 * c / (a+b+c);

  {

    vector< pair< pair<double,int> , pair<int,int> > > v;
    for(int i = 0 ; i < 10000 ; i++){
      if( i * weight[0] > total_weight + EPS ) continue;
      for(int j = 0 ; j < 10000 ; j++){
        int kc = (int)(( total_weight - i * weight[0] - j * weight[1] ) / weight[2]);

        if( kc < 0 ) break;
        if( kc >= 10000) continue;
        for(int k = max(0,kc-1) ; k <= min(10000,kc+1) ; k++){
          if( fabs( total_weight - i * weight[0] - j * weight[1] - k * weight[2] ) < EPS ){
            double a = 1.0 * i / (i+j+k);
            double b = 1.0 * j / (i+j+k);
            double c = 1.0 * k / (i+j+k);
            v.push_back(make_pair(make_pair(fabs(ra-a) + fabs(rb-b) + fabs(rc-c),i),make_pair(j,k)));
          }
        }
      }
    }
    //63 132 72 115 120 80
    sort(v.begin(),v.end());
    cout << "====[1]==== weight:" << total_weight << endl;
    for(int i =0 ; i < min<int>(10,v.size()) ; i++){
      double wei = v[i].first.second*weight[0]+v[i].second.first*weight[1]+v[i].second.second*weight[2];
      cout <<v[i].first.first << "("<< v[i].first.second << "," << v[i].second.first <<"," << v[i].second.second <<")" << " " << wei << endl;
    }

  }

  cout << endl;

  {
    int x[3],y[3];

    cout << "Large's range  [a,b] : "; cin >> x[0] >> y[0];
    cout << "Medium's range [a,b] : "; cin >> x[1] >> y[1];
    cout << "Small's range  [a,b] : "; cin >> x[2] >> y[2];
    cout << endl;
    vector< pair< pair<double,int> , pair<int,int> > > v;
    for(int i = 0 ; i < 10000 ; i++){
      if( i * weight[0] > total_weight + EPS ) continue;
      for(int j = 0 ; j < 10000 ; j++){
        int kc = (int)(( total_weight - i * weight[0] - j * weight[1] ) / weight[2]);

        if( kc < 0 ) break;
        if( kc >= 10000) continue;
        for(int k = max(0,kc-1) ; k <= min(10000,kc+1) ; k++){
          if( !(x[0] <= i && i <= y[0]) || !(x[1] <= j && j <= y[1]) || !(x[2] <= k && k <= y[2]) ){
            continue;
          }
          double a = 1.0 * i / (i+j+k);
          double b = 1.0 * j / (i+j+k);
          double c = 1.0 * k / (i+j+k);
          double w = 0;//fabs(ra-a) + fabs(rb-b) + fabs(rc-c);

          if( fabs( total_weight - i * weight[0] - j * weight[1] - k * weight[2] ) < EPS ){
            v.push_back(make_pair(make_pair(w+fabs( total_weight - i * weight[0] - j * weight[1] - k * weight[2]),i),make_pair(j,k)));
          }
        }
      }
    }
    sort(v.begin(),v.end());

    cout << "====[2]====" << endl;
    for(int i =0 ; i < min<int>(20,v.size()) ; i++){
      double wei = v[i].first.second*weight[0]+v[i].second.first*weight[1]+v[i].second.second*weight[2];
      cout <<v[i].first.first << "("<< v[i].first.second << "," << v[i].second.first <<"," << v[i].second.second <<")" << " " << wei << endl;
    }
  }

  cout << endl;        cout << endl;        cout << endl;
  calc(aaaa[small], aaaa[medium], aaaa[large], true);
}

void con_mouse_callback(int event, int x, int y, int flag ,void*) {
  bool f = false;
  if(event==EVENT_LBUTTONDOWN||event==EVENT_MBUTTONDOWN||event==EVENT_RBUTTONDOWN) {
    point.push_back(std::make_pair(cv::Point(x,y), weapon));
    aaaa[weapon]++;
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
  std::cout << "load image: " << path + "DSC_"+std::string(a)+".jpg" << std::endl;
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
  if(argc!=2) {
    std::cout << "./css [loaddir]" << std::endl;
    return 0;
  }
  image_index = 0;
  for(int i=0;i<1000;++i) {
    image_index++;
    if(image_index > 999) image_index = 0;
    if(load_image(argv[1]) == true) break;
  }
  //setMouseCallback("source", src_mouse_callback, 0);
  std::time(&start_time);


  while(1){
    int key = cv::waitKey(100);
    if(key == 'z' && point.empty() == false) {
      auto &tmp = *point.rbegin();
      cv::Point p = tmp.first;
      int k = tmp.second;
      point.pop_back();
      aaaa[k]--;
    }
    if(key == 'v') {
      memo[image_index] = point;
      for(int i=0;i<1000;++i) {
        image_index++;
        if(image_index > 999) image_index = 0;
        if(load_image(argv[1]) == true) break;
      }
      point.clear();
      point = memo[image_index];
    }
    if(key == 'c') {
      memo[image_index] = point;
      for(int i=0;i<1000;++i) {
        image_index--;
        if(image_index < 0) image_index = 999;
        if(load_image(argv[1]) == true) break;
      }
      point.clear();
      point = memo[image_index];
    }
    if(key == '1') { weapon = large; }
    if(key == '2') { weapon = medium; }
    if(key == '3') { weapon = small; }
    if(key == 13) { hoge(); }
    draw();
  }

  return 0;
}
