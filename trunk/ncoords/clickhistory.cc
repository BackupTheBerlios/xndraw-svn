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



#include "clickhistory.h"
#include "pathsection.h"
#include "subpath.h"
#include <iterator>
#include <fstream>
//#include <cassert>
//using namespace xNDRAW;
namespace NCOORDS{
  double  SegmentWalker::min_rot_short_default(0.05);//radians(approx 3 deg)
  double SegmentWalker::min_rot_long_default(0.01);
  double  SegmentWalker::tolerance_default(10.0);
  double  SegmentWalker::min_deviation_abs_default(0.75);

  double  SegmentWalker::max_deviation_rel_default(0.25);
  double  SegmentWalker::min_length_default(1.);

  simple_click_history::process_copy_function simple_click_history::flatten_fn(NULL);

  coords simple_click_history::get_centre()const{
    cRect rect;
    get_extents(rect);
    return rect.get_centre();
  }
  bool simple_click_history::point_within(const coords& pt, 
					  bool closesubpaths, 
					  unsigned int segcount)const{
    simple_click_history* pnotconst= const_cast<simple_click_history*>(this);
    std::deque<SubPath> subs = pnotconst->as_subpaths();
    for(std::deque<SubPath>::const_iterator w = subs.begin();w != subs.end(); w++){
      if((*w).point_within(pt, closesubpaths,segcount )){return true;}
    }
    return false;
  }
  std::deque<SubPath> simple_click_history::as_subpaths(simple_click_history::size_type minlength){
    std::deque<SubPath> res;
    iterator prev= begin();
    bool firstmove(false), ignore(true);
    for(iterator w = begin(); w!= end(); w++){
      if(((size_type)(w-prev)) >= minlength){ignore=false;}
      if((*w).second& MOVE_CLICK){
	if(!firstmove){ firstmove= true; }
	else if(!ignore){ 
	  res.push_back(SubPath(this, prev, w)); ignore=true;
	}
	prev = w;
      }
    }
    if(!ignore){
      res.push_back(SubPath(this, prev,end()));
    }
    return res;
  }

std::deque<SubPath*> 
simple_click_history::as_ptr_subpaths(simple_click_history::size_type minlen){
    std::deque<SubPath*> res;
    iterator prev= begin();
    bool firstmove(false), ignore(true);;
    for(iterator w = begin(); w!= end(); w++){
      if(((size_type)(w-prev)) >= minlen){ignore=false;}
      if((*w).second& MOVE_CLICK){
	if(!firstmove){	  firstmove= true;	}
	else if(!ignore){
	    res.push_back(new SubPath(this, prev, w));
	    ignore=true;
	  }
	prev = w;
      }
    }
    if(!ignore){
      res.push_back(new SubPath(this, prev,end()));
    }
    return res;
}

  const coords&  simple_click_history::get_first_point(int flag)const{
    for(const_iterator w = begin();w!=end();w++){
      if((*w).second & flag){
	return (*w).first;
      }
    }
    throw xn_Error("no such point");
  }

  simple_click_history simple_click_history::reflect(const Linear_function<double>& axis)const{
    simple_click_history reflection;
    for(const_iterator w =  begin();w!= end(); w++){
      coords pt = axis.opposite_pt((*w).first);
      reflection.push_back(std::pair<coords, int>(pt, (*w).second));
    }
    return reflection;
  }

  //get copy of all the subpaths as discrete paths
  std::deque<simple_click_history> 
  simple_click_history::subpaths_as_paths()const{
    std::deque<simple_click_history> res;
    simple_click_history* current(NULL);
    for(const_iterator w= begin();w != end(); w++){
      const std::pair<NCOORDS::coords,int>& ppr=*w;
      if(ppr.second== MOVE_CLICK){
	res.push_back(simple_click_history());
	current=&(*(res.rbegin()));
      }
      if(current){
	//ignore anything before first MOVE_TO
	current->push_back(ppr);
      }
    }
    return res;
  }

  simple_click_history& operator*=( simple_click_history& path, 
				    const coords& scalevalue){
    path.scale(scalevalue);
    return path;
  }

  simple_click_history& operator*=( simple_click_history& path,double scaleval){
    coords scaler(scaleval, scaleval);
    path *= scaler;
    return path;
  }
  simple_click_history& operator -=(simple_click_history& path, 
				    const coords& minuscoords){
    for(simple_click_history::iterator w=path.begin(); w !=path.end(); w++){
      (*w).first -= minuscoords;
    }
    return path;
  }
  //
  std::pair<simple_click_history::iterator, coords*> 
  simple_click_history::get_subpath_start(simple_click_history::iterator start, 
					  simple_click_history::iterator current){
    std::pair<iterator, coords*> result(current, NULL);
    if(current==start){return result;/*fail*/}
    while(current !=start){
      current--;
      int state= (*current).second;
      if((state & MOVE_CLICK) ||
	 (current==start &&( state & (LINE_CLICK | BEZIER_CLICK)))){
	result.first= current;
	result.second = &((*current).first);
	return result;//success
      }
    }
    //result.first=current;
    return result;//fail
  }
  simple_click_history::iterator 
  simple_click_history::find_subpath_start(simple_click_history::iterator subpathlast){
    for(;subpathlast !=begin();subpathlast--){
      if((*subpathlast).second && MOVE_CLICK){
	return subpathlast;
      }
    }
    return subpathlast;
  }
  simple_click_history::const_iterator 
  simple_click_history::find_subpath_start(simple_click_history::const_iterator subpathlast)const{
    for(;subpathlast !=begin();subpathlast--){
      if((*subpathlast).second && MOVE_CLICK){
	return subpathlast;
      }
    }
    return subpathlast;
  }

