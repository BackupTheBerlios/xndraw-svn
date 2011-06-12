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
 * @file ncoords.h
declares class NCOORDS::ncoords
 * a point with n equidistant 'satellites'
each rotated by 360/n degrees compared to the previous
 */


/* Copyright (C) 2006-10 John O'Driscoll - biggerbumme@ yahoo.co.in
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


#ifndef NCOORDS_NCOORDS_H
#define NCOORDS_NCOORDS_H

#include "xndrawexceptions.h"
#if HAVE_CONFIG_H
#include "config.h" 
#endif

#include "coords.h"
#include <vector>
#include <cmath>
#include<set>
#include<deque>
#include <iterator>
#include "deleteptr.h"
#include "linesegment.h"

enum NodePatternStyle{
  XNDRAW_NODEPATTERN_INVALID = 0,
  XNDRAW_NODEPATTERN_REGULAR =1, 
  XNDRAW_NODEPATTERN_INVERTED = XNDRAW_NODEPATTERN_REGULAR << 1,
  XNDRAW_NODEPATTERN_REVERSED = XNDRAW_NODEPATTERN_INVERTED << 1,
  XNDRAW_NODEPATTERN_SKIP_ODD   = XNDRAW_NODEPATTERN_REVERSED << 1, 
  XNDRAW_NODEPATTERN_SKIP_EVEN = XNDRAW_NODEPATTERN_SKIP_ODD << 1,
XNDRAW_NODEPATTERN_FAKECORRECT = XNDRAW_NODEPATTERN_SKIP_EVEN << 1
};


namespace NCOORDS{

  class simple_click_history;//fwd decl(see clickhistory.h)

  template<typename COORDS>
  class Segment;//fwd decl(linesegment.h)

  template <typename A, typename B, typename C>
  struct Trio: public std::pair<A, B>{
    typedef typename std::pair<A, B> pair_type;
    C third;
    Trio(){}
    Trio(const A& a, const B& b, const C& c):pair_type(a,b), third(c){}
  };

    typedef Trio<std::pair<coords, int>, std::pair<coords, int>, std::pair<coords, int> > Last3Clicks;

  //used for ncoords::rotate_path
 


  class deqC :public std::deque<coords*>{
  protected:
    void delete_pointers();
   
    
  public:
    deqC(int i);
    
    deqC(){}
    virtual ~deqC();
    void clear();
    
    deqC& operator=( const deqC& src);//allocate new storage not copy pointer

    deqC(const deqC& src);//allocate new storage not copy pointer
    
    friend std::ostream& operator<<(std::ostream& os,const deqC& list);

    //void transform(const cairo_matrix_t& matrix);

    const coords& find_closest(const coords& searchitem)const;

    void load(const coords& pt);
    iterator erase(iterator which);
      
  };

  typedef std::set<coords*> setC;
  typedef std::set<coords*>::iterator setC_iter;
  typedef deqC::iterator deqC_iter;
  class ncoords;
  //TODO: -for all types (sigh)
  //template<class CHARTYPE>
  //std::basic_ostream<CHARTYPE>& operator<<(std::basic_ostream<CHARTYPE>& os, const ncoords& pointcircle);
  std::ostream& operator<<(std::ostream& os, const ncoords& pointcircle);
  ncoords operator+(const ncoords& nleft, const coords& right);
  ncoords operator-(const ncoords& nleft, const coords& right);
  ncoords& operator+=(ncoords& nleft, const coords& right);
  ncoords& operator-=( ncoords& nleft, const coords& right);
  class ncoords: public coords
  {
  public:
    typedef size_t size_type;


  protected:
    
    double radius;
    coords radial;
    
    unsigned int number_pts;
   
    
  public:
    
    ncoords( coord_state stateval = COORDS_BASESTATE);
    ncoords(const coords& centr, const coords& radia, unsigned int n=3);
    ncoords(const coords& centr, double radiuss,double angl, unsigned int n=3);
    ncoords(const ncoords& src);
    ncoords& operator=(const ncoords& src);
    
    virtual ~ncoords(){}
    /*
    void transform(const cairo_matrix_t& matrix){
      coords::transform(matrix);
      cairomatrix_transform_XY(matrix, radial);
    }
    */
 friend ncoords operator+(const ncoords& nleft, const coords& right);
  friend ncoords operator-(const ncoords& nleft, const coords& right);
  friend ncoords& operator+=(ncoords& nleft, const coords& right);
    friend ncoords& operator-=(ncoords& nleft, const coords& right);
    
    
    ncoords add(const coords& other);
    
    void set(const coords& centr, const coords& radial, unsigned int n=3);

    double get_angle(const coords* other = NULL) const;

    coords get_centre()const ;
    void set(const ncoords& src);
    void set(double exy, double wy);

    void set_radial(const coords& src);

    void set_state(coord_state state);
    
    coords* get_next(coords* outcoords = NULL);
    //reference is better
    inline coords* get_radial() {
      return &radial;
    }
    inline const coords* get_radial() const{
      return &radial;
    }
    template <class ITER>
  void  to_iterator(ITER outiter){
      coords centre(x,y);
      coords rad(radial);
      std::cerr<< "centre: "<<x << ", "<< y<<"\nradial: "<< radial.x
	       << ", "<<radial.y<<std::endl;
      coords* radchild =new coords(rad);
      if((radchild->get_state() & COORDS_UP) != (radial.get_state() & COORDS_UP)){
	throw XNCoordsError("state not copied in coords copy ctor as it should be");
      }
   if((radchild->get_state() & COORDS_UP) == (this->get_state() & COORDS_UP)){
     std::ostringstream ostmp;
     ostmp << "Error: ncoords and radial are both in a"<<  ((this->get_state() & COORDS_UP) ? "n 'UP'" : " 'DOWN'")
	   << " state , when they should be opposite states -\nat least until this function is overloaded in nodes.cc/halso TODO: is fixing get/set state functions to respect the other bits in the state value";
     throw XNCoordsError(ostmp.str());
      }
#ifdef DEBUG
      std::cerr<<"ncoords.state\n\t1)\tself : "<< ((this->get_state() & COORDS_UP) ? "UP" : "DOWN")
	       <<"\n\t2)\tradial : "  << ((radial.get_state() & COORDS_UP) ? "UP" : "DOWN")
	       <<"\n\t2)\tthe generated coords(from radial) : "
	       << ((radchild->get_state() & COORDS_UP) ? "UP" : "DOWN") << std::endl;
#endif
      std::cerr<<"to_iterator()" <<std::endl;
      *outiter++ = radchild;
      std::cerr <<"points: 0th => "<< radchild->get_x() <<", "
		<<radchild->get_y() <<std::endl;
      for (unsigned int i = 1;i < number_pts; i++){
	centre.rotate(&rad, get_step());
	  coords* child = new coords(rad);
	  std::cerr <<i<< "th"<<child->get_x() <<", "
		    <<child->get_y() <<std::endl;
	  *outiter++ = child;
	
      }
    }

    Segment<coords> get_nth_radial_segment(const coords& satellite,
					   size_type n)const;
   Segment<coords>  get_nth_radial_segment(const simple_click_history& path,
				  const coords& offset,
					   size_type n)const;
 
    Segment<coords> get_radial_segment(const coords& satellite)const;
    
    coords get_point_n(unsigned int n)const ;
    
    double get_radius () const {
      return radius;
    }
    
    inline double get_step()const {
      return 2.0 * M_PI / number_pts;
    }
    virtual coords&  rotate_n_steps(coords& pt, size_type steps)const;
    // Segment<coords>
    bool fill_container(deqC& pt_list, bool invert = false, bool reverse = false)const;
    
    void invert();
    
    friend std::ostream& operator<<(std::ostream& os, const ncoords& pointcircle);
    
    void flip();
    
    unsigned int get_base()const{
      return number_pts;
    }
