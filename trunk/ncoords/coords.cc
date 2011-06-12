/* Copyright (C) 2006 John O'Driscoll - biggerbumme@ yahoo.co.in
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
 */ // -*- C++ -*-

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



# include "coords.h"
# include "linesegment.h"

namespace NCOORDS{
  ///minimum radius for point rotation-could set much smaller
  /// about a billionth of a unit anyway
  distance_threshold<coords> coords::rot_minimum = 1e-9;

  double coords::get_distance(const coords* other)const{
    double rad_x = x - other->get_x();
    double rad_y = y - other->get_y();
    double radi = sqrt((rad_x*rad_x) + (rad_y*rad_y));
    return radi;
  }

  coords::coords(double ex, double why, int st )
    :
    basecoords((coord_state)st),
    x(ex),
    y(why)
  {
  }
  coords::coords(const std::pair<double, double>& xy, int st ):
    basecoords((coord_state)st),
    x(xy.first),
    y(xy.second){}
    
  coords& coords::operator=(const coords& src){
    if(&src != this){
      basecoords::operator=(src);
      x = src.x;
      y= src.y;
    }
    return *this;
  }

  coords::coords(int st)
    :
    basecoords((coord_state)st),
    x(0),
    y(0)
  {  }
	    
  coords::~coords() { }	

  coords::coords(const coords& src)
    :
    basecoords( src),
    x(src.x),
    y(src.y)
  {    }

	

  void coords::translate(const coords* offset){
    x = x + offset->x;
    y = y +offset->y;
  }

  void coords::set(const coords& src){
    x = src.x;
    y = src.y;
    state = src.state;
  }

  void coords::set(double exy, double wy){
    x = exy;
    y = wy;
    //what to do with state
  }



  double coords::get_angle(const coords* other)const{
    if(!other){
      throw xn_Error("passed NULL coords");
    }
    double dx = other->get_x() - x;
    double dy = other->get_y() -y ;
    double radius_local = sqrt((dx * dx) + (dy * dy));
    if(radius_local < min_radius()){//operator() would recalculate distance
      std::ostringstream ostmp;
      ostmp<<"get_angle(const coords* other) => points too close!\npts: ("
	   <<x<<"."<<y<<"), ("<<other->x<<"."<<other->y<<")\ndistance = "
	   <<radius_local<<"\nmin distance for get_angle():  "<<min_radius();
      throw XNGeometryError(ostmp.str());
    }
    double e = dx/radius_local;
    //double f = dy/radius_local;
    if (dx >= 0.0){
      if(dy < 0.0){
#ifdef DEBUG
	std::cerr <<"if (dx >= 0.0){if(dy < 0.0){value of (asin(e): "<<asin(e)
		  <<std::endl;
#endif
	return (double)(asin(e)); 
      }
      else{
#ifdef DEBUG
	std::cerr <<"if (dx >= 0.0){if(dy > 0.0){value of (asin(e): "<<asin(e)
		  <<"\nvalue of M_PI - asin(e): "<< M_PI - asin(e)<<std::endl;
#endif
	return (double)( M_PI - asin(e)); 
      }
    }
    else{
      if (dy >= 0.0){
#ifdef DEBUG
	std::cerr <<"if (dx < 0.0){if(dy >= 0.0){value of (asin(e): "<<asin(e)
		  <<"\nvalue of M_PI + asin(-e): "<< M_PI + asin(-e)<<std::endl;
#endif
	return (double)(M_PI + asin(-e));
      }
      else {  //if (dy < 0.0)
#ifdef DEBUG
	std::cerr <<"if (dx < 0.0){if(dy < 0.0){value of (asin(e): "<<asin(e)
		  <<"\nvalue of (2.0*M_PI) - asin(-e): "<< (2.0*M_PI) - asin(-e)<<std::endl;
#endif
	return (double)((2.0*M_PI) - asin(-e));     
      }
    }
    //meaningless compiler warning here('no return statement...')
  }


  double coords::get_angle(double ex, double why, double rad) const{
    double dx =  ex / (rad);
    double dy = why / rad;
    if (dx >= 0.0){
      if(dy < 0.0){
	return (double)(asin(dx));
      }
      else{
	return (double)( M_PI - asin(dx));
      }
    }
    else{
      if (dy >= 0.0){
	return (double)(M_PI + asin(dx));     
      }
      else{
	return (double)(( 2.0*M_PI) - asin(dx));
      }
    }
    //compiler worries about no return statement here
    //but can't get here
  }