  void simple_click_history::scale(const coords& scaler,  const coords* centre,
				   bool use_selection){
    use_selection= points_selected();
 
    coords offset;
    register bool bez_previous(false);
    if(centre){
      offset = *centre;
    }
    else{
      cRect box;
      get_extents(box);
      offset = box.get_centre();
    }
    for(iterator w = begin();w!=end();w++){
      if(!use_selection || ((*w).second & CLICK_SELECTED)
	 || (bez_previous &&((*w).second & BEZIER_HANDLE))){
	(*w).first -= offset;
	(*w).first *= scaler;
	(*w).first += offset;
      }
      if((((*w).second & CLICK_SELECTED) &&((*w).second & BEZIER_CLICK)) || 
	 (bez_previous && ((*w).second & BEZIER_HANDLE))){
	bez_previous=true;
      }
      else{
	bez_previous=false;
      }
    }
  }

  bool simple_click_history::draggable_selection(const coords& loc,
						 double tolerance)const{
    for(const_iterator w = begin();w != end(); w++){
      if ((*w).second & CLICK_SELECTED && drawing_click(*w)){
	if((*w).first.get_distance(&loc) <= tolerance){return true;}
      }
    }
  
    return false;
  }
  bool simple_click_history::points_selected()const{
    //some cache variable?- but it needs to be kept up to date...
    for(const_iterator w = begin();w != end(); w++){
      if ((*w).second & CLICK_SELECTED){return true;}
    }
    return false;
  }
  LineSegment 
  simple_click_history::pre_segment(simple_click_history::iterator rangestart, 
				    simple_click_history::iterator current_iter,
				    simple_click_history::iterator end_sentinel){
    if(!(is_drawing_click(current_iter)|| is_closepath_click(current_iter))){
      return LineSegment(NULL, NULL);/*invalid*/
    }
    coords* firstone(NULL), *secondone(NULL);
    if(is_drawing_click(current_iter)){
      secondone=&((*current_iter).first);
    }
    else{ 
      std::pair<iterator, coords*> subpathstart = get_subpath_start(rangestart, current_iter);
      secondone = subpathstart.second;
      //secondone=&(*current_iter).first;
    }
    while(current_iter != end_sentinel){
      --current_iter;
      if(is_drawing_click(current_iter)|| is_closepath_click(current_iter)){
	firstone=&((*current_iter).first);
	//don't use LineSegment after  any reallocation of path,
	//just as an object for duration of one loop/call.
 
	return LineSegment(firstone,  secondone);
      }
    }
    return LineSegment(firstone,  secondone);//if LineSegment::first is NULL,
    //@param  secondone is the first drawing click in paath(and possibly begin()
  }


  bool simple_click_history::point_on_path(const coords& pt, double tolerance)const{
    if(!size()){return false;}
    else if(size()==1){
      const coords& mypt= (*begin()).first;
      return mypt.get_distance(&pt) <=tolerance;
    }
    else{
      for (const_iterator w= begin() +1;w!= end();w++){
	Segment<const coords*> seg(&((*(w-1)).first), &((*w).first));
	if(seg.min_distance_to(pt)< tolerance){return true;}
      }
    }
    return false;
  }


  //return aN iterator
  bool simple_click_history::segment_on_path(const Segment<coords*>& segment, double tolerance)const{
    //throw xn_Error("segment_on_path(():fix me");
    if(size() < 2){return false;}
    for (const_iterator w = begin(); w!= (end()-1); w++){
      Segment<coords> segtmp(segment);
      Segment<const coords*> pathseg(&((*w).first), &((*(w + 1)).first));
      if(pathseg.max_separation(segtmp) < tolerance){return true;}
    }
    return false;
  }

  PathFragment* simple_click_history::common_subpath(const simple_click_history& otherpath){
    throw xn_Error("common_subpath():fix me");
  }

    std::deque<std::pair<coords, Segment<coords*> > >
  SegmentWalker::find_intersections_with_segment(const Linear_function<double>& fn,
						 bool implicitly_closed)const{
    typedef std::pair<coords, Segment<coords*> > value_type;
    typedef std::deque<value_type> result_type;
    result_type res;
    if ( container->size()<2){return res;  }
    std::deque<SubPath> subpaths = container->as_subpaths();
    for(std::deque<SubPath>::const_iterator w= subpaths.begin(); w!= subpaths.end();w++){
      ///@todo const version in SubPath
      SubPath& unconst = const_cast<SubPath&>(*w);
      unconst.find_intersections_with_segment(fn, res, implicitly_closed);
    }
      return res;
  }

  LineSegment 
  simple_click_history::post_segment(simple_click_history::iterator start, 
				     simple_click_history::iterator current_iter,
				     simple_click_history::iterator sentinel){
    if(!(is_drawing_click(current_iter)|| is_closepath_click(current_iter))){
      return LineSegment(NULL, NULL);/*invalid*/
    }
    coords* firstone(NULL), *secondone(NULL);
    if(is_drawing_click(current_iter)){
      firstone=&(*current_iter).first;
    }
    else{ 
      std::pair<iterator, coords*> subpathstart = get_subpath_start(start, current_iter);
      firstone = subpathstart.second;

    }
    while(++current_iter != sentinel){
      if(is_drawing_click(current_iter)|| is_closepath_click(current_iter)){
	secondone=&((*current_iter).first);
	return LineSegment(firstone,  secondone);
      }
    }
    return LineSegment(firstone,  secondone);//if LineSegment::first is NULL,
    //@param  secondone is the first drawing click in path(and possibly *(end()-1))
  }

