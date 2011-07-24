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
/**
@file coords.h
coordinate classes and functors to manipulate them
*/

#ifndef NCOORDS_COORDS_H
#define NCOORDS_COORDS_H

#include "xndrawexceptions.h"
#if HAVE_CONFIG_H
#include "config.h"
#endif
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <functional>
#include <algorithm>
#include <utility>
#include <vector>
#include "basecoords.h"
//#include <cairomm/context.h> //cairo_matrix_t -remove ...
#include "defines.h"

//REMEMBER to remove/or shift into configure.ac/config.h
//and conditionalise
//algo/heuristic  needs testing...
//#define DEBUG_NEARLY_EQUAL 1

namespace NCOORDS{

  template <typename ITER, typename CONTAINER>
  CONTAINER& to_ptr_container(ITER first, ITER last, CONTAINER& result){
typedef typename CONTAINER::value_type value_type;
    for(;first !=last;first++){
      result.push_back(const_cast<value_type>(&(*first)));
    }
    return result;
  }

  /**
is the difference between 2 floating pt numbers less than a threshold
   */
  template <class FLOAT>
  struct Nearly_Equal: 
    public std::binary_function<const FLOAT&, const FLOAT&, bool>{
  protected:
    //TODO: min and max defaults, and code to correct threshhold so it lies between, as well as sanity enforcement(ie min_threshhold >0, and max_threshhold > min_threshold etc
    //static FLOAT min_threshhold,  max_threshhold;
    
    FLOAT threshhold;
  public:
    Nearly_Equal( FLOAT t =0.000000001):
      threshhold(t){
      if(threshhold < 0){
	threshhold= -threshhold;
      }
    }
    
    bool close_to_zero(const FLOAT& v){
      FLOAT val =v;
      if(v <0){val=-v;}
      return val <=threshhold;
    }
    
    bool operator()(const FLOAT& left, const FLOAT& right){
      FLOAT diff = left - right;
      
      diff = diff >=0 ? diff : -diff;
    FLOAT leftabs = left >=0 ? left : -left;
    FLOAT rightabs = right >=0 ? right : -right;
    FLOAT valratio(-1), smaller, larger;
    if(leftabs <=rightabs){
      if(!close_to_zero(rightabs)){
	valratio = leftabs / rightabs;
      }
      smaller= leftabs;
      larger=rightabs;
    }
    else{
      if(!close_to_zero(leftabs)){
      valratio = rightabs /  leftabs;
      }
      smaller= rightabs;
      larger= leftabs;
    }
    FLOAT diffratio(-1);
    if(!close_to_zero(smaller)){
      diffratio = diff / smaller;
    }
#ifdef DEBUG_NEARLY_EQUAL
    std::cerr << " functor struct Nearly_Equal<FLOAT> : \nthreshhold: "
	      << threshhold << "\nvalues to compare: left => "<< left
	      <<"\tright => " << right <<"\ndiff: "<< diff 
	      << "\nsmaller(abs) : "<< smaller
	      <<"\tlarger(abs) : "<< larger <<"\ndiffratio(diff/smaller): "
	      << diffratio << "valratio(smaller/larger): "<< valratio
	      <<"\n1- valratio" <<1- valratio <<"\n";
#endif
    bool result(false);
    if(smaller >=1){
#ifdef DEBUG_NEARLY_EQUAL
      std::cerr << "smaller >=1\ndiffratio <= threshhold ?" 
		<< (diffratio <= threshhold ? "true" : "false")
		<<"\n(1- valratio) <= threshhold ? "
		<<((1- valratio) <= threshhold ?"true" : "false")<<"\n";
#endif
      result =  (diffratio <= threshhold) && ((1- valratio) <= threshhold);
    }
    else if(smaller > threshhold){//and less than 1
#ifdef DEBUG_NEARLY_EQUAL
      std::cerr << "smaller <1 && smaller > threshhold"
		<<"\ndiffratio <= threshhold ?" 
		<< (diffratio <= threshhold ? "true" : "false")
		<<"\n(1- valratio) <= threshhold ? "
		<<((1- valratio) <= threshhold ?"true" : "false")
		<<"\ndiff <= threshold" 
		<< (diff <= threshhold ?"true" : "false");
#endif
      result =  ((diffratio <= threshhold) && ((1- valratio) <= threshhold)) || \
	(diff <= threshhold);
    }
    else{
#ifdef DEBUG_NEARLY_EQUAL
      std::cerr << "smaller < threshhold\ndiff <= threshhold ?"
		<< (diff <= threshhold ?"true" : "false")<<"\n";
#endif
      result =  (diff <= threshhold);
}
#ifdef DEBUG_NEARLY_EQUAL
  std::cerr << "returning RESULT ===> "
	    <<( result ?"true: ie Nearly equal" : "false: ie not Nearly equal")
      <<std::endl;
#endif
  return result;
  }
};
  /**
@class Nearly_Equal_Relative : as for Nearly_Equal<FP>
except tests if a ratio between them too insignificant
   */
template <class FLOAT>
struct Nearly_Equal_Relative: public Nearly_Equal<FLOAT>{
public:
  typedef Nearly_Equal< FLOAT> base_type;
  using base_type::threshhold;

protected:
  unsigned char orders_of_magnitude;
public:
  //
  Nearly_Equal_Relative(unsigned char orders =16):
    Nearly_Equal<FLOAT>((FLOAT)1 / (FLOAT)( 1 << orders)),
orders_of_magnitude(orders){}
 
