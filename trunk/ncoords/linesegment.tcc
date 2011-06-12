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


#ifndef NCOORDS_LINESEGMENT_TCC
#define NCOORDS_LINESEGMENT_TCC

#ifndef  NCOORDS_LINESEGMENT_H
#error "linesegment.tcc is a template implementation file:\nit MUST BE #includeD from 'linesegment.h'"
#endif

//included by linesegment.h
namespace NCOORDS{

template<typename T>
Nearly_Equal<T> Linear_function<T>::parallel_threshold(1e-16);



  //what about machine readable version?
template <typename T>
std::ostream& operator<<(std::ostream& os, 
				  const Linear_function<T> & fn){
  os<<"Linear_function: "<<fn.get_a()<<"*x - "<<fn.get_b()
    <<"*y + "<<fn.get_c() <<" = 0";
  return os;
}

 template <typename COORDS>
  double Segment< COORDS>::zero_angle = 1e-6;

  template <typename COORDS>
  double Segment<COORDS*>::zero_angle = 1e-6;

 
 template <typename COORDS>
  std::ostream& operator<<(std::ostream& os, const Segment<COORDS>& segment){
    //#ifdef CHECK_SEGMENT
  os << "[("<<segment.first.x<<", "<<segment.first.y<<")===("
       <<segment.second.x<<", "<<segment.second.y<<") | "
     <<segment.get_a()<<"*x";
  double b= segment.get_b();
  double c=segment.get_c();
  if(b<0){os<< " + "<< -b;}
  else{os<<" - "<< b;}
  os<<"*y";
  if(c<0){os <<" - "<<-c;}
  else{os<<" + "<< c;}
  os<<" | g=";
  //#endif
    if(segment.vertical()){
      os<<"INF (vert)";
    }
    else{
      os<<segment.gradient();
    }
    return os <<"]";
  }

 template <typename COORDS>
  std::ostream& operator<<(std::ostream& os, const Segment<COORDS*>& segment){
os << "[("<<segment.first->x<<", "<<segment.first->y<<")===("
       <<segment.second->x<<", "<<segment.second->y<<") | "
     <<segment.get_a()<<"*x";
  double b= segment.get_b();
  double c=segment.get_c();
  if(b<0){os<< " + "<< -b;}
  else{os<<" - "<< b;}
  os<<"*y";
  if(c<0){os <<" - "<<-c;}
  else{os<<" + "<< c;}
  os<<" | g=";
  //#endif
    if(segment.vertical()){
      os<<"INF (vert)";
    }
    else{
      os<<segment.gradient();
    }
    return os <<"]";
 }
   


  template <typename T>
  template <typename COORDS>
  T Linear_function<T>::get_distance(const COORDS& pt)const{
    Linear_function perp = create_perpendicular(pt);

    COORDS inter(perp.solve(*this));
    return inter.get_distance(&pt);
}
 template <typename T>
  template <typename COORDS>
  T Linear_function<T>::get_distance_x(const COORDS& pt)const{
Linear_function perp = create_perpendicular(pt);

    COORDS inter(perp.solve(*this));
    return inter.x- pt.x;
}
 template <typename T>
  template <typename COORDS>
  T Linear_function<T>::get_distance_y(const COORDS& pt)const{
Linear_function perp = create_perpendicular(pt);

    COORDS inter(perp.solve(*this));
    return inter.x- pt.x;
}

  template <typename T>
  bool value_in_range(T v, T minn, T maxx){
    if(minn > maxx){
      T tmp(minn);
      minn= maxx;
      maxx=tmp;
    }
    return v >= minn && v <=maxx;
  }



  template <typename T>
  bool ranges_overlap(T first0, T first1, T second0, T second1){    return value_in_range(first0, second0, second1) ||
      value_in_range(first1, second0, second1);
  }

  template<typename T>
  T  Linear_function<T>::gradient()const{
    if(vertical()){//div by zero
      throw xn_Error("called gradient() when vertical");
    }
    return a/b;
  }
  template<typename T>
  T Linear_function<T>::get_tanangle()const{
    //returns a value between -M_PI/2  <=> M_PI/2 (ie -90 to 90 degrees)
    if(vertical()){return M_PI/2.0;}
    return std::atan(gradient());
  }

  template <typename T, typename Q>
  bool operator==(const Linear_function<T>& left, 
		  const Linear_function<Q>& right){
    return left.get_a() == right.get_a() &&	    
      left.get_b() == right.get_b() &&	left.get_c() == right.get_c(); 
  }

  template <typename T>  template <typename D>
  bool  Linear_function<T>::check_value(D value, bool dothrow)const{
    return check_nan_and_inf(value, dothrow);
  }

  template <typename TP>
  bool fp_type_check(TP dummy, bool dothrow){
    //fixme: need compile time check
    dummy =0.5;
    TP low(0),high(1);
    if((dummy == low) || (dummy == high)){
      if(dothrow){
	throw xn_Error("integral!");
      }
      return false;
    }
    return true;
  }