  simple_click_history::simple_click_history():
    path_cleaner(this){
    set_pathid();
 
  }
  void simple_click_history::scale(double scale_x, double scale_y, 
				   const coords* centre,
				   bool use_selection){
    coords scaler(scale_x,scale_y);
    scale(scaler, centre,use_selection );
  }
  size_t simple_click_history::consolidate_internal_selection(int whichway,bool clear_source){
    size_t res=0;//return the count of copied selection flags
    for(iterator w=begin(); w!=end();w++){
      if (whichway == SELECTION_COPY_FROM_COORDS){
	// basecoords::transfer_selection_to_pathclick(*w,false,true)//default
	basecoords::transfer_selection_to_pathclick(*w);
      }
      else{
	basecoords::transfer_selection_from_pathclick(*w);
      }
    } 
    return res;
  }
  void SegmentWalker::get_tan_angles(const LineSegment& current, coords* midpoint,
				     double& a_angle, double& b_angle,
				     double& cur_angle)const{
    cur_angle = current.get_tan_angle();
    LineSegment a(current.first, midpoint);
    LineSegment b(midpoint,current.second );
    a_angle = a.get_tan_angle();
    b_angle = b.get_tan_angle();
  }
  void SegmentWalker::set_min_deviation_abs(double d){
    if(d <0){d=min_deviation_abs_default;}
    if(d > min_length /2.){ d = min_length /4.;}
    min_deviation_abs = d;
  }
  double SegmentWalker::too_short(const LineSegment& seg)const{
    return seg.length() < min_length;
  }
  void SegmentWalker::set_min_length(double m, bool force){
    if(m<= 3 && !force){m = 3;}
    min_length = m;
  }

  SegmentWalker::SegmentWalker(simple_click_history* owner):
    container(owner),
    cur_segment(NULL, NULL),tolerance(tolerance_default),
    min_length(min_length_default),
    min_deviation_abs(min_deviation_abs_default),
    max_deviation_rel(max_deviation_rel_default),
    min_rot_short(min_rot_long_default),
    min_rot_long(min_rot_long_default)
  {}

  void SegmentWalker::reset_to_defaults(){
    tolerance=tolerance_default;
    min_length = min_length_default;
    min_deviation_abs = min_deviation_abs_default;
    max_deviation_rel = max_deviation_rel_default;
    min_rot_short = min_rot_long_default;
    min_rot_long = min_rot_long_default;
  }

  SegmentWalker::SegmentWalker(simple_click_history* owner,double tol, double minlen,
			       double min_dev, double max_dev_rel,
			       double min_short,double min_long):
    container(owner),cur_segment(NULL, NULL),tolerance(tol),
    min_length(minlen), min_deviation_abs( min_dev),
    max_deviation_rel(max_dev_rel),min_rot_short( min_short),
    min_rot_long(min_long)
  {}

  void SegmentWalker::set_min_rot_short(double m){
    if(m > M_PI/16){m = M_PI/24;}
    min_rot_short =m;
  }

  void SegmentWalker::set_min_rot_long(double m){
    if(m >min_rot_short /4 ){m = min_rot_short/ 5;}
    min_rot_long =m;
  }

  void SegmentWalker::set_max_deviation_rel(double d){
    d =(d < 0 ? -d : d);
    d=(d >.5 ? .3 : d);
    max_deviation_rel=d;
  }

  bool SegmentWalker::operator()(const std::pair<coords, int>& click){
      
    if(click.second & CLICK_MARKED_4_DESTRUCTION){
      std::cerr <<"segmentwalker::operator():found  CLICK_MARKED_4_DESTRUCTION"<<std::endl;
      return true;
    }
    return false;
  }

  bool SegmentWalker::too_straight(const LineSegment& current, coords* midpoint)const{
    double dist = current.min_distance_to(*midpoint);
    LineSegment a(current.first, midpoint);
    LineSegment b(midpoint,current.second );
    double a_angle , b_angle ,cur_angle;
    get_tan_angles(current, midpoint,  a_angle,  b_angle, cur_angle);
    double a_cur_angle_diff= (a_angle > cur_angle? 
			      a_angle- cur_angle: cur_angle - a_angle);
    double b_cur_angle_diff= (b_angle > cur_angle? 
			      b_angle- cur_angle: cur_angle - b_angle);
    double min_short(get_min_rot_short()), min_long(get_min_rot_long());
    if(a_cur_angle_diff > b_cur_angle_diff){
      if(a_cur_angle_diff < min_short &&
	 b_cur_angle_diff < min_long){return true;}
    }
    else{
      if(b_cur_angle_diff < min_short &&
	 a_cur_angle_diff < min_long){return true;}
    }
    if(dist < get_min_deviation_abs()){
      double a_len =a.length();
      double b_len= b.length();
      double short_len = (a_len< b_len ? a_len: b_len);
      if(dist /(short_len/min_length) <  max_deviation_rel){return true;}
    }
    return false;
  }

  void SegmentWalker::set_param(PathCleanerSettings which, double val){
    if(val <0){throw xn_Error("value must be positive");}
    switch(which){
    case SEGMENT_TOLERANCE:
      if(val <=0){
	val=tolerance_default;
      }
      tolerance=val;
      break;
    case  SEGMENT_MIN_LENGTH:
      if(val <=0){
	val=min_length_default;
      }
      min_length=val;
      break;
    case SEGMENT_MIN_DEVIATION_ABS:
      if(val <=0){
	val=min_deviation_abs_default;
      }
      min_deviation_abs=val;
      break;
    case  SEGMENT_MAX_DEVIATION_REL:
      if(val <=0){
	val=max_deviation_rel_default;
      }
      max_deviation_rel=val;
      break;
    case  SEGMENT_MIN_ROT_SHORT:
      if(val <=0){
	val=min_rot_short_default;
      }
      min_rot_short=val;
      break;
    case  SEGMENT_MIN_ROT_LONG:
      if(val <=0){
	val=min_rot_long_default;
      }
      min_rot_long=val;
      break;
    }
  }

  void SegmentWalker::set_default(PathCleanerSettings which, double val){
    if(val <0){throw xn_Error("value must be positive");}
    switch(which){
    case SEGMENT_TOLERANCE:
      tolerance_default=val;
      break;
    case  SEGMENT_MIN_LENGTH:
      min_length_default=val;
      break;
    case SEGMENT_MIN_DEVIATION_ABS:
      min_deviation_abs_default=val;
      break;
    case  SEGMENT_MAX_DEVIATION_REL:
      max_deviation_rel_default=val;
      break;
    case  SEGMENT_MIN_ROT_SHORT:
      min_rot_short_default=val;
      break;
    case  SEGMENT_MIN_ROT_LONG:
      min_rot_long_default=val;
      break;
    }
  }

