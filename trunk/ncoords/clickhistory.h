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



#ifndef X3DRAW_CLICKHISTORY_H
#define X3DRAW_CLICKHISTORY_H
#include "xndrawexceptions.h"

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <iostream>
#include <stdexcept>
#include <numeric>
#include <iterator>
#include "nodes.h"
#include "deleteptr.h"
#include "stupidiorefs.h"
#include "fixwidechar.h"
#include "coordinatefunctors.h"
#include "beziercoords.h"
#include "stupidiorefs.h"
#include "linesegment.h"

  //move to new header with UseTracker
//#include <bitset>

/*
  NAMESPACE_APPCOMMON{
  template<>
  struct RawDataPrinter<std::pair<NCOORDS::coords, int> >{
  typedef std::pair<NCOORDS::coords, int> value_type;
  const value_type& printdata;
  RawDataPrinter(const value_type& val):printdata(val){}
 
  template <typename CHARTYPE>
  friend std::basic_ostream<CHARTYPE>& operator <<(std::basic_ostream<CHARTYPE>& os, RawDataPrinter src){
  std::locale loc; //current locale
  CHARTYPE SPACE = std::use_facet<std::ctype<CHARTYPE> >(loc).widen(' ');
  CHARTYPE NEWLINE = std::use_facet<std::ctype<CHARTYPE> >(loc).widen('\n');
  os << src.printdata.first.x << SPACE << src.printdata.first.y 
  << SPACE << src.printdata.second <<NEWLINE;
  return os;
  }
  };
*/
/*
  template<typename CHARTYPE>
  std::basic_ostream<CHARTYPE>&
  operator<< <CHARTYPE, std::pair<NCOORDS::coords, int> >(std::basic_ostream<CHARTYPE>& os,
  const RawDataPrinter<std::pair<NCOORDS::coords, int> >& src){
  std::locale loc; //current locale
  CHARTYPE SPACE = std::use_facet<std::ctype<CHARTYPE> >(loc).widen(' ');
  CHARTYPE NEWLINE = std::use_facet<std::ctype<CHARTYPE> >(loc).widen('\n');
  os << src.printdata.first.x << SPACE << src.printdata.first.y 
  << SPACE << src.printdata.second <<NEWLINE;
  return os;
  }

  template<typename CHARTYPE>
  std::basic_istream<CHARTYPE>& operator>> <CHARTYPE, std::pair<NCOORDS::coords, int> >(std::basic_ostream<CHARTYPE>& os,
  const NAMESPACE::RawDataReader<std::pair<NCOORDS::coords, int> >& src){
  os >> src.readdata.first.x      //double
  >> src.readdata.first.y          //double
  >> src.readdata.second;               //'int'
  return os;
  }
*/


namespace NCOORDS{

  enum PathOverlap{
    PATH_OVERLAP_NONE=0,
    PATH_OVERLAP_INTERSECT=1,
    PATH_OVERLAP_INSIDE=2,
    PATH_OVERLAP_OUTSIDE=3
  };
 enum PathCombine{
    PATH_COMBINE_OR,
    PATH_COMBINE_AND,
    PATH_COMBINE_NOTAND,
    PATH_COMBINE_DIFF
  } ;

  struct PathEmpty:public std::exception{};

  template <typename CHARTYPE>
  bool curstyle_svggood(CHARTYPE s);

  template <typename CHARTYPE>
  int curstyle_to_clicktype(CHARTYPE s);

  class PathFragment;
  class SubPath;

  enum PathCleanerSettings{
    SEGMENT_TOLERANCE=1,
    SEGMENT_MIN_LENGTH=2,
    SEGMENT_MIN_DEVIATION_ABS=3,
    SEGMENT_MAX_DEVIATION_REL=4,
    SEGMENT_MIN_ROT_SHORT=5,
    SEGMENT_MIN_ROT_LONG=6
  };
  class simple_click_history;

  enum{
    SELECTION_COPY_FROM_COORDS=1,
    SELECTION_COPY_TO_COORDS=2
  };

  template <typename T>
  std::deque<T*> as_pointer_container(std::deque<T>& container){
    std::deque<T*> res;
    for(typename std::deque<T>::iterator w = container.begin();w!= container.end();w++){
      res.push_back(&(*w));
    }
    return res;
  }

