#include<iostream>
#include<opencv2/core/core.hpp>
using namespace std;

#define OPENCV_VERSION(a,b,c) (((a)<<16)+((b)<<8)+(c))
#define OPENCV_VERSION_CODE OPENCV_VERSION(CV_MAJOR_VERSION,CV_MINOR_VERSION,CV_SUBMINOR_VERSION)

int main(){
	cout<<"version:"<<CV_VERSION<<endl;
	cout<<" major:"<<CV_MAJOR_VERSION<<endl;
	cout<<" minor:"<<CV_MINOR_VERSION<<endl;
	cout<<" subminor:"<<CV_SUBMINOR_VERSION<<endl;
	cout<<"OpenCV>=2.4.0:"<<(OPENCV_VERSION_CODE>=OPENCV_VERSION(2,4,0)?"true":"false")<<endl;
}