  void simple_click_history::set_pathid(std::string s){
    if(!s.size()){
      std::stringstream ostmp;
      ostmp<<"path#"<<this;
      s = ostmp.str();
    }
    pathid = s;
  }

  std::string simple_click_history::get_pathid()const{
    return pathid;
  }

  simple_click_history operator+(const simple_click_history& left, 
				 const coords& right){
    simple_click_history res;
    typedef simple_click_history::value_type value_type;
    for(simple_click_history::const_iterator w = left.begin();w != left.end();w++){
      res.push_back(value_type((*w).first + right, (*w).second));
    }
    return res;
  }

  simple_click_history simple_click_history::copy_selected(){
    simple_click_history res;
    register bool new_subpath =true;
    register bool previous_selected = false;
    consolidate_internal_selection();
    for(simple_click_history::const_iterator w = begin();w!=end();w++){
      unsigned char stepahead = 0;
      int ptflags = (*w).second;
      if(ptflags & CLICK_SELECTED){
	if (new_subpath){
	  if(ptflags & DRAWING_CLICK){
	    new_subpath = false;
	    if(ptflags & LINE_CLICK){
	      ptflags ^=LINE_CLICK ;
	      ptflags |= MOVE_CLICK;
	    }
	    else if (ptflags & BEZIER_CLICK){
	      ptflags ^= BEZIER_CLICK;
	      ptflags |= MOVE_CLICK;
	      bool stop(false);
	      for (int n =1;w+n !=end() && !stop; n++){
		if((*(w+n)).second & BEZIER_HANDLE){stepahead++;}
	      }
	    }
	  }
	}
	else{
	
	}
	res.push_back(std::pair<coords, int>((*w).first, ptflags));
	previous_selected = true;
	if(stepahead){
	  for(;stepahead;stepahead--){
	    w++;
	    res.push_back(std::pair<coords, int>((*w).first, (*w).second | CLICK_SELECTED));
	  }
	}
	else{
	  if(  previous_selected  ){
	    if(ptflags & BEZIER_HANDLE){
	      res.push_back(std::pair<coords, int>((*w).first, ptflags));
	    }
	    else{previous_selected = false;}
	  }
	  if(!new_subpath){ //there is apath
	    if(ptflags & CLOSE_PATH){
	      res.push_back(std::pair<coords, int>((*w).first, ptflags));
	    }
	    else if (ptflags & MOVE_CLICK){
	      new_subpath = true;
	    }
	  }
	}
      }
    }
    return res;
  }

  bool SegmentWalker::too_insignificant(const LineSegment& current, coords* midpoint)const{
#ifdef CHECK_SEGMENT
    std::cerr<<"\n\nStart of segment comparison in too_insignificant()\n"
	     <<"3 points in order follow:\n"<< current.first->x
	     << " "<<current.first->y<<"\n"<<midpoint->x<<" "
	     << midpoint->y<<"\n"<<current.second->x<<" "
	     <<current.second->y<<"\nThe Segments\n'current'(whole path):\n";
#endif    
    LineSegment a(current.first, midpoint);
    LineSegment b(midpoint, current.second);
#ifdef CHECK_SEGMENT
    LineSegment::line_function_type a_fn =a.get_function();
    LineSegment::line_function_type b_fn =b.get_function();

    std::cerr<<"created linrfunctions"<<std::endl;
    coords a_solved(a_fn.solve(b_fn));
    std::cerr<<"called ' coords a_solved(a_fn.solve(b_fn));'"<<std::endl;
    coords b_solved(b_fn.solve(a_fn));
    std::cerr<<"called ' coords b_solved(b_fn.solve(a_fn));'"<<std::endl;
    std::cerr<<"midpoint: "<<midpoint->x<< " "<<midpoint->y
	     <<"\na's solution: "<<a_solved.x<<" "<<a_solved.y
	     <<"\nb's solution: "<<b_solved.x<<" "<<b_solved.y;
#endif
    if(a.near_parallel(b) ||
       (current.near_parallel(b) &&current.near_parallel(a))){
#ifdef CHECK_SEGMENT
      std::cerr<<"paths 'neear_parallel - returning true"
	       <<"\n********exiting too_insignificant()*******"<<std::endl;
#endif
      return true;
    }
#ifdef CHECK_SEGMENT
    std::cerr<<"called a.near_parallel(b)etc"<<std::endl;
#endif
    double a_ang,b_ang, cur_ang;
    get_tan_angles(current, midpoint, a_ang, b_ang, cur_ang);
    bool shortres(false);
    if(too_short(a)){
#ifdef CHECK_SEGMENT
      std::cerr<<"called tooshort()"<<std::endl;
#endif
      double a_diff=(cur_ang>a_ang?cur_ang-a_ang:a_ang-cur_ang);
      if(a_diff < M_PI /16){shortres=true;}
      if(too_short(current)&& a_diff < M_PI /6){shortres=true;}
      if(shortres){ 
#ifdef CHECK_SEGMENT
	std::cerr<<"path 'a' too short: returning true\n**********"
		 <<"exiting too_insignificant()********"<<std::endl;
#endif
	return true;
      }
    }
    if(too_short(b)){
#ifdef CHECK_SEGMENT
      std::cerr<<"called tooshort(b)"<<std::endl;
#endif
      double b_diff=(cur_ang > b_ang ? cur_ang - b_ang : b_ang - cur_ang);
      if(b_diff < M_PI /16){shortres= true;}
      if(too_short(current)&& b_diff < M_PI /6){shortres=true;}
      if(shortres){
#ifdef CHECK_SEGMENT
	std::cerr<<"path 'b' too short: returning true\n**********"
		 <<"exiting too_insignificant()********"<<std::endl;
#endif
	return true;
      }
    }
    if(too_short(current)){
#ifdef CHECK_SEGMENT
      std::cerr<<"called tooshort(current)"<<std::endl;
#endif
      double ab_diff=(a_ang>b_ang ? a_ang-b_ang:b_ang-a_ang);
      if(ab_diff < M_PI /8){
#ifdef CHECK_SEGMENT
	std::cerr<<"path 'current' too short: returning true\n**********"
		 <<"exiting too_insignificant()********"<<std::endl;
#endif
	return true;
      }
    }
#ifdef CHECK_SEGMENT
    std::cerr<<"entering too straight"<<std::endl;
#endif
    if(too_straight(current, midpoint)){
#ifdef CHECK_SEGMENT
      std::cerr<<"paths 'a/b/current' too straight: returning true\n"
	       <<"**********exiting too_insignificant()********"<<std::endl;
#endif
      return true;
    }
#ifdef CHECK_SEGMENT
    std::cerr<<"paths look ok, no need to erase "<<midpoint->x<<", "
	     << midpoint->y<<"\nreturning false...\n"
	     <<"*******exiting too_insignificant()********"
	     <<std::endl;
#endif
    return false;
  }
  coords* SegmentWalker::find_next(SegmentWalker::iterator currn){
    if(currn== container->end()){return NULL;}
    currn++;

    for(;currn != container->end();currn++){
      int state = (*currn).second;
      if((!(state & CLICK_MARKED_4_DESTRUCTION)) &&(state & (LINE_CLICK|BEZIER_CLICK))){
	return &(*currn).first;
      } 
      else if(state & (MOVE_CLICK |BEZIER_HANDLE)){return NULL;}
    }
    return NULL;
  }
  const coords* SegmentWalker::find_previous(SegmentWalker::const_iterator currn)const{
    for(;currn != container->begin();--currn){
      int state = (*currn).second;
      if((!(state & CLICK_MARKED_4_DESTRUCTION)) &&(state & (MOVE_CLICK | LINE_CLICK))){
	return &(*currn).first;
      }
      else if(state & (BEZIER_CLICK |BEZIER_HANDLE)){return NULL;}
    }
    return NULL;
  }
  coords* SegmentWalker::find_previous(SegmentWalker::iterator currn){
    for(;currn != container->begin();--currn){
      int state = (*currn).second;
      if((!(state & CLICK_MARKED_4_DESTRUCTION)) &&(state & (MOVE_CLICK | LINE_CLICK))){
	return &(*currn).first;
      }
      else if(state & (BEZIER_CLICK |BEZIER_HANDLE)){return NULL;}
    }
    return NULL;
  }