  template <typename CHARTYPE, typename TRAITS>
  std::basic_ostream<CHARTYPE, TRAITS>& operator<<(std::basic_ostream<CHARTYPE, TRAITS>&, const std::pair<coords, int>& point);
  
  template <typename CHARTYPE, typename TRAITS>
  std::basic_ostream<CHARTYPE, TRAITS>& operator<<(std::basic_ostream<CHARTYPE, TRAITS>& out, const std::pair<coords*, int>& point);

  template <typename CHARTYPE, typename TRAITS>
  std::basic_ostream<CHARTYPE, TRAITS>& operator<<(std::basic_ostream<CHARTYPE, TRAITS>& os, const simple_click_history& clicks);
  
  template <typename CHARTYPE, typename TRAITS>
  std::basic_istream<CHARTYPE, TRAITS>& operator>>(std::basic_istream<CHARTYPE, TRAITS>& ifs, std::deque<simple_click_history>& dest);

  template <typename CHARTYPE, typename TRAITS>
  std::basic_istream<CHARTYPE, TRAITS>& operator>>(std::basic_istream<CHARTYPE, TRAITS>& s_in, std::pair<coords, int>& point);
  //template <typename T>
  class RadialLine_Generator{
  protected:
    coords centre;

    unsigned int count;

  public:
    RadialLine_Generator(const coords& pt, unsigned int  gen_count):centre(pt),
								    count(gen_count){}
    Linear_function<double> get_nth_path(unsigned int n)const{
      coords satellite(centre.x, centre.y +1);
      centre.rotate(&satellite,get_angle(n));
      Segment<coords> seg(centre, satellite);
      return seg.get_function();
    }

    double get_angle(unsigned int n)const{
      if(n > count){n = n% count;}
      return (n * M_PI) /count;
    }

  };  
  simple_click_history& operator*=( simple_click_history& path, const coords& scalevalue);
  simple_click_history& operator*=( simple_click_history& path,double scaleval);
  simple_click_history& operator -=(simple_click_history& path, const coords& minuscoords);

  typedef Segment<coords*> LineSegment;
  typedef Segment<const coords*> const_LineSegment;
  typedef Segment<coords> CopyLineSegment;

  /**
@class SegmentWalker some functions for eliminating excess points
@todo: integrate with SubPath
   */
  class SegmentWalker:
    public std::unary_function<const std::pair<coords, int>&, bool> {
  public:
    typedef std::pair<coords, int> value_type;
    //maybe a separate segment_iterator class
    //would be useful
    typedef std::deque<value_type>::iterator iterator;
    typedef std::deque<value_type>::const_iterator const_iterator;

    //typedef simple_click_history::iterator iterator;
    //typedef simple_click_history::const_iterator const_iterator;

  protected:
    simple_click_history* container;
    LineSegment cur_segment;
    //tunables - need work/testing
    double tolerance;
    double min_length;
    double min_deviation_abs;
    double max_deviation_rel;
    double min_rot_short;
    double min_rot_long;

    static double tolerance_default;
    static double min_length_default;
    static       double min_deviation_abs_default;
    static       double max_deviation_rel_default;
    static       double min_rot_short_default;
    static       double min_rot_long_default;
       

    static void set_default(PathCleanerSettings which, double val);
  public:
    void reset_to_defaults();
    void set_param(PathCleanerSettings which, double val);

    coords* find_next(iterator currn);
    coords* find_previous(iterator currn);
    const coords* find_previous(const_iterator currn)const;
    bool too_insignificant(const LineSegment& current, coords* midpoint)const;

    void get_tan_angles(const LineSegment& current, coords* midpoint,
			double& a_angle, double& b_angle,
			double& cur_angle)const;

    bool too_straight(const LineSegment& current, coords* midpoint)const;
    double get_min_deviation_abs()const{return min_deviation_abs;}
    void set_min_deviation_abs(double d);
    double too_short(const LineSegment& seg)const;
    void set_min_length(double m, bool force = false);

    SegmentWalker(simple_click_history* owner);
    SegmentWalker(simple_click_history* owner,double tol, double minlen,
		  double min_dev, double max_dev_rel,
		  double min_short=0.1,double min_long=0.01);

    double get_min_rot_long()const{return min_rot_long;}
    double get_min_rot_short()const{return min_rot_short;}

    void set_min_rot_short(double m);
    void set_min_rot_long(double m);

    double get_max_deviation_rel()const{return max_deviation_rel;}
    void set_max_deviation_rel(double d);

    bool operator()(const std::pair<coords, int>& click);
      
    size_t clean_path();

    
    std::deque<coords> find_intersections(const Linear_function<double>& fn,
					  bool implicitly_closed=true)const;
    
    ///problem is the donut
    ///a shape that is inside without intersecting
    ///not to mention 'almost donut' shapes
    ///it looks necessary to  be able to create the OR_path of each SubPath,
    ///or actually maybe all the shapes it defines
    ///either by copying or iterate over the raw paths
    std::deque<std::pair<coords, Segment<coords*> > >
    find_intersections_with_segment(const Linear_function<double>& fn,
				    bool implicitly_closed=true)const;

    /** @return =>counts of intersections on either side of fn .
@todo also return count where (point.get_distance(&  centre)<coords::min_radius())  */
    template <typename T>
    std::pair<int,  int> get_intersection_count(const Linear_function<T>& fn,
						const coords& centre,
						bool implicitly_closed=true)const{
      double minrad(coords::min_radius());
#if CHECK_SUBPATH_SECTION
      if(fn.get_distance(centre)> minrad){
	throw xn_Error("point not on function");
      }
#endif
      std::deque<coords> inters= find_intersections(fn,implicitly_closed);
      std::pair<int,  int> res(0,0);
      int bullseyes(0);
      for(std::deque<coords>::const_iterator w = inters.begin(); w != inters.end(); w++){
	if((*w).get_distance(&centre) > minrad){
	if(!fn.vertical()){
	  if((*w).x < centre.x){res.first++;}
	  else if((*w).x > centre.x){res.second++;}
	  else{
	    std::cerr<<"?possible for pt.x and centre.x  to be equalwhen not vertical?, and pts separated?\n";
	    bullseyes++;
	  }
	}
	else{
	  ///@todo does this accord with any other methods separating points on either side of a line?
	  if((*w).y < centre.y){res.first++;}
	  else if((*w).y > centre.y){res.second++;}
	  else{std::cerr<<"wha?...  ys the same despite seopartion of > "<<minrad<<" betweeen pts\n";
	    bullseyes++;}
	}
      }
      else{bullseyes++;}
    }
      return res;
    }

  };

