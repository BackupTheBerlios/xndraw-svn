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


#ifndef NCOORDS_LINESEGMENT_H
#define NCOORDS_LINESEGMENT_H
#include "xndrawexceptions.h"
#
#if HAVE_CONFIG_H
#include "config.h"
#endif

#include "coords.h"
#define DEBUG_REFLECTION 1
//debuggery
//#define CHECK_SEGMENT 1

//throws error on fail
//fixme: runtime check: should fail/succeed at compile time
#define IS_FLOATINGPOINT_TYPE(tp) fp_type_check(tp, true)

namespace NCOORDS{

  struct Segment_Too_Short:public xn_Error{
    Segment_Too_Short():xn_Error("too short!"){}
  };

  bool be_a_dummy(std::string toprint = std::string(),std::ostream& os=std::cerr);

  ///What side of the line do we sit on?
  ///
  enum PointSide{
    POINTSIDE_ERROR=-2,
    POINTSIDE_LEFT=-1,
    POINTSIDE_CENTRE=0,
    POINTSIDE_RIGHT=1};

  /**
@return => a string for human-read debug output
   */
  std::string pointside_as_string(int side);

  /**
@arg pside => one of values in PointSide
@return => true if pside is one of POINTSIDE_LEFT,  POINTSIDE_CENTRE,or  
POINTSIDE_RIGHT
   */

  bool pointside_OK(int pside);


#ifdef CHECK_SEGMENT
  class line_function_test;
  class line_segment_test;
#endif

  //testing testing
  //TODO: amalgamate/sanityze all similar functions in some utility file,
  //maybe in appcommon.
  //important  to compare the difference in values with their magnitudes
  bool doubles_equal_(double left, double right);

  template <typename T>
  bool value_in_range(T v, T minn, T maxx);

  template <typename T>
  bool ranges_overlap(T first0, T first1, T second0, T second1);

  //unused
  struct Angle{
    double th; //// radians

    Angle(double d):th(Angle::normalise(d)){}
    Angle(const Angle& src):th(src.th){}

    ~Angle(){}

    //Angle& operator=(const Angle& src);

    static double normalise(double d){return d;}

    static Angle& reversed(Angle& a){
      a.th += M_PI; 
      if (a.th > (2*M_PI)){
	a.th -= 2*M_PI;
      }
      return a;
    }

    bool operator==(const Angle& other)const{return other.th ==th;}

    bool operator==(double d)const{return d ==th;}
    bool parallel(const Angle& other)const{return *this == other;}

  };



    bool operator==(const Angle& l, const Angle& r);
  bool operator!=(const Angle& l, const Angle& r);

template <typename TP>
bool fp_type_check(TP dummy, bool dothrow=true);

  //fwd decls
  template <typename T>
  class Linear_function;

template<typename COORDS>
class Segment;

  template <typename T>
  std::ostream& operator<<(std::ostream& os, const Linear_function<T> & fn);
  //TODO: sometime - other comparison operators
  template <typename T, typename Q>
  bool operator==(const Linear_function<T>& left, 
		  const Linear_function<Q>& right);

  template <typename T>
  class Linear_function{
    //ax +by +c=0

  public:
    typedef std::pair<T, T> pair_type;

#ifdef CHECK_SEGMENT
    friend class line_function_test;
    friend class line_segment_test;
#endif

  protected:
    T a;
    T b;
    T c;

    static Nearly_Equal<T> parallel_threshold;
  public:
    template<typename D>
    Linear_function (const Linear_function<D>& src):
      a(src.a),b(src.b), c(src.c){}

    Linear_function (T a, T b, T c):a(a),b(b), c(c){
      check_values();
    }
  
    inline T get_a()const{return a;}
    inline T get_b()const{return b;}
    inline T get_c()const{return c;}

    Linear_function create_perpendicular(const std::pair<T,T>& pt)const;

    template <typename COORDS>
    Linear_function create_perpendicular(const COORDS& pt)const;
    Linear_function reflection(const Linear_function& axis)const;


    // bool parallel(Linear_function<D>& other){

    // }
template <typename COORDS>
T get_distance(const COORDS& pt)const;

template <typename COORDS>
T get_distance_x(const COORDS& pt)const;

template <typename COORDS>
T get_distance_y(const COORDS& pt)const;

  protected:
    template <typename D>
    bool  check_value(D value, bool dothrow=false)const;
    /**
@return POINTSIDE_LEFT, POINTSIDE_RIGHT or POINTSIDE_CENTRE
///what 'side'of the line they are
    ///doesn't have an iidea of line's 'orientation' -(see Segment)
    */
  public:
    template<typename COORDS>
    int pointside(const COORDS& pt)const;
    /**
@fn opposite_pt()
 - find point at same distance from Linear_function as pt 
on other side of line
along perpendicular line through pt
     */
template<typename COORDS>
COORDS opposite_pt(const COORDS& pt)const;

 template<typename COORDS>
 std::pair<COORDS, COORDS> opposite_segment(const Segment<COORDS>& segment)const;

    bool vertical()const{return b==0;}
    bool horizontal()const{return a==0;}
    T gradient()const;

    T get_tanangle()const;

    bool check_values()const;

  public:
    double get_x(T y)const;
    double get_y(T x)const;

    template <typename D>
    bool parallel(const Linear_function<D>& other)const{
      
      if(a ==0){//vertical
	if(other.a==0){return true;}
      }
      else if(b ==0 && other.b==0){return true;}
      T angle = get_tanangle();
      T o_angle = other.get_tanangle();
      return parallel_threshold(angle, o_angle);
    }