  void set_threshhold(unsigned char orders){
    orders_of_magnitude = orders;
    threshhold = 1. / (FLOAT)( 1 << orders);
  }
};

template <class FLOAT>
bool nearly_equals(const FLOAT& left, const FLOAT& right){
  Nearly_Equal<FLOAT> operation;
  return operation(left, right);
}
  
  template<class WITH_X>
  struct CompareX:public std::binary_function<const WITH_X, const WITH_X, bool>{
    
    bool operator()(const WITH_X left, const WITH_X right){
      return left.x > right.x; 
    }
  };
  
  template<class WITH_X>
  struct CompareX<WITH_X*> :public std::binary_function<const WITH_X*, const WITH_X*, bool>{
    bool operator()(const WITH_X* left, const WITH_X* right){
      return left->x > right->x; 
    }
  };
  
  template<class WITH_Y>
  struct CompareY:public std::binary_function<const WITH_Y, const WITH_Y, bool>{
    
    bool operator()(const WITH_Y left, const WITH_Y right){
      return left.y > right.y; 
    }
  };
  
  template<class WITH_Y>
  struct CompareY<WITH_Y*> :public std::binary_function<const WITH_Y*, const WITH_Y*, bool>{
    bool operator()(const WITH_Y* left, const WITH_Y* right){
      return left->y > right->y; 
    }
  };
  
  class coords/*:public basecoords*/;//declared below
  class cRect;  


  bool operator==(const coords& l, const coords& r);
  bool operator!=(const coords& l, const coords& r);
  bool operator<(const coords& l, const coords& r);
  bool operator<=(const coords& l, const coords& r);
  bool operator>(const coords& l, const coords& r);
  bool operator>=(const coords& l, const coords& r);
  std::ostream& operator<<(std::ostream&, const coords&);
  ///
  coords operator-(const coords& invert);
coords operator~(const coords& antipoint);
  ///
  coords operator+(const coords& l, const coords& r);
  coords&  operator+=(coords& l, const coords& r);
  coords operator-(const coords& l, const coords& r);
  coords&  operator-=(coords& l, const coords& r);
  coords operator*(const coords& l, const coords& r); //scale
   coords& operator*=(coords& l, const coords& r);
coords operator/(const coords& l, const coords& r);
coords& operator/=(coords& l, const coords& r);

  /**
@class coords
a 2D coordinate, with methods for rotating a point, calculating distance, scaling etc
  the base type in flatland
   */
  
