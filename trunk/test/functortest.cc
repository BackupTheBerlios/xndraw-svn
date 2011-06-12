#include "linesegment.h"
#include <map>
namespace NCOORDS{

  static double some_doubles[]= {-0.99998, 1.0000001,sqrt(2.), M_PI, -M_PI_2, 5 ,-5,
				1.0001, 1e-9    , 1e9 ,  1e5   ,  sqrt(2.)* 1e7, sqrt(2.)* 1e-7, 0};
  std::multimap<double, double> create_d_map(){
    std::multimap<double, double> doublemap;
    const  double* doublestart= some_doubles; 
    double* doubles= some_doubles;  
    int i(0);
    while(*doubles){
      double v=*doubles;
      for( int j=0;doublestart[j]; j++){
	std::pair<double, double> valpair(v, v + v* doublestart[j]);
	doublemap.insert(valpair);
      }
      doubles++;
    }
    return doublemap;
  }
  void test(){
    Nearly_Equal_Relative<double> doubletester;
    std::cerr<<"Nearly_Equal_Relative<double>: testing!"<<std::endl;
    int fails(0), successes(0);
    std::multimap<double, double> dmap = create_d_map();
    for(std::multimap<double, double>::const_iterator w= dmap.begin(); 
	w!= dmap.end();w++){
      const std::pair<double, double> vals= *w;
      bool res= doubletester(vals.first, vals.second);
      std::cerr<<"values: "<<vals.first<< ", "<< vals.second <<"\nresult : "
	       << (res?"TRUE-ie nearly equal": "FALSE -ie not nearly equal")
	       <<std::endl;
      if(res){successes++;}
      else{fails++;}
    }
  }
}
  int main(int c ,char** v){

    NCOORDS::test();
    return 0;
  }