unsigned int get_n()const{
      return number_pts;
    }
    virtual void set_baseno(unsigned int n){
      number_pts = n;
    }

    void translate(const coords* offset);
    //    ncoords& operator +=(const coords& offset);
    
    // ncoords& operator -=(const coords& offset);
    
    void subtract(const coords* offset);


    template<typename INITER, typename OUTCONTAINER>
    void copy_rotate_first(INITER start, INITER last, OUTCONTAINER& out, double angleradians)const{
      double arcsection = 2. * M_PI / number_pts;
      for(;start != last; start++){
	coords result = (*start).first; 
	out.push_back(*start);
#ifdef DEBUG
	std::cerr <<"\n\t\t**** --->ncoords::rotate_first()...DELETE ME<---\n"<< "coords pre rotation: "
		  << (*start).first<<std::endl;
#endif
	try{
	result = coords::rotate(&result, angleradians);
	}
	catch(XNGeometryError& e){ //too close to pivot
	  result = (*start).first;
	}
	out.push_back(result, (*start).second);
#ifdef DEBUG
	std::cerr << "coords post rotation: "<< (*(out.end()-1)).first<<"\n\t\t****"<<std::endl;
#endif
      }
    }

    template<typename PATHHISTORY>
    void create_path_rosette(const PATHHISTORY& path, std::deque< PATHHISTORY>& rosette, 
			     bool invert = false,
			     bool reverse = false,
			     int base = -1){
      if (base < 1){base = number_pts;}
      double step = 2 * M_PI / (double)base;
      for(int i = 0;i < base;i++){
	if(i >0){
	  double arcsection = step * i;
	  if(reverse){
	    arcsection =2 * M_PI -arcsection;
	  }
	  if(invert){
	    arcsection = arcsection + M_PI;
	   
	  }
	  rosette.push_back(PATHHISTORY());
	  copy_rotate_first(path.begin(), path.end(), *(rosette.end()-1), arcsection);
	}
	else{ 
	  if(!invert){rosette.push_back(path);}
	  else{
	    rosette.push_back(PATHHISTORY());
	    copy_rotate_first(path.begin(), path.end(), *(rosette.end()-1),M_PI);
	  }
	}
      }
    }
  };
  /*

  class ncoLine:public coords{
    coords pt2;
    double orientation;
  public:
    ncoLine(){}
    ncoLine(coords& hub,coords& end):coords(hub), pt2(end), orientation(hub.get_angle(&end)){}
    ncoLine(const ncoLine& src) :coords(src), pt2(src.pt2), orientation(src.orientation){}
    ncoLine& operator=(const ncoLine& src);

    ncoLine& perpendicular(coords& perpendiculator);

    ncoLine& rotate(coords* fulcrum , double angle = M_PI/2.0);

    friend std::ostream& operator<<(std::ostream& stream , const ncoLine& line){
      stream << "ncoLine: hub: "<< line.get_x()<< ", " <<  line.get_y()<< "\n end: " << line.pt2.get_x()<< ", " << line.pt2.get_y()<< std::endl;
      return stream;
    }

  };
  */

} 
#endif /*NCOORDS_NCOORDS_H*/


