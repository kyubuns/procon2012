#include "cv.h"
#include "highgui.h"

using namespace cv;

int main(int, char**) {
  VideoCapture cap(0); // デフォルトカメラをオープン
  if(!cap.isOpened())  // 成功したかどうかをチェック
    return -1;

  Mat edges;
  namedWindow("edges",1);
  for(;;)
  {
    Mat frame;
    cap >> frame; // カメラから新しいフレームを取得
    //GaussianBlur(edges, edges, Size(7,7), 1.5, 1.5);
    //Canny(edges, edges, 0, 30, 3);
    //imshow("edges", edges);
    imwrite("test.png", frame);
    break;
  }
  // VideoCapture デストラクタにより，カメラは自動的に終了処理されます
  return 0;
}
