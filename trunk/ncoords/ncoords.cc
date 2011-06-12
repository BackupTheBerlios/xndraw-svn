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




#include <iostream>

#include "ncoords.h"
#include "clickhistory.h"

namespace NCOORDS{




deqC::~deqC(){
  delete_pointers();
}
deqC::iterator deqC::erase(deqC::iterator which){
  coords* pt= *which;
  if(!pt){
    throw xn_Error("NULL");
  }
  deqC::iterator res = std::deque<coords*>::erase(which);
  delete pt;
  pt=NULL;
  return res;
}

void deqC::clear(){
  delete_pointers();
  std::deque<coords*>::clear();
  
}

deqC::deqC(int i):std::deque<coords*>(i){

}

void deqC::delete_pointers(){
  deleter_void<coords> functor;
std::for_each(begin(),end(),functor);
}

deqC& deqC::operator=( const deqC& src){
  if(this != &src){
    clear();
  for(deqC::const_iterator walk = src.begin(); walk != src.end();walk++){
    this->load(*(*(walk)));
  }
  }
  return *this;
  //
}

deqC::deqC(const deqC& src){
  
 for(deqC::const_iterator walk = src.begin(); walk != src.end();walk++){
   this->load(*(*(walk)));
  }
  }
void deqC::load(const coords& pt){
      push_back(new coords(pt));
    }
/*
void deqC::transform(const cairo_matrix_t& matrix){
      for (iterator walk = begin(); walk != end(); walk++){
	cairomatrix_transform_XY(matrix, *(*walk));
      }
    }
*/
//ncoords::number_pts = 3;
const coords& deqC::find_closest(const coords& searchitem) const{
  return NCOORDS::find_closest(*this, searchitem);
}
std::ostream& operator<<(std::ostream& os,const deqC& list){
      deqC::const_iterator walk = list.begin();
       ptrdiff_t n = list.end()- walk;
      std::cout << "deqC friend std::ostream operator<< hhhhh-*niemo*-gnjhm,A<"  << std::endl;
      for (ptrdiff_t i = 0;i < n;i++){
	coords* point = *( walk + i);
	os << *point;
      }
      return os;
    }

ncoords operator+(const ncoords& nleft, const coords& right){
      ncoords result(nleft);
      result.x += right.x;
      result.y += right.y;
      coords * rad = result.get_radial();
      *rad += right;
      return result;
    }
ncoords operator-(const ncoords& nleft, const coords& right){
  ncoords result(nleft);
  result.x -= right.x;
  result.y -= right.y;
  coords * rad = result.get_radial();
  *rad -= right;
  return result;
}

ncoords& operator-=(ncoords& nleft, const coords& right){
  nleft.x -= right.x;
  nleft.y -= right.y;
  coords * rad = nleft.get_radial();
  *rad -= right;
  return nleft;
}

ncoords& operator+=(ncoords& nleft, const coords& right){
  nleft.x += right.x;
  nleft.y += right.y;
  coords * rad = nleft.get_radial();
  *rad += right;
  return nleft;
}


bool ncoords::fill_container(deqC& pt_list,bool invert, bool reverse)const{//less complicated..didn't work,.. maybe now it does..
  coords centre(x,y);
  centre.set_state(get_state());
  coords rad(radial);
  double step = get_step();
  double rotangle = step;
  if(reverse){
    rotangle =2 * M_PI -rotangle;
  }
  if(invert){
    rad  =centre.rotate(&rad, M_PI);
  }
  pt_list.clear();
  for(unsigned int i = 0; i < number_pts;i++ ){
   pt_list.load(rad);
    rad  =centre.rotate(&rad, rotangle);
  }
 return true;
}


    

 
    void ncoords::set_state(coord_state stat){
      coords::set_state(state);
      radial.set_state(state);
      radial.flip_stateflag(COORDS_UP);
    }

coords ncoords::get_centre()const {
      return coords(x,y);
    }

void ncoords::set_radial(const coords& src){
      coord_state st = radial.get_state();
      radial.set(src);
      radial.set_state(st);
    }

void  ncoords::set(double exy, double wy){
      coords::set(exy,wy);//ie sets centre
    }

double ncoords::get_angle(const coords* other) const{
      if(!other){
	return coords::get_angle(&radial);
      }
      return coords::get_angle(other);
      }
coords ncoords::get_point_n(unsigned int n) const{// note counting from zero
  if (n > number_pts - 1){
    throw xn_Error("index out of range");
  }
  coords res(radial);
  return rotate_n_steps(res, n);
}


void ncoords::set(const coords& centr, const coords& radl, unsigned int n){
  x = centr.get_x();
  y= centr.get_y();
  radius = centr.get_distance(&radl);
  number_pts = n;  
  radial.set(radl);
  set_state (centr.get_state());
}

coords* ncoords::get_next(coords* outcoords){
  //you own the result
  if(outcoords){
    outcoords->set(radial);
  }
  else{
  outcoords = new coords(radial);
  }
  return &(rotate(outcoords, get_step()));
}
ncoords::ncoords(coord_state st):number_pts(3){
set_state(st);
}

ncoords::ncoords(const coords& centr, const coords& radia, unsigned int n)
  :
  coords(centr),
  radial(radia),
  number_pts(n)
{
  radius = centr.get_distance(&radia);
  set_state(centr.get_state());
}

ncoords::ncoords(const ncoords& src)
  :
  coords(src),
  radius(src.radius),
  radial(src.radial),
  number_pts(src.number_pts)
{
  if (get_state() == radial.get_state()){
    throw xn_Error("internal error : centre and radial states shouldn't be equal");
  }
}
ncoords::ncoords(const coords& centr, double radiuss,double angl, unsigned int n)
      :
        coords(centr),
	radius(radiuss),
	radial(centr.get_x(), centr.get_y()-radiuss),
	number_pts(n)
     	{
	  rotate(&radial, angl);
	  int new_st = state ^ COORDS_UP;
	  radial.set_state((coord_state) new_st);
	}