    std::pair<T,T> solve(const Linear_function& other)const;
  };

  // template <typename COORDS>
  //class Segment;


 
  template <typename COORDS>
  std::ostream& operator<<(std::ostream& os, const Segment<COORDS>& segment);

 template <typename COORDS>
  std::ostream& operator<<(std::ostream& os, const Segment<COORDS*>& segment);

  enum Segment_Flags{
    SEG_VALID_SEGMENT=0,
    SEG_INVALID_CREATED_BY_HASHTABLE=1,
    SEG_INVALID_IS_POINT = 2,
    SEG_INVALID_NONEXISTENT= 4
  };
  enum Segment_Return{
    SEGMENTS_EQUAL=1,
    SEGMENTS_DIRECTION_REVERSED =1 << 1,
    SEGMENTS_CONTAIN=1 <<2,
    SEGMENTS_CONTAINED_BY=1 <<3,
    SEGMENTS_OVERLAP=1 << 4,
    SEGMENTS_SHARE_POINT=1 << 5,
    SEGMENTS_SHARE_NOTHING = 1 << 6,
    SEGMENTS_OTHER_INVALID= 1 << 7,
   SEGMENTS_BOTH_INVALID= 1 << 8
  };


 template <typename COORDS>
  class Segment: public std::pair<COORDS, COORDS>{
  public:
    typedef std::pair<COORDS, COORDS> pair_type;
 typedef std::pair<COORDS*, COORDS*> pointer_pair_type;
   typedef Segment<COORDS*> pointer_segment_type;
   typedef Linear_function<double> line_function_type;
    using pair_type::first;
    using pair_type::second;


    Segment reflection (line_function_type axis , bool acrossaxis = true)const;

  protected:
    static double zero_angle;//angle considered as 'zero' diff
    //line_function_type<double> line_function;
    //where if (abs(angle-M_PI) < zero_angle)
    //the angle is treated as if it were M_PI, for near_vertical etc
    //COORDS* first;
   Segment_Flags invalidity_flags;
  public:
   Segment(Segment_Flags f = SEG_INVALID_CREATED_BY_HASHTABLE):
     invalidity_flags(f)
{}//satisfy hashtable_policy.h(but not check length
   Segment(COORDS* pt1, COORDS* pt2):pair_type(*pt1, *pt2) ,
				     invalidity_flags(SEG_VALID_SEGMENT){throw_if_too_short();}
    Segment(const COORDS& pt1, const COORDS& pt2):pair_type(pt1, pt2),
				     invalidity_flags(SEG_VALID_SEGMENT){throw_if_too_short();}
   Segment(const Segment& src):pair_type(src),
			       invalidity_flags(src.invalidity_flags){
throw_if_too_short();
}

   template<typename COORDS2>
   Segment(const Segment<COORDS2>& src):
     invalidity_flags(src.get_invalid_flags()){
     if(src){
       COORDS2 fst(src.first);
       COORDS2 snd(src.second);
       SelfRef<COORDS2> reffst(fst);
       SelfRef<COORDS2> refsnd(snd);
       first=reffst.as_reference();
       second= refsnd.as_reference();
     }
     else{
       throw xn_Error("NULL src coords");
     }
     throw_if_too_short();
   }

   explicit Segment(const Segment<COORDS*>& src):
     pair_type(*(src.first), *(src.second)),
     invalidity_flags(src.get_invalid_flags()){throw_if_too_short();}

    virtual ~Segment(){}

   operator bool()const{return true;}
   bool check_length()const{
     return coords::distance_OK(first, second);
   }

   

   void throw_if_too_short()const throw(Segment_Too_Short){
     if(! check_length()){throw Segment_Too_Short();}
   }
    //auto copy ctor etc ok
    template <typename COORDS1>
    int pointside(const COORDS1& pt)const;

   std::pair<double, double> get_midpoint()const{
     double x =(first.x+ second.x) / 2.0;
     double y =(first.y+ second.y) / 2.0;
     return std::pair<double, double>(x,y);
   }

    bool valid()const{
      return invalidity_flags ==SEG_VALID_SEGMENT;
    }
    Segment_Flags get_invalid_flags()const{
      return invalidity_flags ;
    }

   void set_invalidflag(Segment_Flags flag){
     invalidity_flags =flag;
   }
   //or return references
   COORDS get_first()const{return first;}
COORDS get_last()const{return second;}

   bool bisected(const line_function_type& fn)const;
   bool vertical()const;
   bool horizontal()const;
   double length()const;
   //template < typename  COORDS1,  typename COORDS2 >
   //std::pair< bool, Segment<coords> > 
   //common_segment(const Segment<COORDS1>& L,
   //               const Segment<COORDS2>& R,
   //               DOUBLE TOLERANCE =1);
   template < typename  COORDS1>
   int compare(const Segment<COORDS1>& other){
  return SEGMENTS_SHARE_NOTHING;
}
template <typename COORDS1>
    bool direction_reversed(const Segment<COORDS1>& other){
      double tanagl = get_tan_angle();
      double otanagl = other.get_tan_angle();
      if(x_up()){
	tanagl += M_PI;
      }
      if(other.x_up()){
	otanagl += M_PI;
      }
      double diff =  tanagl - otanagl;
      if(diff <0){diff = -diff;}
      if((diff < (M_PI /2.0)) || (diff > (3.0 * M_PI /2.0))){return false;}
      return true;
    }