  template<typename T>
  std::pair<T,T> Linear_function<T>::solve(const Linear_function& other)const{
    T x(-1), y(-1), x0(-1), y0(-1);
#ifdef CHECK_SEGMENT
    T y_from_x0(-1), x_from_y0(-1),y_from_x(-1), x_from_y(-1)
      std::cerr<<"entering solve\n a: "<<a<<"\nb: "<<b<<"\nc: "<<c<< std::endl;
#endif
   
    if(a ==0){//vertical
#ifdef CHECK_SEGMENT
      std::cerr<<"fn vertical=> a==0"<<std::endl;
#endif
      if(other.a==0){
	throw xn_Error("lines paralleland vertical-can't meet!");
      }
      y=y0=-c;
      if(other.b==0){	  x=x0= -other.c;	}
      else{x=x0=other.get_x(y);}
    }
    else if(b ==0){
#ifdef CHECK_SEGMENT
      std::cerr<<"fn horizontal=> b==0"<<std::endl;
#endif
      if(other.b==0){
	throw xn_Error("lines paralleland horizontical-can't meet!");
      }
      x=x0=-c;
      if(other.a==0){
	y=y0= -other.c;
      }
      else{y=y0=other.get_y(x);}
    }
    else if(other.a ==0){
#ifdef CHECK_SEGMENT
      std::cerr<<"other.fn vertical=> other.a==0"<<std::endl;
#endif
      x =x0= -other.c;
      y=y0=get_y(x);
    }
    else if(other.b ==0){
#ifdef CHECK_SEGMENT
      std::cerr<<"other.fn horixontal=> other.b==0"<<std::endl;
#endif
      y =y0= -other.c;
      x=x0= get_x(y);
    }
    else if(a == other.a && b==other.b){
      throw xn_Error("lines parallel");
    }
    else {
#ifdef CHECK_SEGMENT
      std::cerr<<"not horiz, vert, or parallel"<<std::endl;

      y0=(((c/a) -(other.c/other.a))*(other.a * a))/(((other.a *b)-(a*other.b)));
      x0= (((other.c/other.b)-(c/b) )*(other.b * b))/(((other.b *a)-(b*other.a)));
      y_from_x0=get_y(x0);
      x_from_y0=get_x(y0);
#endif
      x =((c* other.b)-(other.c* b))/((b* other.a)-(a * other.b));
      y=((c* other.a)-(other.c* a))/((b* other.a)-(a * other.b));
#ifdef CHECK_SEGMENT	
      y_from_x= get_y(x);
      x_from_y=get_x(y); 
#endif
    }
#ifdef CHECK_SEGMENT
    std::cerr <<"DEBUG: (X, Y) from solve(): "<< x<<", "<<y
	      <<"\nx from y: "<<x_from_y<<", y from x: "<< y_from_x
	      <<"\ncompeting expressionn: x0,y0: "<< x0<<", "
	      <<y0 <<"\nx from y0: "<< x_from_y0<<", y from x0: "
	      << y_from_x0 <<std::endl;
    std::cerr <<"equations:\n"<< *this<<"\n"<< other
	      <<"\nsolve() returning ("<<x<<","<<y<<")"<<std::endl;
#endif    
    return pair_type(x,y);
  }

  template<typename T>
  double Linear_function<T>::get_y(T x)const{
    if(!check_value(x,false)){
      throw xn_Error("bad value(nan or inf) passed to  get_y()");
    }
    if(b==0){
      throw xn_Error("b ==0: vert line -get_y() makes no sense");
    }
    return (a*x +c)/b;//for horizontal line y= c/b=c(since b will be 1)
  }


  template<typename T>
  double  Linear_function<T>::get_x(T y)const{
    if(!check_value(y,false)){
      throw xn_Error("bad value(nan or inf) passed to  get_x()");
    }
    if(a==0){
      throw xn_Error("a ==0: horiz line -get_x() makes no sense");
    }
    return (b*y -c)/a;//for vertical line   x == -c/a == -c
  }

  template <typename T> 
  template <typename COORDS>
  Linear_function<T>  Linear_function<T>::create_perpendicular(const COORDS& pt)const{
    if(    vertical()){
      return Linear_function(0,1,  -pt.y);
    }
    else if (horizontal()){
      return Linear_function(1, 0, -pt.x);
    }
    else{
      double g = -1/gradient();//perpendicular gradient
      double x0 =pt.x -(1/g * pt.y);//'c'
      return Linear_function(1, 1/g, -x0);
    }
  }

  template <typename T>
  Linear_function<T> Linear_function<T>::create_perpendicular(const std::pair<T,T>	& pt)const{
    if(    vertical()){
      return Linear_function(0,1,  -pt.second);
    }
    else if (horizontal()){
      return Linear_function(1, 0, -pt.first);
    }
    else{
      double g = -1/gradient();//perpendicular gradient
      double x0 =pt.first -(1/g * pt.second);//'c'
      return Linear_function(1, 1/g, -x0);
    }
  }