  /////////////////
  //TODO
  //chop all the subpaths at each intersection
  //(ie adding a new coords at the intersection if required)
  //organising a list of subpaths according to:
  //rule 1: joins at common end point(should be no  common points which aren't endpoints: ERROR otherwise)
  //
  //operator|(), operator|=()
  //rule2: only include subpaths which are 'outside'
  //operator^() operator^=()
  //rule2: only include subpaths which are 'outside', 
  //or which border space an odd number of path crossingss inside
  //operator&() operator&=()
  //rule2:only include subpaths which  border space an even number of path crossingss inside
  simple_click_history operator|(const simple_click_history& left, const simple_click_history& right);
  simple_click_history operator&(const simple_click_history& left, const simple_click_history& right);
  simple_click_history operator^(const simple_click_history& left, const simple_click_history& right);
simple_click_history operator-(const simple_click_history& left, const simple_click_history& right);
  simple_click_history& operator|=(simple_click_history& left, const simple_click_history& right);
  simple_click_history& operator&=(simple_click_history& left, const simple_click_history& right);
  simple_click_history& operator^=(simple_click_history& left, const simple_click_history& right);
  simple_click_history& operator-=(simple_click_history& left, const simple_click_history& right);
  ////////////////
  /**
@class simple_click_history
a structure to hold path data and memberss to manipulate it.
@todo: simplify the API, and make it all work together
   */
  class simple_click_history: public std::deque<std::pair<coords ,int> >{
  
  protected:
    friend class SegmentWalker;
    std::string pathid;
    SegmentWalker path_cleaner;

  public:
    typedef std::deque<std::pair<coords, int> > container_type;
    typedef container_type::value_type value_type;
    typedef container_type::pointer pointer;
    typedef container_type::const_pointer const_pointer;
    typedef container_type::reference reference;
    typedef container_type::const_reference const_reference;
    typedef container_type::size_type size_type;
    typedef container_type::difference_type difference_type;
    typedef container_type::iterator iterator;
    typedef container_type::const_iterator const_iterator;
   
    typedef cRect box_type;
    typedef simple_click_history(*process_copy_function)(const simple_click_history&);
    typedef void(*process_function)(simple_click_history&);
  protected:
    static process_copy_function flatten_fn;
  public:

    static void set_flattener(process_copy_function f){
      flatten_fn =f;
    }

    simple_click_history();


    template <class ITER>
    simple_click_history(ITER start, ITER finish, 
			 std::string path_id= std::string() );

    void set_pathid(std::string s=std::string());

    std::string get_pathid()const;
    void cleanup_path(){
      path_cleaner.clean_path();
    }
    simple_click_history(const simple_click_history& copied);

    virtual ~simple_click_history();
    size_t copy_cleaned_path(simple_click_history& dest);

    virtual void load(coords* npoints, int cliktype = LINE_CLICK);
    
    virtual void load(const coords& point, int cliktype = LINE_CLICK){
      push_back(point, cliktype);
    }
    virtual void load(const std::pair<coords, int>& point){
      push_back(point);
    }
    virtual void load(std::pair<coords* , int > point);

    simple_click_history& operator=(const simple_click_history& copied);
   
    unsigned  int get_count()const{return this->size();}
    void push_back(const NCOORDS::coords& click ,int cliktype= LINE_CLICK);
    void push_back(const std::pair<NCOORDS::coords, int>& click);
    
    void push_front(const NCOORDS::coords& click,int cliktype= LINE_CLICK);
    void push_front(const std::pair<NCOORDS::coords, int>& click);

    void push_back(const NCOORDS::coords* click ,int cliktype= LINE_CLICK){
      return push_back(*click,  cliktype);
    }

    void push_back(const std::pair<NCOORDS::coords*, int>& click){
      return this->push_back(*(click.first), click.second);
    }
    
    void push_front(const NCOORDS::coords* click,int cliktype= LINE_CLICK){
      return push_front(*click, cliktype);
    }
    void push_front(const std::pair<NCOORDS::coords*, int>& click){
      return this->push_back(*(click.first), click.second);
    }

    virtual void pop_front(); 
    virtual void pop_front(size_type n);
    virtual void pop_back();
	  
    virtual void pop_back(size_type n);
    ///
    //TODO: check(* current_iterator).second  &(drawing_click())
    std::pair<iterator, coords*> get_subpath_start(iterator start, 
						   iterator current);
    ///unused - remove
    LineSegment pre_segment(iterator rangestart, iterator current_iter, 
			    iterator sentinel);

    ///unused - remove
    LineSegment post_segment( iterator rangestart, 
			      iterator current_iter, 
			      iterator sentinel);
      
    LineSegment get_reflectaxis(ncoords& anchor, std::pair<coords,coords>& outproxy ){
      outproxy.first=(coords&)anchor;
      outproxy.second= get_first_point(MOVE_CLICK);
      return  LineSegment(&(outproxy.first),&(outproxy.second)); 
    }
    //ostream etc
    void write_data(const simple_click_history& path,std::string& filename); 
    simple_click_history read_data(std::string& fname);

    friend std::ostream& operator<<(std::ostream& out, const simple_click_history& clicks);

    double get_area()const;
    void get_extents(cRect& outval)const;

    bool points_selected()const;
    static simple_click_history combine(const simple_click_history& left,
					const simple_click_history& right,
					int flag= PATH_COMBINE_OR){
      simple_click_history result(left);
      return result.combine_with(right, flag);
    }



  /**
implementation of boolean operations for shapes
|=, &=, and ^= call this directly
|, &, and ^ do something like:
@code
int flag=PATH_COMBINE_OR;
simple_click_history res(left);
return res.combine_with(right, flag);
@endcode
@arg other => rhs argument: a path
(NB implicit 'this' arg is the left hand side)
@arg flag => one of PATH_COMBINE_OR, PATH_COMBINE_AND, PATH_COMBINE_NOTAND, or PATH_COMBINE_DIFF : CONTROLS THE kind of combination operation
   */
    simple_click_history& combine_with(const simple_click_history& other, 
				       int flag= PATH_COMBINE_OR);

    //void get_axis(std::pair<coords, coords>& outval;
    template< typename T>
    std::pair<int, int> get_intersection_count(const Linear_function<T>& fn,
					       const coords& centre,
					       bool implicitclosepath)const{
      return path_cleaner.get_intersection_count(fn, centre,  implicitclosepath);
    }
    //is some selected point within tolerance units of loc?
    bool draggable_selection(const coords& loc,
			     double tolerance)const;