  size_t SegmentWalker::clean_path(){
  
    if ( container->size()<3){
      return 0;
    }
    std::deque<size_t> erasure_list;
    size_t cleared(0);
    LineSegment seg_b4(NULL,NULL), seg_after(NULL,NULL);
    iterator w = container->begin();
    while(w!=container->end() && !((*w).second & MOVE_CLICK)){w++;}
    w++;
    for(;w!=container->end()-1;w++){
      if((*w).second & LINE_CLICK){ 
	coords* prev = find_previous(w);
	    
	if(!prev){
	  std::cerr<<"start of path or subpath-no previous move or line click"
		   <<std::endl;
	  continue;
	}
	seg_b4.first = prev;
	seg_b4.second=&(*w).first;
	coords* next = find_next(w);
	if(!next){break;}
	else{
	  cur_segment=LineSegment( prev, next);
	  seg_after.first = &(*w).first;
	  seg_after.second=next;
	  if(too_insignificant(cur_segment, &((*w).first))){
	    std::cerr <<"clearing element "<< w-container->begin()
		      <<"\nmaking "<<++cleared<< " erasures"<<std::endl;
	    (*w).second |= CLICK_MARKED_4_DESTRUCTION;
	    //RUN REMOVE_IF after
	  }
	  continue;
	}
      }
      while(w+1 !=container->end() 
	    && !((*w).second & (MOVE_CLICK|LINE_CLICK))){w++;}
    }
    size_t oldsz= container->size();
    //	std::remove_if(container->begin(), container->end(), *this);
    size_t newsz = container->size();
    //if((oldsz -newsz) != cleared){
    std::cerr<< "number of coords marked for deletion: "<< cleared
	     <<std::endl;
    return oldsz -newsz;
	
    std::cerr << "erased "<< cleared<< "clicks"<<std::endl;
    return cleared;
  }
  size_t simple_click_history::copy_cleaned_path(simple_click_history& dest){
    size_t oldsz= size();
    size_t destb4sz=dest.size();
    if( destb4sz){
      std::cerr <<"destination path already has content: appending to enbd\n";
    }
    for(const_iterator w= begin();w!= end();w++){
      if(!path_cleaner(*w)){//looks for 'MARKEDFORDESTRUCTION -or so- flag
	dest.push_back(*w);
      }
    }
    //remove_copy_if() causes segfault:
    // for some reason the constructor isn't called, 
    //and memory not allocated
    //chokes at the line 'state = src.state' in the basecoords::operator=
    //because the 'this' is at 0x30 supposedly(on an acer/intel t4300 laptop
    //	std::remove_copy_if(begin(), end(), dest.end(),path_cleaner);

    size_t newsz =dest.size()-destb4sz;
    if(newsz ==oldsz){
      std::cerr<<"old and new paths the same...\n";
    }
    return newsz;
  }

  simple_click_history::simple_click_history(const simple_click_history& copied):
    container_type(copied),
    //make sure to point path_cleaner to the new container
    path_cleaner(this){
    set_pathid();
  }


  simple_click_history& simple_click_history::operator=(const simple_click_history& copied){
    if(&copied != this){
      container_type::operator=(copied);
    }
    return *this; 
  }

  double simple_click_history::get_area()const{
    cRect rect;
    get_extents(rect);
    return rect.area();
  }
    