  coords coords::diff(const coords& neighbour){
    return coords(x - neighbour.x, y - neighbour.y);
  }

  // void coords::transform(const cairo_matrix_t& matrix){
  // cairo_matrix_transform_point(&matrix, &x, &y);
  //}	    

  void coords::invert(){
    x = -x;
    y = - y;
  }

  coords& coords::subtract(coords* other){
    x = x - other->x;
    y = y - other->y;
    return *this;
  }

  coords& coords::add_inplace(const coords& other){
    x += other.x;
    y += other.y;
    return *this;
  }
	    
  coords coords::add(const coords& other){
    return coords(other.x + x, other.y + y);
  }



  coords& coords::operator*=(double ratio){
    x *= ratio;
    y *= ratio;
    return *this;
  }
  void coords::rotate(double& xx, double& yy, double angl){
    coords tmp(xx, yy);
    tmp = this->rotate(&tmp, angl);
    xx = tmp.x;
    yy=  tmp.y;
  }


  coords&  coords::rotate(coords* satellite, double angl) const{// rotates another coords about (x,y)
  
    double e = satellite->get_x() - x;//deduct origin
    double f = satellite->get_y() - y;
    double corrector = sqrt((e*e) +(f*f));
    if(corrector <= rot_minimum.get_value()){return *satellite;}
    angl = angl + get_angle( satellite);
 
    e = sin(angl);
    f = -cos(angl);
   
    satellite->set((e * corrector)+ x, (f * corrector) + y);
    return *satellite;
  }

  cRect& operator+=(cRect& r, const coords& offset){
    r.topL +=offset;
    r.bottomR  +=offset;
    return r;
  }
  cRect operator+(const cRect& r, const coords& offset){
    cRect res(r);
    res += offset;
    return res;
  }
  cRect& operator-=(cRect& r, const coords& offset){
    r.topL -=offset;
    r.bottomR  -=offset;
    return r;
  }
  cRect operator-(const cRect& r, const coords& offset){
    cRect res(r);
    res -= offset;
    return res;
  }
  cRect& operator*=(cRect& r, const coords& offset){ 
    r.topL *=offset;
    r.bottomR  *=offset;
    return r;
  }
  cRect operator*(const cRect& r, const coords& offset){
    cRect res(r);
    res *= offset;
    return res;
  }
  cRect& operator/=(cRect& r, const coords& offset){ 
    r.topL /=offset;
    r.bottomR  /=offset;
    return r;
  }
  cRect operator/(const cRect& r, const coords& offset){
    cRect res(r);
    res /= offset;
    return res;
  }
  cRect::operator bool()const{return area();}
  bool operator!(const cRect& rect){return !(rect.area());}

  cRect& operator+=(cRect& l,const cRect& r){
    if(!l){
      l= r;
      return l;
    }
    if(l.x() > r.x()){l.topL.x= r.x();}
    if(l.xmax() < r.xmax()){l.bottomR.x= r.xmax();}
    if(l.y() > r.y()){l.topL.y= r.y();}
    if(l.ymax() < r.ymax()){l.bottomR.y= r.ymax();}
    return l;
  }
  cRect operator+(const cRect& l,const cRect& r){
    cRect res(l);
    res +=r;
    return res;
  }



  bool coords::is_in_box(const cRect& box)const{
    return x >=box.x() && x <= box.xmax() &&
      y >=box.y() && y <= box.ymax();
  }

std::pair<coords, int> operator+(const std::pair<coords, int>& left, 
				 const std::pair<coords, int>& right){
  std::pair<coords, int> res= left;
  res.first += right.first;
res.second = right.second;
  return res;
}

std::pair<coords, int>& operator+=(std::pair<coords, int>& left, 
				   const std::pair<coords, int>& right){
left.first += right.first;
left.second = right.second;
  return left;
}

std::pair<coords, int> operator-(const std::pair<coords, int>& left, 
				   const std::pair<coords, int>& right){
  std::pair<coords, int> res= left;
  res.first -= right.first;
  res.second = right.second;
  return res;
}
std::pair<coords, int>& operator-=(std::pair<coords, int>& left, 
				   const std::pair<coords, int>& right){
left.first -= right.first;
left.second = right.second;
  return left;
}


