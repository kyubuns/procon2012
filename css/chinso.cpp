#include <iostream>
#include <cstdlib>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <queue>
#include <vector>
#include <cassert>
#include <cmath>
#include <algorithm>
using namespace std;

//argv[]={total_weight,L,M,S}
int main(int argc, char *argv[]){
	const double EPS = 10;
	double weight[] = {5.710,1.328,0.277};

	double total_weight;
	while(1){
		if(argc>1){
			total_weight=atoi(argv[1]);
			goto pass_total;
		}
		cout << "Input total_weight:";
		if(!(cin >> total_weight))break;
pass_total:;
		int a,b,c;
		if(argc>4){
			a=atoi(argv[2]);
			b=atoi(argv[3]);
			c=atoi(argv[4]);
			goto pass_ratio;
		}
		cout << "Large Medium Small:";
		cin >> a >> b >> c;
pass_ratio:;

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

			if(argc>6){
				x[0]=atoi(argv[5]);
				y[0]=atoi(argv[6]);
				goto pass_LRange;
			}
			cout << "Large's range  [a,b] : "; cin >> x[0] >> y[0];
pass_LRange:;
			if(argc>8){
				x[0]=atoi(argv[5]);
				y[0]=atoi(argv[6]);
				goto pass_MRange;
			}
			cout << "Medium's range [a,b] : "; cin >> x[1] >> y[1];
pass_MRange:;
			if(argc>10){
				x[0]=atoi(argv[5]);
				y[0]=atoi(argv[6]);
				goto pass_SRange;
			}
			cout << "Small's range  [a,b] : "; cin >> x[2] >> y[2];
pass_SRange:;
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

if(argc>10)break;

			cout << "====[2]====" << endl;
			for(int i =0 ; i < min<int>(20,v.size()) ; i++){
				double wei = v[i].first.second*weight[0]+v[i].second.first*weight[1]+v[i].second.second*weight[2];
				cout <<v[i].first.first << "("<< v[i].first.second << "," << v[i].second.first <<"," << v[i].second.second <<")" << " " << wei << endl;
			}	

		}

		cout << endl;        cout << endl;        cout << endl;
		
	}

}
