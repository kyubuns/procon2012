#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

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

int main (int argc, char *argv[]) {
  cv::Mat src_img = cv::imread(argv[1]);
  if(src_img.empty()) return -1;

  cv::imshow("test", src_img);
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
    else if(key == 'v') weight+=10;
    else if(key == 'f') weight-=10;
    else if(key == 'b') weight+=1;
    else if(key == 'g') weight-=1;
    calc(a[small], a[medium], a[large], weight);
  }
  return 0;
}