    simple_click_history cut_path(unsigned int position, 
				  unsigned int point_no, bool relative = true);
    void append_path(simple_click_history& subpath, bool relative =true);
    void prepend_path(simple_click_history& subpath, bool relative =true);
    void insert_path(simple_click_history& subpath, unsigned int position, bool relative =true);
    
    void translate(const coords& offset);
    
    std::vector<const coords*> sorted_byX_const()const;
    std::vector<const coords*> sorted_byY_const()const;
    std::vector< coords*> sorted_byX();
    std::vector<coords*> sorted_byY();
    const coords& find_closest(const coords& searchitem)const;
   
    simple_click_history& operator+=(const coords& offset);
    simple_click_history& rotate(double	radians){
      coords c_pt=get_centre();
      return rotate(c_pt, radians);
    }

    iterator find_subpath_start(iterator subpathlast);
    const_iterator find_subpath_start(const_iterator subpathlast)const;

    simple_click_history& rotate(  const coords& origin, double	radians);
    simple_click_history& rotate_around_begin(double radians);
    void scale(double scale_x, double scale_y, 
	       const coords* centre = NULL, bool use_selection = true);

    size_t consolidate_internal_selection(int mod = SELECTION_COPY_FROM_COORDS,
					  bool clear_source=true);
    void scale(const coords& scaler,  
	       const coords* centre = NULL, 
	       bool use_selection = true);
    
    template <typename ITER>
    bool valid_iterator(ITER checkiter)const{
      const_iterator iter=checkiter;
      ptrdiff_t front=iter - begin();
#if CHECK_SUBPATH_SECTION
      if(front <0){std::cerr<<"valid_iterator: iter b4 begin();"<<std::endl;}
#endif
      ptrdiff_t back=end() - iter;
#if CHECK_SUBPATH_SECTION
      if(back<0){std::cerr<<"valid_iter() fail: iter beyond end()"<<std::endl;}
#endif
      return (front >=0) && (back >= 0);
    }

    //get copy of all the subpaths as discrete paths
    std::deque<simple_click_history> subpaths_as_paths()const;

    template<typename CHARTYPE, typename TRAITS>
    std::basic_istream<CHARTYPE, TRAITS>& read_datastream(std::basic_istream<CHARTYPE, TRAITS>& data);

    simple_click_history copy_selected();

    const coords&  get_first_point(int flag)const;

    /**
is ITER not a pointer to some curve point or handle? .
tests that (*iter).second isn't BEZIER_*
@arg iter some iterator that derefs to a std::pair<coords, int>:
ie a simple_click_history::.*iterator
@return => a bool: equiv to =>
`return  ! (((*iter).second & BEZIER_CLICK) ||
	 ((*iter).second & BEZIER_HANDLE))'
     */
    template<typename ITER>
static bool iter_is_flat(ITER iter){
      if(((*iter).second & BEZIER_CLICK) ||
	 ((*iter).second & BEZIER_HANDLE)){return false;}
      // could check for exotic values of (*iter).second
      return true;
}
    /**

tests iter_is_flat(iter), as well as if ((*iter).second &(LINE_CLICK | MOVE_CLICK  |CLOSE_PATH). If successful, (*iter).first is definitely a simple part of a path made of straight segments
@return =>bool:rv=(iter_is_flat(iter) && ((*iter).first & STRICTLY_FLAT_CLICK));
@arg iter some iterator that derefs to a std::pair<coords, int>:
ie a simple_click_history::.*iterator
     */


template<typename ITER>
static bool iter_is_strictly_flat(ITER iter){
  return iter_is_flat(iter) && ((*iter).first & STRICTLY_FLAT_CLICK);
}
    /**
@return => true if path contains no BEZIER_.* .
ie: path contains no curved segments
     */
    bool is_flat()const{
      for (const_iterator w = begin();w!=end();w++){
	if(!(iter_is_flat(w))){
	  return false;
	}
      }
      return true;
    }

    void clear_short_segments();

    /**
calls a function pointer from cairo
@return a copy of path, flattening any curved segment into a series of straight segments. 
@bug terminates with out of memory
     */
    simple_click_history flatten_copy()const{
      if(!flatten_fn){
	throw xn_Error("set a function pointer to flatten path - simple_click_history::flatten_fn");
      }
      simple_click_history res= (*flatten_fn)(*this);;
      res.clear_short_segments();
      return res;
    }