  class coords:public basecoords{

  protected:

    static distance_threshold<coords> rot_minimum;

  public:// change of mind - maybe just 'struct' instead
    double x;
    double y;    
    
    explicit coords( int st = COORDS_BASESTATE);	
    coords(double ex, double why, int st = COORDS_BASESTATE);
    coords(const std::pair<double, double>& xy, int st = COORDS_BASESTATE);
    coords& operator=(const coords& src);
    
    coords(const coords& src); 
    
    virtual ~coords();
    
    friend std::ostream& operator<<(std::ostream& os, const coords& point);
    ///min distance between coords for rotation
static double min_radius(){return rot_minimum.get_value();}

    ///coords far enough separated to form a segment
  static bool distance_OK(const coords& lef, const coords& righ){
     return lef.get_distance(& righ) >= coords::min_radius();
   }

  /**
@method get_x()

    */    
    double get_x()const{return x;}
    
    double get_y()const{return y;}
    
    void translate(const coords* offset);
    
    double correct_angle(double angl);
    virtual	  void set(const coords& src);
    
    virtual	  void set(double exy, double wy);
    
    //virtual void transform(const cairo_matrix_t& matrix);

    bool too_close(const coords& other)const{return rot_minimum(*this, other);}
    
    double calc_angle(double ex, double why, double rad) const;    
    
    double get_angle(const coords* first, const coords* second)const;
    
    double get_distance(const coords* other) const;
    double get_angle(double ex, double why, double rad) const;
    
    double get_angle(const coords* other) const;

    template <typename PATH_DATA_T>
    void rotate_data(PATH_DATA_T* data, int index, double angle){
    if (index <= 0 || index >= data[0].header.length){
      throw xn_Error("path data index out of range");
    }
    this->rotate(data[index].point.x, data[index].point.y, angle);
  }
    
    
    void rotate(double& xx, double& yy, double angl);
    
    template <class X_Y>
    X_Y& rotate_xy(X_Y* satellite, double angl) const{
      coords tmp(satellite->x, satellite->y);
      double e = satellite->x -x;
      double f = satellite->y - y;
      double corrector = sqrt((e*e) +(f*f));
      if(corrector > rot_minimum.get_value()){
      angl = angl + get_angle(&tmp);
      }
      else{	return *satellite;   }
      e = sin(angl);
      f = -cos(angl);  
      satellite->x = (e * corrector)+ x;
      satellite->y = (f * corrector) + y;
      return *satellite;
    }
    
    virtual coords&  rotate(coords* satellite, double angle) const;
    
    bool unequal(const coords& other) const;	    
    bool equal(const coords& other) const;
    bool less(const coords& other) const;
    coords& operator*=(double ratio);
    
    virtual coords operator* ( double multiplier);
    
    coords diff(const coords& neighbour);
    
    void invert();
    
    coords& subtract(coords* other);
    coords add(const coords& other);

    bool is_in_box(const cRect& box)const;
    
    coords& add_inplace(const coords& other);
    
    friend bool operator==(const coords& l, const coords& r);
    friend bool operator!=(const coords& l, const coords& r);
    friend bool operator<(const coords& l, const coords& r);
    friend bool operator<=(const coords& l, const coords& r);
    friend bool operator>(const coords& l, const coords& r);
    friend bool operator>=(const coords& l, const coords& r);
    friend coords operator~(const coords& antipoint);
    friend coords operator-(const coords& antipoint);
    friend coords operator+(const coords& l, const coords& r);
    friend coords&  operator+=(coords& l, const coords& r);
    friend coords operator-(const coords& l, const coords& r);
    friend coords&  operator-=(coords& l, const coords& r);
    friend coords operator*(const coords& l, const coords& r);
    friend coords operator/(const coords& l, const coords& r);
  };
  cRect& operator+=(cRect& r, const coords& offset);
 cRect operator+(const cRect& r, const coords& offset);
cRect& operator-=(cRect& r, const coords& offset);
 cRect operator-(const cRect& r, const coords& offset);
 cRect& operator*=(cRect& r, const coords& offset);
 cRect operator*(const cRect& r, const coords& offset);
cRect& operator/=(cRect& r, const coords& offset);
 cRect operator/(const cRect& r, const coords& offset);