  void simple_click_history::get_extents(cRect& outval)const{
    const_iterator w= begin();
    while(w!= end() && !drawing_click(*w) ){w++;}
    if(w==end()){
      std::cerr << "empty path: get_extents() returning {0,0,0,0}\n//Logic Error:should have a internal value to indicate no extent or location whatever"<< std::endl;
      outval= cRect();
      return;
    }
    double x, y;
    coords first, second;
    first.x = second.x = (*w).first.x;
    first.y = second.y = (*w).first.y;
    w++;
    for( ;w != end();w++){
      x = (*w).first.x;
      y = (*w).first.y;
      if(x < first.x) {first.x = x;}
      else if (x > second.x){ second.x = x;}
      if(y < first.y) {first.y = y;}
      else if (y > second.y){ second.y = y;}
    }  
    outval = cRect(first, second);
  }

  void simple_click_history::translate(const coords& offset) {
  
    bool use_selection = points_selected();
    for (iterator w = begin(); w != end();w++){
      if(!use_selection || (*w).second & CLICK_SELECTED){
	(*w).first += offset;
      }
    }
    //translate_coordspair functor(offset);
    //std::for_each(begin(),end(), functor);
  }
  simple_click_history operator-(const simple_click_history& left, 
				 const coords& right){
    simple_click_history result;
    for(simple_click_history::const_iterator w = left.begin(); w !=left.end();w++){
      result.push_back((*w).first+ right,(*w).second); 
    }
    return result;
  }
  simple_click_history& simple_click_history::operator+=(const coords& offset){
    for (iterator walk = begin(); walk != end();walk++){
      coords& temp =    (*walk).first;
      temp += offset;
    }
    return *this;
  }

  simple_click_history& simple_click_history::rotate_around_begin(double radians){
    coords& origin = (*begin()).first;
    return rotate(origin, radians);
  }

  std::vector<const coords*> simple_click_history::sorted_byX_const()const{
    std::vector<const coords*> sortrange;
    sortrange.reserve(size());
    for(const_iterator start = begin();start !=end();start++){
      if((*start).second){
	sortrange.push_back(&((*start).first));
      }
    }
    CompareX<const coords*> xfunctor;
    std::sort(sortrange.begin(), sortrange.end(), xfunctor);
    return sortrange;
  }

  std::vector<coords*> simple_click_history::sorted_byX(){
    std::vector<coords*> sortrange;
    sortrange.reserve(size());
    for(iterator start = begin();start !=end();start++){
      if((*start).second){
	sortrange.push_back(&((*start).first));
      }
    }
    CompareX<coords*> xfunctor;
    std::sort(sortrange.begin(), sortrange.end(), xfunctor);
    return sortrange;
  }
  std::vector<coords*> simple_click_history::sorted_byY() {
    std::vector<coords*> sortrange;
    sortrange.reserve(size());
    for(iterator start = begin();start !=end();start++){
      if((*start).second){
	sortrange.push_back(&((*start).first));
      }
    }
    CompareY<coords*> yfunctor;
    std::sort(sortrange.begin(), sortrange.end(), yfunctor);
    return sortrange;
  }
  std::vector<const coords*> simple_click_history::sorted_byY_const()const {
    std::vector<const coords*> sortrange;
    sortrange.reserve(size());
    for(const_iterator start = begin();start !=end();start++){
      if((*start).second){
	sortrange.push_back(&((*start).first));
      }
    }
    CompareY<const coords*> yfunctor;
    std::sort(sortrange.begin(), sortrange.end(), yfunctor);
    return sortrange;
  }
  const coords& simple_click_history::find_closest(const coords& searchitem)const{
    if(!size()){throw xn_Error("history empty");}
    typedef std::vector<std::pair<coords, int> >vector_type;
    typedef vector_type::iterator Vec_iterator;

    vector_type  temp;
    coords smallbeer;
    temp.push_back(std::pair<coords, int>(smallbeer, 0));
    Vec_iterator out = temp.begin();
    compare_distance_pair<coords> compare(searchitem);

    std::partial_sort_copy(begin(), end(), out, out + 1, compare);
    return (*temp.begin()).first;
  }

  simple_click_history::~simple_click_history(){}

  void simple_click_history::append_path(simple_click_history& subpath, bool relative ){
    if (relative == false){
      std::copy(subpath.begin(), subpath.end(), back_inserter(*this));
    }
    else{
      coords adjust = (*(end()-1)).first;
      pair_relativiser R(adjust);
      std::transform(subpath.begin(), subpath.end(),back_inserter(*this), R);
    }
  }

  void simple_click_history::push_back(const NCOORDS::coords& click ,int cliktype){
    container_type::push_back(value_type(click, cliktype));
  }

  void simple_click_history::push_back(const std::pair<NCOORDS::coords, int>& click){
    container_type::push_back(click);
  }

  void simple_click_history::push_front(const NCOORDS::coords& click,int cliktype){
    container_type::push_front(value_type(click, cliktype));
  }

  void simple_click_history::push_front(const std::pair<NCOORDS::coords, int>& click){
    container_type::push_front(click);
  }

simple_click_history& 
simple_click_history::combine_with(const simple_click_history& other, 
				   int flag){
  //create 2 sets of flattened subpaths
  //and then call the subpath combine functions
  //assign to and return *this as the result
  simple_click_history left, right;
  if(!is_flat()){
    if(!other.is_flat()){
      left= flatten_copy();
      right= other.flatten_copy();
    }
    else{//other.is_flat()
      left= flatten_copy();
      right=other;
      right.clear_short_segments();
    }
  }
  else if(!other.is_flat()){
    left= *this;
left.clear_short_segments();
    right= other.flatten_copy();
  }
  else{
    left= *this;
    right=other;
left.clear_short_segments();
      right.clear_short_segments();
  }
  //delete these ptrs later
  //TODO: use container of objects
  std::deque<SubPath*> p_subleft =  left.as_ptr_subpaths();
 std::deque<SubPath*> p_subright =  right.as_ptr_subpaths();
  *this = SectionGroup<SubPath>::combine_paths(p_subleft, p_subright,flag);
  //von_boob();//reminder compile error 
  return *this;
}
 