    void test_common_segment(){
      if(!size()){return;}
      Segment<coords> firsty((*(begin())).first, (*(begin()+1)).first);
      for(const_iterator w = begin()+1; w!= end();w++){
	Segment<coords>  next((*(w-1)).first, (*w).first);
	Segment<coords*> pseg((firsty.first), (firsty.second));
	std::pair<int,Segment<coords> > r1=pseg.common_segment(next);
	std::pair<int,Segment<coords> > r2=firsty.common_segment(next);
	//std::pair<int,Segment<coords> > r1=r2;//firsty.common_segment(pseg);
	std::pair<int,Segment<coords> > r3=next.common_segment(firsty);
	std::cerr<<"r1 "<<r1.first<<"\nr2 "<<r2.first<<"\nr3 "<<r3.first<<std::endl;
      }
    }
	
    simple_click_history reflect_on_pt(const coords& centre)const{
      /**
	 @fn reflect_on_pt(const coords& centre)
	 reflect path on axis between centre and the first 'MOVE_CLICK'
	 @param centre -> first point in axis(2nd is first point in path)

      */ 
      Segment<const coords> axis_seg(&centre, &(get_first_point(MOVE_CLICK)));
      Linear_function<double> axis = axis_seg.get_function();
      return reflect(axis);
    }
    
    coords get_centre()const;
    
    simple_click_history reflect(const Linear_function<double>& axis)const;
    /**
TODO: consolidate all the .*_click methods etc in one easily included location
*/
    template<typename ITER>
    static bool is_cairo_click(ITER iter){
      return cairo_click(*iter);
    }
 
    template<typename ITER>
    static bool is_click_selected(ITER iter){
      return click_selected(*iter);
    }

    template<typename ITER>
    static bool is_drawing_click(ITER iter){
      return drawing_click(*iter);
    }

    template<typename ITER>
    static bool is_handle_click(ITER iter){
      return handle_click(*iter);
    }

    template<typename ITER>
    static bool is_closepath_click(ITER iter){
      return closepath_click(*iter);
    }
    bool point_within(const coords& pt, bool closesubpaths=true,
		      unsigned int segcount=256)const;
    std::deque<SubPath> as_subpaths(size_type minlength=3);
std::deque<SubPath*> as_ptr_subpaths(size_type minlength=3);
    //std::deque<const SubPath> as_subpaths()const;

    ///////////////////////////////////
    //TODO:
    bool point_on_path(const coords& pt, double tolerance)const;
    bool segment_on_path(const Segment<coords*>& segment, double tolerance)const;
    PathFragment* common_subpath(const simple_click_history& otherpath);

    void  write_human_stream_file(const std::string& s_filename){
      std::ofstream f(s_filename.c_str());
      write_human_stream(f);
      f.close();
    }
    /////////////////////////////////////
 template <typename CHARTYPE, typename TRAITS>
  std::basic_ostream<CHARTYPE, TRAITS>& 
 write_human_stream(std::basic_ostream<CHARTYPE, TRAITS>& os);

    template <typename CHARTYPE, typename TRAITS>
    std::basic_ostream<CHARTYPE, TRAITS>& write_datastream(std::basic_ostream<CHARTYPE, TRAITS>& os);
    
    template <typename CHARTYPE>
    static std::basic_ostream<CHARTYPE>& write_data_to_stream(std::basic_ostream<CHARTYPE>& os, 
							      const simple_click_history& path){
      return path.write_datastream(os);
    }
    template <typename CHARTYPE>
    static std::basic_istream<CHARTYPE>& read_data_from_stream(std::basic_istream<CHARTYPE>& os, 
							       const simple_click_history& path){
      //return path.read_datastream(os);
      os >>path;
      return os;
    }
  };
   