    bool x_up()const{
      return first.x > second.x;
    }

 

   template < typename  COORDS1>
   std::pair< int, Segment > 
   common_segment( const Segment<COORDS1>& R,
		   double tolerance =1){
     typedef std::pair< int,Segment> result_type;
     result_type segpair(SEGMENTS_BOTH_INVALID, SEG_INVALID_NONEXISTENT);
     if(!valid()||!R.valid()){ }
     segpair.first=0;
     if(direction_reversed(R)){
       segpair.first=SEGMENTS_DIRECTION_REVERSED;
     };
     if(is_equal_to(R), tolerance){
       segpair.second = *this;
       segpair.first |= SEGMENTS_EQUAL;
     }
     else if(contains(R, tolerance)){
       segpair.second = Segment<COORDS>(R);
       segpair.first |= SEGMENTS_CONTAIN;
     }
     else if (R.contains(*this, tolerance)){
       segpair.second = Segment<COORDS>(*this);
	 segpair.first |= SEGMENTS_CONTAINED_BY;
     }
     else if(contains_start(R, tolerance)){
       if( R.contains_start(*this, tolerance)){
	 segpair.second=Segment(get_first(),R.get_first());
	 if(segpair.second.length() < tolerance){
	   segpair.second.set_invalidflag(SEG_INVALID_IS_POINT);
	   segpair.first |= SEGMENTS_SHARE_POINT;
	   /*its a point*/
	 }
	 else{
	   segpair.first |= SEGMENTS_OVERLAP;
	 }
       }
       else if( R.contains_end(*this, tolerance)){
	 segpair.second=Segment(R.get_first(), get_last());
	 if(segpair.second.length() < tolerance){
	   segpair.second.set_invalidflag(SEG_INVALID_IS_POINT);
	   segpair.first |= SEGMENTS_SHARE_POINT;
	 }
	 else{segpair.first |= SEGMENTS_OVERLAP;}
       }
     }
     else if(contains_end(R, tolerance)){
       if( R.contains_start(*this, tolerance)){
	 segpair.second=Segment(get_first(),R.get_last());
	 if(segpair.second.length() < tolerance){
	   segpair.second.set_invalidflag(SEG_INVALID_IS_POINT);
	   segpair.first |= SEGMENTS_SHARE_POINT;
	   /*its a point*/
	 }
	 else{
	   segpair.first |= SEGMENTS_OVERLAP;
	 }
       }
       else if( R.contains_end(*this, tolerance)){
	 segpair.second=Segment(R.get_last(), get_last());
	 if(segpair.second.length() < tolerance){
	   segpair.second.set_invalidflag(SEG_INVALID_IS_POINT);
	   segpair.first |= SEGMENTS_SHARE_POINT;
	 }
	 else{segpair.first |= SEGMENTS_OVERLAP;}
       }
     }
     return segpair;
   }

    template <typename COORDS1>
     bool contains(const Segment<COORDS1>& other, double tolerance =1)const{
       //tolerance: maximum distance to other from line
       return intersects(other.first, tolerance  ) 
	 && intersects(other.second, tolerance  ) ;
   }

template <typename COORDS1>
     bool contains_start(const Segment<COORDS1>& other, 
			 double tolerance =1)const{
  return intersects(other.first, tolerance  ) ;
}
template <typename COORDS1>
     bool contains_end(const Segment<COORDS1>& other, 
			 double tolerance =1)const{
  return intersects(other.second, tolerance  ) ;
}

Segment<coords> rotate_copy(double radians){
      coords fst(first), snd(second), mid(get_midpoint());
      fst= mid.rotate(&fst, radians);
      snd = mid.rotate(&snd, radians);
      return Segment<coords>(fst, snd);
    }

template <typename T>
    bool function_intersects(const Linear_function<T>& fn)const{
      line_function_type myfn = get_function();
      if(myfn.parallel(fn)){return false;}
      coords solution(myfn.solve(fn));
      if(!vertical()){
	if(first.x < second.x){
	  return (solution.x >= first.x) &&( solution.x <= second.x);
	}
	else{
	  return (solution.x <= first.x) &&( solution.x >= second.x);
	}
      }
      else{
	if(first.y < second.y){
	  return (solution.y >= first.y) &&( solution.y <= second.y);
	}
	else{
	  return (solution.y <= first.y) &&( solution.y >= second.y);
	}
      }
    }
   template <typename COORDS1>
    bool is_equal_to(const Segment<COORDS1>& seg, double tolerance =1)const{
      if(!valid() ||!seg.valid()){ return false;}
      COORDS segpt1= seg.get_first();
      COORDS segpt2= seg.get_last();
      COORDS pt1= get_first();
      COORDS pt2= get_last();
      if((pt1.get_distance(&segpt1)<tolerance) 
	 &&(pt2.get_distance(&segpt2)<tolerance) ){return true;}
      if((pt1.get_distance(&segpt2)<tolerance) 
	 &&(pt2.get_distance(&segpt1)<tolerance) ){return true;}
      return false;
    }

    template <typename COORDS1>
    bool point_on_line_segment(const COORDS1& pt)const;

    template <typename COORDS1>
    double min_distance_to(const  COORDS1& pt)const;

template <typename COORDS1>
double max_separation(const Segment<COORDS1>& other)const{
  double d1 = min_distance_to(other.first);
double d2 = min_distance_to(other.second);
 return std::max(d1, d2);
}