  std::deque<coords> 
  SegmentWalker::find_intersections(const Linear_function<double>& fn,
				    bool closed_implicitly)const{
    std::deque<coords> res;
    if ( container->size()<2){return res;}
    std::deque<SubPath> subbies= container->as_subpaths();
    for( std::deque<SubPath>::const_iterator w= subbies.begin();w!=subbies.end();w++){
      (*w).find_intersections(fn, res, closed_implicitly);
    }
    return res;
  }

    simple_click_history operator|(const simple_click_history& left, const simple_click_history& right){
    return simple_click_history::combine(left,right, PATH_COMBINE_OR);
  }
simple_click_history& operator|=(simple_click_history& left, const simple_click_history& right){
    return left.combine_with(right, PATH_COMBINE_OR);
  }
    ///if weird behaviour results, remove this operator 
    simple_click_history operator&(const simple_click_history& left, const simple_click_history& right){
    return simple_click_history::combine(left,right, PATH_COMBINE_AND);
  }
simple_click_history& operator&=(simple_click_history& left, const simple_click_history& right){
    return left.combine_with(right, PATH_COMBINE_AND);
  }
  simple_click_history operator^(const simple_click_history& left, const simple_click_history& right){
    return simple_click_history::combine(left,right, PATH_COMBINE_NOTAND);
  }
  simple_click_history& operator^=(simple_click_history& left, const simple_click_history& right){
    return left.combine_with(right, PATH_COMBINE_NOTAND);
  }
simple_click_history operator-(const simple_click_history& left, const simple_click_history& right){
    return simple_click_history::combine(left,right, PATH_COMBINE_DIFF);
  }
  simple_click_history& operator-=(simple_click_history& left, const simple_click_history& right){
    return left.combine_with(right, PATH_COMBINE_DIFF);
  }

  std::ostream& operator<<(std::ostream& out, 
			   const simple_click_history& clicks){
    out << "\n\nsimple_click_history:" ;
    size_t j = clicks.size();
    for(size_t i =0;i < j; i++){
      out << "click no" << i;
      if( clicks[i].second & CLICK_ERROR){
	out <<"click error... details may follow"<< std::endl;
      }
      if (clicks[i].second & NULL_CLICK){
	out <<"clicktype====> NULL click - no data" << std::endl;
	continue;
      }
      else if(clicks[i].second & CLOSE_PATH){
	out <<"clicktype====> clicktype====>close path - no click data"<< std::endl;
	continue;
      }
      out << "----> " << clicks[i].first<< "\n";
      out << "clicktype====>";
      if(clicks[i].second & MOVE_CLICK){
	out << "moveclick" << std::endl;
	continue;
      }
      else if (clicks[i].second & LINE_CLICK){
	out << "lineclick" << std::endl;
	continue;
      }
      else if (clicks[i].second & BEZIER_CLICK){
	out << "bezier click" << std::endl;
	continue;
      }
      else if (clicks[i].second & BEZIER_HANDLE){
	out << "bezier handle" << std::endl;
	continue;
      }
      else if (clicks[i].second & ORIGIN_CLICK){
	out << "origin click" << std::endl;
	continue;
      }
      else if (clicks[i].second & OFFSET_CLICK){
	out << "offset click" << std::endl;
	continue;
      }
      else{
	out << "unknown" << std::endl;
     
      }
    }
    return out;
  }

	  
  void simple_click_history::pop_front(){
    container_type::pop_front();
  }
  void simple_click_history::pop_front(size_type n){
    for(size_type i = 0; i< n; i++){
      pop_front();
    }
  }
  void simple_click_history::pop_back(){
    container_type::pop_back();
  }
	  
  void simple_click_history::pop_back(size_type n){
    for(size_type i = 0; i< n; i++){
      pop_back();
    }
  }


  void simple_click_history::prepend_path(simple_click_history& subpath, bool relative ){
    if (relative == false){
      std::copy(subpath.rbegin(), subpath.rend(), front_inserter(*this));
    }
    else{
      coords adjust((*(begin())).first);
      pair_relativiser R(adjust);
      std::transform(subpath.begin(), subpath.end(),std::front_inserter(*this), R);
    }
  }


  void simple_click_history::insert_path(simple_click_history& subpath, unsigned int position, bool relative){
    if (position >= this->size()){
      append_path(subpath, relative);
    
      return;
    }
    iterator it  = begin() +  position;
    if (relative == false){
      std::copy(subpath.begin(), subpath.end(), std::inserter(*this, it));
    }
    else{
      simple_click_history reappend_path= cut_path(position, size() - position, relative);
      append_path(subpath, relative);
      append_path(reappend_path, relative);
    }
  }

  void simple_click_history::write_data(const simple_click_history& path,std::string& filename){
    using namespace std;
    string filename_ = filename + ".simple_click_history.bin";
    ofstream bindata(filename_.c_str(), ios::binary);
    write_datastream(bindata);
    bindata.close();
  }

  simple_click_history simple_click_history::read_data(std::string& fname){
    // TODO: check for correct filename suffix("*.(xnd|XND)")
    //-create a mimetype and Gtk::FileFilter
    using namespace std;
    typedef ifstream::traits_type traits_type;
    typedef ifstream::int_type int_type;

    ifstream bindata(fname.c_str(), ios::binary);
    bindata.setf(std::ios_base::skipws);
    N_COUT( "reading simple_click_history  from file: " << fname);
    simple_click_history result;
    value_type tmpresult;
    //RawDataReader<value_type> readdata(tmpresult);
    bindata.exceptions(std::ios_base::failbit | std::ios_base::eofbit);
    try{
      bool reading = true;
      //TODO: name all enums and use names
      while(reading){
	bindata >> tmpresult.first.x
		>> tmpresult.first.y //ignore tmpresult.first.state
		>> tmpresult.second;
	int_type c = bindata.peek();
	if (c == traits_type::eof()){ reading = false;}
	result.push_back(tmpresult);
      }
    }
    catch(std::exception& e){
      //whats the excetion io throws?
      std::cerr <<"read_data threw"<<std::endl;
      std::cerr <<e.what()<< std::endl;
    }
    return result;
  }