  cRect& operator+=(cRect& l,const cRect& r);
cRect operator+(const cRect& r,const cRect& l);

  
  bool operator!(const cRect& rect);
template<typename T>
   class Segment;

 

 struct cRect{
    coords topL;
   coords bottomR;
   bool nosize_;
   cRect(const coords first, const coords& second);
   cRect():nosize_(true){}//first and second each 0,0
   //all these should throw if nosize_
   double x()const{return  topL.x;}
   double y() const {return  topL.y;}
   double xmax()const{return  bottomR.x;}
   double ymax() const{return  bottomR.y;}

   double width()const{return bottomR.x - topL.x;}
   double height()const{return bottomR.y - topL.y;}
   double area()const{return width() * height();}

   bool is_point()const{ return (!width()) && (!height());}
   bool is_line()const{ 
     return (width() && (!height())) || (height() && (!width()));
   }
   bool is_square()const{return width() == height();}
   bool wide()const{return width() > height();}
   bool tall()const{return width() < height();}

   bool no_size()const{return nosize_;}

   bool overlap (const cRect& other)const;
   /**
    *@method distance_from_boundary
    @param pt => pt to check
    @return => distance to nearest boundary/point, positive if inside, negative outside
   */
   double distance_from_boundary(const coords& pt)const;

   enum Side{
     TOP=1,
     BOTTOM=2,
     LEFT=3,
     RIGHT=4
   };
   

   XndrawMap<Side, Segment<coords> > boundary_segments()const;

   coords get_centre()const{
     coords res((bottomR.x + topL.x)/2.0, (bottomR.y + topL.y)/2.0);
     return res;
   }

   bool contains(const coords& pt)const{
     return (pt.x >= x()) && (pt.x <= xmax()) &&
       (pt.y >= y()) && (pt.y <= ymax());
   }

   operator bool()const;
   bool intersect(const cRect& other)const;
 }; 


  template <typename CONTAINER, typename COORDS>
  bool value_is_unique(CONTAINER& cont, COORDS& to_check){
    typedef COORDS value_type;
    typedef CONTAINER container_type;
    typedef typename container_type::const_iterator const_iterator;
#if DEBUG_NODES
    std::cerr<<"value_is_uniq: checking ("<<to_check.x<<", "
	     <<to_check.y<< ")"<<std::endl;
#endif
    for (const_iterator iter = cont.begin(); iter != cont.end();iter++){
#if DEBUG_NODES
      const coords& pt=*(*iter);
      std::cerr<<"against : ("<< pt.x<<", "<< pt.y<<")"<<std::endl;
#endif
      if (to_check == *(*iter)){
#if DEBUG_NODES
	std::cerr<<"failed"<<std::endl;
#endif
	return false;
      }
    }
    return true;
  }
  
  
  template <class XY_CONTAINER>
  void get_container_extents(XY_CONTAINER& container, cRect& outval){
    typedef typename XY_CONTAINER::const_iterator const_iterator;
    coords first, second;
    if(!container.size()){
#ifdef DEBUG_PARANOID
throw xn_Error("empty container has no bounding box");
#else
 std::cerr<<"empty container: no extents: returning (0,0),(0,0)"<<std::endl;
 return cRect(first, second);
#endif
}
    double x,  y;
    const_iterator w= container.begin();
    first.x = second.x = (*w).x;
    first.y = second.y = (*w++).y;
    if(container.size() > 1){
      for(;w != container.end();w++){
	x = (*w).x;
	y = (*w).y;
	if(x < first.x) {first.x = x;}
	else if (x > second.x){ second.x = x;}
	if(y < first.y) {first.y = y;}
	else if (y > second.y){ second.y = y;}
      }
    }
    return cRect(first,second);
  }
  