  template<typename T>
  bool Linear_function<T>::check_values()const{
    IS_FLOATINGPOINT_TYPE(a);//failure is Error
    if(!check_nan_and_inf(a, false)){
      throw xn_Error("'a' is a NaN or infinite");
    }
    if(!check_nan_and_inf(b, false)){
      throw xn_Error("'b' is a NaN or infinite");
    }
    if(!check_nan_and_inf(c, false)){
      throw xn_Error("'c' is a NaN or infinite");
    }
    if(vertical()&&horizontal() && c!=0){
      throw xn_Error("a and b cannot both be 0, except for nonsense case where c==0 \n(ie the function describing the point (0,0): 0x +0y +0=0");
    }
    return true;
  }

  template<typename T> 
  template<typename COORDS>
  COORDS Linear_function<T>::opposite_pt(const COORDS& pt)const{
    Linear_function perp=create_perpendicular(pt);
    std::pair<T,T> intersection(solve(perp));
    T xdiff =intersection.first-pt.x;
    T ydiff =intersection.second-pt.y;
    COORDS result(intersection.first + xdiff, intersection.second + ydiff);
    return result;
  }
  template<typename T> 
  template<typename COORDS>
  std::pair<COORDS, COORDS> Linear_function<T>::opposite_segment(const Segment<COORDS>& segment)const{
    typedef std::pair<COORDS, COORDS> result_type;
    return result_type(opposite_pt(*(segment.first)), opposite_pt(*(segment.second)));
  }

  template<typename T> 
  template<typename COORDS>
  int Linear_function<T>::pointside(const COORDS& pt)const{ 
    T val;
    if(horizontal()){
	val= get_y(pt.x);
	return (val >pt.y ? POINTSIDE_LEFT: 
		(val < pt.y? POINTSIDE_RIGHT :  POINTSIDE_CENTRE));
      }
    else if(vertical() ||(gradient() <0)){
      val= get_x(pt.y);
      return (val >pt.x ? POINTSIDE_LEFT :
		   (val < pt.x? POINTSIDE_RIGHT : POINTSIDE_CENTRE));
    }
    else{//!horiz, !vert, and gradient() >0
      val= get_y(pt.x);
      return	 (val <pt.y ? POINTSIDE_LEFT  : 
		  (val > pt.y? POINTSIDE_RIGHT : POINTSIDE_CENTRE));
    }
  }

  template <typename T>
  Linear_function<T> Linear_function<T>::reflection(const Linear_function<T>& axis)const{
    std::pair<T, T> soln = this->solve(axis);
    const T& X(soln.first), Y(soln.second);
    const T m_angle=get_tanangle();
    T anglediff = m_angle - axis.get_tanangle();
    anglediff= (anglediff >=0 ? anglediff : -anglediff) ;
    //value between 0 and M_PI
    if(anglediff > (M_PI /2.0)){
      anglediff = M_PI -anglediff;
    }
    T complement = M_PI - (2*anglediff);
    T newangle= m_angle + complement;
    if(newangle < -(M_PI /2.)){newangle +=M_PI;}
    else if(newangle >(M_PI /2.)){newangle -=M_PI;}
    T newgrade= std::tan(newangle);
    T by_minus_ax((Y * (1. /newgrade)) - X);
    //T newC=by_minus_ax;
    Linear_function res(1, 1. /newgrade, by_minus_ax);

    T testX = res.get_x(Y);
    T testY = res.get_y(X);
#ifdef DEBUG_REFLECTION
    std::cerr <<"original soln x,y: "<< X <<" "<< Y
	      <<"get_x(),get_y of reflection: "<< testX <<  testY<<std::endl;
    //test this
#endif
    T xdiff = testX - X;
    T ydiff = testY - Y;
    T sq_dist = (xdiff * xdiff ) + (ydiff * ydiff);//positive value
    if(sq_dist > 0.0001/*easy target for crappiest fp math*/){
      std::stringstream ostmp;
      ostmp << "function broken: square of distance to point at intersection(should be 0) : "<<sq_dist;
      std::string e(ostmp.str());
      throw std::logic_error(e.c_str());
    }
    return res;
  }

 template <typename COORDS>
Linear_function<double> 
Segment<COORDS>::get_function()const{
      //double a, b, c;
      line_function_type res(0, 1, -first.y);
      //      std::cerr<<"get_function()\n";
      if(horizontal()){
	//	std::cerr<<"horizontal()";
	res =line_function_type (0, 1, -first.y);
      }
      else if(vertical()){
	//	std::cerr<<"vertical():";
	res= line_function_type(1, 0, -first.x);
      }
      else{
#ifdef CHECK_SEGMENT
	std::cerr<<"(not vertical() or horizontal()";
#endif
	double g= gradient();
	res= line_function_type(1, 1/g, get_c());
      }
      //  std::cerr<<"...returning ...\n"<<res<<"\n+++++++"<<std::endl;
      return res;
    }