  std::ostream& operator<<(std::ostream& os, const coords& point) {
    using namespace std;
    if (&point ==NULL){
      return os <<"[ NULL ]" << endl;
    }
    os << "[ ("<< point.get_x() << "," <<point.get_y() << ") " << ((point.get_state()& COORDS_UP)?("up") :("down")) << " ]";

#ifdef COUNT_COORDS
    os << "serial_no: " << point.serial_no << "/nTotal coords produced: " << point.get_Ccount() << "\n";
    os  << "Remaining.." <<  point.total_count << "/nDcount: " << point.get_Dcount();
    os << "address of coords no : " << point.serial_no << ": "<< &point << endl;
#endif // COUNT_COORDS

    return os;
  } 


  double coords::correct_angle(double angl){
    return angl;
  }

  // by ref instead?
  double coords::get_angle(const coords* first, const coords* second)const{
    if(!first || !second){
      throw xn_Error("one of both coords NULL");
    }
    double rad_x = second->get_x() - first->get_x();
    double rad_y = second->get_y() - first->get_y();
    double radi = sqrt((rad_x*rad_x) + (rad_y*rad_y));
    return          calc_angle(rad_x, rad_y, radi) ;
  }

  double coords::calc_angle(double ex, double why, double rad)const{
    double dx = ex / rad;
    double dy = why / rad;
    double angle = 0;  
    if (dx > 0.0){
      if(dy < 0.0){
	angle = asin(dx);
	return angle;
      }
      else {
	angle = M_PI - asin(dx);
	return angle;
      }
    }
    else {
      if (dy > 0.0){
	angle = M_PI + asin(-dx);    
	return angle;
      }
      else{
	angle = (2.0*M_PI) - asin(-dx);     
	return angle;
      }
    }
  }


  //member operators 
  bool coords::less(const coords& right) const{
    double dx = x - right.x;
    double dy = y - right.y;
    if (((dx * dx) + (dy * dy)) >= atom_limit){
      if (x != right.x){
	return x  < right.x;
      }
      else if(y != right.y){ 
	return y < right.y;  
      }
    }
    return false;
  }
  
  bool coords:: unequal(const coords& other) const{
    return !equal(other);
  }
  /**
currently the op==() implementation, but leaves much to be desired
tests if other is within atom_limit ( an integral value, currently 5) units
which makes it useless for many things => see min_radius()
@todo: find the operator==(const coords&, const coords&) calls that are called
incessantly in the gui/pt generation(eg in Nodes, drawtool)
and change to call this directly
and then fix operator==(const coords&, const coords&) to test if point is within
min_radius() of the point
 /changed, as old test far too slack
  dxy less than 'atom_limit(==5)'
  anyway that pertains more to the drawing loop, and not even that, now?
  now the test is for min_radius() - minimum distance for determining angle
but sqrt() fart too slow
  */
 bool coords::equal(const coords& other) const{
   double dx= other.x-x;
   double dy= other.y-y;
   //double rad= min_radius(); - much slower with a smaller number!
   return ((dx*dx) +(dy*dy)) <= atom_limit;
  }

  coords coords::operator* ( double multiplier){
    return coords(x * multiplier, y* multiplier);
  }

  //nonmember operators
  bool operator==(const coords& l, const coords& r){
    return l.equal(r);
  }

  bool operator!=(const coords& l, const coords& r){
    return !(l == r);
  }
  bool operator<(const coords& l, const coords& r){
    return l.less(r);
  }
  bool operator<=(const coords& l, const coords& r){
    return (l < r || l == r);
  }
  bool operator>(const coords& l, const coords& r){
    return !(l <= r);
  }
  bool operator>=(const coords& l, const coords& r){
    return !(l < r);
  }

  coords operator+(const coords& l, const coords& r){
    coords ret(l.x + r.x,l.y + r.y);
    ret.state = l.state;
    return ret;
  }