  template <class XY_PTRCONTAINER>
  void get_ptrcontainer_extents(XY_PTRCONTAINER& container, cRect& outval){
    typedef typename XY_PTRCONTAINER::const_iterator const_iterator;
    coords first, second;
    if(!container.size()){
      std::cerr<<"empty container has no bounding box?returning (0,0),(0,0)"<<std::endl;
      return cRect(first, second);
}
    double x, y;
    const_iterator w= container.begin();
    first.x = second.x = (*w)->x;
    first.y = second.y = (*w++)->y;
    if(container.size() > 1){
      for(;w != container.end();w++){
	x = (*w)->x;
	y = (*w)->y;
	if(x < first.x) {first.x = x;}
	else if (x > second.x){ second.x = x;}
	if(y < first.y) {first.y = y;}
	else if (y > second.y ){ second.y = y;}
      }
    }
    return cRect(first, second);
  }
  
  
  template<class XY>
  class compare_distance:public std::binary_function<const XY*, const XY*, bool>{
    const coords anchor;
  public:
    compare_distance(const coords& ankh): anchor(ankh) {}
    bool operator()(const XY* left, const XY* right){
      return anchor.get_distance(left) > anchor.get_distance(right);
    }
  };

  //CaUTION:
  //these operators keep state from right
  //used by adsjacent_diff to make a relative path,
  //summing neighbours to get back to abs
  //(first point aLKWAYS REMAINS THE SAME)
  std::pair<coords, int> operator+(const std::pair<coords, int>& left, 
				   const std::pair<coords, int>& right);
std::pair<coords, int>& operator+=(std::pair<coords, int>& left, 
				   const std::pair<coords, int>& right);
std::pair<coords, int> operator-(const std::pair<coords, int>& left, 
				   const std::pair<coords, int>& right);
std::pair<coords, int>& operator-=(std::pair<coords, int>& left, 
				   const std::pair<coords, int>& right);

  cRect create_cRect(const coords& first, const coords& second);
  
  template<class XY>
  class compare_distance_pair:public std::binary_function<const std::pair<XY, int>&, const std::pair<XY, int>&, bool>{
    const coords  anchor;
  public:
    typedef std::pair<XY, int> pair_type;
    
    compare_distance_pair(const coords& ankh): anchor(ankh) {}
   
    bool operator()(const pair_type& left, const pair_type& right){
      return anchor.get_distance(&(left.first)) > anchor.get_distance(&(right.first));
    }
  };
  
  
  
  template <class CONTAINER>
  const coords& find_closest(const CONTAINER& container, const coords& searchitem){
    if(!container.size()){throw xn_Error("can't find closest - no coords in container!");}
    std::vector<coords*> temp;
    //temp.push_back(NULL);//what?
    compare_distance<coords> compare(searchitem);
    std::partial_sort_copy(container.begin(), container.end(), temp.begin(), temp.end(), compare);
    return *(*temp.begin());
  }
  /**
   * @class ClosestPoint
a functor which finds the closer of 2 points to a reference point
pass to std::sort() etc
   */
  class ClosestPoint:
    public std::binary_function<const coords&, const coords&, bool>{
  protected:
    coords ranker;
  public:
    ClosestPoint(const coords& pt):ranker(pt){}
    ///if left closer returns true, othrwise false
    bool operator()(const coords& left, const coords& right)const;
};  

} //namespace NCOORDS;

#if HAVE_HASHMAP
#include "wanthash.h"
BEGIN_HASH_NAMESPACE


   template<>
struct hash<NCOORDS::cRect::Side>{
  typedef NCOORDS::cRect::Side value_type;
  size_t operator()(value_type v)const{
    hash<int> inthash;
    return inthash((int)v);
  }
   };

END_HASH_NAMESPACE
#endif

#endif /*NCOORDS_COORDS_H*/