    template <typename COORDS1>
    bool intersects(const COORDS1& pt, double tolerance =1)const;
   /*template <typename COORDS1>
    bool intersects(const COORDS1* pt, double tolerance =1)const{
      double  minx, maxx,  miny, maxy;
      xy_min_max( minx, maxx , miny, maxy);
      if(pt->x > maxx||pt->x < minx || pt->y > maxy||pt->y < miny){return false;}
      line_function_type perpfn=create_perpendicular(*pt);
      line_function_type fn = get_function();
      std::pair<double,double> real_inter= fn.solve(perpfn);
      double xdiff = real_inter.first - pt->x;
      double ydiff = real_inter.second -pt->y;
      double len = std::sqrt((xdiff * xdiff)+(ydiff * ydiff));
      return len <=tolerance;
      }*/

    bool intersects(const COORDS* pt, 
		    double tolerance )const;
   bool angle_close_enough(double angle, double ref_angle)const;
   double y_for_x(double xx)const;
   double x_for_y(double yy)const;

template <typename T>
COORDS get_intersection(const Linear_function<T>& fn)const{
  if(!function_intersects(fn)){
    throw xn_Error("no intersection");
  }
  Linear_function<double> myfn= get_function();
  COORDS res = myfn.solve(fn);
  return res;
}

    template <typename COORDS2>
    bool parallel(const Segment<COORDS2>& other)const ;

template <typename COORDS2>
line_function_type create_perpendicular(const COORDS2& pt)const;

   line_function_type create_perpendicular()const {
    coords midpoint = get_midpoint();
    return create_perpendicular(midpoint);
  }
 
 std::pair<coords, line_function_type > 
 create_perpendicular_and_midpoint()const{ 
   typedef std::pair<coords, Linear_function<double> > pair_type;
    coords midpoint = get_midpoint();
    return pair_type(midpoint, create_perpendicular(midpoint));
  }
line_function_type get_function_from_midpoint(double radians){
      ///does linear function have rotate() method for this?
      COORDS centre=get_midpoint();
      COORDS proxy(first);
      centre.rotate(&proxy,radians);
      Segment<COORDS> fn_src(proxy, centre);
      return fn_src.get_function();
    }

   line_function_type get_function()const;
    void xy_min_max(double& xmin, double& ymin, 
		    double& xmax, double& ymax)const;
   double y_at_x(double d)const;
   double x_at_y(double d)const;
    /**
       @method  lines_cross(Segment other)
       whether the _segments_(not the rays) cross
    */

    template <typename COORDS2>
    std::pair<double, double>
    intersection(const Segment<COORDS2&> other)const;
   //TODO: ptr specialisations/overloads where fn(const Segment<COORDS2&> other) addresses other.first or  other.second 
   //bool lines_cross(const Segment& other)const;

    template <typename COORDS2>
    bool lines_cross(const Segment<COORDS2>& other)const;

template <typename COORDS2>
    bool lines_cross(const Segment<COORDS2*>& other)const;

    template <typename COORDS2>
    bool perpendicular(const Segment<COORDS2>& other)const ;

    template <typename COORDS2>
    bool near_perpendicular(const Segment<COORDS2>& other)const ;

    template <typename COORDS2>
    bool near_parallel(const Segment<COORDS2>& other)const ; 

   bool near_vertical()const;
   bool near_horizontal()const;

   double gradient()const;
   ///returns a value between -M_PI and +M_PI.
   ///ie: no information about direction
   double get_tan_angle()const;
   ///returns a value between 0 and 2*M_PI.
   ///ie: full 360 degree circle
   double get_directional_angle()const;
   ///returns a positive value representing the angle between the segment
   /// and s segment drawn from pt to either first(if use_first==true)
   /// or second
   //template <typename COORDS2>
   double angle_with_coords(const COORDS& pt, bool from_first=true)const;

   double get_c()const;
    //don't mix with line_fn.get_a(), get_b(), get_c()
    //incompatible expressions
   double get_a()const;

   double get_b()const;
   static threshold<double> get_zero_angle();
  };

 

  //////////////////////
  template <typename COORDS>
  class Segment<COORDS*>: public std::pair<COORDS*, COORDS*>{
  public:
    typedef std::pair<COORDS*, COORDS*> pair_type;
    //typedef std::pair<COORDS, COORDS> obj_pair_type;
    //typedef std::pair<Segment, obj_pair_type> safe_pair_type;
    typedef Segment<COORDS> object_segment_type;
    typedef Linear_function<double> line_function_type;
    using pair_type::first;
    using pair_type::second;

#ifdef CHECK_SEGMENT
    friend class line_function_test;
    friend class line_segment_test;
#endif
   

    Segment<COORDS> reflection (line_function_type axis , bool acrossaxis = true)const;

  protected:
    static double zero_angle;//angle considered as 'zero' diff
    //line_function_type<double> line_function;
    //where if (abs(angle-M_PI) < zero_angle)
    //the angle is treated as if it were M_PI, for near_vertical etc
    //COORDS* first;
    Segment_Flags invalidity_flags;

  public:
    Segment(Segment_Flags f = SEG_INVALID_CREATED_BY_HASHTABLE):
      pair_type(NULL,NULL),
      invalidity_flags(f){}//satisfy hashtable_policy.h

    Segment(const COORDS& p1, const COORDS& p2):
      pair_type(const_cast<COORDS*>(&p1), const_cast<COORDS*>(&p2)),
      invalidity_flags(SEG_VALID_SEGMENT){
      throw_if_invalid();
      throw_if_too_short();
    }

