#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <cstdlib>
#include <list>
#include <cmath>
#include <queue>
#include <stdexcept>
#include "count.hpp"



#include <vector>
#include <cmath>
#include <queue>
#include <ctime>
#include <cassert>
#define ___ROUND(x) ((int)(x+0.5))
#define GetMatIdx(img,y,x) (y*img.cols*img.channels()+x*img.channels())

class kyuri {
public:
  static void main(cv::Mat src_img_, cv::Mat canny_img, cv::Mat erasebg_img, double total_weight) {
    if(canny_img.channels() != 1) throw std::runtime_error("kyuri: channels() != 1");
    cv::Mat src_img = src_img_.clone();

    std::vector<std::vector<cv::Point> > contours;
    cv::findContours(canny_img, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
    cv::namedWindow("bef", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
    cv::imshow("bef", canny_img);
    CannyFix(canny_img);
    /*for(int i = 0; i < contours.size(); ++i) {
      size_t count = contours[i].size();
      if(count < 10 || count > 100) continue; // （小さすぎる|大きすぎる）輪郭を除外

      cv::Mat pointsf;
      cv::Mat(contours[i]).convertTo(pointsf, CV_32F);
    // 楕円フィッティング
    cv::RotatedRect box = cv::fitEllipse(pointsf);
    // 楕円の描画
    cv::ellipse(canny_img, box, cv::Scalar(0,0,0), 4, 2);
    //rrects.push_back(box);
    //std::cout << i << " : " << box.center.x << " " << box.center.y << " | " << box.size.width << " " << box.size.height << " | " << box.angle << std::endl;	
    }*/

    cv::vector< cv::Point > middles;
    std::vector<double> radiuses;
    for(int i = 0 ; i < contours.size() ; i++){
      size_t count = contours[i].size();
      if(count < 10 || count > 100) continue; // （小さすぎる|大きすぎる）輪郭を除外

      double x = 0;
      double y = 0;
      for(int j = 0 ; j < contours[i].size() ; j++){
        x += contours[i][j].x;
        y += contours[i][j].y;
        //middles.push_back(cv::Point(x,y));
      }
      x /= contours[i].size();
      y /= contours[i].size();

      //背景か確認
      int index = GetMatIdx(erasebg_img, static_cast<int>(y), static_cast<int>(x));
      if(index<0 || index+2>=erasebg_img.cols*erasebg_img.rows*erasebg_img.channels()) continue;
      int b = static_cast<int>(erasebg_img.data[index]);
      int g = static_cast<int>(erasebg_img.data[index+1]);
      int r = static_cast<int>(erasebg_img.data[index+2]);
      if(b==255 && g==0 && r==0) continue;

      double rad = 0;
      for(int j = 0 ; j < contours[i].size() ; j++){
        rad = std::max( (x-contours[i][j].x)*(x-contours[i][j].x)+(y-contours[i][j].y)*(y-contours[i][j].y) , rad); 
        //middles.push_back(cv::Point(x,y));
      }
      radiuses.push_back(sqrt(rad));
      middles.push_back(cv::Point(x,y));
    }
    // sort 
    for(int i =0 ; i < middles.size() ; i++){
      for(int j = 1 ; j < middles.size() ; j++){
        if( middles[j-1].x > middles[j].x ){
          std::swap(middles[j-1],middles[j]);
          std::swap(radiuses[j-1],radiuses[j]);
        }
      }
    }
    //std::cout << dx << " " << dy << " " << " " << loopCounter << " [" << edgeData.channels() << std::endl;
    std::vector< std::vector<cv::Point> > vp = clustering(canny_img,middles,radiuses);

    cv::namedWindow("Canny", CV_WINDOW_AUTOSIZE|CV_WINDOW_FREERATIO);
    cv::imshow("Canny", canny_img);

    for(int i = 0 ; i < vp.size() ; i++){
      cv::Scalar s = cv::Scalar(rand()%256,rand()%256,rand()%256);
      for(int j = 0 ; j < vp[i].size() ; j++){
        cv::circle(src_img, vp[i][j], 2, s, 3, 4);
      }
      for(int j = 0 ; j < vp[i].size() ; j++){
        for(int k = j+1 ; k < vp[i].size() ; k++){
          cv::line(src_img, vp[i][j], vp[i][k], s, 1);
        }
      }
    }
    cv::imshow("fit ellipse", src_img);
    count::main(vp, total_weight);
    cv::waitKey(0);
  }

private:
  static void CannyFix(cv::Mat &src){
    static const double PI = acos(-1);
    using namespace std;
    const int dx[] = {-1,0,1,-1,1,-1,0,1};
    const int dy[] = {-1,-1,-1,0,0,1,1,1};
    cv::Mat cpy = src.clone();
    for(int i = 0 ; i < src.rows ; i++){
      for(int j = 0 ; j < src.cols ; j++){
        if( src.data[i*src.step+j] > 0 ){
          vector<int> u;
          queue<int> Q;
          Q.push(i*src.step+j);
          src.data[i*src.step+j] = 0;

          while(Q.size()){
            int q = Q.front(); Q.pop();
            int y = q / src.step;
            int x = q % src.step;
            //cout << x << "," << y << " " << src.cols << " " << src.rows << endl;
            u.push_back(q);
            for(int d = 0 ; d < 8 ; d++){
              int tx = x + dx[d];
              int ty = y + dy[d];
              if( tx >= 0 && tx < src.step && ty >= 0 && ty < src.rows && src.data[ ty * src.step + tx ] > 0 ){
                src.data[ ty * src.step + tx ] = 0;
                Q.push(ty*src.step+tx);
              }
            }
          }
          if( u.size() <= 20 ){
            for(int k = 0 ; k < u.size() ; k++){
              cpy.data[u[k]] = 0;
            }
            continue;
          }

          int	sumx = 0 , sumy = 0;
          for(int k = 0 ; k < u.size() ; k++){
            int y = u[k] / src.step;
            int x = u[k] % src.step;
            sumx += x;
            sumy += y;
          }
          sumx /= u.size();
          sumy /= u.size();

          int maxdist = 0;
          for(int k = 0 ; k < u.size() ; k++){
            int y = u[k] / src.step;
            int x = u[k] % src.step;
            maxdist = max( maxdist, (x-sumx)*(x-sumx)+(y-sumy)*(y-sumy) );
          }
          double len = 2 * sqrt(1.0*maxdist) * PI;
          //cout << u.size() << " " << len << " " <<  ( len - u.size() )  / len << endl;

          if( fabs( len - u.size() )  / len < 0.4	 ){
            for(int k = 0 ; k < u.size() ; k++){
              cpy.data[u[k]] = 0;
            }
          }
        }

      }
    }
    src = cpy.clone();
  }

  static bool isProbableConnecting(const cv::Mat &edgeData,const cv::Point &a,const cv::Point &b,const double &ra,const double &rb){
    if( (a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y) < 1e-3 ) return true; // for same petals
    if( fabs(ra-rb) / (rb) > 0.3) return false;
    //std::cout <<  sqrt( (a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y) - ra - rb  << std::endl;
    //if( sqrt( (a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y) ) - ra - rb > 3*std::min(ra,rb) ) return false;
    double dx = ( b.x - a.x );
    double dy = ( b.y - a.y );
    int loopCounter;
    if( abs(dy) < abs(dx) ){
      loopCounter = abs(dx);
      dy /= abs(dx);
      dx /= abs(dx);
    }else{
      loopCounter = abs(dy);
      dx /= abs(dy);
      dy /= abs(dy);
    }


    double cx = a.x;
    double cy = a.y;

    int cx_int = ___ROUND(cx);
    int cy_int = ___ROUND(cy);
    int A = a.y < b.y ? a.y : b.y;
    int B = a.y > b.y ? a.y : b.y;
    int C = a.x < b.x ? a.y : b.x;
    int D = a.x > b.x ? a.y : b.x;

    int k = 2;
    int p = 0;
    //std::cout << std::endl;
    while(loopCounter--){
      cx += dx;
      cy += dy;
      //std::cout <<  ___ROUND(cy) << " " <<  ___ROUND(cx) << "[" << ___ROUND(cy)*edgeData.cols+___ROUND(cx) << "]" << std::endl;
      if( edgeData.data[___ROUND(cy)*edgeData.cols+___ROUND(cx)] ){
        if( p == 0 ) k--;
        if(!k)return false;
        p = 1;
      }else{
        p = 0;
      }
    }
    /*std::cout << "(" << a.x << "," << a.y << ") - ("<< b.x << "," << b.y << ")" << std::endl;
      for(int i = A ; i <= B ; i++){
      for(int j = C ; j <= D ; j++){
      std::cout << (cpy.data[___ROUND(i)*edgeData.cols+___ROUND(j)]!=255?cpy.data[___ROUND(i)*edgeData.cols+___ROUND(j)]:2);
      }
      std::cout << std::endl;
      }*/
    return true;
  }
  // O(n^2) n := number of petals.
  static std::vector< std::vector<cv::Point> > clustering(const cv::Mat &edgeData,const std::vector<cv::Point> &petals,const std::vector<double> &radiuses){
    std::vector< std::vector<cv::Point> > answer;
    std::vector< std::vector<double> > answer_r;


    std::vector<bool> used(petals.size(),false);
    for(int i = 0 ; i < petals.size() ; i++){
      if( used[i] ) continue;
      used[i] = true;
      answer.push_back(std::vector<cv::Point>());
      answer_r.push_back(std::vector<double>());
      answer.back().push_back(petals[i]);
      answer_r.back().push_back(radiuses[i]);

      for(int j = i + 1 ; j < petals.size() ; j++){
        if( used[j] ) continue;
        int ps = answer.back().size();
        bool flag = true;
        double minDist = 1e9, maxDist = -1e9;

        for(int k = 0 ; k < answer.back().size() ; k++){
          if( !isProbableConnecting(edgeData,answer.back()[k],petals[j],answer_r.back()[k],radiuses[j]) ){
            flag = false;
            break;
          }
          double ra = answer_r.back()[k];
          double rb = radiuses[j];
          cv::Point a = answer.back()[k];
          cv::Point b = petals[j];

          minDist = std::min(minDist,sqrt( (a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y) ) - ra - rb);
          maxDist = std::max(maxDist,sqrt( (a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y) ) - ra - rb);

        }
        bool flag2 = maxDist < 4*answer_r.back()[0] && minDist < 2.5*answer_r.back()[0];
        if( flag && flag2){
          answer.back().push_back(petals[j]);
          answer_r.back().push_back(radiuses[j]);

          used[j] = true;
          if( answer.back().size() >= 6 ) break;
        }
      }
      //std::cout << i << std::endl;
    }
    return answer;
  }
};
