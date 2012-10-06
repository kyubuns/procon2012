#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/calib3d/calib3d.hpp>

int main()
{
  // モザイキングする画像
  std::string fn1 = "image0.png";
  std::string fn2 = "image1.png";

  // 画像読み込み（グレースケールで読み込み）
  cv::Mat_< unsigned char > image1 = cv::imread( fn1, 0 );
  cv::Mat_< unsigned char > image2 = cv::imread( fn2, 0 );

  // 特徴点の検出
  cv::SurfFeatureDetector detector;
  std::vector< cv::KeyPoint > keys1, keys2;
  detector.detect( image1, keys1 );
  detector.detect( image2, keys2 );

  // 特徴量の記述
  cv::SurfDescriptorExtractor extractor;
  cv::Mat desp1, desp2;
  extractor.compute( image1, keys1, desp1 );
  extractor.compute( image2, keys2, desp2 );

  // 特徴点の対応付け
  std::vector< cv::DMatch > matches;
  cv::FlannBasedMatcher matcher;
  matcher.match( desp1, desp2, matches );

  // 対応点をstd::vectorに格納
  std::vector< cv::Vec2f > points1( matches.size() );
  std::vector< cv::Vec2f > points2( matches.size() );

  for( size_t i = 0 ; i < matches.size() ; ++i )
  {
    points1[ i ][ 0 ] = keys1[ matches[ i ].queryIdx ].pt.x;
    points1[ i ][ 1 ] = keys1[ matches[ i ].queryIdx ].pt.y;

    points2[ i ][ 0 ] = keys2[ matches[ i ].trainIdx ].pt.x;
    points2[ i ][ 1 ] = keys2[ matches[ i ].trainIdx ].pt.y;
  }

  // 画像間の平面射影変換行列を取得
  cv::Mat_< float > homography = cv::findHomography( points1, points2, CV_RANSAC );

  // 画像の読み込み（RGB画像で読み込み直し）
  cv::Mat_< cv::Vec3b > rgb_image1 = cv::imread( fn1, 1 );
  cv::Mat_< cv::Vec3b > rgb_image2 = cv::imread( fn2, 1 );

  // 画像1を画像2の空間に投影
  // 出力画像の大きさは適当に決定
  cv::Mat_< cv::Vec3b > result;
  cv::warpPerspective( rgb_image1, result, homography, cv::Size( static_cast<int>( rgb_image1.cols * 1.5 ), rgb_image1.rows ) );

  // 画像2を結果画像にコピー
  // 合成方法は上書き
  for( int y = 0 ; y < rgb_image2.rows ; ++y )
  {
    for( int x = 0 ; x < rgb_image2.cols ; ++x )
    {
      result( y, x ) = rgb_image2( y, x );
    }
  }

  // モザイキング結果を表示
  cv::imshow( "Mosaicing", result );
    imwrite("test.png", result);
  cv::waitKey();

  return 0;
}