    Segment(COORDS* pt1, COORDS* pt2):pair_type(pt1, pt2),			      invalidity_flags(SEG_VALID_SEGMENT){
      throw_if_invalid();
      if(pt1 &&pt2){	
	throw_if_too_short();}
    }
   
    Segment(const Segment& src): pair_type(src),
				 invalidity_flags(src.invalidity_flags){
      throw_if_invalid();
      if(first && second){
	throw_if_too_short();}
    }

    ///@warning : src must stay alive
    ///or this becomes invalid
    explicit Segment(const Segment<COORDS>& src):
      pair_type(const_cast<COORDS*>(&(src.first)), 
		const_cast<COORDS*>(&(src.second))),
      invalidity_flags(src.invalidity_flags){throw_if_too_short();}
    virtual ~Segment(){}

    operator bool ()const{return first && second;}
 void throw_if_invalid()const{
      if(first &&(first == second)){
	throw xn_Error("first and second point to same coords");
      }
    }
 bool check_length()const{
   return coords::distance_OK(*first, *second);
   }
   void throw_if_too_short()const throw(Segment_Too_Short){
     if(! check_length()){throw Segment_Too_Short();}
   }

    Segment<coords> rotate_copy(double radians){
      coords fst(*first), snd(*second), mid(get_midpoint());
      fst= mid.rotate(&fst, radians);
      snd = mid.rotate(&snd, radians);
      return Segment<coords>(fst, snd);
    }

    line_function_type get_function_from_midpoint(double radians){
      ///does linear function have rotate() method for this?
      COORDS centre=get_midpoint();
      COORDS proxy(*first);
      centre.rotate(&proxy,radians);
      Segment<COORDS> fn_src(proxy, centre);
      return fn_src.get_function();
    }