  template <typename PATH>
  class Section;//between 2 adjacent intersections when comparing subpaths
  template <typename PATH>
  class Intersection;
template <typename PATH>
  class SectionShape;

 
  /*
  template <typename ORDERED_CONTAINER>
  class TrackerBase{
  public:
typedef ORDERED_CONTAINER container_type;
 typedef typename container_type::iterator iterator ;
 typedef typename container_type::const_iterator const_iterator ;
    typedef typename container_type::reference reference;
 typedef typename container_type::const_reference const_reference;
   
  protected:
container_type container;
  public:
    TrackerBase(){}
    TrackerBase(const container_type& cc):container(cc){}
    virtual ~TrackerBase(){}

    virtual reference at(size_t pos){
      return container.at(pos);
    }
virtual const_reference at(size_t pos)const{
      return container.at(pos);
    }
    virtual bool used(size_t pos)const{return false;}

 static unsigned long trackersize(unsigned long c_size){
    unsigned long sz= sizeof(unsigned long);
    unsigned long val = ((c_size /sz) +1)*sz;
    return val >= 1024? val:1024;  
}

  };

 

  template <typename ORDERED_CONTAINER, unsigned int MAXSZ>
  class UseTracker:public TrackerBase<ORDERED_CONTAINER>{
  public:
    typedef TrackerBase<ORDERED_CONTAINER> base_type;
    typedef std::bitset<MAXSZ> bitset_type;
    typedef ORDERED_CONTAINER container_type;
    typedef typename container_type::iterator iterator ;
 typedef typename container_type::const_iterator const_iterator ;
    typedef typename container_type::reference reference;
 typedef typename container_type::const_reference const_reference;
   protected:
    using base_type::container;
    bitset_type tracker;
  public:
    virtual ~UseTracker(){}
    UseTracker():tracker(0){}
    UseTracker(const container_type& c):base_type(c), tracker(0){}
    void use(size_t pos){
      check_position(pos);
      tracker.set(pos);
    }
    UseTracker& operator=(const container_type& c){
      if(c.size() > maxsize()){
	throw xn_Error("assigning to oversize container");
      }
      container= c;
      tracker.reset();
      return *this;
    }
    void free_use(size_t pos){
      check_position(pos);
      tracker.reset(pos);
    }

    size_t maxsize()const throw(){return MAXSZ;}

    void push_back(const_reference objref){
      if(container.size() >=MAXSZ){
	std::ostringstream ostmp;
	ostmp <<"container already at maximum size: "<<MAXSZ<<"can't add any more elements";
	throw xn_Error(ostmp.str());
      }
      container.push_back(objref);
    }

    \/\**if it returns all should be well.
    /// check container size  not greater than bitset size
    /// and throw an error on failure
    \*\/
    void check_container_size()const throw (xn_Error){
      if(container.size() > MAXSZ){
	std::ostringstream ostmp;
	ostmp << "container size : "<< container.size()
	      <<"\n is greater than bitset MAXSZ: "<< MAXSZ;
	throw xn_Error(ostmp.str());
      }
    }
/\**if it returns all should be well.
    /// check against container size  and bitset size
    /// and throw an error on failure
    *\/
    void check_position(size_t pos)const throw (xn_Error){
      if(pos >=MAXSZ || pos>= container.size()){
	std::ostringstream ostmp;
	if(pos>= container.size()){
	  ostmp<< "position => "<< pos<<"<= out of range (container size is "
	       <<container.size()<<")";
	}
	if(pos >=MAXSZ){
	  ostmp<< "position => "<< pos<<"<= out of range (Bitset MAXSZ is "
	       <<MAXSZ<<").";
	}
	throw xn_Error(ostmp.str());
      }
    }
    bool used(size_t pos)const{
      check_position(pos);
      return tracker[pos];
    }

    bool exhausted()const{
      for(unsigned int i = 0; i < container.size(); i++){
	if(!tracker[i]){return false;}
      }
      return true;
    }

    reference at(size_t pos){
      check_position(pos);
      if(used(pos)){
	throw xn_Error("this position already used");
      }
      return container.at(pos);
    }


    bool in_range(size_t pos)const{
      return pos <MAXSZ && pos< container.size();
    }

     
  };
  
  template <typename C>
  TrackerBase<C>* create_tracker(const C& container){
    size_t tsz=TrackerBase<C>::trackersize();
    return  new UseTracker<C, tsz>(container);
  }
  */
  

  /**
sorting functor which ranks Intersection*. 
ranks Intersections according to their proximity to a reference point
(uses ClosestPoint to delegate the work)
@arg left => first Intersection to be compared
@arg right => second Intersection to be compared
@return =>true if left 'wins'
   */

