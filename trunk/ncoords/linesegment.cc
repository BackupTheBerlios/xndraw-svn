// -*- C++ -*-

/* Copyright (C)2006-2011 John O'Driscoll - biggerbumme@ yahoo.co.in
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#include "linesegment.h"

namespace NCOORDS{



std::string pointside_as_string(int side){
    std::ostringstream ostmp;
    switch(side){
    case POINTSIDE_ERROR:
      ostmp<<"POINTSIDE_ERROR ("<<POINTSIDE_ERROR<<")";
      break;
    case POINTSIDE_LEFT:
      ostmp<<"POINTSIDE_LEFT ("<<POINTSIDE_LEFT<<")";
      break;
    case   POINTSIDE_CENTRE:
      ostmp<<"POINTSIDE_CENTRE ("<<POINTSIDE_CENTRE<<")";
      break;
    case   POINTSIDE_RIGHT:
      ostmp<<"POINTSIDE_RIGHT ("<<POINTSIDE_RIGHT<<")";
      break;
    default:
      ostmp<<"... invalid value...("<<side<<")";
    }
    return ostmp.str();
 }


  bool operator==(const Angle& l, const Angle& r){
    return l.operator==(r);
  }
  bool operator!=(const Angle& l, const Angle& r){
    return ! l.operator==(r);
  }

  bool be_a_dummy(std::string toprint ,std::ostream& os){
    //something to print inside a condition
    //like so:
    //if(be_a_dummy("whatever(x) called") || whatever(x)){//do something;}
    os << "be_a_dummy()-dummy conditional-\n";
    if(!toprint.size()){
      toprint == "no information provided";
    }
    os << toprint <<std::endl;
    return false;
  }

  bool pointside_OK(int pside){
    if( (pside== POINTSIDE_LEFT) || (pside==  POINTSIDE_CENTRE) ||  
	(pside == POINTSIDE_RIGHT)){return true;}
    else if(pside != POINTSIDE_ERROR){
      std::cerr<<"WARNING: value not one of POINTSIDE_LEFT), POINTSIDE_CENTRE) \n POINTSIDE_RIGHT, or POINTSIDE_ERROR, but "<<pside<<std::endl;;
    }
    return false;
  }

#if CHECK_SUBPATH_SECTION

  bool test_segment_pointside(const coords& pt, Segment<coords>& seg){
    //will return POINTSIDE_CENTRE every time
    coords mid(seg.get_midpoint());
    if(mid.get_distance(&pt)<= coords::min_radius()){return true;}
    int startside=seg.pointside(pt), changecount(0);
    std::cerr<<"seg no 0 of 24:pointside for "<<pt.x<<","<<pt.y
	       <<"\n"<<pointside_as_string(startside)<<std::endl;
    if((startside == POINTSIDE_CENTRE)){changecount++;}
    int curside=startside;
    for (int i=1 ; i <24;i++){
      double angle= (i * M_PI)/24;
      Segment<coords> current= seg.rotate_copy(angle);
      int nxtside= current.pointside(pt);
      std::cerr<<"seg no "<<i <<" of 24:pointside for "<<pt.x<<","<<pt.y
	       <<"\n(testseg)"<<current<<"\n"<<pointside_as_string(nxtside)<<std::endl;
      if(nxtside != curside){
	if(curside ==POINTSIDE_CENTRE){curside=nxtside;}
	else if(nxtside != POINTSIDE_CENTRE){
	  curside=nxtside; changecount++;
	}
      }
    }
    if(curside!=startside){changecount++;}
    return (changecount==2);
  }
  

std::string do_FN_pointside_test(const coords& checkpt){
   double hint(0);
   std::ostringstream ostmp;
   ostmp<<"+++++start ptside test+++++\nchecking pointside for the pt => "
	<<checkpt.x<<","<<checkpt.y<<"\n";
   std::deque<Linear_function<double> > fns=create_test_fn_range(hint);
   typedef std::deque<Linear_function<double> >::iterator  iterator;
   for (iterator w = fns.begin();w != fns.end(); w++){
     ostmp<<"Fn => "<<*w <<"\n";
     int ptside = (*w).pointside(checkpt);
     ostmp <<"result: "<<pointside_as_string(ptside) 
		     <<"\n====end test output=====\n";
   }
   return ostmp.str();
 }

  bool FN_pointside_works(const coords& checkpt){
  double hint(0);
   std::deque<Linear_function<double> > fns=create_test_fn_range(hint);
   typedef std::deque<Linear_function<double> >::iterator  iterator;
   int subres(POINTSIDE_ERROR);
   bool did_cross(false);
   for (iterator w = fns.begin();w != fns.end(); w++){
     if(((w+1)==fns.end()) && ((*w).horizontal())){
       std::cerr << "final fn is 2nd vhorizontal - if it goes bang continually, remove it or the horizontal at start"<<std::endl;
     }
     int cur_res=(*w).pointside(checkpt);
     std::cerr << pointside_as_string(cur_res)<<std::endl;;
     if((subres==POINTSIDE_ERROR) || (subres==POINTSIDE_CENTRE)){
       subres= cur_res;
     }
     else if((cur_res==subres) ||(cur_res == POINTSIDE_CENTRE)){continue;}
     else if(!did_cross){did_cross=true; subres= cur_res;}
     else{return false;}
   }
   return did_cross;
  }

 void write_FN_pointside_test_to_file(const coords& checkpt,
				      const std::string& filepath){
   std::ofstream outfile(filepath.c_str());
   outfile <<do_FN_pointside_test(checkpt)<<std::endl;
 }



#endif //CHECK_SUBPATH_SECTION


}
