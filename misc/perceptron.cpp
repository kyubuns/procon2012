#include <iostream>
#include <algorithm>
#include <array>
#include <cmath>

template<int T>
class perceptron {
public:
  perceptron() { for(int i=0; i<T; ++i) w[i] = 0.0; }
  bool test(std::array<double, T> x) { return (dot(w, x)>=0); }
  void learn(std::array<double, T> x, bool y_) {
    if(test(x) == y_) return;
    int y = (y_?1:-1);

    //wを更新
    std::array<double, T> tmp;
    std::array<double, T> n(normalize(x));
    for(int i=0; i<T; ++i) tmp[i] = w[i] + y * n[i];
    w = tmp;
  }

  double dot(const std::array<double, T> &a, const std::array<double, T> &b) {
    double tmp;
    for(int i=0; i<T; ++i) tmp += a[i]*b[i];
    return tmp;
  }

  std::array<double, T> normalize(std::array<double, T> a) {
    double len = 0.0;
    for(int i=0; i<T; ++i) len += a[i]*a[i];
    len = std::sqrt(len);

    for(int i=0; i<T; ++i) a[i] = a[i]/len;
    return a;
  }

private:
  std::array<double, T> w;
};

int main() {
  perceptron<2> p;
  p.learn(std::array<double, 2>{-0.75, 0.9}, true);
  p.learn(std::array<double, 2>{-0.77, 0.6}, true);
  p.learn(std::array<double, 2>{-0.53, 0.65}, true);
  p.learn(std::array<double, 2>{-0.4, 0.75}, true);
  p.learn(std::array<double, 2>{-0.1, -0.25}, true);
  p.learn(std::array<double, 2>{0.25, 0.75}, false);
  p.learn(std::array<double, 2>{0.4, -0.5}, false);
  p.learn(std::array<double, 2>{0.45, 0.3}, false);
  p.learn(std::array<double, 2>{0.65, -0.9}, false);
  p.learn(std::array<double, 2>{0.95, 0.9}, false);
  std::cout << p.test(std::array<double, 2>{-0.8, -0.8}) << std::endl;
  std::cout << p.test(std::array<double, 2>{0.8, 0.8}) << std::endl;
  return 0;
}