  coords operator-(const coords& antipoint){
    coords res = antipoint;
    res.x = -res.x;
    res.y = -res.y;
    res.state ^= COORDS_UP;
    return res;
  }
coords operator~(const coords& antipoint){
    coords res = antipoint;
    res.x = -res.y;
    res.y = -res.x;
    res.state ^= (COORDS_UP|COORDS_SELECTED);
    return res;
  }
  coords&  operator+=(coords& l, const coords& r){
    l.x += r.x;
    l.y += r.y;
    return l;
  }
  coords operator-(const coords& l, const coords& r){
    coords ret(l.x - r.x,l.y - r.y);
    ret.state = l.state;
    return ret;
  }
  coords&  operator-=(coords& l, const coords& r){
    l.x -= r.x;
    l.y -= r.y;
    return l;
  }
  coords operator*(const coords& l, const coords& r){
    coords ret(l.x * r.x,l.y * r.y);
    ret.state = l.state;
    return ret;
  }

  coords& operator*=(coords& l, const coords& r){
    l.x *= r.x;
    l.y *= r.y;
    return l;
  }
  coords operator/(const coords& l, const coords& r){
    //todo: guard div by zero
    if(r.x == 0|| r.y == 0){
      throw xn_Error("div by zero");
    }
    coords ret(l.x / r.x,l.y / r.y);
    ret.state = l.state;
    return ret;
  }
  coords& operator/=(coords& l, const coords& r){
    if(r.x == 0|| r.y == 0){
      throw xn_Error("div by zero");
    }
    l.x /= r.x;
    l.y /= r.y;
    return l;
  }

  double cRect::distance_from_boundary(const coords& pt)const{
     XndrawMap<Side, Segment<coords> > segs=boundary_segments();
     double d;
     double d_try = segs[TOP].min_distance_to(pt);
     d=d_try;
     d_try= segs[LEFT].min_distance_to(pt);
     if(d_try <d){d=d_try;}
     d_try= segs[RIGHT].min_distance_to(pt);
     if(d_try <d){d=d_try;}
     d_try= segs[BOTTOM].min_distance_to(pt);
     if(d_try <d){d=d_try;}
     if(d<0){throw xn_Error("min_distance_to should return positive value");}
     if(this->contains(pt)  || d==0){return d;}
     return -d;
   }

  XndrawMap<cRect::Side, Segment<coords> > cRect::boundary_segments()const{
    typedef Segment<coords> Seg;//lazytypist
     XndrawMap<Side, Segment<coords> > res;
     coords topR(bottomR.x, topL.y), bottomL(topL.x, bottomR.y);
     res[TOP]= Seg(topL, topR);
     res[BOTTOM]=Seg(bottomR, bottomL);
     res[LEFT] =  Seg(bottomL, topL);
     res[RIGHT]= Seg(topR, bottomR);
     return res;
   }

  cRect create_cRect(const coords& first, const coords& second){
    return cRect(first, second);
  }

  bool  cRect::intersect(const cRect& other)const{
    return (((topL.x >= other.topL.x) &&( topL.x <= other.bottomR.x))||
	    ((topL.x <= other.topL.x) && ( bottomR.x >= other.topL.x))) &&
      (((topL.y >= other.topL.y) &&( topL.y <= other.bottomR.y))||
       ((topL.y <= other.topL.y) && ( bottomR.y >= other.topL.y)));
  }

bool cRect::overlap (const cRect& other)const{
     if(no_size() || other.no_size()){return false;}
     double my_min(x()), my_max( my_min + width()), 
       u_min(other.x()),u_max(u_min + other.width());
     if((my_max < u_min) || (my_min > u_max)){return false;}
     my_min=y();
     my_max= my_min+ height();
     u_min=other.y();
     u_max=u_min + other.height();
     return !((my_max < u_min) ||( my_min > u_max));
   }

  cRect::cRect(const coords first, const coords& second):nosize_(false){
    if(first.x < second.x){
      topL.x= first.x;
      bottomR.x= second.x;
    }
    else{
      topL.x= second.x;
      bottomR.x= first.x;
    }
    if(first.y < second.y){
      topL.y= first.y;
      bottomR.y= second.y;
    }
    else{
      topL.y = second.y;
      bottomR.y= first.y;
    }
  }

  bool ClosestPoint::operator()(const coords& left, const coords& right)const{
    double distanceleft= ranker.get_distance(&left);
    double distanceright= ranker.get_distance(&right);
    return distanceleft <distanceright;
  }    



} //namespace NCOORDS