    //auto copy ctor etc ok
    template <typename COORDS1>
    int pointside(const COORDS1& pt)const{
      if((first->x-second->x)==0){//vertical
	double xdiff=pt.x-first->x;
	double absdiff=(xdiff>0?xdiff:-xdiff);
	if(absdiff <=coords::min_radius()){return POINTSIDE_CENTRE;}
	return (first->y>second->y? (xdiff < 0 ? POINTSIDE_LEFT : POINTSIDE_RIGHT ):
		(xdiff > 0 ? POINTSIDE_LEFT : POINTSIDE_RIGHT ));
      }
      else if((first->y-second->y)==0){//horiz
	double ydiff=pt.y-first->y;
	double absdiff=(ydiff>0?ydiff:-ydiff);
	if(absdiff <=coords::min_radius()){return POINTSIDE_CENTRE;}
	return (first->x>second->x? (ydiff > 0 ? POINTSIDE_LEFT : POINTSIDE_RIGHT ):
		(ydiff < 0 ? POINTSIDE_LEFT : POINTSIDE_RIGHT ));
      }
      line_function_type fn = get_function();
      Nearly_Equal_Relative<double> areequal;
      double tanangle   = std::atan(gradient());
      double coordsangle= first->get_angle(second);
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
 std::pair<double, double> get_midpoint()const{
     double x =(first->x+ second->x) / 2.0;
     double y =(first->y+ second->y) / 2.0;
     return std::pair<double, double>(x,y);
   }
    bool valid()const{
      return invalidity_flags ==SEG_VALID_SEGMENT;
    }
    Segment_Flags get_invalid_flags()const{
      return invalidity_flags ;
    }

    void set_invalidflag(Segment_Flags flag){
      invalidity_flags = flag;
    }

    bool bisected(const line_function_type& fn)const{
      return fn.pointside(*first) !=fn.pointside(*second);
    }
//or return references
   COORDS get_first()const{return *first;}
COORDS get_last()const{return *second;}

    bool vertical()const{
      return (second->x- first->x) == 0.0;
    }
    bool horizontal()const{
      return (second->y- first->y) == 0.0;
    }

    double length()const{
      double ydiff =second->y- first->y;
      double xdiff =second->x- first->x;

      return std::sqrt((xdiff * xdiff)+ (ydiff * ydiff));
    }


template < typename  COORDS1>
std::pair< int, Segment<COORDS> > 
   common_segment( const Segment<COORDS1>& R,
		   double tolerance =1){
     typedef std::pair< int,Segment<COORDS> > result_type;
     result_type segpair(SEGMENTS_BOTH_INVALID, SEG_INVALID_NONEXISTENT);
     if(!valid()||!R.valid()){ }
     segpair.first=0;
     if(direction_reversed(R)){
       segpair.first=SEGMENTS_DIRECTION_REVERSED;
     };
     if(is_equal_to(R), tolerance){
       segpair.second = *this;
       segpair.first |= SEGMENTS_EQUAL;
     }
     else if(contains(R, tolerance)){
       segpair.second = Segment<COORDS>(R);
       segpair.first |= SEGMENTS_CONTAIN;
     }
     else if (R.contains(*this, tolerance)){
       segpair.second = Segment<COORDS>(*this);
	 segpair.first |= SEGMENTS_CONTAINED_BY;
     }
     else if(contains_start(R, tolerance)){
       if( R.contains_start(*this, tolerance)){
	 segpair.second=Segment(get_first(),R.get_first());
	 if(segpair.second.length() < tolerance){
	   segpair.second.set_invalidflag(SEG_INVALID_IS_POINT);
	   segpair.first |= SEGMENTS_SHARE_POINT;
	   /*its a point*/
	 }
	 else{
	   segpair.first |= SEGMENTS_OVERLAP;
	 }
       }
       else if( R.contains_end(*this, tolerance)){
	 segpair.second=Segment(R.get_first(), get_last());
	 if(segpair.second.length() < tolerance){
	   segpair.second.set_invalidflag(SEG_INVALID_IS_POINT);
	   segpair.first |= SEGMENTS_SHARE_POINT;
	 }
	 else{segpair.first |= SEGMENTS_OVERLAP;}
       }
     }
     else if(contains_end(R, tolerance)){
       if( R.contains_start(*this, tolerance)){
	 segpair.second=Segment(get_first(),R.get_last());
	 if(segpair.second.length() < tolerance){
	   segpair.second.set_invalidflag(SEG_INVALID_IS_POINT);
	   segpair.first |= SEGMENTS_SHARE_POINT;
	   /*its a point*/
	 }
	 else{
	   segpair.first |= SEGMENTS_OVERLAP;
	 }
       }
       else if( R.contains_end(*this, tolerance)){
	 segpair.second=Segment(R.get_last(), get_last());
	 if(segpair.second.length() < tolerance){
	   segpair.second.set_invalidflag(SEG_INVALID_IS_POINT);
	   segpair.first |= SEGMENTS_SHARE_POINT;
	 }
	 else{segpair.first |= SEGMENTS_OVERLAP;}
       }
     }
     return segpair;
   }

     template <typename COORDS1>
     bool contains(const Segment<COORDS1>& other, double tolerance =1)const{
       //tolerance: maximum distance to other from line
       return intersects(other.first, tolerance  ) 
	 && intersects(other.second, tolerance  ) ;
   }
    template <typename COORDS1>
     bool contains_start(const Segment<COORDS1>& other, 
			 double tolerance =1)const{
  return intersects(other.first, tolerance  ) ;
}
template <typename COORDS1>
     bool contains_end(const Segment<COORDS1>& other, 
			 double tolerance =1)const{
  return intersects(other.second, tolerance  ) ;
}

    template <typename COORDS1>
    bool point_on_line_segment(const COORDS1& pt)const{
      return this->intersects(pt);
    }
template <typename COORDS1>
double max_separation(const Segment<COORDS1>& other)const{
  double d1 = min_distance_to(other.first);
double d2 = min_distance_to(other.second);
 return std::max(d1, d2);
}
    template <typename COORDS1>
    double min_distance_to(const  COORDS1& pt)const{
      if(point_on_line_segment(pt)){return 0;}
      line_function_type perpfn = create_perpendicular(pt);
      line_function_type i_fn = get_function();
      double xdiff, ydiff;
      coords pt1(i_fn.solve(perpfn));
      SelfRef<const COORDS1> ptref(pt);
      if(value_in_range(pt1.x, first->x, second->x)){
	xdiff= ptref.as_reference().x -pt1.x;
	ydiff= ptref.as_reference().y -pt1.y;
	return std::sqrt((xdiff *xdiff) +(ydiff *ydiff));
      }
      xdiff= ptref.as_reference().x -first->x;
      ydiff= ptref.as_reference().y -first->y;
      double xdiff0 =pt.x-second->x;
      double ydiff0 = pt.y - second->y;
      double d = std::sqrt((xdiff *xdiff) +(ydiff *ydiff));
      double d0 = std::sqrt((xdiff0 *xdiff0) +(ydiff0 *ydiff0));
      return (d < d0 ? d : d0);
    }

    template <typename COORDS1>
    bool is_equal_to(const Segment<COORDS1>& seg, double tolerance =1)const{
      if(!valid() ||!seg.valid()){ return false;}
      COORDS segpt1= seg.get_first();
      COORDS segpt2= seg.get_last();
      COORDS pt1= get_first();
      COORDS pt2= get_last();
      if((pt1.get_distance(&segpt1)<tolerance) 
	 &&(pt2.get_distance(&segpt2)<tolerance) ){return true;}
      if((pt1.get_distance(&segpt2)<tolerance) 
	 &&(pt2.get_distance(&segpt1)<tolerance) ){return true;}
      return false;
    }

  template <typename COORDS1>
    bool intersects(const COORDS1* pt, double tolerance =1)const{
      double  minx, maxx,  miny, maxy;
      xy_min_max( minx, maxx , miny, maxy);
      if(pt->x > maxx||pt->x < minx || pt->y > maxy||pt->y < miny){return false;}
      line_function_type perpfn=create_perpendicular(*pt);
      line_function_type fn = get_function();
      std::pair<double,double> real_inter= fn.solve(perpfn);
      double xdiff = real_inter.first - pt.x;
      double ydiff = real_inter.second -pt.y;
      double len = std::sqrt((xdiff * xdiff)+(ydiff * ydiff));
      return len <=tolerance;
    }
    template <typename COORDS1>
    bool intersects(const COORDS1& pt, double tolerance =1)const{
      double  minx, maxx,  miny, maxy;
      SelfRef<const COORDS1> ptref(pt);
      xy_min_max( minx, maxx , miny, maxy);
      if(pt.x > maxx||pt.x < minx || pt.y > maxy||pt.y < miny){return false;}
      line_function_type perpfn=create_perpendicular(pt);
      line_function_type fn = get_function();
      std::pair<double,double> real_inter= fn.solve(perpfn);
      double xdiff = real_inter.first - pt.x;
      double ydiff = real_inter.second -pt.y;
      double len = std::sqrt((xdiff * xdiff)+(ydiff * ydiff));
      return len <=tolerance;
    }

template <typename T>
COORDS get_intersection(const Linear_function<T>& fn)const{
  if(!function_intersects(fn)){
    throw xn_Error("no intersection");
  }
  Linear_function<double> myfn= get_function();
  COORDS res = myfn.solve(fn);
  return res;
}

    template <typename T>
    bool function_intersects(const Linear_function<T>& fn)const{
      line_function_type myfn = get_function();
      if(myfn.parallel(fn)){return false;}
      coords solution(myfn.solve(fn));
      if(!vertical()){
	if(first->x < second->x){
	  return (solution.x >= first->x) &&( solution.x <= second->x);
	}
	else{
	  return (solution.x <= first->x) &&( solution.x >= second->x);
	}
      }
      else{
	if(first->y < second->y){
	  return (solution.y >= first->y) &&( solution.y <= second->y);
	}
	else{
	  return (solution.y <= first->y) &&( solution.y >= second->y);
	}
      }
    }

    bool angle_close_enough(double angle, double ref_angle)const{
      double diffangle = (angle > ref_angle ? angle - ref_angle : ref_angle - angle);
      return diffangle <=zero_angle;
    }
    double y_for_x(double xx)const{
      if(vertical()){
	throw xn_Error("can't predict value of y from x in vertical line");
      }
      if(horizontal()){return first->y;}
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

    double x_for_y(double yy)const{
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

    template <typename COORDS2>
    bool parallel(const Segment<COORDS2>& other)const {
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

template <typename COORDS2>
    line_function_type create_perpendicular(const COORDS2& pt)const{
	line_function_type ours = get_function();
	line_function_type theirs(0,1,  -pt.y);
      if(    vertical()){
#ifdef CHECK_SEGMENT
	std::cerr<< "create_perp() vertical()\n";
#endif
	theirs= line_function_type(0,1,  -pt.y);
      }
      else if (horizontal()){
#ifdef CHECK_SEGMENT
	std::cerr<< "create_perp():horizontal\n";
#endif
	theirs= line_function_type(1, 0, -pt.x);
      }
      else{
	double g = -1/gradient();//perpendicular gradient
	double x0 =pt.x -(1/g * pt.y);//'c'
	//invert gradgient as for get_function()
	theirs= line_function_type(1, 1/g, -x0);
      }
#ifdef CHECK_SEGMENT
	coords joiner(ours.solve(theirs));
	double testx = theirs.get_x(pt.y);
	double testy =theirs.get_y(pt.x);
	double segtestx = x_for_y(pt.y);
`	double segtesty = y_for_x(pt.x);
	std::cerr<< "create_perp(): the fn: "<<theirs<<"\nthe pt: "
		 <<pt.x<<", "<<pt.y<<"\ntheirs.get_x(pt.y): "<<testx
		 <<"\ntheirs.get_y(pt.x)"<<testy<<"should match the former...\n"
		 <<"\nsegment.x_for_y(pt.y),y_for_x(pt.x)-should also agree"
		 <<segtestx<<", "<<segtesty<<"\ninntersection with this line: \n(with function "<<ours
		 <<")\n"<<joiner.x<<", "<<joiner.y<<std::endl;
#endif
	return theirs;
}

 line_function_type create_perpendicular()const {
    coords midpoint = get_midpoint();
    return create_perpendicular(midpoint);
  }
 
 std::pair<coords, line_function_type > 
 create_perpendicular_and_midpoint()const{ 
   typedef std::pair<coords, Linear_function<double> > fn_pair_type;
    coords midpoint = get_midpoint();
    return fn_pair_type(midpoint, create_perpendicular(midpoint));
  }


    line_function_type get_function()const{
      //double a, b, c;
      line_function_type res(0, 1, -first->y);
      //      std::cerr<<"get_function()\n";
      if(horizontal()){
	//	std::cerr<<"horizontal()";
	res =line_function_type (0, 1, -first->y);
      }
      else if(vertical()){
	//	std::cerr<<"vertical():";
	res= line_function_type(1, 0, -first->x);
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

    void xy_min_max(double& xmin, double& ymin, 
		    double& xmax, double& ymax)const{
      if(first->y > second->y){
	ymax=first->y;
	ymin= second->y;
      }
      else{
	ymin=first->y;
	ymax= second->y;
      }
      if(first->x > second->x){
	xmax=first->x;
	xmin=second->x;
      }
      else{
	xmin=first->x;
	xmax=second->x;
      }
    }
    double y_at_x(double d)const{
      return y_for_x(d);
    }  
    double x_at_y(double d)const{
      return x_for_y(d);
    }  

    /**
       @method  lines_cross(Segment other)
       whether the _segments_(not the rays) cross
    */

    template <typename COORDS2>
    std::pair<double, double>
    intersection(const Segment<COORDS2>& other)const{
      if (parallel(other)){
	throw xn_Error("parallel lines don't converge - no intersection:check with parallel() before calling intersection()");
      }
      line_function_type i_fn = get_function();
      line_function_type u_fn = other.get_function();
      return i_fn.solve(u_fn);
    }
    //template <typename COORDS2>
    //bool lines_cross(const Segment<COORDS2*>& other)const;

    template <typename COORDS2>
    bool lines_cross(const Segment<COORDS2>& other)const{
      if (parallel(other)){return false;}
      
      //bool u_horiz=other.horizontal();
      // bool u_vert(other.vertical());
      double xmin, xmax, ymin, ymax;
      double u_xmin, u_xmax, u_ymin, u_ymax;
      xy_min_max(xmin, ymin, xmax, ymax);
      other.xy_min_max(u_xmin, u_ymin, u_xmax, u_ymax);
      /*if(vertical()){	
	if(u_horiz){ 
	  return value_in_range(other.first->y , ymin, ymax)&&
	    value_in_range(first->x, u_xmin, u_xmax);
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
	    value_in_range(first->y, u_ymin, u_ymax);
	}
	else{
	  double u_x =  other.x_at_y(ymin);
	  return value_in_range(u_x ,xmin ,xmax) &&
	    value_in_range(u_x ,u_xmin ,u_xmax) &&
	    value_in_range(ymin, u_ymin, u_ymax);
	}
      }
      else{
*/
	line_function_type i_fn = get_function();
	line_function_type u_fn = other. get_function();
	coords inter = i_fn.solve(u_fn);
	return value_in_range(inter.x, xmin ,xmax) &&
	  value_in_range(inter.x, u_xmin ,u_xmax);// &&
	//  value_in_range(inter.y, ymin ,ymax) &&
	//  value_in_range(inter.x, u_ymin ,u_ymax) ;
      
	//}
    }
  

    //coords
    //solve eqn
    //almost never true when using double?
    template <typename COORDS2>
    bool perpendicular(const Segment<COORDS2>& other)const {
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

    template <typename COORDS2>
    bool near_perpendicular(const Segment<COORDS2>& other)const {
      //or actually perpendicular
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


    template <typename COORDS2>
    bool near_parallel(const Segment<COORDS2>& other)const {
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

    bool near_vertical()const{
      if(vertical()){return true;}
      if (horizontal()){return false;}
      double tanangle= std::atan(gradient());
    
      double up(M_PI/2.), down(3 *M_PI /2 );
   
      return angle_close_enough(tanangle, up) ||angle_close_enough(tanangle, down) ;
    }

    bool near_horizontal()const{
      if(vertical()){return false;}//vertical will cause div by zero if gradient() called
      if(horizontal()){return true;}
     
      double tanangle= std::atan(gradient());

      double flat(M_PI), altflat(0);//if second
      return angle_close_enough(tanangle,flat) 
	||angle_close_enough(tanangle, altflat) ;
    }

    double gradient()const{
      double grad_x = second->x- first->x;
      double grad_y = second->y- first->y;
      if(grad_x == 0.){
	throw xn_Error("gradient() flatlined: div by zero");
      }
      return grad_y / grad_x;
    }
    template <typename COORDS1>
    bool direction_reversed(const Segment<COORDS1>& other){
      double tanagl = get_tan_angle();
      double otanagl = other.get_tan_angle();
      if(x_up()){
	tanagl += M_PI;
      }
      if(other.x_up()){
	otanagl += M_PI;
      }
      double diff =  tanagl - otanagl;
      if(diff <0){diff = -diff;}
      if((diff < (M_PI /2.0)) || (diff > (3.0 * M_PI /2.0))){return false;}
      return true;
    }


    bool x_up()const{
      return first->x > second->x;
    }
 ///returns a positive value representing the angle between the segment
   /// and  segment drawn from pt to either first(if use_first==true)
   /// or second
   //template <typename COORDS2>
   double angle_with_coords(const COORDS& pt, bool from_first=true)const;
    double get_directional_angle()const;
    double get_tan_angle()const{
      if(horizontal()){
	return 0;
      }
      else if (vertical()){
	return M_PI /2;
      }
      return std::atan(gradient());
    }

    double get_c()const{
      //get the constant value from the equation describing line(ax-by+c=0)
      //ie y = (gradient() * x) - c
      //ie c = value of x when y ==0
      if(vertical()){
	return -first->x;
      }
      if(horizontal()){
	return -first->y;
      }
      double inverse_g = 1 / gradient();
      double x_at_y0=first->x - (first->y * inverse_g); 
      return -x_at_y0;
    }
    //don't mix with line_fn.get_a(), get_b(), get_c()
    //incompatible expressions
    double get_a()const{
      if(vertical()){return 1;}
      if (horizontal()){return 0;}
      return gradient();//line_fn returns 1
    }

    double get_b()const{
      if(vertical()){return 0;}
      if (horizontal()){return 1;}
      return -1;//line_fn returns -1/gradient()
    }

    static threshold<double> get_zero_angle(){
      return  threshold<double>(zero_angle);
    }
  };
  
#if CHECK_SUBPATH_SECTION

  template <typename T>
 std::deque<Linear_function<T> > create_test_fn_range(T compiler_hint);

  bool test_segment_pointside(const coords& pt, Segment<coords>& seg);
  //template <typename T>
  //std::deque<std::pair< Linear_function<T>, int > > FN_pointside_results(const coords& checkpt, T hint);

  std::string do_FN_pointside_test(const coords& checkpt);

  bool FN_pointside_works(const coords& checkpt);
  
  void write_FN_pointside_test_to_file(const coords& checkpt,
				       const std::string& filepath);

  //template <typename PT>
  // std::deque<std::pair< Segment<PT>, int> > 
  // SEG__pointside_results(const PT& checkpt);
#endif

}

//template bodies
#include "linesegment.tcc"

#endif //NCOORDS_LINESEGMENT_H
