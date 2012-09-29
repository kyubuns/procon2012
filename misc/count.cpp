#include <opencv2/core/core.hpp>
#include <iostream>
#include <vector>

enum size {
  small,
  medium,
  large
};

int main() {
  //サンプルデータ
  //点集合
  std::vector<std::vector<cv::Point> > points;
  {
    std::vector<cv::Point> a;
    a.push_back(cv::Point(0, 0));
    a.push_back(cv::Point(5, 5));
    a.push_back(cv::Point(0, 13));
    points.push_back(a);
  }

  //総重量(入力)
  double total_weight = 120;

  //長さ 要調整
  const double length[3] = { 100, 200, 300 };

  //========================================================================

  //重量
  const double weight[3] = { 0.274, 1.35, 5.902 };

  //比率
  double raito[3];
  raito[small]  = 0.0;
  raito[medium] = 0.0;
  raito[large]  = 0.0;

  for(auto& plane : points) {
    double max_len = 0.0;
    if(plane.size() == 1) continue;
    for(int i=0; i<plane.size(); ++i) {
      int next_id = (i==plane.size()-1)?0:i+1;
      cv::Point &p0 = plane[i];
      cv::Point &p1 = plane[next_id];

      double tmp = (p0.x-p1.x)*(p0.x-p1.x)+(p0.y-p1.y)*(p0.y-p1.y);
      max_len = std::max(tmp, max_len);
      std::cout << i << "," << next_id << "=" << tmp << std::endl;
    }
    std::cout << "max:" << max_len << std::endl;
    for(int i=0;i<3;++i) {
      if(max_len > length[i]) continue;
      std::cout << i << std::endl;
      raito[i] += 1.0;
      break;
    }
    std::cout << "---" << std::endl;
  }

  double raito_total = raito[small] * weight[small] + raito[medium] * weight[medium] + raito[large] * weight[large];
  if(raito_total == 0.0) {
    std::cout << "raito_total == 0" << std::endl;
    return -1;
  }
  std::cout << "small:  " << static_cast<int>(raito[small]*(total_weight/raito_total)+0.5) << std::endl;
  std::cout << "medium: " << static_cast<int>(raito[medium]*(total_weight/raito_total)+0.5) << std::endl;
  std::cout << "large:  " << static_cast<int>(raito[large]*(total_weight/raito_total)+0.5) << std::endl;

  return 0;
}