 template <typename COORDS>
void Segment<COORDS>::xy_min_max(double& xmin, double& ymin, 
		    double& xmax, double& ymax)const{
      cRect box(first, second);
      xmin = box.x();
      ymin=box.y();

      xmax=box.xmax();
      ymax=box.ymax();
    }

 template <typename COORDS>
double Segment<COORDS>::y_at_x(double d)const{
      return y_for_x(d);
    }

  template <typename COORDS>
template <typename COORDS2>
    bool Segment<COORDS>::lines_cross(const Segment<COORDS2>& other)const{
      if (parallel(other)){return false;}
      bool u_horiz=other.horizontal();
      bool u_vert(other.vertical());
      double xmin, xmax, ymin, ymax;
      double u_xmin, u_xmax, u_ymin, u_ymax;
      xy_min_max(xmin, ymin, xmax, ymax);
      other.xy_min_max(u_xmin, u_ymin, u_xmax, u_ymax);
      /*if(vertical()){	
	if(u_horiz){ 
	  return value_in_range(other.first.y , ymin, ymax)&&
	    value_in_range(first.x, u_xmin, u_xmax);
	}
	else{ 
	  double u_y= other.y_at_x(xmin);
	  return value_in_range(u_y ,ymin ,ymax) &&
	    value_in_range(u_y ,u_ymin ,u_ymax) &&
	    value_in_range(xmin, u_xmin, u_xmax);
	}
      }
      else if( horizontal()){
	if(u_vert){ 
	  return value_in_range(other.first.x, xmin, xmax)&&
	    value_in_range(first.y, u_ymin, u_ymax);
	}
	else{
	  double u_x =  other.x_at_y(ymin);
	  return value_in_range(u_x ,xmin ,xmax) &&
	    value_in_range(u_x ,u_xmin ,u_xmax) &&
	    value_in_range(ymin, u_ymin, u_ymax);
	}
      }
      else{*/
	line_function_type i_fn = get_function();
	line_function_type u_fn = other.get_function();
	coords inter = i_fn.solve(u_fn);
	return value_in_range(inter.x, xmin ,xmax) &&
	  value_in_range(inter.x, u_xmin ,u_xmax) ;// &&
	//value_in_range(inter.y, ymin ,ymax) &&
	//value_in_range(inter.x, u_ymin ,u_ymax) ; 
	//}
    }
   template <typename COORDS>
template <typename COORDS2>
    bool Segment<COORDS>::lines_cross(const Segment<COORDS2*>& other)const{
      if (parallel(other)){return false;}
      bool u_horiz=other.horizontal();
      bool u_vert(other.vertical());
      double xmin, xmax, ymin, ymax;
      double u_xmin, u_xmax, u_ymin, u_ymax;
      xy_min_max(xmin, ymin, xmax, ymax);
      other.xy_min_max(u_xmin, u_ymin, u_xmax, u_ymax);
      /*if(vertical()){	
	if(u_horiz){ 
	  return value_in_range(other.first->y , ymin, ymax)&&
	    value_in_range(first.x, u_xmin, u_xmax);
	}
	else{ 
	  double u_y= other.y_at_x(xmin);
	  return value_in_range(u_y ,ymin ,ymax) &&
	    value_in_range(u_y ,u_ymin ,u_ymax) &&
	    value_in_range(xmin, u_xmin, u_xmax);
	}
      }
      else if( horizontal()){
	if(u_vert){ 
	  return value_in_range(other.first->x, xmin, xmax)&&
	    value_in_range(first.y, u_ymin, u_ymax);
	}
	else{
	  double u_x =  other.x_at_y(ymin);
	  return value_in_range(u_x ,xmin ,xmax) &&
	    value_in_range(u_x ,u_xmin ,u_xmax) &&
	    value_in_range(ymin, u_ymin, u_ymax);
	}
      }
      else{*/
	line_function_type i_fn = get_function();
	line_function_type u_fn = other. get_function();
	coords inter = i_fn.solve(u_fn);
	return value_in_range(inter.x, xmin ,xmax) &&
	  value_in_range(inter.x, u_xmin ,u_xmax); // &&
	  //value_in_range(inter.y, ymin ,ymax) &&
	  //value_in_range(inter.x, u_ymin ,u_ymax) ; 
	//     }
    }

   template <typename COORDS>
template <typename COORDS2>
    std::pair<double, double>
    Segment<COORDS>::intersection(const Segment<COORDS2&> other)const{
      if (parallel(other)){
	throw xn_Error("parallel lines don't converge - no intersection:check with parallel() before calling intersection()");
      }
      line_function_type i_fn = get_function();
      line_function_type u_fn = other.get_function();
      return i_fn.solve(u_fn);
    }