  template <typename PATH>
  class ClosestIntersection:
    public std::binary_function<const Intersection<PATH>* const&,
				const Intersection<PATH>* const&, bool>{
  protected:
    ClosestPoint ranker;
  public:
    ClosestIntersection(const coords& pt):ranker(pt){}
    bool operator()(const Intersection<PATH>* const& left,
		    const Intersection<PATH>*const& right)const{
      coords leftinter= left->get_intersection();
      coords rightinter= right->get_intersection();
      return ranker(leftinter, rightinter);
    }
  };

  /**
sorting functor which ranks std::pair<Intersection*, int> . 
ranks Intersections( in a pair with an int)according to their proximity to a reference point
only left.first and right.first are considered(uses ClosestIntersection to delegate the work)
@arg left => first pair to be compared
@arg right => second pair to be compared
@return =>true if left 'wins'(or is 'less than' right)
   */

  template <typename PATH>
  class ClosestIntersectionPair:
    public std::binary_function<const std::pair<Intersection<PATH>*, int>&,
				const std::pair<Intersection<PATH>*, int>&, bool>{
  protected:
    ClosestIntersection<PATH> ranker;
  public:
    ClosestIntersectionPair(const coords& pt):ranker(pt){}
    bool operator()(const std::pair<Intersection<PATH>*, int>& left,
		    const std::pair<Intersection<PATH>*, int>& right)const{
      return ranker(left.first, right.first);
    }
  };
  
//betwe


  struct relative_click_history: public simple_click_history{
    relative_click_history(){}
    relative_click_history(const simple_click_history& path){
      relpath(path);
    }
    
    void relpath(const simple_click_history& absp);

    simple_click_history abspath()const;
  };
    
  simple_click_history operator-(const simple_click_history& left, const coords& right);
  simple_click_history operator+(const simple_click_history& left, const coords& right);

  //simple_click_history& operator-=(simple_click_history& left, const coords& right);
  //simple_click_history& operator+=(simple_click_history& left, const coords& right);
  

  template <typename CHARTYPE, typename TRAITS>
  std::basic_ostream<CHARTYPE, TRAITS>& operator<<(std::basic_ostream<CHARTYPE, TRAITS>& os, const std::pair<coords, int>& point){
    std::locale loc;
    CHARTYPE SPACE = std::use_facet<std::ctype<CHARTYPE> >(loc).widen(' ');
    //CHARTYPE NEWLINE = std::use_facet<std::ctype<CHARTYPE> >(loc).widen('\n');
    os << point.first.x <<SPACE  << point.first.y << SPACE 
       << point.second ;//<<NEWLINE;
    return os;
  }
  
  template <typename CHARTYPE, typename TRAITS>
  std::basic_ostream<CHARTYPE, TRAITS>& operator<<(std::basic_ostream<CHARTYPE, TRAITS>& out, const std::pair<coords*, int>& point){
    std::pair<coords, int> pointcopy(*(point.first), point.second);
    out << pointcopy;
    return out;
  }

  template <typename CHARTYPE, typename TRAITS>
  std::basic_ostream<CHARTYPE, TRAITS>& operator<<(std::basic_ostream<CHARTYPE, TRAITS>& os, const simple_click_history& clicks){
    //not symmetrical with operator>>
    //does space separatedvalues, not svg style
    typedef simple_click_history::const_iterator const_iterator ;
    std::locale loc;
    CHARTYPE STRUDEL=std::use_facet<std::ctype<CHARTYPE> >(loc).widen('@');
    CHARTYPE NEWLINE = std::use_facet<std::ctype<CHARTYPE> >(loc).widen('\n');
    os << STRUDEL << clicks.get_pathid()<< NEWLINE;
    for(const_iterator w = clicks.begin();w != clicks.end();w++){
      os << *w << NEWLINE;
    }
    return os;
  }

  /// how many space separated numbers  on line?
  template <typename CHARTYPE, typename TRAITS, typename AL>
  signed char numbercount_line(const std::basic_string<CHARTYPE, TRAITS, AL>& line);



  template <typename CHARTYPE, typename TRAITS>
  std::basic_istream<CHARTYPE, TRAITS>& operator>>(std::basic_istream<CHARTYPE, TRAITS>& s_in, std::pair<coords, int>& point){
    s_in >> point.first.x ;
    s_in >>  point.first.y ;
    s_in >>  point.second ;
    return s_in;
  }

 
 
 

}//namespace NCOORDS

  namespace xNDRAW{
    //refugees from clickhistory

  }
#include "clickhistory.tcc"

#endif /**X3DRAW_CLICKHISTORY_H**/