    ncoords& ncoords::operator=(const ncoords& src){
      if (&src != this){
	coords::operator=(src);
      radial = src.radial;
      number_pts= src.number_pts;
    }
 return *this;
  }

    ncoords ncoords::add(const coords& other){
      ncoords temp(*this);
      temp.radial = radial + other;
      temp.x += other.get_x() ;
      temp.y += other.get_y() ;
      return temp;
    }


void ncoords::invert(){
	double newx =  x - radial.get_x();
	double newy = y - radial.get_y();
	radial.set(newx,newy);
	flip_stateflag(COORDS_UP);
}

   void ncoords::subtract(const coords* offset){
     x  -= offset->get_x();
     y -= offset->get_y();
radial.set(radial.get_x() - offset->get_x(),radial.get_y() - offset->get_y());
      }
/*
    ncoords& ncoords::operator +=(const coords& offset){
      translate(&offset);
      return *this;
    }

 ncoords& ncoords::operator -=(const coords& offset){
      subtract(&offset);
      return *this;
    }
*/

   void ncoords::translate(const coords* offset){
	coords::translate(offset);
	radial.set(radial.get_x() + offset->get_x(),radial.get_y() + offset->get_y());
	
      }

void ncoords::set(const ncoords& src){
  radial = src.radial;
  number_pts= src.number_pts;
set_state(src.get_state());
}

void ncoords::flip(){
  double diffx = radial.get_x() - get_x();
    double diffy  = radial.get_y() - get_y();
    radial.set (get_x() - diffx, get_y() - diffy);
}

std::ostream& operator<<(std::ostream& os, const ncoords& pointcircle){
      os << "\n[ ("<< pointcircle.x << ", " << pointcircle.y<< "), n=" << pointcircle.number_pts << ", ( ";
      coords centre(pointcircle.x,pointcircle.y);
      coords rad(pointcircle.radial);
      for (unsigned int i = 0;i < pointcircle.number_pts; i++){
	os << "(" << rad.get_x() << ", " << rad.get_y() << ")";
	if (i < pointcircle.number_pts -1){
	  os << ", ";
	  rad = centre.rotate(&rad,  pointcircle.get_step());
	}
	else{
	  os << " ),";
	}
      }
     return  os << ((pointcircle.get_state() & COORDS_UP)?("up") :("down")) << " ]";
    }

  /*
ncoLine& ncoLine::operator=(const ncoLine& src){
      if (&src != this){
	coords::operator=(src);
	pt2=src.pt2;
	orientation = src.orientation;
      }
      return *this;
    }

ncoLine& ncoLine::perpendicular(coords& perpendiculator){
  ncoLine flatline = rotate((coords*)this, M_PI - orientation);
  coords rotated = coords::rotate(&perpendiculator, M_PI - orientation);
  coords intersection(rotated.get_x(), flatline.get_y());
  
  ncoLine perp(intersection, rotated);
  std::cout << perp<< std::endl;
  return perp.rotate((coords*)this,M_PI +orientation);


}


		   ncoLine& ncoLine::rotate(coords* fulcrum , double angle){
		     coords hub(*this);
		     coords end(pt2);
		     ncoLine* result= new ncoLine(fulcrum->rotate(&hub,angle), fulcrum->rotate(&end, angle));
		     return *result;
		   }

  */
  Segment<coords> ncoords::get_radial_segment(const coords& satellite)const{
    return Segment<coords>((const coords&)(*this), satellite);
  } 
  Segment<coords> ncoords::get_nth_radial_segment(const coords& satellite,
						  size_type n)const{
    coords tmp(satellite);
    tmp= rotate_n_steps(tmp, n);
    return Segment<coords> ((const coords&)(*this),tmp);
  } 
  Segment<coords> 
  ncoords::get_nth_radial_segment(const simple_click_history& path,
				  const coords& offset,
				  size_type n)const{
    coords pt=path.get_first_point(MOVE_CLICK) + offset;
    pt= rotate_n_steps(pt, n);
    return Segment<coords> ((const coords&)(*this),pt);
  }

  coords&  ncoords::rotate_n_steps(coords& pt, size_type steps)const{
    steps %= get_n();
    if(!steps){return pt;}
    double a=steps* get_step();
    return coords::rotate(&pt, a);
  }

}//namespace NCOORDS