    template <typename COORDS>
double Segment<COORDS>::x_at_y(double d)const{
      return x_for_y(d);
    }  

template <typename COORDS>
template <typename COORDS2>
Linear_function<double> 
Segment<COORDS>::create_perpendicular(const COORDS2& pt)const{
  COORDS2 pt1(pt);
  SelfRef<COORDS2> ptref(pt1);
	line_function_type ours = get_function();
	line_function_type theirs(0,1,  -ptref.as_reference().y);
      if(    vertical()){
#ifdef CHECK_SEGMENT
	std::cerr<< "create_perp() vertical()\n";
#endif
	theirs= line_function_type(0,1,  -ptref.as_reference().y);
      }
      else if (horizontal()){
#ifdef CHECK_SEGMENT
	std::cerr<< "create_perp():horizontal\n";
#endif
	theirs= line_function_type(1, 0, -ptref.as_reference().x);
      }
      else{
	double g = -1/gradient();//perpendicular gradient
	double x0 =ptref.as_reference().x -(1/g * ptref.as_reference().y);//'c'
	//invert gradgient as for get_function()
	theirs= line_function_type(1, 1/g, -x0);
      }
#ifdef CHECK_SEGMENT
	coords joiner(ours.solve(theirs));
	double testx = theirs.get_x(ptref.as_reference().y);
	double testy =theirs.get_y(ptref.as_reference().x);
	double segtestx = x_for_y(ptref.as_reference().y);
`	double segtesty = y_for_x(ptref.as_reference().x);
	std::cerr<< "create_perp(): the fn: "<<theirs<<"\nthe pt: "
		 <<pt.x<<", "<<pt.y<<"\ntheirs.get_x(pt.y): "<<testx
		 <<"\ntheirs.get_y(pt.x)"<<testy<<"should match the former...\n"
		 <<"\nsegment.x_for_y(pt.y),y_for_x(pt.x)-should also agree"
		 <<segtestx<<", "<<segtesty<<"\ninntersection with this line: \n(with function "<<ours
		 <<")\n"<<joiner.x<<", "<<joiner.y<<std::endl;
#endif
	return theirs;
}

 template <typename COORDS>
template <typename COORDS2>
 bool  Segment<COORDS>::parallel(const Segment<COORDS2>& other)const {
      bool i_horiz=horizontal();
      bool i_vert= vertical();
      bool u_horiz=other.horizontal();
      bool u_vert= other.vertical();
      if(i_horiz || u_horiz || i_vert || u_vert){
	if( (i_horiz && u_horiz)||( i_vert && u_vert)){return true;}
	return false;
      }
      return gradient() == other.gradient();
    }

 template <typename COORDS>
double  Segment<COORDS>::x_for_y(double yy)const{
      if(horizontal()){
	throw xn_Error("can't predict value of x from y in horizontal line");
      }
      if(vertical()){return first->x; }
      double inverse_g = 1 / gradient();

      double res = (inverse_g * yy) - get_c();
#ifdef CHECK_SEGMENT
      line_function_type fn = get_function();
      double fn_x= fn.get_x(yy);
      std::cerr<<"Segment.x_for_y():\nsegment result: "<<res
	       <<"line function result: "<<fn_x<<std::endl;
#endif
      return res;
    }

template <typename COORDS>
double Segment<COORDS>::y_for_x(double xx)const{
      if(vertical()){
	throw xn_Error("can't predict value of y from x in vertical line");
      }
      if(horizontal()){return first.y;}
      double g=gradient();
      double res= (g * xx) +(g*get_c());//g *get_c() => offset on y axis at x==0
#ifdef CHECK_SEGMENT
      line_function_type fn = get_function();
      double fn_y= fn.get_y(xx);
      std::cerr<<"Segment.y_for_x():\nsegment result: "<<res
	       <<"line function result: "<<fn_y<<std::endl;
#endif
      return res;
    }
  template <typename COORDS>
double Segment<COORDS>::get_tan_angle()const{
      if(horizontal()){
	return 0;
      }
      else if (vertical()){
	return M_PI /2;
      }
      return std::atan(gradient());
    }

 template <typename COORDS>
 double Segment<COORDS*>::angle_with_coords(const COORDS& pt, 
					   bool use_first)const{
     if(!use_first){
       Segment<COORDS> proxy(*second, *first);
       return proxy.angle_with_coords(pt, true);
     }
     if(first->get_distance(&pt)< coords::min_radius()){
       throw xn_Error("segment too short");
     }
     Segment<COORDS> other(*first, pt);
     double outangle= other.get_directional_angle()-get_directional_angle();
     if(outangle<0){outangle+= 2*M_PI;}
     return outangle;
   }

 template <typename COORDS>
 double Segment<COORDS>::angle_with_coords(const COORDS& pt, 
					   bool use_first)const{
     if(!use_first){
       Segment<COORDS> proxy(second, first);
       return proxy.angle_with_coords(pt, true);
     }
     if(first.get_distance(&pt)< coords::min_radius()){
       throw xn_Error("segment too short");
     }
     Segment<COORDS> other(first, pt);
     double outangle= other.get_directional_angle()-get_directional_angle();
     if(outangle<0){outangle+= 2*M_PI;}
     return outangle;
   }

 template <typename COORDS>
 double Segment<COORDS>::get_directional_angle()const{
   if(horizontal()){
     if(first.x < second.x){	return 0;}
     else{return M_PI;}
   }
   else if (vertical()){
     if(first.y < second.y){return M_PI /2;}
     else{return 3 * M_PI /2;}
   }
      double tanangle= std::atan(gradient());
      if(tanangle>0){
	//assumes clockwise
	if(first.x > second.x){return tanangle;}
	else{return tanangle +M_PI;}
      }
      else if(first.x < second.x){return tanangle+M_PI;}
      else{return tanangle+ (2*M_PI);}
 }

 template <typename COORDS>
 double Segment<COORDS*>::get_directional_angle()const{
   if(horizontal()){
     if(first->x < second->x){	return 0;}
     else{return M_PI;}
   }
   else if (vertical()){
     if(first->y < second->y){return M_PI /2;}
     else{return 3 * M_PI /2;}
   }
      double tanangle= std::atan(gradient());
      if(tanangle>0){
	//assumes clockwise
	if(first->x > second->x){return tanangle;}
	else{return tanangle +M_PI;}
      }
      else if(first->x < second->x){return tanangle+M_PI;}
      else{return tanangle+ (2*M_PI);}
 }

 template <typename COORDS>
double Segment<COORDS>::get_b()const{
      if(vertical()){return 0;}
      if (horizontal()){return 1;}
      return -1;//line_fn returns -1/gradient()
    }

template <typename COORDS>
threshold<double> Segment<COORDS>::get_zero_angle(){
      return  threshold<double>(zero_angle);
    }

template <typename COORDS>
double  Segment<COORDS>::get_a()const{
      if(vertical()){return 1;}
      if (horizontal()){return 0;}
      return gradient();//line_fn returns 1
    }

 template <typename COORDS>
double  Segment<COORDS>::get_c()const{
      //get the constant value from the equation describing line(ax-by+c=0)
      //ie y = (gradient() * x) - c
      //ie c = value of x when y ==0
      if(vertical()){
	return -first.x;
      }
      if(horizontal()){
	return -first.y;
      }
      double inverse_g = 1 / gradient();
      double x_at_y0=first.x - (first.y * inverse_g); 
      return -x_at_y0;
    }

template <typename COORDS>
double Segment<COORDS>::gradient()const{
      double grad_x = second.x- first.x;
      double grad_y = second.y- first.y;
      if(grad_x == 0.){
	throw xn_Error("gradient() flatlined: div by zero");
      }
      return grad_y / grad_x;
    }

template <typename COORDS>
bool Segment<COORDS>::near_horizontal()const{
      if(vertical()){return false;}//vertical will cause div by zero if gradient() called
      if(horizontal()){return true;}
     
      double tanangle= std::atan(gradient());

      double flat(M_PI), altflat(0);//if second
      return angle_close_enough(tanangle,flat) 
	||angle_close_enough(tanangle, altflat) ;
    }

 template <typename COORDS>
bool Segment<COORDS>::near_vertical()const{
      if(vertical()){return true;}
      if (horizontal()){return false;}
      double tanangle= std::atan(gradient());
   
      double up(M_PI/2.), down(3 *M_PI /2 );
   
      return angle_close_enough(tanangle, up) ||angle_close_enough(tanangle, down) ;
    }

template <typename COORDS>
template <typename COORDS2>
    bool Segment<COORDS>::near_parallel(const Segment<COORDS2>& other)const {
      //trick is to avoid calling gradient() on vertical, and horiz lines
      //to avoid possibility of div by zero
      if(parallel(other)){return true;}
      bool i_horiz=horizontal();
      bool i_vert= vertical();
      bool u_horiz=other.horizontal();
      bool u_vert= other.vertical();
      if(i_horiz || u_horiz || i_vert || u_vert){
	if(i_horiz){
	  if(u_vert){return false;}
	  if(other.near_horizontal()){return true;}
	  return false; 
	}
	if(u_horiz){
	  if(i_vert){return false;}
	  if(near_horizontal()){return true;}
	  return false; 
	}
	if(i_vert){
	  if(u_horiz){return false;}
	  if(other.near_vertical()){return true;}
	  return false; 
	}
	if(u_vert){
	  if(i_horiz){return false;}
	  if(near_vertical()){return true;}
	  return false; 
	}
      }
      
      double u_angle= std::atan(other.gradient());
      double i_angle= std::atan(gradient());
      return angle_close_enough(u_angle,  i_angle);
    }  

 
template <typename COORDS>
template <typename COORDS2>
    bool Segment<COORDS>::near_perpendicular(const Segment<COORDS2>& other)const {
      //trick is to avoid calling gradient() on vertical, and horiz lines
      //to avoid possibility of div by zero
      if(perpendicular(other)){return true;}
      bool i_horiz=horizontal();
      bool i_vert= vertical();
      bool u_horiz=other.horizontal();
      bool u_vert= other.vertical();
      if(i_horiz || u_horiz || i_vert || u_vert){
	if(i_horiz){
	  if(u_horiz){return false;}
	  if(other.near_vertical()){return true;}
	  return false; 
	}
	if(u_horiz){
	  if(i_horiz){return false;}
	  if(near_vertical()){return true;}
	  return false; 
	}
	if(i_vert){
	  if(u_vert){return false;}
	  if(other.near_horizontal()){return true;}
	  return false; 
	}
	if(u_vert){
	  if(i_vert){return false;}
	  if(near_horizontal()){return true;}
	  return false; 
	}
      }
    
      double u_angle= std::atan(other.gradient());
      double u_angle_perp=u_angle+ (M_PI /2);
      //rotate 180 deg to stay in atan's range
      if( u_angle_perp > M_PI/2){
	u_angle_perp -= M_PI;
      }
      double i_angle= std::atan(gradient());
      //atan returns angle in range => -M_PI <-> M_PI(ie -90 t0 90 degrees)
      return angle_close_enough(u_angle_perp,  i_angle);
    }  

template <typename COORDS>
template <typename COORDS2>
    bool Segment<COORDS>::perpendicular(const Segment<COORDS2>& other)const {
      bool i_horiz=horizontal();
      bool i_vert= vertical();
      bool u_horiz=other.horizontal();
      bool u_vert= other.vertical();
      if(i_horiz || u_horiz || i_vert || u_vert){
	if( (i_horiz && u_vert)||( i_vert && u_horiz)){return true;}
	return false;
      }
      return gradient() == 1.0 / other.gradient();
    }


 template <typename COORDS>
bool Segment<COORDS>::angle_close_enough(double angle, double ref_angle)const{
      double diffangle = (angle > ref_angle ? angle - ref_angle : ref_angle - angle);
      return diffangle <=zero_angle;
    }

template <typename COORDS>
template <typename COORDS1>
    bool Segment<COORDS>::intersects(const COORDS1& pt, 
				     double tolerance )const{
      double  minx, maxx,  miny, maxy;
      xy_min_max( minx, maxx , miny, maxy);
      COORDS1 pt1(pt);
      SelfRef<COORDS1> ptref(pt1);
	if(ptref.as_reference().x > maxx||ptref.as_reference().x < minx 
    || ptref.as_reference().y > maxy||ptref.as_reference().y < miny){
  return false;
}
      line_function_type perpfn=create_perpendicular(pt);
      line_function_type fn = get_function();
      std::pair<double,double> real_inter= fn.solve(perpfn);
      double xdiff = real_inter.first - ptref.as_reference().x;
      double ydiff = real_inter.second -ptref.as_reference().y;
      double len = std::sqrt((xdiff * xdiff)+(ydiff * ydiff));
      return len <=tolerance;
    }

template <typename COORDS>
template <typename COORDS1>
double Segment<COORDS>::min_distance_to(const  COORDS1& pt)const{
      if(point_on_line_segment(pt)){return 0;}
      line_function_type perpfn = create_perpendicular(pt);
      line_function_type i_fn = get_function();
      double xdiff, ydiff;
      coords pt1(i_fn.solve(perpfn));
      if(value_in_range(pt1.x, first.x, second.x)){
	xdiff= pt.x -pt1.x;
	ydiff= pt.y -pt1.y;
	return std::sqrt((xdiff *xdiff) +(ydiff *ydiff));
      }
      xdiff= pt.x -first.x;
      ydiff= pt.y -first.y;
      double xdiff0 =pt.x-second.x;
      double ydiff0 = pt.y - second.y;
      double d = std::sqrt((xdiff *xdiff) +(ydiff *ydiff));
      double d0 = std::sqrt((xdiff0 *xdiff0) +(ydiff0 *ydiff0));
      return (d < d0 ? d : d0);
    }

template <typename COORDS>
bool Segment<COORDS>::horizontal()const{
      return (second.y- first.y) == 0.0;
    }

template <typename COORDS>
bool Segment<COORDS>::vertical()const{
      return (second.x- first.x) == 0.0;
    }
template <typename COORDS>
  template <typename COORDS1>
    bool Segment<COORDS>::point_on_line_segment(const COORDS1& pt)const{
      return this->intersects(pt);
    }

template <typename COORDS>
  bool Segment<COORDS>::bisected(const line_function_type& fn)const{
      return fn.pointside(first) !=fn.pointside(second);
    }

  template <typename COORDS>
   template <typename COORDS1>
  int Segment<COORDS>::pointside(const COORDS1& pt)const{
    //if vertical still cause bug, reverse answer, horiz too
    if((first.x-second.x)==0){//vertical - gradient == infinity div0
      double xdiff=pt.x-first.x;
      double absdiff=(xdiff>0?xdiff:-xdiff);
      if(absdiff <=coords::min_radius()){return POINTSIDE_CENTRE;}
      return (first.y>second.y? (xdiff < 0 ? POINTSIDE_LEFT : POINTSIDE_RIGHT ):
	      (xdiff > 0 ? POINTSIDE_LEFT : POINTSIDE_RIGHT ));
    }
    else if((first.y-second.y)==0){//horiz
      double ydiff=pt.y-first.y;
      double absdiff=(ydiff>0?ydiff:-ydiff);
      if(absdiff <=coords::min_radius()){return POINTSIDE_CENTRE;}
      return (first.x>second.x? (ydiff > 0 ? POINTSIDE_LEFT : POINTSIDE_RIGHT ):
	      (ydiff < 0 ? POINTSIDE_LEFT : POINTSIDE_RIGHT ));
    }
    line_function_type fn = get_function();
    Nearly_Equal_Relative<double> areequal;
    double tanangle= std::atan(gradient());
      double coordsangle= first.get_angle(&second);
int res=fn.pointside(pt);
      if(!res){return res;}
      if(!( areequal(tanangle, coordsangle))){res= -res;}
      double diffangle= angle_with_coords(pt, false);
      if(diffangle < M_PI){
#if XNDRAW_VERBOSE
	if(res!=POINTSIDE_LEFT){
	  std::cerr<<"old pointside falsely claims POINTSIDE_RIGHT\ntanangle = "
		   <<tanangle<<"\tdiffangle ="<<diffangle
		   <<std::endl;
	}
#endif
	return POINTSIDE_LEFT;
      }
      else{
#if XNDRAW_VERBOSE
	if(res!=POINTSIDE_RIGHT){
	  std::cerr<<"old pointside falsely claims POINTSIDE_LEFT\ntanangle = "
		   <<tanangle<<"\tdiffangle ="<<diffangle
		   <<std::endl;
	}
#endif
	return POINTSIDE_RIGHT;
      }
      
    }

 template <typename COORDS>
  double  Segment<COORDS>::length()const{
      double ydiff =second.y- first.y;
      double xdiff =second.x- first.x;

      return std::sqrt((xdiff * xdiff)+ (ydiff * ydiff));
    }

  template <typename COORDS>
  Segment<COORDS>
  Segment<COORDS>::reflection (Linear_function<double> axis , 
			       bool acrossaxis)const{
    std::pair<COORDS, COORDS> tmp;
    if(acrossaxis){
      tmp= axis.opposite_segment(*this);
    }
    else{
      line_function_type fn = get_function();
      COORDS pt(axis.solve(fn));
      line_function_type perp= axis.create_perpendicular(pt);
      tmp=(perp.opposite_segment(*this));
    }
      return Segment<COORDS>(tmp);
  }

  template <typename COORDS>
  Segment<COORDS>
  Segment<COORDS*>::reflection (Linear_function<double> axis , 
			       bool acrossaxis)const{
    std::pair<COORDS, COORDS> tmp;
    if(acrossaxis){
      tmp= axis.opposite_segment(*this);
    }
    else{
      line_function_type fn = get_function();
      COORDS pt(axis.solve(fn));
      line_function_type perp= axis.create_perpendicular(pt);
      tmp=(perp.opposite_segment(*this));
    }
      return Segment<COORDS>(tmp);
  }
#if CHECK_SUBPATH_SECTION

  template<typename T>
  std::deque<Linear_function<T> > create_test_fn_range(T compiler_hint){   
    std::deque<Linear_function<T> > res;
    Linear_function<T> horizfn(0,-1,0);
    res.push_back(horizfn);
    for(int power = -4; power <6;power++){
      double a = -1 * std::pow(10.0, power);	
      std::cerr <<"a = "<<a<<std::endl;
      Linear_function<T> fn(a,1,0);
      res.push_back(fn);
    }
    Linear_function<T> vertfn(1,0,0);
    res.push_back(vertfn);
    for(int pluspower = 5; pluspower >-5;pluspower--){
      double      a(1), b(1), c(0);
      a= a* std::pow(10.0, pluspower);	
      std::cerr <<"a = "<<a<<"\tb = "<<b<<"\tc = "<<c<<std::endl;
      Linear_function<T> fn(a,b,c);
      res.push_back(fn);
    }
    //Linear_function<T> horizfn2(0,1,0);
    //res.push_back(horizfn2);
    return res;
  }

#endif //CHECK_SUBPATH_SECTION
}
#endif //FILEGUARD