  simple_click_history& simple_click_history::rotate(const coords& origin,
						     double radians){
    bool have_selection = points_selected();
 
    //rotae in
    for (iterator walk = begin(); walk != end(); walk++){
      if(!have_selection || (*walk).second & CLICK_SELECTED){
	origin.rotate(&(*walk).first, radians);
      }
    }
    return *this;
  }
void simple_click_history::clear_short_segments(){
      if(size()<2){return;}
      std::deque<SubPath> subpaths = as_subpaths();
      typedef std::deque<SubPath>::iterator subpath_iterator;
      simple_click_history result;
      for(subpath_iterator subiter= subpaths.begin(); subiter != subpaths.end(); subiter++){
	SubPath& sub=*subiter;
	bool started(false);
	iterator first= sub.begin();
	iterator second(first), guard(first), subend(sub.end());
	//bool explicitlyclosed(false);
	for(;(first!= subend &&second!=guard) || !started;++second){
	  if(!started){
	    result.push_back(*first);  
	    for(;(second!=subend)&&(!((*first).second & MOVE_CLICK));first++){
	      std::cerr<<clicktype_as_string((*first).second )
		       << "at start of path"<<std::endl;
	      result.push_back(*first);
	      second++;
	    }
	    started=true;
	  }
	  if((*first).second & (LINE_CLICK|MOVE_CLICK)){
	    coords& prior=(*first).first;
	    while((!((*second ).second & (LINE_CLICK|CLOSE_PATH)))&& (second != subend)){second++;}
	    if(second != subend){
	      if((*second ).second & LINE_CLICK){
		coords& duo = (*second ).first;
		if(prior.get_distance(&duo)>=coords::min_radius()){
		  result.push_back(*second);
		  first++;
		}
	      }
	      else if((*second ).second & CLOSE_PATH){
		coords& duo = (*guard ).first;
		if(prior.get_distance(&duo)<coords::min_radius()){
		  std::cerr <<"closepath segment too short"<<std::endl;
		  result.erase(result.end()-1);
		}
		result.push_back(*second);
		first++;
	      }
	    }
	    else{
	      coords& duo = (*guard ).first;
	      if(prior.get_distance(&duo)<coords::min_radius()){
		  std::cerr <<"implicitrclosepath segment too short: not erasing for now"<<std::endl;//result.erase(result.end()-1);
	      }
	      first++;	  
	    }
	  }
	  else if((*first).second & CLOSE_PATH){
	    //done
	    first++;second=guard;
	  }
	  else{
	    std::ostringstream ostmp;
	    ostmp << "bad value(path must be flat to call the method): "<<clicktype_as_string((*first).second);
	    throw xn_Error(ostmp.str());
	  }
	}
      }
      //make sure all segments are at least coords::min_radius() long
      //throw xn_Error("FINISHME");
      *this=result;
    }

  simple_click_history simple_click_history::cut_path(unsigned int position, unsigned int point_no, bool relative){
    simple_click_history cutpath;
    simple_click_history reappend_path;
    if (position >= size()){
      return cutpath;
    }
    if (!relative){
      unsigned int i = 0;
      for (simple_click_history::iterator walk = begin() + position;walk != end(); walk++ ){ 
	if (i < point_no){
	  cutpath.push_back(*walk);
	}
	else{
	  reappend_path.push_back(*walk);
	}
	i++;
      }
      pop_back(i);
      append_path(reappend_path, false);
      return cutpath;
    }
    else{
      coords cut_relativiser((*(begin() + position -1)).first);
      std::pair<coords,int> temp = *(begin() + position + point_no-1);
      coords reappend_relativiser( temp.first - cut_relativiser);
      unsigned int i = 0;
      for (simple_click_history::iterator walk = begin() + position;walk != end(); i++, walk++ ){ 
	if (i < point_no){
	  (*walk).first -= cut_relativiser;
	  cutpath.push_back(*walk);
	}
	else{
	  coords value((*walk).first - reappend_relativiser );
	  reappend_path.push_back(value, (*walk).second);
	}
      }
      pop_back(i);
      append_path(reappend_path, true);
    }
    return cutpath;
  }


  void simple_click_history::load(coords* point, int cliktype){
    std::pair<coords*,int> ptpr(point, cliktype);
    push_back(ptpr);
  }

  void simple_click_history::load(std::pair<coords* , int > point){
    load(point.first,point.second);
  }

  
  void relative_click_history::relpath(const simple_click_history& absp){
    clear();
    const_iterator first = absp.begin();
    const_iterator last =absp.end();
    if(first==last){return;}
    while(!is_cairo_click(first)){
      first++;
      if(first==last){return;}
    }
    std::back_insert_iterator<relative_click_history> outiter(*this);
    if(!((*first).second & MOVE_CLICK)){throw xn_Error("must start with move");}
    std::pair<coords, int> value= *first;
    *outiter++= value;
    while(++first != last){
      if(!is_drawing_click(first) || ! is_handle_click(first)){
	if(is_closepath_click(first)){
	  *outiter++= *first;
	}
	continue;
      }
      std::pair<coords, int> next=  *first;
      value -= next;
      *outiter++= value;
    }
  }

  simple_click_history relative_click_history::abspath()const{
    const_iterator w = begin();
    const_iterator last=end();
    simple_click_history res;
    if(w == last){return res;}
    std::back_insert_iterator<simple_click_history> outiter(res);
    std::pair<coords, int> value;
    *outiter = *w;
    while (++w != last){
      std::pair<coords, int> tmp= *w;
      *outiter =  tmp+value;// so (*outiter).second == tmp.second
      tmp= value;
    }
    return res;
  }
}
