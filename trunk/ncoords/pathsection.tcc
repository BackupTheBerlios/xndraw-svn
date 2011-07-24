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
@file pathsection.tcc
templae implementations frompathsection.h
 */

#ifndef X3DRAW_PATHSECTION_TCC
#define X3DRAW_PATHSECTION_TCC


namespace NCOORDS{

 

  template<typename CONTAINER, typename PATH>
bool remove_from_list(CONTAINER& container, const std::pair<Section<PATH>*,int>& obj){
 size_t sz = container.size();
    for(typename CONTAINER::iterator w = container.begin(); w!= container.end();w++){
      if((*w).first== obj.first){
	w=container.erase(w);
	if(w==container.end()){break;}
      }
    }
    return container.size() != sz;
  }



  //we can increase this value when all is well
  template <typename PATH>
  const typename Section<PATH>::size_type Section<PATH>::crossings_absmax= 64;


  template <typename PATH>
  std::ostream& operator <<(std::ostream& os, const Intersection<PATH>& src){
    coords pt=src.get_intersection();
    os <<"Intersection ("<<pt.x<<","<<pt.y<<")\nfirstpath: "<< src.first.first
       <<" after offset "<<src.first.second<<"\nsecondpath: "<<src.second.first
       <<" after offset "<<src.second.second;
    if(src.pathcount()>2){
      os<<"extras:";
      
    }
    return os;
  }


  template <typename PATH>
  std::ostream& operator <<(std::ostream& os, const Section<PATH>& src){
    os <<"Section: path"<<src.subpath<<"\nleft => "<< src.startinter;
    if( src.startinter){os << ": "<<* (src.startinter);}
    os <<"\nright=> "<< src.endinter;
    if( src.endinter){os << ": "<<*(src.endinter);}
    if(src.marked()){os <<"\nmarked(): crossings = "<< src.crossings;}
    else{os<<"\nnot marked()"<<std::endl; }
    if(src.use_backwards()){os<<"backwards"<<std::endl;}
    else{os <<"forwards"<<std::endl;}
    return os;
  }

template <typename PATH>
 PATH*  Intersection<PATH>::get_path(int whichpath){
      switch(whichpath){
      case INTERSECTION_FIRST:
	return first.first;
      case INTERSECTION_SECOND:
	return second.first;
      default:
	;
      }
  if(whichpath >= INTERSECTION_EXTRA){
	int index=whichpath-INTERSECTION_EXTRA;
	if(supernumaries.size() <=(size_type)index){
	  std::ostringstream ostmp;
	  ostmp<<"index "<<index<<" out of range\nvalid range: 0 - "<<supernumaries.size()-1;
	  throw xn_Error(ostmp.str());
	}
	else{
	  std::pair<path_type*, unsigned int> pr= supernumaries[index];
	  return pr.first;
	}
      }
      std::ostringstream ostmp;
      ostmp <<"whichpath must be INTERSECTION_FIRST or INTERSECTION_SECOND,not "
	    <<whichpath;
      throw  xn_Error(ostmp.str());
}

 template <typename PATH>
 const PATH* Intersection<PATH>::get_path(int whichpath)const{
      switch(whichpath){
      case INTERSECTION_FIRST:
	return first.first;
      case INTERSECTION_SECOND:
	return second.first;
      default:
	;
      }
  if(whichpath >= INTERSECTION_EXTRA){
	int index=whichpath-INTERSECTION_EXTRA;
	if(supernumaries.size() <=index){
	  std::ostringstream ostmp;
	  ostmp<<"index "<<index<<" out of range\nvalid range: 0 - "<<supernumaries.size()-1;
	  throw xn_Error(ostmp.str());
	}
	else{
	  std::pair<path_type*, unsigned int> pr= supernumaries[index];
	  return pr.second;
	}
      }
      std::ostringstream ostmp;
      ostmp <<"whichpath must be INTERSECTION_FIRST or INTERSECTION_SECOND,not "
	    <<whichpath;
      throw xn_Error(ostmp.str());
    }

 template <typename PATH>
   typename Intersection<PATH>::size_type 
 Intersection<PATH>::get_offset(int whichpath)const{
      switch(whichpath){
      case INTERSECTION_FIRST:
	return first.second;
      case INTERSECTION_SECOND:
	return second.second;
      default:
	;
      }
      if(whichpath >= INTERSECTION_EXTRA){
	int index=whichpath-INTERSECTION_EXTRA;
	if(supernumaries.size() <=(size_type)index){
	  std::ostringstream ostmp;
	  ostmp<<"index "<<index<<" out of range\nvalid range: 0 - "<<supernumaries.size()-1;
	  throw xn_Error(ostmp.str());
	}
	else{
	  std::pair<path_type*, unsigned int> pr= supernumaries[index];
	  return pr.second;
	}
      }
      std::ostringstream ostmp;
      ostmp <<"whichpath must be INTERSECTION_FIRST or INTERSECTION_SECOND, or (INTERSECTION_EXTRA + index)\nwhere index is a valid index into supernumaries, and not "
	    <<whichpath;
      throw xn_Error(ostmp.str());
    }
  ///make this a static member of Intersection
  template <typename PATH, typename FWDITER>
  std::pair<Intersection<PATH>*, int> 
  find_intersection_at( FWDITER first,  FWDITER last,
			PATH* p, unsigned int location, 
			unsigned int which){
    if(first==last){throw xn_Error("no candidate intersections");}
    //typedef  typename    std::deque<Intersection<PATH>*>::iterator intersection_iterator;
    typedef typename std::pair<Intersection<PATH>*, int> result_type;
      std::deque<result_type> finalists;
    for(FWDITER w= first; w!= last; w++){
      Intersection<PATH>* candidate = const_cast<Intersection<PATH>*>(&(*w));
      if(!candidate){throw xn_Error("NULL candidate");}
      if((p == candidate->get_first_path()) && (location == candidate->get_first_offset())){
	finalists.push_back(result_type(candidate, INTERSECTION_FIRST));
      }
      else if((p ==candidate->get_second_path()) && (location == candidate->get_second_offset())){
	finalists.push_back(result_type(candidate, INTERSECTION_SECOND));
      //return candidate;
      }
      else{
	typedef PATH path_type;
	typedef typename Intersection<PATH>::const_pathpair_iterator const_supernumary_iterator;
	std::deque<std::pair<path_type*, unsigned int> > supernumaries=candidate->get_extras();
	const_supernumary_iterator mark=supernumaries.begin();
	for(const_supernumary_iterator z = mark; z != supernumaries.end();z++){
	  const std::pair<path_type*, unsigned int>& pr =*z;
	   if((pr.first== p) && (pr.second == location)){
	     int count= z-mark;
	     finalists.push_back(result_type(candidate, INTERSECTION_EXTRA + count));
	     break;
	   }
	}
      }
  }
  if(finalists.size() <= which){
    throw xn_Error("no matching candidate in container");
  }
  //rank according to proximity to beginning of segment
  ClosestIntersectionPair<PATH> pointranker((*(p->begin() +  location)).first);
  std::sort(finalists.begin(), finalists.end(), pointranker);
  return *(finalists.begin()+ which); 
}

  /**
get the segment of source.first starting at source.second
   */
  template<typename PATH>
  Segment<coords*> 
  Intersection<PATH>::get_intersecting_segment(std::pair<path_type*, unsigned int>& source){
    iterator w= source.first->begin() + source.second;
      std::pair<coords, int>& first_coords = *w;
      std::pair<coords, int>* second_coords(NULL);
      //loop looking for drawing_click(W)?
      w++;
      if(w==source.first->end() || (*w).second & CLOSE_PATH){
	second_coords= &(*(source.first->begin()));
      }
      else{
	second_coords= &(*w);
      }
      Segment<coords*> res (&(first_coords.first), &(second_coords->first));
      return res;
      
    }

  template <typename PATH>
  bool operator==(const  Section<PATH>& left, const  Section<PATH>& right){
    if(&left == &right){return true;}
    if(left.get_path() != right.get_path()){return false;}
    if(left.get_start_offset() != right.get_start_offset()){return false;}
    if(left.get_end_offset() != right.get_end_offset()){return false;}
    //mult-segment section: only one possible
    if(left.get_start_offset() !=left.get_end_offset()){return true;}
    coords leftextra= left.get_start_intersection_coords();
    coords rightextra= right.get_start_intersection_coords();
    return leftextra.get_distance(&rightextra) <= coords::min_radius();
  }

template <typename PATH>
  bool operator<(const Section<PATH>& left, const Section<PATH>& right){
      if((size_t)left.get_path() < (size_t)right.get_path()){return true;}
      else if((size_t)left.get_path() > (size_t)right.get_path()){return false;}
      else if(/*same path*/left.get_start_offset() < right.get_start_offset()){return true;}
      else if(left.get_start_offset() > right.get_start_offset()){return false;}
      else if(left.get_end_offset() < right.get_end_offset()){return true;}
      else if(left.get_end_offset() > right.get_end_offset()){return false;}
      else{//both sectors are a part of a single segment
	coords leftstart = left.get_start_intersection_coords();
	coords rightstart = right.get_start_intersection_coords();
	coords prevpt = (*(left.get_path()->begin() + left.get_start_offset())).first;
	double left_d = prevpt.get_distance(&leftstart);
	double right_d = prevpt.get_distance(&rightstart);
	if(left_d < right_d){return true;}
	else if (left_d > right_d){return false;}
	else{//same sector!
	  return false;
	}
      }
}
   template <typename PATH>
   bool operator==(const  Intersection<PATH>& left, const  Intersection<PATH>& right){
     if(&left == &right){return true;}
     if(((left.first.first == right.first.first) && 
	 (left.first.second == right.first.second))&&
	((left.second.first == right.second.first)&& 
	 (left.second.second == right.second.second))){
#if CHECK_SUBPATH_SECTION
       std::cerr<<"operator==(Intersection)success - direct match"<<std::endl;
#endif
       return true;
     }
     if(((left.first.first == right.second.first)&&
	 (left.first.second == right.second.second))&&
	((left.second.first == right.first.first) && 
	 (left.second.second ==  right.first.second))){
 #if CHECK_SUBPATH_SECTION
      std::cerr<<"operator==(Intersection)succeeded - mirror match"<<std::endl;
 #endif
      return true;
     }
     return false;
   }

template<typename PATH>
const Segment<coords*> Intersection<PATH>::get_intersecting_segment(const std::pair<PATH*, unsigned int>& source)const{
      const_iterator w= source.first->begin() + source.second;
      const std::pair<coords, int>& first_coords = *w;
      const std::pair<coords, int>* second_coords(NULL);
      //loop looking for drawing_click(W)?
      w++;
      if(w==source.first->end() || (*w).second & CLOSE_PATH){
	second_coords= &(*(source.first->begin()));
    }
    else{
      second_coords= &(*w);
    }
    //or return const Segment<const coords*>
    const Segment<coords*> res (const_cast<coords*>(&(first_coords.first)), 
				const_cast<coords*>(&(second_coords->first)));
    return res;
  }

template<typename PATH>
coords Intersection<PATH>::get_intersection()const{
    const Segment<coords*> seg1=get_first_segment();
    const Segment<coords*> seg2=get_second_segment();
    if(!seg1.lines_cross(seg2)){
      throw xn_Error("no intersection");
    }
return seg1.intersection(seg2);
  }

 template<typename PATH>
 Section<PATH>::Section(PATH* p, std::pair<Intersection<PATH>*, int> l, 
	  std::pair<Intersection<PATH>*, int>  r, int outcrossings):
   subpath(p),startinter(l.first), endinter(r.first),
    crossings( outcrossings),
    use_flags(0), 
   p_start(l.second), p_end(r.second){
#if CHECK_SUBPATH_SECTION
#if XNDRAW_VERBOSE
   std::cerr <<*this<<" @ "<< this <<std::endl;
#endif
#endif
    if(!p){throw xn_Error("Section(): PATH* must not be NULL");}
    if(startinter){
      startextra = startinter->get_intersection();
    }
    if(endinter){
      endextra= endinter->get_intersection();
    }
    int startoff(0), endoff(subpath->size()-1);
    if(startinter){startoff=startinter->get_offset(p_start);}
    if(endinter){endoff=endinter->get_offset(p_end);}
    if((startoff ==endoff) && startinter && endinter) {  
      if(startextra.get_distance(&endextra) < coords::min_radius()){
	throw xn_Error("dumb empty Section error(or why dont Intersection::operator==() nowork?)");
      }
    }  
  }

template<typename PATH>
void Section<PATH>::set_used(bool used){
    if(!used){
      if(used_for_output()){use_flags ^= SECTION_USED;}
    }
    else{use_flags |=  SECTION_USED;}
  }

 template<typename PATH>
std::ostream& Section<PATH>::human_points(std::ostream& os)const{
   os<<"startinter: ";
	if(startinter){  os << startextra.x<<", "<<startextra.y<<std::endl; }
	else{os << "NULL"<<std::endl;}
	for(const_iterator w= begin(); w!= end(); w++){
	  os << (*w).first.x<<", "<< (*w).first.y<< "\t"
	     <<clicktype_as_string((*w).second)<<std::endl;
	}
	os <<"endinter: ";
	if(endinter){ os << endextra.x<<", "<<endextra.y;}
	else{os << "NULL";}
	return os;
 }

 template<typename PATH>
std::ostream& Section<PATH>::debug_print(std::ostream& os)const{
	os<<*this<<"\n @ "<< this<<"\npoints ...\n";
	return human_points(os);
      }


 
template<typename PATH>
template <typename ITER>
    Section<PATH>* Section<PATH>::get_terminal_section(ITER first,
						 ITER last, 
						 int joinend)const{
      for(ITER w= first;w!= last;w++){
	if((subpath== (*w).get_path()) &&
	   (!(*w).has_intersection(joinend))){
	  Section* res= &(*w);
	  if ((res==this)&&((*w).has_intersection(swap_joinend_flag(joinend)))){
	    throw xn_Error( "get_terminal_section() found caller, which in the circumstances - path is broken in multiple Sections - is incorrect");
	  }
	  return res;
	}
      }
      throw xn_Error("can't find 'terminal' section -ie either start or end - internal error");
    }

template<typename PATH>
std::deque<SectionShape<PATH>* > 
SectionGroup<PATH>::generate_output_shapes(int combine_flag, 
					   bool recursive){
  std::ostringstream ostmp;//error stream
  std::deque<SectionShape<PATH>* >res;
  if(combine_flag==PATH_COMBINE_OR){
    for(typename std::deque<SectionShape<PATH> >::iterator w= OR_shapes.begin();
	w != OR_shapes.end();w++){
      res.push_back(&(*w));
    }
  }
  else if( combine_flag==PATH_COMBINE_DIFF){
    create_DIFF_shapes();
    for(typename std::deque<SectionShape<PATH> >::iterator w= DIFF_shapes.begin();
	w != DIFF_shapes.end();w++){
      res.push_back(&(*w));
    }
    //OR the left and right sides components
    //
  }
  else{
    ostmp <<"bad combine_flag: "<<combine_flag<<
      "\none of PATH_COMBINE_OR or PATH_COMBINE_DIFF,required";
    throw xn_Error(ostmp.str());
  }
  return res;
}

template<typename PATH>
std::deque<SectionShape<PATH>* > 
SectionGroup<PATH>::get_output_shapes(int combine_flag, 
				      bool recursive){
      std::ostringstream ostmp;//error stream
      std::deque<SectionShape<PATH>* >res;
      int crossval(1), count(0);
      switch(combine_flag){
case  PATH_COMBINE_OR:
case  PATH_COMBINE_DIFF:
  return generate_output_shapes(combine_flag, recursive);
      case  PATH_COMBINE_AND:
	crossval=2;
      case  PATH_COMBINE_NOTAND:
	do{
	  count=0;
	  for (const_shape_iterator w = shapes.begin(); w!=shapes.end();w++){
	    std::cerr<<"shape no "<<count<<std::endl;
	    try{
	      if((*w).crossings_to_outside()==crossval){
		res.push_back(const_cast<SectionShape<PATH>*>(&(*w)));
		count++;
	      }
	    }
	    catch(const xn_Error& e){
	      std::cerr<< "caught error in crossings_to_outside():\n"<<e.what()<<std::endl;
	      std::cerr<<"our sections:\n";
	      for(const_section_iterator e_iter= sections.begin();e_iter!= sections.end(); e_iter++){
		std::cerr<< &(*e_iter)<<std::endl;
	      }
	      throw e;
	    }
	  }
	  if(!count){recursive=false;}
	  else{crossval+=2;}
	} while(recursive);
	if(!res.size()){
	  std::cerr <<"no shapes found for crossinval of "<<crossval<<std::endl;
	}
	break;
      default:
	ostmp <<"bad combine_flag: "<<combine_flag<<
	  "\none of PATH_COMBINE_OR, PATH_COMBINE_DIFF, PATH_COMBINE_AND orPATH_COMBINE_NOTAND required";
	throw xn_Error(ostmp.str());
      }
      return res;
    }

template<typename PATH>
Section<PATH>* 
SectionGroup<PATH>::find_first_output_section(int flag){
    for(section_iterator w= sections.begin();w!= sections.end(); w++){
   
      if(!(*w).marked()){
	throw xn_Error("mark_sections() b4 calling this");
      }
      if(flag== PATH_COMBINE_OR || flag== PATH_COMBINE_NOTAND){
	if(!((*w).crossings)){return &(*w);}
      }
      else if(flag== PATH_COMBINE_AND){
	if((*w).crossings == 1){return &(*w);}
      }
      else if(PATH_COMBINE_DIFF){
	throw xn_Error("no way to know where to start for PATH_COMBINE_DIFF without knowing which path comes 'first' ie which is subtracted from");
      }
    }
    throw xn_Error("shouldm't get here");
  }
 
   template<typename PATH>
bool SectionGroup<PATH>::does_loop_crossings_on_output(int flag){
      switch(flag){
      case PATH_COMBINE_OR:
      case PATH_COMBINE_DIFF:
	return false;
      case PATH_COMBINE_AND:
      case PATH_COMBINE_NOTAND:
	return true;
      default:
	;
      }
      std::ostringstream ostmp;
      ostmp <<"bad flag: should be one of  PATH_COMBINE_OR,  PATH_COMBINE_AND,  PATH_COMBINE_NOTAND or  PATH_COMBINE_DIFF, rather than :=>"<< flag << " <=:";
      throw xn_Error(ostmp.str());
    }

    template<typename PATH>
 int  SectionGroup<PATH>::get_startcrossings(int flag){
      switch(flag){
      case PATH_COMBINE_OR:
      case PATH_COMBINE_DIFF:
      case PATH_COMBINE_NOTAND:
	return 0;
      case PATH_COMBINE_AND:
      	return 1;
      default:
	;
      }
      std::ostringstream ostmp;
      ostmp <<"bad flag: should be one of  PATH_COMBINE_OR,  PATH_COMBINE_AND,  PATH_COMBINE_NOTAND or  PATH_COMBINE_DIFF, rather than :=>"<< flag << " <=:";
      throw xn_Error(ostmp.str());
    }

template< typename PATH >
bool operator==(const SectionShape<PATH>& left, 
		const SectionShape<PATH>& right){
  if(&left == &right){return true;}
  if((left.outlines.size() != right.outlines.size()) ||
     (left.crossings_to_outside() != right.crossings_to_outside()))
    {return false;}
  typedef typename SectionShape<PATH>::size_type size_type;
  if(!left.outlines.size()){return true;} //is really an error
  for(size_type sx =0;sx != left.outlines.size();sx++){
    SectionShape<PATH> tmp(right);
    tmp.rotate(sx);
    if(tmp.outlines== left.outlines){return true;}
  }
  SectionShape<PATH> tmp2(right);
  tmp2.reverse();
  for(size_type sy =1;sy != left.outlines.size();sy++){
    SectionShape<PATH> tmp(tmp2);
    tmp.rotate(sy);
    if(tmp.outlines== left.outlines){return true;}
  }
  return false;
}


template< typename PATH >
std::ostream& operator<<(std::ostream& os, const SectionShape<PATH>& src){
typedef typename SectionShape<PATH>::const_iterator const_iterator;
 os<<"SectionShape at "<<&src<<"\noutlines at:"<<std::endl;
  for(const_iterator w = src.outlines.begin(); w!= src.outlines.end();w++){
    os <<*w<<std::endl;
  }
  return os;
}
 template< typename PATH >
 bool SectionShape<PATH>::intersects(const SectionShape& shape)const{
 bool addtheirs(false) ,donetheirs(false);;
    for(const_iterator theirs= shape.outlines.begin();(theirs!=shape.outlines.end()) && (!donetheirs) ;theirs++){
      for(const_iterator ours= outlines.begin();(ours!=outlines.end()) && (!donetheirs);ours++){
	
	if(*theirs == *ours){donetheirs=true;addtheirs=false;}
	else if((*ours)->has_common_intersection(*theirs)){
	  addtheirs=true;
	}
      }
    }
    return addtheirs;
 }

  template< typename PATH >
  bool SectionShape<PATH>::is_contiguous(const SectionShape& shape)const{
    bool donetheirs(false);
    for(const_iterator theirs= shape.outlines.begin();(theirs!= shape.outlines.end()) && (!donetheirs);theirs++){
      for(const_iterator ours= outlines.begin();(ours!=outlines.end()) && (!donetheirs);ours++){
	if(&(*ours) == &(*theirs)){
	  return true;
	}
      }
    }
    return false;
  }

template< typename PATH >
template <typename SHAPEITER>
std::deque<SectionShape<PATH> > 
SectionShape<PATH>::get_shapes_that_intersect(SHAPEITER first,
					      SHAPEITER last)const{
  std::deque<SectionShape> res;
  for(;first !=last;first++){
    if(&(*first) == this){continue;}
    if(intersects(*first)){res.push_back(*first); 	}
  }
  return res;
}

  template< typename PATH >
    template< typename SHAPEITER >
    std::deque<SectionShape<PATH> > 
    SectionShape<PATH>::get_contiguous_shapes(SHAPEITER first,
					      SHAPEITER last){
    std::deque<SectionShape> res;
    for(;first !=last;first++){
      if(&(*first) == this){continue;}
      if(is_contiguous(*first)){
	res.push_back(*first);
      }
    }
    return res;
  }
template< typename PATH >
  void SectionShape<PATH>::check_crossings()const{
    std::cerr<<"check_crossings()"<<std::endl;
    for(const_iterator w = outlines.begin(); w!= outlines.end();w++){
      std::cerr<<w-outlines.begin()<<"th section";
      if(!(*w).first->marked()){throw xn_Error("shape outlines must be marked()");}
      else{std::cerr<<"crossings= "<< (*w).first->crossings_to_outside()<<std::endl;}
    }
    }

template<typename PATH>
int SectionShape<PATH>::crossings_to_outside()const{
    int low(-1), high(-1);
    for(const_iterator w = outlines.begin(); w!= outlines.end();w++){
      //if(!(*w)->marked()){throw xn_Error("shape outlines must be marked()");}
      int wcrossings= (*w).first->crossings_to_outside();
      if(low <0){low= wcrossings;}
      else if((low != wcrossings) &&( high != wcrossings)){
	if(high <0){
	  if(wcrossings >low){high=wcrossings;}
	  else{high=low; low=wcrossings;}
	}
	else{
	  std::ostringstream ostmp;
	  ostmp << "values of crossings for outline sections of shape must all be within a range of two adjacent integers\nlow was: "<<low<<"\nhigh was: "<<high<<"\nnew value: "<< wcrossings;
	  throw xn_Error(ostmp.str());
	}
      }
    }
    if(low <0){throw xn_Error("shape has no outlines");}
    else if(high < 0){return low+1;}
    else if((high - low) != 1){
      throw xn_Error("values of crossings for outline sections of shape must all be within a range of two adjacent integers");
    }
    return high;
  }
  /*
  ///this not used
template<typename PATH>
  std::pair<Section<PATH>*, bool> 
SectionGroup<PATH>::find_next_output_section( Section<PATH>& starter,
					      int flag,
					      bool previous_forwards){
  int outside = (previous_forwards? -groupside : groupside);
  typedef std::pair<Section<PATH>*, bool> return_type;
  Intersection<PATH>* the_inter(NULL);
    int section_endflag(SECTION_END);
    if(previous_forwards){
      the_inter=starter.endinter;
      if(!starter.has_end_intersection()){
	Section<PATH>* sec=starter.get_pathstart_section(sections);
	return return_type(sec, previous_forwards);
      }
    }
    else{
      the_inter=starter.startinter;
      section_endflag = SECTION_START;
      outside =-outside;
      if(!starter.has_start_intersection()){
	Section<PATH>* sec=starter.get_pathend_section(sections);
	return return_type(sec, previous_forwards);
      }
    }
    //trivial cases dealt with...
    //this is a mess - fix soon
    std::pair<Section<PATH>*, int> bestguesspair = choose_output_section(starter,  section_endflag, (previous_forwards ? outside :-outside), flag);
    Section<PATH>* bestguess=bestguesspair.first;
    if(! bestguess ){throw xn_Error("choose_uused_section returned NULL");}
    Intersection<PATH>* common(NULL);
    if(previous_forwards){common=  starter.endinter;}
    else{common=  starter.startinter;}
    bool newdirection=  Section<PATH>::junction_continues_direction(*bestguess, starter, common);
    return std::pair<Section<PATH>*, bool>(bestguess, newdirection?previous_forwards: !previous_forwards);
      //von_boob();//compile error
  }
*/
template<typename PATH>
bool Section<PATH>::has_unmarked_sections(const std::deque<Section<PATH> >& sections){
    for(const_section_iterator w = sections.begin(); w!= sections.end(); w++){
      if(!(*w).marked()){return true;}
    }
    return false;
  }
  /**
flags to get all Section that might possibly adjoina Section
@todo could just return an int with all the values ORed
like: `return (SECTION_START |SECTION_END|SECTION_ABOVE |SECTION_BELOW|SECTION_CONTINUED);'\n
.  Another issue is if we create a child Intersection class allowing 3+ paths to intersect,
and the intersection to occur at an existing point from one of more of them
(We could somehow mark that point as 'stolen',
but of course this further complicates Section::begin() and Section::end()
and iterator ops, and means some child class of Section would probably be required to 
encapsulate this and maintain some degree of sanity)
Both child classes will need a virtual API?
   */
template<typename PATH>
std::deque<int> Section<PATH>::get_all_adjoining_sections_flags(){
    std::deque<int> res;
    res.push_back(SECTION_START |SECTION_ABOVE);
    res.push_back(SECTION_START|SECTION_BELOW);
    res.push_back(SECTION_START|SECTION_CONTINUED);
    res.push_back(SECTION_END|SECTION_ABOVE);
    res.push_back(SECTION_END|SECTION_BELOW);
    res.push_back(SECTION_END|SECTION_CONTINUED);
    return res;
  }
template<typename PATH>
std::deque<int> Section<PATH>::get_adjoining_sections_flags(int joinend)const{
std::deque<int> res;
 if(joinend & SECTION_START){
   if(has_start_intersection()){
     res.push_back(SECTION_START |SECTION_ABOVE);
     res.push_back(SECTION_START|SECTION_BELOW);
     res.push_back(SECTION_START|SECTION_CONTINUED);
   }
 }
 else if(joinend & SECTION_END){
   if(has_end_intersection()){
     res.push_back(SECTION_END|SECTION_ABOVE);
     res.push_back(SECTION_END|SECTION_BELOW);
     res.push_back(SECTION_END|SECTION_CONTINUED);
   }
 }
 else{throw xn_Error("SECTION_START or SECTION_END as joinend here");}
 return res;
}
template<typename PATH>
std::deque<int> Section<PATH>::get_adjoining_sections_flags()const{
std::deque<int> res;
 if(has_start_intersection()){
   res.push_back(SECTION_START |SECTION_ABOVE);
   res.push_back(SECTION_START|SECTION_BELOW);
   res.push_back(SECTION_START|SECTION_CONTINUED);
 }
 if(has_end_intersection()){
   res.push_back(SECTION_END|SECTION_ABOVE);
   res.push_back(SECTION_END|SECTION_BELOW);
   res.push_back(SECTION_END|SECTION_CONTINUED);
 }
 return res;
}
template<typename PATH>
std::deque<int> Section<PATH>::get_closest_adjoining_sections_flags()const{
std::deque<int> res;
 if(has_start_intersection()){
   res.push_back(SECTION_START |SECTION_ABOVE);
   res.push_back(SECTION_START|SECTION_BELOW);
   // res.push_back(SECTION_START|SECTION_CONTINUED);
 }
 if(has_end_intersection()){
   res.push_back(SECTION_END|SECTION_ABOVE);
   res.push_back(SECTION_END|SECTION_BELOW);
   //res.push_back(SECTION_END|SECTION_CONTINUED);
 }
 return res;
}
template<typename PATH>
std::deque<int> Section<PATH>::get_closest_adjoining_sections_flags(int joinend)const{
std::deque<int> res;
if(joinend & SECTION_START){
 if(has_start_intersection()){
   res.push_back(SECTION_START |SECTION_ABOVE);
   res.push_back(SECTION_START|SECTION_BELOW);
 }
 }
else if(joinend & SECTION_END){
 if(has_end_intersection()){
   res.push_back(SECTION_END|SECTION_ABOVE);
   res.push_back(SECTION_END|SECTION_BELOW);
 }
 }
 else{throw xn_Error("SECTION_START or SECTION_END as joinend here");}
 return res;
}

template<typename PATH>
std::deque<Section<PATH>*> 
Section<PATH>::get_adjoining_sections(std::deque<Section<PATH> >& sections)const{
    std::deque<Section*> result;
    typedef std::deque<int>::const_iterator int_iterator;
    std::deque<int>  sectionflags = get_adjoining_sections_flags();
    for(int_iterator flagiter =  sectionflags.begin();
	flagiter !=sectionflags.end(); flagiter++){
      int flag=*flagiter;
      Section* adj = get_adjacent_section(sections.begin(), sections.end(), flag);
      if(adj){
	result.push_back(adj);
	std::deque<Section*> extratmp= get_extra_adjacent_sections(sections.begin(), sections.end(), flag);
	if(extratmp.size()){
	  for(typename std::deque<Section*>::iterator w = extratmp.begin();w!= extratmp.end();w++){
	    result.push_back(*w);
	  }
	}
      }
    }
    return result;
  }
  
  template<typename PATH>
  template <typename FWDITER>
  std::deque<std::pair<Section<PATH>*, int> > 
  Section<PATH>::get_closest_adjoining_sections_with_joinend(FWDITER first, FWDITER last,
							    int joinend, int numcross){
    if(!((joinend == SECTION_START)|| (joinend == SECTION_END))){
      throw xn_Error("joinend should be SECTION_START or SECTION_END only in :get_closest_adjoining_sections_with_joinend");
    }
    std::deque<std::pair<Section<PATH>*, int> > result;
    if(!get_intersection(joinend)){return result;}
    typedef std::deque<int>::const_iterator int_iterator;
    std::deque<int>  sectionflags = get_closest_adjoining_sections_flags(joinend);
    for(int_iterator flagiter =  sectionflags.begin();
	flagiter !=sectionflags.end(); flagiter++){
      int flag=*flagiter;
      Section* adj = get_adjacent_section(first,last, flag);
      if(adj &&(adj->crossings == numcross)){
	result.push_back(std::pair<Section<PATH>*, int>(adj,flag));
	std::deque<Section*> extratmp= get_extra_adjacent_sections(first,last, 
								     flag);
	if(extratmp.size()){
	  for(typename std::deque<Section*>::iterator w = extratmp.begin();w!= extratmp.end();w++){
	    if(*w && (numcross == (*w)->crossings)){
	      result.push_back(std::pair<Section<PATH>*, int>(*w, flag));
	    }
	  }
	}
      }
    }
    return result;
  }

template<typename PATH>
template <typename FWDITER>
std::deque<std::pair<Section<PATH>*, int> > 
Section<PATH>::get_closest_adjoining_sections_with_joinend(FWDITER first, 
							   FWDITER last,
							   int joinend){
  if(!((joinend == SECTION_START)|| (joinend == SECTION_END))){
    throw xn_Error("joinend should be SECTION_START or SECTION_END only in :get_closest_adjoining_sections_with_joinend");
  }
  std::deque<std::pair<Section<PATH>*, int> > result;
    typedef std::deque<int>::const_iterator int_iterator;
    std::deque<int>  sectionflags = get_closest_adjoining_sections_flags(joinend);
    for(int_iterator flagiter =  sectionflags.begin();
	flagiter !=sectionflags.end(); flagiter++){
      int flag=*flagiter;
      Section* adj = get_adjacent_section(first,last, flag);
      if(adj){
	result.push_back(std::pair<Section<PATH>*, int>(adj,flag));
	std::deque<Section*> extratmp= get_extra_adjacent_sections(first,last, 
								   flag);
	if(extratmp.size()){
	  for(typename std::deque<Section*>::iterator w = extratmp.begin();w!= extratmp.end();w++){
	    result.push_back(std::pair<Section<PATH>*, int>(*w, flag));
	  }
	}
      }
    }
    return result;
}

  ///doesn't return SECTION_CONTINUED values 
  ///unlike get_closest_adjoining_sections()
template<typename PATH>
std::deque<Section<PATH>*> 
Section<PATH>::get_closest_adjoining_sections(std::deque<Section<PATH> >& sections)const{
    std::deque<Section*> result;
    typedef std::deque<int>::const_iterator int_iterator;
    std::deque<int>  sectionflags = get_closest_adjoining_sections_flags();
    for(int_iterator flagiter =  sectionflags.begin();
	flagiter !=sectionflags.end(); flagiter++){
      int flag=*flagiter;
      //if(has_adjoining_section(flag) ){
      //get_adjoining_sections_flags() only gets the flags matching sections
      // that the object actually possesses, so a test unnecessary
	result.push_back(get_adjacent_section(sections.begin(), sections.end(), flag));
	//}
    }   
    return result;
  }
template<typename PATH>
std::deque<Section<PATH>*> SectionGroup<PATH>::sections_needing_shapes(int crosscnt){
std::deque<Section<PATH>*> osections=get_marked_sections(crosscnt);
 std::deque<Section<PATH>*> res;
    typedef typename std::deque<Section<PATH>*>::iterator ptr_iterator;
    for(ptr_iterator w= osections.begin(); w!= osections.end();w++){
      if(((*w)->crossings==crosscnt)&&
	 (!section_exhausted_for_shapes(*(*w), crosscnt))){
	res.push_back(*w);
      }
    }
    return res;
}

  template<typename PATH>
 int SectionGroup<PATH>::max_crossings()const{
    int res(-2);
    for(const_section_iterator w = sections.begin(); w!=sections.end();w++){
      if((*w).crossings >res){res=(*w).crossings;}
    }
    return res;
  }

  template<typename PATH>
  bool SectionGroup<PATH>::need_shapes()const{
    if(has_unmarked_sections()){return true;}
    size_type maxcross = max_crossings();
    std::deque<Section<PATH>*> osections=const_cast<SectionGroup<PATH>*>(this)->get_marked_sections(maxcross);
    typedef typename std::deque<Section<PATH>*>::iterator ptr_iterator;
    for(ptr_iterator w= osections.begin(); w!= osections.end();w++){
      if(((*w)->crossings==(int)maxcross)&&
	 (!section_exhausted_for_shapes(*(*w), maxcross))){return true;}
    }
    return false;
  }

template<typename PATH>
simple_click_history& 
SectionShape<PATH>::add_to_path(simple_click_history& outpath){
    bool forwards(true), firstseg(true);
    for (iterator w = outlines.begin(); w != outlines.end();w++){
      int start_joinend=Section<PATH>::swap_joinend_flag((*w).second);
      Intersection<PATH>* connector=(*w).first->get_intersection((*w).second);
      if((!firstseg) && connector){
	if(start_joinend == SECTION_START){forwards = true;	}
	else if(start_joinend ==SECTION_END){forwards=false;}
	else{throw xn_Error("bad flag at shape.second");}
      }
      (*w).first->add_to_path(outpath, forwards, firstseg);
      firstseg=false;
    }
    return outpath;
  }

template<typename PATH>
int Section<PATH>::swap_joinend_flag(int joinend){
      if(joinend &SECTION_START){
	joinend ^= SECTION_START;
	joinend |= SECTION_END;
      }
      else if (joinend &SECTION_END){
	joinend ^= SECTION_END;
	joinend |= SECTION_START;
      }
      else{
	throw xn_Error("joinend should contain SECTION_START or SECTION_END");
      }
      return joinend;
    }

  template<typename PATH>
  void Section<PATH>::add_to_path(simple_click_history& outp,
				  bool forwards,
				  bool first_section){
    if(!subpath->size()){return;}
    coords* previous(NULL);
    if(forwards){	
      iterator current= begin();
      if(startinter){previous=&startextra;}
      else if(current!=end()){
	previous= &((*current++).first);}
      else{throw xn_Error("invalide sectiom");}
      if(first_section ){
	outp.push_back(std::pair<coords, int>(*previous, MOVE_CLICK));
      }
      //else if(previous != &startextra){
      //outp.push_back(std::pair<coords, int>(*previous, LINE_CLICK));
      //}	
      for(;current !=end();current++){
	  if(!previous){throw xn_Error("previous pt NULL");}
	  std::pair<coords, int>& ptpr=*current;
	  if(ptpr.second &CLOSE_PATH){
	    std::cerr<<"add_to_path(): ptpr.second &CLOSE_PATH\nWatch this for odd behaviourwhen there are examples to test-usual path does without"<<std::endl;
	    ptpr=*(subpath->begin());
	  }
	  if(ptpr.first.get_distance(previous) > coords::min_radius()){
	    outp.push_back(std::pair<coords, int> (ptpr.first, LINE_CLICK));
	    previous=&(ptpr.first);
	  }
	}
	if(endinter){
	  if(!previous){throw xn_Error("previous pt NULL");}
	  else if(endextra.get_distance(previous) <= coords::min_radius()){
	    (*( outp.rbegin())).first= endextra;
	  }
	  outp.push_back(std::pair<coords, int>(endextra, LINE_CLICK));
	}
	else if(outp.size() && 
		(!((*(outp.rbegin())).second & CLOSE_PATH))){
	  outp.push_back(std::pair<coords, int>((*(subpath->begin())).first, LINE_CLICK));
	  //outp.push_back(std::pair<coords, int>((*(subpath->begin())).first, CLOSE_PATH));
	}
      }
      else{//backwards
	reverse_iterator current=rbegin();
	if(endinter){previous=&endextra;}
	else { previous=&((*(subpath->begin())).first);}
	if((current!=rend()) && ((*current).second &CLOSE_PATH)){
	  current++;
	}
	if(first_section ){
	  outp.push_back(std::pair<coords, int>(*previous, MOVE_CLICK));
	}	
	for(;current !=rend();current++){
	  if(!previous){throw xn_Error("previous pt NULL");}
	  std::pair<coords, int>& ptpr=*current;
	  if(!(ptpr.second &CLOSE_PATH)){
	    if(ptpr.first.get_distance(previous) > coords::min_radius()){
	    outp.push_back(std::pair<coords, int>(ptpr.first, LINE_CLICK));
	    previous= &(ptpr.first);
	    }
	  }//add next bit if problem persist
	  /* else if((*(subpath->begin())).first.get_distance(previous)> coords::min_radius()){
	    previous= &((*(subpath->begin())).first);
	    outp.push_back(std::pair<coords, int>(*previous,LINE_CLICK));}
	    }*/
	}
	if(startinter){
	  if(!previous){throw xn_Error("previous pt NULL");}
	  else if(startextra.get_distance(previous) <= coords::min_radius()){
	    throw xn_Error("can't make last segment");
	  }
	  if(startextra.get_distance(previous) > coords::min_radius()){
	    outp.push_back(std::pair<coords, int>(startextra, LINE_CLICK));
	  }
	  else{
	    (*( outp.rbegin())).first= startextra;
	  }
	}
	else if(outp.size() && 
		(!((*(outp.rbegin())).second & CLOSE_PATH))){
	  outp.push_back(std::pair<coords, int>((*(subpath->begin())).first, LINE_CLICK));
	}
      }
  }

 template<typename PATH>
void SectionGroup<PATH>::create_shapes(){
      int crossingcount(0);
      while(need_shapes()){
	std::deque<Section<PATH>*> osections;
	if(has_unmarked_sections()){
	  osections=get_marked_sections(crossingcount);
	}
	else{osections=sections_needing_shapes(crossingcount);}
	if(!osections.size()){
	  std::ostringstream ostmp;
	  ostmp<<unmarked_section_count() << " unmarked sections from "
	       <<sections.size()<<" total. but get_marked_sections("
	       <<crossingcount<<") empty";
	  throw xn_Error(ostmp.str());
	}
	typedef typename std::deque<Section<PATH>*>::iterator ptr_iterator;
	for(ptr_iterator w= osections.begin(); w!= osections.end();w++){
	  Section<PATH>* startsector=*w;
	  //outside segments have a shape only on one side(inside of donut?)
	  std::deque<std::pair<Section<PATH>*, int> > shapeoutlines;
#if CHECK_SUBPATH_SECTION
	  std::cerr<<w-osections.begin() <<"th section: crossval="
		   <<crossingcount<<"\n"
		   <<"starting next shape at sector: "<<startsector<<std::endl;
#endif
	  int joinend( SECTION_END), inside(-groupside);
	  if((!(startsector->crossings))&& (OR_shapes[0].uses_outline(*startsector))){
	    int shapejoinend = OR_shapes[0].get_joinend(*startsector);
	    if(shapejoinend==SECTION_START){inside = -inside;}
	  }
	  int extend(joinend);
	  shapeoutlines.push_back(std::pair<Section<PATH>*, int>(startsector, joinend));
	  Section<PATH>* cursector =startsector;
	  bool sectionused = section_exhausted_for_shapes(*cursector, crossingcount);
	  //reverse inside's value if we keep finding the outside
	  if(!sectionused){
	    if(crossingcount){
	      Section<PATH>* cursector2(cursector),* sectextra(NULL);
	      bool extrasection(true);
	      std::cerr<<"new'extra' shapefrom sector:"<<startsector<<std::endl;
	      std::deque<std::pair<Section<PATH>*, int> > extoutlines;
	      extoutlines.push_back(std::pair<Section<PATH>*, int>(startsector, extend));
	      while(extrasection){
		if(!(cursector2->has_intersection(extend))){
		  sectextra = cursector2->get_terminal_section(sections.begin(), sections.end(), Section<PATH>::swap_joinend_flag(extend));
		}
		else{
		  std::pair<Section<PATH>*, int> extrapair= cursector2->choose_adjacent_section(sections.begin(), sections.end(), extend, inside);
		  extend= extrapair.second;
		  sectextra=extrapair.first;
		}
		if(!sectextra->marked()){
		  sectextra->set_crossings(crossingcount + 1);
		}
		cursector2=sectextra;
		if(sectextra == startsector){extrasection=false;}
		else{
		  std::pair<Section<PATH>*,int> extshapepair(sectextra, extend);
		    extoutlines.push_back(extshapepair);}
	      }
	      SectionShape<PATH> shape(extoutlines);
	      std::cerr<<"shape with "<<extoutlines.size()<<"sides"<<std::endl;
	      add_shape(shape);
	    }
	    while(!sectionused){
	      std::cerr<<"while(!sectionused){\n";
	      Section<PATH>* nxt(NULL);
	      if(!cursector->has_intersection(joinend)){
		nxt = cursector->get_terminal_section(sections.begin(),sections.end(), Section<PATH>::swap_joinend_flag(joinend));
		std::cerr<<"no intersection this end"<<std::endl;
	      }
	      else{
		//cursector->print_choices( sections.begin(), sections.end(), std::cerr,joinend, -inside);
		std::pair<Section<PATH>*, int> curpair= cursector->choose_adjacent_section(sections.begin(), sections.end(), joinend, -inside);
		std::pair<Section<PATH>*, int> altpair= cursector->choose_adjacent_section(sections.begin(), sections.end(), joinend, inside);
		std::cerr <<"chosen section: "<<curpair.first<<"\nthe alt: "<<altpair.first<<std::endl;
		nxt= curpair.first;
		if(altpair.first==nxt){
		  if(altpair.second == curpair.second){
		    throw xn_Error("choose_adjacent_section broken:no shapeside");
		  }
		  if(nxt->startinter &&(nxt->startinter==nxt->endinter)){
		    std::cerr<<"section looped to self intersect"<<std::endl;
		    if(is_twist_intersection(*(nxt->startinter))){
		      std::cerr<<"is_twist_intersection() returns true"<<std::endl;
		    }
		  }
		}
		if((!crossingcount) && (nxt!=startsector) &&(nxt!=cursector) &&
		   (cursector->crossings==0) && (nxt->crossings ==0)){
		  if(!OR_shapes.size()){throw xn_Error("empty OR_shapes!?!");}
		  if(!(altpair.first->crossings==0)){
		    std::cerr<<"cursector,next->crossings==0, but alt not"
			     <<std::endl;
		  }
		  if(cursector->has_intersection(joinend)){
		    if(OR_shapes[0].uses_outline(*nxt)){
		    std::cerr<<"\n******ERROR********\nadjacent all marked?\nthe sections:cursector:\n"
			     <<*cursector<<"\nnext=>"<<*nxt<<std::endl
			     <<"\nALL the sections: \n"<<*this;
		    throw xn_Error("adjacent sectors both marked -tracing on wrong side?");
		    }
		  }
		  else{/*no matter*/		  }
		}
		joinend= curpair.second;
	      }
	      std::cerr <<"next section: "<<nxt<<std::endl;
	      if(!nxt->marked()){nxt->set_crossings(crossingcount + 1);}
	      if(nxt == startsector){sectionused=true;}
	      else{std::pair<Section<PATH>*, int> shapepair(nxt, joinend);
		shapeoutlines.push_back(shapepair);}
	      cursector=nxt;
	    }
	    size_t deletablecheck=shapes.size();
	    SectionShape<PATH> shape(shapeoutlines);
	    add_shape(shape);
	    if(shapes.size() > deletablecheck){std::cerr<<"success";}
	    else{std::cerr<<"not added -already collected-";}
	    std::cerr<<"=> now "<<shapes.size()<<"shapes collected"<<std::endl;
	  }
	}
	crossingcount++;
	std::cerr<<"incremented crossincount to "<<crossingcount<<std::endl;
      }
      std::cerr<<"after create_shapes():"<<shapes.size() <<" shapes"<<std::endl;
      check_shapes();
	//<<*this<<std::endl;
 }

  template<typename PATH>
  void SectionGroup<PATH>::check_shapes()const{
    int count(0);
    for (const_shape_iterator w = shapes.begin(); w!=shapes.end();w++){
      std::cerr<<"shape no "<<count++<<"\ncrossings: "<<
	(*w).crossings_to_outside()<<std::endl;;
      if((*w).crossings_to_outside()> 64){//crossings_absmax
	throw xn_Error("too large");
      }
    }
  }

  template<typename PATH>
  void SectionGroup<PATH>::mark_adjacent_sections(Section<PATH>* markfrom){
    if(!markfrom){throw xn_Error("NULL arg");}
    if(markfrom->crossings < 0){throw xn_Error("set from a marked section");}
    int crossval=markfrom->crossings+1;
    std::deque<std::pair<Section<PATH>*, int> > startadjoining
      = markfrom->get_closest_adjoining_sections_with_joinend(sections.begin(), sections.end(), SECTION_START);
    std::deque<std::pair<Section<PATH>*, int> > endadjoining
      = markfrom->get_closest_adjoining_sections_with_joinend(sections.begin(), sections.end(), SECTION_END);
    JoinRank<Section<PATH> > sorter(*markfrom, true);
    std::sort(startadjoining.begin(),startadjoining.end(), sorter);
    std::sort(endadjoining.begin(),endadjoining.end(), sorter);
    if(startadjoining.size() >= 2){
      Section<PATH>* pstart((*(startadjoining.begin())).first);
      Section<PATH>* pend((*(startadjoining.end()-1)).first);
      if(!pstart->marked()){pstart->set_crossings(crossval);}
      if(!pend->marked()){pend->set_crossings(crossval);}
    }
    if (endadjoining.size() >= 2){
      Section<PATH>* pstart((*(endadjoining.begin())).first);
      Section<PATH>* pend((*(endadjoining.end()-1)).first);
      if(!pstart->marked()){pstart->set_crossings(crossval);}
      if(!pend->marked()){pend->set_crossings(crossval);}
    }
  }

template<typename PATH>
bool Section<PATH>::has_adjoining_section(int flag)const{
    if(flag & SECTION_START){
      return has_start_intersection();
    }
    if(flag & SECTION_END){
      return has_end_intersection();
    }
    throw xn_Error("has_adjoining_section(int flag)=>flag arg must containSECTION_START or SECTION_END");
  }

   template<typename PATH>
   std::pair<Section<PATH>*, int> 
   Section<PATH>::smallest_adjacent_mark(std::deque<Section<PATH> >& sections)const{
    std::pair<Section*, int> res(NULL,-1);
    int restmp(-1);
    Section* sect(NULL);
    ///@todo: should only choose from the 4 'closest' adjoining sections
    /// ie those with the acutest angle on either side
    std::deque<Section*> adjacent = get_adjoining_sections(sections);

    for(typename std::deque<Section*>::iterator sectiter= adjacent.begin(); sectiter != adjacent.end();sectiter++){
      sect =*sectiter;
      restmp= sect->crossings;
      if(restmp >=0 && (res.second <0 || res.second >restmp)){
	res.first=sect;
	res.second=restmp;
      }
    }
    return res;
  }

 template<typename PATH>
 int Section<PATH>::whichend(const Intersection<PATH>* an_inter)const{
    if(an_inter==startinter){return -1;}
    else if(an_inter==endinter){return 1;}
    throw xn_Error("not one of MY intersections says the section");
  }

template<typename PATH>
 bool Section<PATH>::has_common_intersection(const Section<PATH>& other)const{
  return (startinter &&((startinter== other.startinter) || (startinter== other.endinter ))) ||
    (endinter &&((endinter== other.startinter) ||( endinter== other.endinter )));
}
  template<typename PATH>
  std::deque<Intersection<PATH>*> 
  Section<PATH>::get_common_intersections(const Section& other)const{
    std::deque<Intersection<PATH>*> res;
      if(startinter &&((startinter== other.startinter) || (startinter== other.endinter ))){
	res.push_back(startinter);
      }
      if(endinter &&((endinter== other.startinter) ||( endinter== other.endinter ))){
	res.push_back(endinter);
      }
      return res;
  }

 template<typename PATH>
 Intersection<PATH>* 
Section<PATH>::get_common_intersection(const Section<PATH>& other)const{
   if(startinter &&((startinter== other.startinter) || (startinter== other.endinter ))){
      return startinter;
    }
   else if(endinter &&((endinter== other.startinter) ||( endinter== other.endinter ))){
      return endinter;
    }
   std::ostringstream ostmp;
   ostmp<< "no common intersection between \n"<<*this<<"\n\t+++++++++++   and ++++++++++++"
	<< other<<"\n\t****************";
   std::string s_e=ostmp.str();
   std::cerr<< s_e;
   throw xn_Error(s_e);
  }
 
  template<typename PATH>
 template <typename ITER>
  Section<PATH>* 
    Section<PATH>::get_pathstart_section(ITER first, ITER last)const{
    for(ITER w= first;w!=last;w++){
      if((subpath== (*w).get_path()) &&
	 (!(*w).has_start_intersection())){return &(*w);}
    }
    throw xn_Error("no start section without an intersection");
  }
  

 template<typename PATH>
 Section<PATH>* 
 Section<PATH>::get_pathstart_section(std::deque<Section<PATH> >& sections)const{
    for(section_iterator w= sections.begin();w!=sections.end();w++){
      if((subpath== (*w).get_path()) &&
	 (!(*w).has_start_intersection())){return &(*w);}
    }
    throw xn_Error("no start section without an intersection");
  }
  
template <typename PATH>
  template <typename ITER>
  Section<PATH>* 
  Section<PATH>::get_pathend_section(ITER first, ITER last)const{
for(ITER w= first;w!= last;w++){
      if((subpath== (*w).get_path()) &&
	 (!(*w).has_end_intersection())){return &(*w);}
    }
    throw xn_Error("no start section without an intersection");
}
 template<typename PATH>
Section<PATH>* 
 Section<PATH>::get_pathend_section(std::deque<Section<PATH> >& sections)const{
    for(section_iterator w= sections.begin();w!=sections.end();w++){
      if((subpath== (*w).get_path()) &&
	 (!(*w).has_end_intersection())){return &(*w);}
    }
    throw xn_Error("no start section without an intersection");
  }

template<typename PATH>
void Section<PATH>::set_backwards(const Section<PATH>& adjoining, int joinend){
  //joinend of adjoining!
  const Intersection<PATH>* the_inter = adjoining.get_intersection(joinend);
  if(((joinend & SECTION_END) &&(!(adjoining.endinter))) || 
     ((joinend & SECTION_START) && (!(adjoining.startinter)))){
    if(adjoining.get_path() == subpath){
      if(adjoining.use_backwards()){use_flags |=SECTION_BACKWARDS;}
      else if(use_flags & SECTION_BACKWARDS){use_flags ^= SECTION_BACKWARDS;}
    }
    else{throw xn_Error("not really adjoining?");}
  }
  else if(junction_continues_direction(adjoining, *this, the_inter)){
    if(adjoining.use_backwards()){use_flags |=SECTION_BACKWARDS;}
    else if(use_flags & SECTION_BACKWARDS){use_flags ^= SECTION_BACKWARDS;}
  }
  else{
    if(!(adjoining.use_backwards())){use_flags |=SECTION_BACKWARDS;}
    else if(use_flags & SECTION_BACKWARDS){use_flags ^= SECTION_BACKWARDS;}
  }
}
    
 template <typename PATH>
 Segment<coords*> Section<PATH>::get_start_segment(){
    coords* firstone(NULL), * secondone(NULL);
    if(!has_start_intersection()){throw xn_Error("no start intersection");}
    firstone = &startextra;
    iterator iter= begin();
    if((iter != end()) && ((!((*iter).second & (MOVE_CLICK|LINE_CLICK))) || 
			      (startextra.get_distance(&((*iter).first)) <= coords::min_radius()))){iter++;}
    if((iter== end()) || ((*iter).second &CLOSE_PATH)){
      if(has_end_intersection()){
	secondone = &endextra;
      }
      else if(!endinter || ((*iter).second &CLOSE_PATH)){
	iter =subpath->begin();
	secondone=&((*iter).first);
      }
      else{
	std::ostringstream ostmp;
	ostmp <<"section too short - proceeding off section end\nthe bad section:\n";
	debug_print(ostmp);
	std::cerr <<"error: "<<ostmp.str()<<std::endl;
	throw xn_Error(ostmp.str());}
    }
    else{secondone=&((*iter).first);}
    return Segment<coords*>(firstone, secondone);
  }
 
 template <typename PATH>
 const Segment<coords*> Section<PATH>::get_start_segment()const{
const coords* firstone(NULL), * secondone(NULL);
    if(!has_start_intersection()){throw xn_Error("no start intersection");}
    firstone = &startextra;
    const_iterator iter= begin();
    if((iter != end()) && ((!((*iter).second & (MOVE_CLICK|LINE_CLICK))) || 
			   (startextra.get_distance(&((*iter).first)) <= coords::min_radius()))){iter++;}
    if((iter== end()) || ((*iter).second &CLOSE_PATH)){
      if(has_end_intersection()){
	secondone = &endextra;
      }
      else if(!endinter || ((*iter).second &CLOSE_PATH)){
	iter =subpath->begin();
	secondone=&((*iter).first);
      }
      else{throw xn_Error("too short - proceeding off section end");}
    }
    else{  secondone=&((*iter).first);}
    const Segment<coords*> result(const_cast<coords*>(firstone), const_cast<coords*>(secondone));
 return result;
  }

template <typename PATH>
 Segment<coords*> Section<PATH>::get_end_segment(){
  if(!has_end_intersection()){
      throw xn_Error("!has_end_intersection() == no end segment");
  }
  coords* firstone(NULL), * secondone(&endextra);
  reverse_iterator iter=rbegin();
  if((iter != rend()) && (((*iter).second & CLOSE_PATH) ||
			  (endextra.get_distance(&((*iter).first)) <= coords::min_radius()))){iter++;}
  if(iter == rend()){
    if(has_start_intersection()){
      firstone = &startextra;
    }
    else{throw xn_Error("section too short");}
  }
  else{firstone = &((*iter).first);}
    return Segment<coords*>(firstone, secondone);
 }

 template <typename PATH>
 const Segment<coords*> Section<PATH>::get_end_segment()const{
   if(!has_end_intersection()){
      throw xn_Error("!has_end_intersection() == no end segment");
  }
  const coords* firstone(NULL), * secondone(&endextra);
  const_reverse_iterator iter=rbegin();
  if((iter != rend()) && (((*iter).second & CLOSE_PATH) ||
			  (endextra.get_distance(&((*iter).first)) <= coords::min_radius()))){iter++;}
  if(iter == rend()){
    if(has_start_intersection()){
      firstone = &startextra;
    }
    else{throw xn_Error("section too short");}
  }
  else{firstone = &((*iter).first);}
   return Segment<coords*>(const_cast<coords*>(firstone), 
			   const_cast<coords*>(secondone));
 }

  template <typename PATH>
  template <typename ITER>
  std::pair<Section<PATH>*,int> Section<PATH>::choose_adjacent_section(ITER first, 
							ITER last,
							int flag, int whichside){
    int aboveflag(SECTION_START|SECTION_ABOVE),belowflag(SECTION_START|SECTION_BELOW), joinend(SECTION_START);
    const bool at_end=flag & SECTION_END;
    if(at_end){
      joinend=SECTION_END;
      aboveflag =SECTION_END|SECTION_ABOVE;
      belowflag = SECTION_END|SECTION_BELOW;
      if(!has_end_intersection()){
	Section<PATH>* startsector=get_pathstart_section(first, last);
	if(!startsector){throw xn_Error("NULL from get_pathstart_section()");}
	return std::pair<Section<PATH>*,int>(startsector, joinend);
      }
    }
    else if(flag &SECTION_START){
      if(!has_start_intersection()){
	Section<PATH>* endsector=get_pathend_section(first, last);
	if(!endsector){throw xn_Error("NULL from get_pathend_section()");}
	return std::pair<Section<PATH>*,int>(endsector, joinend);
      }
    }
    else{
      throw xn_Error("BAD FLAG: should be SECTION_START OR SECTION_END");
    }
#if CHECK_SUBPATH_SECTION
    //    debug_print(std::cerr);
#endif
    bool anglelarge(true);
    if(whichside==POINTSIDE_LEFT){anglelarge= false;}
    return choose_section_by_angle(first, last, joinend, anglelarge);
  }

  template<typename PATH>
template <typename ITER>
  std::deque<std::pair<Section<PATH>*, int> > 
Section<PATH>::get_adjacent_candidates(ITER first, ITER last, 
				       int  joinend){
    Intersection<PATH>* the_inter=get_intersection(joinend);
    Section* below = get_adjacent_section(first, last, joinend|SECTION_BELOW);
    Section* above = get_adjacent_section(first, last, joinend|SECTION_ABOVE);
    std::deque<std::pair<Section*, int> > candidates;
    if((!above) || (!below)){
      throw xn_Error("either or both of below and abpve sections NULL");
    }
    if(below != this){
      std::pair<Section*, int> belowpr1(below, joinend|SECTION_BELOW);
      candidates.push_back(belowpr1);
    }
      if(above !=this){
	std::pair<Section*, int> abovepr1(above, joinend|SECTION_ABOVE);
	candidates.push_back(abovepr1);
      }
      if(the_inter && the_inter->extras_size()){
	std::cerr<<"extras in Intersection ::supernumaries: count "
		 <<the_inter->extras_size();
	std::deque<Section*> aboves= get_extra_adjacent_sections(first, last, joinend|SECTION_ABOVE);
	for(typename std::deque<Section*>::iterator ab = aboves.begin(); ab != aboves.end();ab++){
	  if(!(*ab)){throw xn_Error ("NULL ftom get_extra_adjacent_sections");}
	  if(*ab != this){
	    std::pair<Section*, int> pr1(*ab, joinend|SECTION_ABOVE);
	    candidates.push_back(pr1);
	  }
	}
	std::deque<Section*> belows= get_extra_adjacent_sections(first, last, joinend|SECTION_BELOW);
	for(typename std::deque<Section*>::iterator be = aboves.begin(); be != aboves.end();be++){
	 if(!(*be)){throw xn_Error ("NULL ftom get_extra_adjacent_sections");}
	   if(*be != this){
std::pair<Section*, int>  pr1(*be, joinend|SECTION_BELOW);
	    candidates.push_back(pr1);
	  }
	}
      }
      return candidates;
  }
template<typename PATH>
template <typename ITER>
std::pair<Section<PATH>*, int> 
Section<PATH>::choose_section_by_angle(ITER first, ITER last, 
				       int  joinend, bool go_high){
   switch(joinend){
   case SECTION_START:
   case SECTION_END:
     break;
   default:
     throw xn_Error("or maybe should just silently fix bad joinend here: should be SECTION_STARTor SECTION_END");
   }
      
   std::deque<std::pair<Section*, int> > candidates = get_adjacent_candidates(first, last, joinend);
      
      JoinRank<Section> sorter(*this, go_high);
      if(!candidates.size()){throw xn_Error("nothing to choose from adjacent");}
      std::pair<Section*, int>  preres(NULL,0);
      if (candidates.size()!=1){
	std::sort(candidates.begin(), candidates.end(), sorter);
      }
      preres= candidates[0];
      if(preres.second& SECTION_ABOVE){preres.second=SECTION_END;}
      else if (preres.second& SECTION_BELOW){preres.second=SECTION_START;}
      else{throw xn_Error("wrong result from sorter");}
      if(!preres.first){throw xn_Error("choose_section_by_angle() found a NULL section");}
      return preres;
    }

  template< typename PATH >
  void SectionGroup<PATH>::add_OR_shape(const SectionShape<PATH>& shape){
    for(typename shape_type::const_iterator pshapeoutline =shape.outlines.begin();pshapeoutline !=shape.outlines.end();pshapeoutline++){
      for (const_shape_iterator wshape= OR_shapes.begin();wshape != OR_shapes.end();wshape++){
	for(typename shape_type::const_iterator ORoutline= (*wshape).outlines.begin();ORoutline!= (*wshape).outlines.end();ORoutline++){
	  if((*ORoutline).first == (*pshapeoutline).first){
	    std::cerr<<"dupe section: "<<*((*pshapeoutline).first)
		     <<"\n+++++ALL sections: "<<*this<<std::endl;
	    throw xn_Error("adding OR_shape with segments already used");
	  }
	}
      }
    }
    OR_shapes.push_back(shape);
  }

template< typename PATH >
std::ostream& operator<<(std::ostream& os, const SectionGroup<PATH>& src){
  os<< "SectionGroup<PATH> @ "<<&src<<"\nsections ................\n";
  for(typename SectionGroup<PATH>::const_section_iterator w= src.sections.begin();
      w != src.sections.end(); w++){
    (*w).debug_print(os);
    os<<"\n++++++++++++"<<std::endl;
  }
  
  return os;
}

  ///wrong: original idea works better!
template<typename PATH>
void SectionGroup<PATH>::mark_shapelet(const Section<PATH>& markfrom){
    }

template<typename PATH>
int SectionGroup<PATH>::compare_type(const Section<PATH>& sect)const{
      const PATH* p=sect.get_path();
      return compare_type(p);
    }

template<typename PATH>
int SectionGroup<PATH>::compare_type(const PATH* p)const{
      for(const_iterator w= paths.begin(); w!= paths.end();w++){
	if(p == (*w).first){
	  int res= (*w).second;
	  if((res == COMPARE_LEFT) || (res == COMPARE_RIGHT)){return res;}
	  std::ostringstream ostmp;
	  ostmp<<"bad flag: "<<res<<"\nshould be COMPARE_LEFT (ie "<<COMPARE_LEFT
	       <<")orCOMPARE_RIGHT (IE "<<COMPARE_RIGHT<<")";
	  throw xn_Error(ostmp.str());
	}
      }
      throw xn_Error("path not in set");
    }


  template<typename PATH>
  void 
  Section<PATH>::add_path_sections(PATH* pat, 
				   const std::deque<Intersection<PATH>*>& pathinters,
				   const std::deque<unsigned int>& offsets,
				   const std::set<Intersection<PATH> >& intersections,
				   std::deque<Section<PATH> >& outsections){
unsigned int offset(-1), countforsegment(0);
      typedef std::pair<Intersection<SubPath>*,int> interpair_type;
      interpair_type startinter(NULL, INTERSECTION_NONE);
      for(std::deque<unsigned int>::const_iterator offiter = offsets.begin(); 
	  offiter != offsets.end(); offiter++){
	unsigned int next=*offiter;
	if(offset==next){countforsegment++;}
	else{countforsegment=0;}
	offset = next;
	interpair_type  endinter= find_intersection_at(intersections.begin(), intersections.end(),
						       pat, offset,countforsegment);
	Section sector(pat, startinter, endinter);
	outsections.push_back(sector);
	startinter= endinter;
      }
      Section sector(pat, startinter,interpair_type(NULL, INTERSECTION_NONE));
      outsections.push_back(sector);
  }

template<typename PATH>
void SectionGroup<PATH>::add_path_sections(PATH* pat, 
			   const std::deque<Intersection<PATH>*>& pathinters,
			   const std::deque<unsigned int>& offsets){
  Section<PATH>::add_path_sections(pat, pathinters, offsets,intersections,sections);
  /*
      unsigned int offset(-1), countforsegment(0);
      typedef std::pair<Intersection<SubPath>*,int> interpair_type;
      interpair_type startinter(NULL, INTERSECTION_NONE);
      for(std::deque<unsigned int>::const_iterator offiter = offsets.begin(); 
	  offiter != offsets.end(); offiter++){
	unsigned int next=*offiter;
	if(offset==next){countforsegment++;}
	else{countforsegment=0;}
	offset = next;
	interpair_type  endinter= find_intersection_at(intersections.begin(), intersections.end(),
						       pat, offset,countforsegment);
	Section<PATH> sector(pat, startinter, endinter);
	sections.push_back(sector);
	startinter= endinter;
      }
      Section<PATH> sector(pat, startinter,interpair_type(NULL, INTERSECTION_NONE));
      sections.push_back(sector);
  */
    }

template<typename PATH>
std::deque<PATH*> SectionGroup<PATH>::flagpointers_get_paths(const std::deque<FlagPointer<PATH> >& flaggedpaths){
      std::deque<PATH*> res;
      typedef typename std::deque<FlagPointer<PATH> >::const_iterator const_deque_iterator;
      for(const_deque_iterator w = flaggedpaths.begin(); w!= flaggedpaths.end();w++){
	res.push_back((*w).first);
      }
      return res;
    }
 
template<typename PATH>
std::deque<PATH*>  SectionGroup<PATH>::flagpointers_get_paths(const std::set<FlagPointer<PATH> >& flaggedpaths){
      std::deque<PATH*> res;
      typedef typename std::set<FlagPointer<PATH> >::const_iterator const_set_iterator;
      for(const_set_iterator w =flaggedpaths.begin(); w!= flaggedpaths.end();w++){
	res.push_back((*w).first);
      }
      return res;
    }

  template<typename PATH>
  bool Intersection<PATH>::uses_path(const PATH& path)const{
    if((&path== first.first) || (&path == second.first)){return true;}
    for(const_pathpair_iterator w = supernumaries.begin();w != supernumaries.end(); w++){
      if((*w).first == &path){return true;}
    }
    return false;
  }

template<typename PATH>
void SectionGroup<PATH>::create_sections(){
      get_intersections();
      for(const_iterator w= paths.begin();w!= paths.end();w++){
	PATH* ppath=(*w).first;
	std::deque<Intersection<PATH>*> own_inters=ppath->get_own_intersections(intersections);
	std::deque<unsigned int> offsets= ppath->get_own_intersection_offsets(own_inters);
	add_path_sections(ppath, own_inters,  offsets);
      }
      mark_sections();
    }

 template <typename PATH>
  std::deque<std::set<FlagPointer<PATH> > > 
 SectionGroup<PATH>::create_pathsets(const std::deque<FlagPointer<PATH> >& pathsin){
    //TODO: finish me => dupe elimination
    //std::set<FlagPointer<PATH> > spaths(paths);
    //paths=spaths;//eliminate dupes
    typedef typename std::deque<FlagPointer<PATH> >::const_iterator path_iterator;
    
    typedef typename std::deque<path_set_type> set_container_type;
    
    set_container_type result;
    for(path_iterator pathiter= pathsin.begin();pathiter!= pathsin.end();pathiter++){
      add_to_setcollection(*pathiter,result);
      //survivor: where a pathe intersects multiple sets , the first - 'survivor' -  is added to the rest of them , which are then deleted. survivor will be the surviving set
    }
    return result;
  }

template <typename PATH>
  void  SectionGroup<PATH>::add_to_setcollection(const FlagPointer<PATH>& path,
			    std::deque<std::set<FlagPointer<PATH> > >& collection){
    if(!path.first){throw xn_Error("NULL path");}
    typedef typename std::deque<path_set_type> collection_type;
    typedef typename collection_type::iterator collection_iterator;
    //typedef typename std::set<FlagPointer<PATH> > set_type;
    //typedef typename set_type::iterator set_iterator;
    std::deque<path_set_type*> extras;
    path_set_type* survivor(NULL);
    for(collection_iterator colliter = collection.begin(); colliter != collection.end(); colliter++){
      bool donelooping(false);
      for(iterator setter = (*colliter).begin(); (setter != (*colliter).end()) && !donelooping; setter++){
	if(path.first->intersects(*((*setter).first))){
	  if(!survivor){
	    survivor= &(*colliter);
	    std::cerr<<"insert "<<&path<<" into existing set at"<<survivor
		     <<std::endl;
	    survivor->insert(path);
	  }
	  else{extras.push_back(&(*colliter));}
	  donelooping=true;
	}
      }
    }
    if(!survivor){
      collection.push_back(path_set_type());
      survivor=&(*(collection.end()-1));
      std::cerr<<"insert "<<&path<<" into new set"<<std::endl;
      survivor->insert(path);
    }
    else if(extras.size()){
      std::cerr<<"size of extra set list: "<<extras.size()
	       <<"\nsize of collection: "<< collection.size() <<std::endl;
      for(typename std::deque<path_set_type*>::iterator w = extras.begin();w!= extras.end(); w++){
	path_set_type* s_extra = *w;
	for(iterator exer= s_extra->begin();exer!= s_extra->end();exer++){
	  survivor->insert(*exer);
	}
	bool deleted(false);
	for(collection_iterator coll2 = collection.begin(); 
	    (coll2 != collection.end())&&!deleted ;coll2++){
	  if(&(*coll2) == s_extra){
	    collection.erase(coll2);deleted=true;
	  }
	}
      }
      std::cerr<<"collection size after merge and purge: "<< collection.size()
	       <<std::endl;
    }
  }

template<typename PATH>
std::deque<SectionGroup<PATH>* > SectionGroup<PATH>::create(const std::deque<FlagPointer<PATH> >& paths){
      if(!paths.size()){
	throw xn_Error("empty input: no PATHs");
      }
      std::deque<SectionGroup*> result;
      std::deque<std::set<FlagPointer<PATH> > > intersectinggroups = create_pathsets(paths);
      typedef typename std::deque<std::set<FlagPointer<PATH> > >::const_iterator set_iterator;
      for( set_iterator w =  intersectinggroups.begin();
	   w!= intersectinggroups.end();w++){
	result.push_back(new SectionGroup(*w));
      }
      return result;
    }

 template<typename PATH>
 std::deque<FlagPointer<PATH> > SectionGroup<PATH>::create_flagged_group(std::deque<PATH>& pathsorig,
									 int paramsyntaxflag){
      std::deque<FlagPointer<PATH> > res;
      for(typename std::deque<PATH>::iterator w=pathsorig.begin(); w!=pathsorig.end();w++){
	res.push_back(FlagPointer<PATH>(&(*w),paramsyntaxflag ));
      }
      return res;
    }

template<typename PATH>
std::deque<FlagPointer<PATH> > SectionGroup<PATH>::create_flagged_group(const std::deque<PATH*>& pathsorig, 
									int paramsyntaxflag){
      std::deque<FlagPointer<PATH> > res;
      for(typename std::deque<PATH*>::const_iterator w=pathsorig.begin(); w!=pathsorig.end();w++){
	res.push_back(FlagPointer<PATH>(*w,paramsyntaxflag ));
      }
      return res;
    }

template<typename PATH>
  simple_click_history SectionGroup<PATH>::combine_paths(const std::deque<PATH*>& pathsleft,
					      const std::deque<PATH*>& pathsright,
					      int flag){
      std::deque<FlagPointer<PATH> > allflagged = create_path_container(pathsleft, pathsright);
      std::deque<PATH*> allpaths=flagpointers_get_paths(allflagged);
      std::deque<std::set<FlagPointer<PATH> > > pathsets=create_pathsets(allflagged);
      std::deque<SectionGroup*> group= create(allflagged);
      simple_click_history path;
      for (typename std::deque<SectionGroup*>::iterator iter= group.begin(); iter!= group.end();iter++){
	(*iter)->add_to_path(path,flag, -(*iter)->groupside);
	delete *iter;
	*iter=NULL;
      }
      return path;
    }

  template<typename PATH>
  std::deque<FlagPointer<PATH> > 
  SectionGroup<PATH>::create_path_container(const std::deque<PATH*>& pathsleft,
					    const std::deque<PATH*>& pathsright){
      std::deque<FlagPointer<PATH> > leftflagged=create_flagged_group(pathsleft, COMPARE_LEFT);
      std::deque<FlagPointer<PATH> > rightflagged=create_flagged_group(pathsright, COMPARE_RIGHT);
  
      std::deque<FlagPointer<PATH> > allflagged(leftflagged);
      for(typename std::deque<FlagPointer<PATH> >::const_iterator q = rightflagged.begin(); q!=rightflagged.end();q++){
	allflagged.push_back(*q);
      }
      return allflagged;
    }

  template<typename PATH>
  void SectionGroup<PATH>::get_intersections(){
    typedef typename path_type::intersection_result_type intersection_result_type;
    typedef typename intersection_result_type::iterator inter_iterator;
    intersection_result_type outcontainer;
    std::set<PATH*> done;
    for (iterator w= paths.begin();w!=paths.end();w++){
      for (iterator q= paths.begin();q!=paths.end();q++){
	if(done.find((*q).first)==done.end()){
	  (*w).first->get_intersections(outcontainer, *((*q).first));
	}
      }
      done.insert((*w).first);
    }
    Intersection<PATH>::add_intersections(intersections, outcontainer);
}

 template<typename PATH>
 template<typename CONTAINER>
  void Intersection<PATH>::add_intersections(CONTAINER& outinters,
					     typename PATH::intersection_result_type rawdata){
   typedef typename PATH::intersection_result_type::const_iterator raw_iterator;
   typedef typename CONTAINER::const_iterator intersection_iterator;
   for(raw_iterator inter= rawdata.begin();inter !=rawdata.end();inter++){
     Intersection<PATH> tmp(*inter);
     coords tmpinter=tmp.get_intersection();
      ///@todo => warts on 'supernumaries'(ie more pair<PATH*,size_t> path/offsets thru same intersection)
      ///Wart no1: a coords can be 'equal'(really where (get_distance(&other) <= coords::min_radius()) 
      ///to 2 others, which are not 'equal' to each other, becos they're further than min_radius() apart, 
      ///ie different results depending on the order encountered.If middle one comes first 
      ///it eats both, otherwise only 1 will go! location will be (a bit)different too
      bool unhandled(true);
      for(intersection_iterator iter=outinters.begin();
	  iter != outinters.end();iter++){
	coords tmpother=(*iter).get_intersection();
	if(tmpinter.get_distance(&tmpother) <= coords::min_radius() ){
	  //compiler demands unconsting?? gcc-4.4
	  Intersection<PATH>& intersection=  const_cast<Intersection<PATH>&>(*iter);
	  intersection.merge_intersection(tmp);
	  unhandled=false;
	  break;
	}
      }
      if(unhandled)
	add_intersection(outinters, tmp);
    }
 }

  template<typename PATH>
  void Intersection<PATH>::merge_intersection(const Intersection<PATH>& inter){
    bool haveit(false);
    if((inter.first != first) && (second != inter.first)){
      for(const_pathpair_iterator supe=supernumaries.begin();
	  supe != supernumaries.end();supe++){
	if(*supe == inter.first){haveit=true;break;}
      }
      if(!haveit){
	supernumaries.push_back(inter.first); 
      }
    }
    haveit=false;
    if((inter.second != first) && (second != inter.second)){
      for(const_pathpair_iterator supe=supernumaries.begin();
	  supe != supernumaries.end();supe++){
	if(*supe == inter.second){haveit=true;break;}
      }
      if(!haveit){
	supernumaries.push_back(inter.second); 
      }
    }
  }

template<typename PATH>
template<typename CONTAINER>
void Intersection<PATH>::add_intersection(CONTAINER& intersections,
					  const Intersection<PATH>& inter){
  typedef typename CONTAINER::const_iterator it_iterator;
  for(it_iterator w= intersections.begin(); w != intersections.end(); w++){
    if (inter== *w){return;}
  }
  intersections.insert(inter);
}

  template<typename PATH>
  void SectionGroup<PATH>::add_intersection(const Intersection<PATH>& inter){
    Intersection<PATH>::add_intersection(intersections, inter);
  }

  template<typename PATH>
  std::deque<Section<PATH>*> 
  SectionGroup<PATH>::get_unused_sections(int crossingcnt){
    std::deque<Section<PATH>*> result;
    for(section_iterator w= sections.begin(); w != sections.end();w++){
      if(((crossingcnt <0) || (crossingcnt == (*w).crossings)) &&
	 (!(*w).used_for_output())){
	result.push_back(&(*w));
      }
    }
    return result;
  }
  ///@todo fix signature to add recursivity
  ///remove th whichside_outside
template<typename PATH>
int SectionGroup<PATH>::add_to_path( simple_click_history& outpath,
				     int flag,
				     int whichside_outside){
  std::deque<SectionShape<PATH>* > outshapes= get_output_shapes(flag);
  for(typename std::deque<SectionShape<PATH>* >::iterator  wshape=outshapes.begin();wshape!= outshapes.end();wshape++){
    (*wshape)->add_to_path(outpath);
  }
  return outshapes.size();
}

  template<typename PATH>
  double Section<PATH>::get_angle(const Section& other,
				  int joinflag)const{
    if(&other==this){std::cerr<<"get_angle with same seg?"<<std::endl;
      if((!(startinter &&(startinter == endinter)))){
	throw xn_Error("getting get_angle(*this,..) where section doesn't selfintersect(ie when both ends don't meet) is dumb");
      }
      else{
	std::cerr<<"getting get_angle(*this,..) where section doesn't selfintersect(ie when both ends don't meet) is dumb,\nand Not too smart even when that is true(like here), because if we're choosing next section from here, we've already traversed the other end"<<std::endl;
      }
}
    //const_cast : try to avoid
    coords the_pt= get_adjacent_point(const_cast<Section*>(&other), joinflag);
    Segment<coords> terminal;
    double distance(0);
    if(joinflag & SECTION_END){
      terminal=get_end_segment(); 
      distance = the_pt.get_distance(&(terminal.second));
    }
    else{
      terminal= get_start_segment();
      distance=the_pt.get_distance(&(terminal.first));
    }
    if(distance < coords::min_radius()){
      std::ostringstream ostmp;
      ostmp<<"Section::get_angle() adjacent pt == fulcrum in segment : \npt to compare: ("
	   <<the_pt.x <<", "<<the_pt.y<<")\nthe_segment :"<< terminal
	   <<"\ndiv by zero(angle can't be determined)";
      throw xn_Error(ostmp.str());
    }
    return terminal.angle_with_coords(the_pt, ((joinflag & SECTION_END)? false : true));
  }

     template<typename PATH>

     int SectionGroup<PATH>::mark_sections(){
       
       std::deque<std::pair<Section<PATH>*, int> > outsections=collect_outside_sections();
#if CHECK_SUBPATH_SECTION
std::deque<Section<PATH>* > insections=collect_inside_sections();
#endif
       if(outsections.size() >= sections.size()){
	 throw xn_Error("toomany outsections: all claim to be 'outside'");
       }
       bool firstloop(true);
       while (outsections.size()){
	 int joinend = SECTION_END;
	 Section<PATH>*sector1(NULL);
	 std::deque<std::pair<Section<PATH>*, int> > OR_sections;
	 int whichside=POINTSIDE_ERROR;
	 if(firstloop){
	   std::pair<section_iterator,int> outer1 = find_outside_section();
	   whichside=outer1.second;
	   sector1=&(*(outer1.first));
	 }
	 else{
	   std::pair<Section<PATH>*, int> sectpair=*(outsections.begin());
	   if(sectpair.first->marked()){throw xn_Error("remove_from_list still not working here");}
	   sector1 = sectpair.first;
	   whichside=sectpair.second;
	   std::cerr<<"sectpair.second: "<<pointside_as_string(sectpair.second)
		    <<"\n-groupside : "<<pointside_as_string(-groupside)<<std::endl;
	   //whichside=-groupside;
	 }
	 Section<PATH>* cursector=sector1;
	 std::pair<Section<PATH>*, int> firstpair(sector1, joinend);
	 OR_sections.push_back(firstpair);
	 remove_from_list(outsections, firstpair);
	 if(sector1->marked()){      throw xn_Error("Section already marked");  }
	 sector1->set_crossings(0);
	 bool iterating=true;
	 while(iterating){
	   if(((joinend==SECTION_END)&&(!cursector->has_end_intersection())) ||
	      ((joinend==SECTION_START) &&(!cursector->has_start_intersection()))){
	     Section<PATH>* next = cursector->get_terminal_section(sections.begin(),sections.end(), Section<PATH>::swap_joinend_flag(joinend));
	     if(next->marked()){	    iterating=false;break;	  }
	     else{
	       next->set_crossings(cursector->crossings_to_outside());
	       //this should not be necessary! lookup joinend in a shape
	       //if necessary to decide a way to turn when choosing sectors
	       //next->set_backwards(*cursector, joinend);
	       std::pair<Section<PATH>*, int> nxtsectpair(next,joinend);
#if CHECK_SUBPATH_SECTION
	       if(in_container(insections.begin(), insections.end(), next)){
#if XNDRAW_STRICT
		 throw xn_Error("section is insid according to is_inside");
#else
		 std::cerr<<"other end section @ "<<next
			  <<" is inside, according to is_inside\n";
#endif
	       }
#endif
	       OR_sections.push_back( nxtsectpair);
	       remove_from_list(outsections, nxtsectpair);
	       cursector=next; continue;
	     }
	   }
	   int curside=-whichside; //(joinend==SECTION_END ? -whichside:whichside);
	   try{
	     //cursector->print_choices( sections.begin(), sections.end(), std::cerr,joinend, -curside);
	   }
	   catch(xn_Error& e){
	     std::cerr<<"print_choices()-an error occurred:\n"<<e.what()
		      <<"\n********end Error output***********"<<std::endl;
	   }
	   std::pair<Section<PATH>*, int> candidatepr = cursector->choose_adjacent_section(sections.begin(), sections.end(), joinend, -curside);
	   std::pair<Section<PATH>*, int> altpr = cursector->choose_adjacent_section(sections.begin(), sections.end(), joinend, curside);
	   Section<PATH>* candidate= candidatepr.first;
	   if(!candidate){throw xn_Error(">choose_adjacent_section returned NULL, despite presence of intersection");}
	   if(candidate==sector1){	iterating=false;break;}
	   else if(cursector->is_adjacent(sections.begin(), sections.end(),
					  *candidate,joinend|SECTION_ABOVE) || 
		   cursector->is_adjacent(sections.begin(), sections.end(),
					  *candidate,joinend|SECTION_BELOW)){
	     if(!candidate->marked()){
	       //candidate->set_backwards(*cursector, joinend);
	       cursector=candidate;
	     }
	     else{
	       std::cerr <<"\n\nSectiongroup at errortime!\n"<< *this<<std::endl;
	       std::cerr <<"OR_shapes count : "<<OR_shapes.size()<<std::endl;
	       if(!(altpr.first->marked())){  std::cerr<<"alt_pr unmarked\n";}
	       if(altpr.first->is_outside(get_paths()).first){
		 std::cerr<<"alt_pr outside\n";
		 std::ostringstream ostmp;
		 ostmp<<"mark_sections():alt_pr outside  despite returning "<< candidate 
		      <<"which is marked";
		 throw xn_Error(ostmp.str());
	       }
	       iterating=false;break;
	       //could check over the list of adjacents here to compare
	     }
	   }
	   else{throw xn_Error("section not really adjacent");}
	   cursector->set_crossings(0);
#if CHECK_SUBPATH_SECTION
	   int oldend=joinend;
#endif

	   joinend=candidatepr.second;
	   std::pair<Section<PATH>*, int> nextpair(cursector,joinend);
#if CHECK_SUBPATH_SECTION
	   if(in_container(insections.begin(), insections.end(), cursector)){
	     if(pair_first_in_container(outsections.begin(),outsections. end(), cursector)){
	       std::cerr<<"in in and out sections???"<<std::endl;
	     }
	     std::cerr<<"ERROR!!!!either is_inside() told a lie, or turning wrong way\nthe sections:\n"<<*this
		      <<"\nold joinend: "<<(oldend &SECTION_END?"SECTION_END":"SECTION_START")
		      <<"\nnew joinend: "<<(joinend &SECTION_END?"SECTION_END":"SECTION_START")
		      <<std::endl;
#if XNDRAW_STRICT
	     throw xn_Error("section is inside according to is_inside");
#else
	     std::cerr<<"section @ "<<cursector
		      <<" is inside, according to is_inside\n";
#endif
	       }
	   if(pair_first_in_container(OR_sections.begin(),OR_sections. end(), cursector)){
	     std::ostringstream ostmp;
	     ostmp<<"section @ "<<cursector
		  <<" already added to list for shape\nthe section\n"
		  <<*cursector;
	     std::cerr<<ostmp.str()<<std::endl;
	     throw xn_Error(ostmp.str());
	   }
#endif
	   OR_sections.push_back( nextpair);
	   remove_from_list(outsections, nextpair);
	 }
	 if(firstloop){
	   groupside= -whichside;
	   firstloop=false;
	 }
	 SectionShape<PATH> OR_outline(OR_sections);
	 add_OR_shape(OR_outline);
       }
       create_shapes();
       return -groupside;
     }

     template<typename PATH>
     const Segment<coords*> 
     Section<PATH>::segment_at(typename Section<PATH>::signed_size_type pos)const{
       std::pair<const Segment<coords*>, signed_size_type> res= get_segment_and_iter(pos);
      return  res.first;
    }

template<typename PATH>
Segment<coords*> 
Section<PATH>::segment_at(typename Section<PATH>::signed_size_type pos){
     std::pair<Segment<coords*>, signed_size_type> res= get_segment_and_iter(pos);
      return  res.first;
}

  template<typename PATH>
  std::deque<Section<PATH>*> 
  SectionGroup<PATH>::collect_inside_sections(){
    std::deque<Section<PATH>*> result;
    std::deque<PATH*> nakedpaths= get_paths();
    for(section_iterator first= sections.begin();first!=sections.end();first++){
      if((*first).is_inside(nakedpaths)){
	result.push_back(&(*first));
      }
    }
    return result;
  }

  template<typename PATH>
  std::deque<std::pair<Section<PATH>*, int> >
  SectionGroup<PATH>::collect_outside_sections(){
    std::deque<std::pair<Section<PATH>*, int> > result;
      section_iterator first= sections.begin();
    section_iterator last= sections.end();
    std::deque<PATH*> nakedpaths= get_paths();
    for(;first != last; first++){
      Section<PATH>& current= *first;
      int outside=POINTSIDE_ERROR;
      bool excluded=false;
      for(typename std::deque<PATH*>::const_iterator wpath=nakedpaths.begin();
	  (wpath != nakedpaths.end()) && (!excluded);wpath++){
	//if this a problem, reinstate is_outside, which is simpler
	//std::pair<bool, int> pathside = current.is_outside(*wpath);
	//after first fixing is_outside
	std::pair<bool, int> pathside = current.can_bounce_outside(*wpath);
	if(!(pathside.first)){excluded=true;}
	else{
#if CHECK_SUBPATH_SECTION
	  std::pair<bool, int> oldpathside = current.is_outside(*wpath);
	  if(oldpathside.first&&(pathside.second != oldpathside.second)){
	    throw xn_Error("is_outside and can_bounce_outside return ing contradictory result");
	  }
#endif
	  if(*wpath== current.get_path()){
	    if(pathside.second== POINTSIDE_ERROR){
	      throw xn_Error("is_outside() returned an error");
	    }
	    outside=pathside.second;
	  }
	}
      }
      if(!excluded){
	if(outside == POINTSIDE_ERROR){
	  throw xn_Error("outside unset from is_outside()");
	}
	if(outside == POINTSIDE_CENTRE){
	  throw xn_Error("outside == POINTSIDE_CENTRE: USING VALUE FROM WRONG PATH from is_outside()-USE SUBPATH");
	}
	result.push_back(std::pair<Section<PATH>*, int>(&current, outside));
      }
    }
    return result;
  }

template<typename PATH>
std::pair<typename SectionGroup<PATH>::section_iterator, int> 
SectionGroup<PATH>::find_outside_section(){
  //const SubPath* otherpath(NULL);
      section_iterator first= sections.begin();
      section_iterator last= sections.end();
      std::deque<PATH*> nakedpaths= get_paths();
      for(;first != last; first++){
	Section<PATH>& current= *first;
	std::pair<bool, int> outside = current.is_outside(nakedpaths);
	if(outside.first){
	  return std::pair<section_iterator, int>(first, outside.second);
	}
      }
      throw xn_Error("no path found outside?");
}


  /**
  /// Is point outside, or on the outside edge of otherpath?  .
  /// @arg otherpath => the path to check against
it must be one of the paths used in creation of Section
for this make any sensse
  /// problemeatic for to use the 'outside value
  /// when calling over a list of paths
  /// rewrite to check each segment against every path in list at once,
as only this can give a reliable idea that a segment is totally at on edge of a group of intersecting paths
@bug return val not expressive enough. If otherpath != subpath, there could be no intersections whatsoever: for this we return (true, POINTSIDE_CENTRE)
*/
  template<typename PATH>
  std::pair<bool,int> Section<PATH>::is_outside(const PATH* otherpath,
						bool closepaths)const{
#if CHECK_SUBPATH_SECTION
    //REMEBER TO REMOVE
    test_segment_and_iter(std::cerr, *this);
#endif
    if(!otherpath){ otherpath = subpath;    }
    //note signed type index
    ssize_t segstart=0;
    if(startinter){segstart=-1;}
    while(segstart != SEG_LAST_RETURN){
      std::pair<const Segment<coords*>, ssize_t> nxtsegpair= get_segment_and_iter(segstart);
      const Segment<coords*>& nxtseg=nxtsegpair.first;
      segstart=nxtsegpair.second;
      int mleft(0), mright(0), mcentre(0);
      std::pair<coords, Linear_function<double> > perp = nxtseg.create_perpendicular_and_midpoint();
      std::deque<coords> mine= otherpath->find_intersections(perp.second);
      if(!(mine.size())){
	if(otherpath == subpath){throw xn_Error("nonsense result: found no intersections with own subpath");}
	//POINTSIDE_CENTRE here indicates both sides
	return std::pair<bool, int>(true, POINTSIDE_CENTRE);
      }
#if CHECK_SUBPATH_SECTION
      if(closepaths &&(mine.size() % 2)){
	std::cerr<<"odd no of path intersections: "<<mine.size()
		 <<"\nshould be even witha closed path"<<std::endl;
	throw xn_Error("should be even no of intersections: check SubPath::get_segment_and_iter() in case it skipping some segment(say the closepath)");
      }
#endif
      int outside(POINTSIDE_ERROR);
      for(std::deque<coords>::iterator w = mine.begin(); w!= mine.end(); w++){
	coords& pt= *w;
#if CHECK_SUBPATH_SECTION
#if XNDRAW_PEDANTIC
	std::cerr<<do_FN_pointside_test(pt)<<std::endl;
	if(!( FN_pointside_works(pt))){
	  throw xn_Error("error in pointside implementation- look at the printed results above");
	}

	if(nxtseg.vertical() || nxtseg.horizontal()){
	  Segment<coords> testseg(nxtseg);
	  std::cerr<<"testing (horiz or vert) segment "<<testseg<<std::endl;
	  if(!(test_segment_pointside(pt, testseg))){
	    throw xn_Error("error in Segment::pointside()- look at the printed results above");
	  }
	}
#endif
#endif
	int valtmp=-(nxtseg.pointside(pt));
	if(!((valtmp == POINTSIDE_CENTRE)||(valtmp==POINTSIDE_LEFT) ||(valtmp==POINTSIDE_RIGHT))){
	  std::ostringstream ostmp;
	  ostmp <<"error from pointside() : rv = "<<pointside_as_string(valtmp)<<" ("<<valtmp<<")";
	  throw xn_Error(ostmp.str());
	}
	if(valtmp==POINTSIDE_LEFT){mleft++;}
	else if(valtmp==POINTSIDE_RIGHT){mright++;}
	else{mcentre++;}
	if((valtmp != POINTSIDE_CENTRE)&&(outside==POINTSIDE_ERROR)){
	  outside= valtmp;
	} 
      }
      if((!mleft) || (!mright)){
	if((!mleft) && (!mright)){
	  std::cerr<<"no left or right hits recorded:\n length of list: "<<mine.size()
		   <<"\ncentre hits: "<<mcentre<<std::endl;
	}
	std::cerr<<"no of POINTSIDE_CENTRE hits: "<<mcentre<<std::endl;
	if(outside ==POINTSIDE_ERROR){
	  throw xn_Error("outside still set to POINTSIDE_ERROR while we try to return true");
	}
	return std::pair<bool, int>(true, outside);
      }
      else if((mleft%2) &&(mright%2)){
	//return std::pair<bool, int>(false,POINTSIDE_ERROR);
	std::cerr <<"odd number of paths crossed both ways: fail"<<std::endl;
	return std::pair<bool, int>(false,POINTSIDE_ERROR);
      }
    }
    return std::pair<bool, int>(false,POINTSIDE_ERROR); 
  }

  template<typename PATH>
  std::pair<bool,int> Section<PATH>::can_bounce_outside(const PATH* otherpath,
							bool closepaths)const{
    if(!otherpath){ otherpath = subpath;    }
    if(is_inside(otherpath, closepaths)){
      return std::pair<bool,int>(false, POINTSIDE_ERROR);
    }
    else{
      std::pair<bool,int> outres= is_outside(otherpath, closepaths);
      if(outres.first){return outres;}
    }
    ssize_t segstart=0;
    if(startinter){segstart=-1;}
    while(segstart != SEG_LAST_RETURN){
      std::pair<const Segment<coords*>, ssize_t> nxtsegpair= get_segment_and_iter(segstart);
      const Segment<coords*>& nxtseg=nxtsegpair.first;
      segstart=nxtsegpair.second;
      std::pair<bool, bool> bearings= Bounce::escapable(nxtseg, otherpath);
      if((!(bearings.first)) &&(!(bearings.second))){
	return std::pair<bool,int>(false, POINTSIDE_ERROR);
      }
      bool leftres(false), rightres(false);
      if(bearings.first){
	try{
	  leftres= Bounce::escape(*otherpath, nxtseg, POINTSIDE_LEFT);
	}
	catch(xn_Error& eleft){//why not catch this error?
	  std::cerr <<"Bounce::escape(POINTSIDE_lefT) error: "<<eleft.what()<<std::endl;
	  return std::pair<bool,int>(false, POINTSIDE_ERROR);
	}
      }
      if(bearings.second){
	try{
	  rightres= Bounce::escape(*otherpath, nxtseg, POINTSIDE_RIGHT);
	}
	catch(xn_Error& eright){
	  std::cerr <<"Bounce::escape(POINTSIDE_RIGHT) error: "<<eright.what()<<std::endl;
	  return std::pair<bool,int>(false, POINTSIDE_ERROR);
	}
      }
      if(leftres && rightres){
	return std::pair<bool,int>(true, POINTSIDE_CENTRE);
      }
      //swapped polarity to be like is_outside
      else if(leftres){return std::pair<bool,int>(true, POINTSIDE_RIGHT);}
      else if(rightres){return std::pair<bool,int>(true, POINTSIDE_LEFT);}
      else{std::cerr<<"didn't find bounce exit, try next seg..."<<std::endl;}
    }
    return std::pair<bool, int>(false,POINTSIDE_ERROR); 
  }

template<typename PATH>
std::pair<bool,int> 
Section<PATH>::can_bounce_outside(const std::deque<PATH*>& paths, 
						      bool closepaths)const{
  int side(POINTSIDE_ERROR);
  std::pair<bool,int> pathpair(false, side);
 for(typename  std::deque<PATH*>::const_iterator w= paths.begin();
       w != paths.end();w++){
   pathpair= can_bounce_outside(*w, closepaths);
   std::cerr<<"can_bounce_outside for "<<(*w==subpath ? "subpath" : "some other path")
	    <<"\nresult: "<<(pathpair.first?"true":"false")<<"\npointside: "
	    <<pointside_as_string(pathpair.second)<<std::endl;
   if(!pathpair.first){return pathpair;}
   if(*w == subpath){side= pathpair.second;}
 }
  if(side==POINTSIDE_ERROR){
    throw xn_Error("subpath not in path collection!");
  }
  return std::pair<bool, int>(true, side);
}
    
  template<typename PATH>
  bool Section<PATH>::is_inside(const PATH* otherpath,
				bool closepaths)const{
    if(!otherpath){ otherpath = subpath;    }
    ssize_t segstart=0;
    if(startinter){segstart=-1;}
    while(segstart != SEG_LAST_RETURN){
      std::pair<const Segment<coords*>, ssize_t> nxtsegpair= get_segment_and_iter(segstart);
      const Segment<coords*>& nxtseg=nxtsegpair.first;
      segstart=nxtsegpair.second;
      int mleft(0), mright(0), mcentre(0);
      std::pair<coords, Linear_function<double> > perp = nxtseg.create_perpendicular_and_midpoint();
      std::deque<coords> mine= otherpath->find_intersections(perp.second);
      if(!(mine.size())){
	if(otherpath == subpath){throw xn_Error("nonsense result: found no intersections with own subpath");}
	return false;
      }
      int outside(POINTSIDE_ERROR);
      for(std::deque<coords>::iterator w = mine.begin(); w!= mine.end(); w++){
	coords& pt= *w;
	int valtmp=-(nxtseg.pointside(pt));
	if(!((valtmp == POINTSIDE_CENTRE)||(valtmp==POINTSIDE_LEFT) ||(valtmp==POINTSIDE_RIGHT))){
	  std::ostringstream ostmp;
	  ostmp <<"error from pointside() : rv = "<<pointside_as_string(valtmp)<<" ("<<valtmp<<")";
	  throw xn_Error(ostmp.str());
	}
	if(valtmp==POINTSIDE_LEFT){mleft++;}
	else if(valtmp==POINTSIDE_RIGHT){mright++;}
	else{mcentre++;}
	if((valtmp != POINTSIDE_CENTRE)&&(outside==POINTSIDE_ERROR)){
	  outside= valtmp;
	} 
      }
      if((!mleft) || (!mright)){   return false; }
      else if((mleft%2) &&(mright%2)){	return true;	}
    }
    return false; 
  }

 template<typename PATH>
  bool Section<PATH>::is_inside(const std::deque<PATH*>& paths, 
				bool closepaths)const{
   for(typename  std::deque<PATH*>::const_iterator w= paths.begin();
       w != paths.end();w++){
     if(is_inside(*w,  closepaths)){return true;}
   }
   return false;
 }

    template<typename PATH>
typename Section<PATH>::size_type Section<PATH>::size()const{
      size_type incontainer = end()-begin();
      if(startinter){incontainer++;}
      //if(endinter){incontainer++;}
      incontainer++; //if endinter, it is an extra,
      //but otherwise we use subpath->begin() for a point implicitly
      //change back if this causes further erors, and just deal with symptoms
      return incontainer;
    }

template<typename PATH>
  unsigned int Section<PATH>::get_end_offset()const{
  if(!endinter ||(p_end ==INTERSECTION_NONE)){return subpath->size();}
  return endinter->get_offset(p_end);
}

template<typename PATH>
  unsigned int Section<PATH>::get_start_offset()const{
  if(!startinter ||(p_start ==INTERSECTION_NONE)){return 0;}
  return startinter->get_offset(p_start);
    }

template<typename PATH>
coords Section<PATH>::get_start_intersection_coords()const{
      if(!startinter){throw xn_Error("no start intersection: section at beginning of subpath");}
      return startinter->get_intersection();
    }

template<typename PATH>
coords Section<PATH>::get_end_intersection_coords()const{
      if(!endinter){throw xn_Error("no end intersection: section at ending of subpath");}
      return endinter->get_intersection();
    }

template<typename PATH>
coords Section<PATH>::get_adjacent_point(Section<PATH>* adjacent_section,
					 int flag_which)const{
      size_type sz=adjacent_section->size();
      if(sz<2){
	std::ostringstream ostmp;
	ostmp<<"too small for segment: "<<*adjacent_section;
	throw xn_Error(ostmp.str());
      }
      Segment<coords*> endseg(NULL, NULL);
      if((flag_which & SECTION_BELOW) ||(flag_which == SECTION_START)){
	endseg=adjacent_section->get_end_segment();
	//return *(endseg.first);
      }
      else if((flag_which & SECTION_ABOVE) || (flag_which == SECTION_END)){
	endseg=adjacent_section->get_start_segment();
	//return *(endseg.second);
      }
      else{
	throw xn_Error("shouldn't get here - invalid flag");
      }
      //avoid possibility of error from div by 0
      return endseg.get_midpoint();
    }

  template<typename PATH>
  Segment<coords*> 
  Section<PATH>::get_adjacent_segment(Section<PATH>* adjacent_section,
				      int flag_which)const{
      if(flag_which & SECTION_ABOVE){
	return  adjacent_section->get_start_segment();
      }
      else if(flag_which & SECTION_BELOW || flag_which & SECTION_START){
	return  adjacent_section->get_end_segment();
      }
      else if(flag_which & SECTION_END){
	return  adjacent_section->get_start_segment();
      }
      throw xn_Error("INVALID FLAG");
    }


/**
@todo if at end()==subpath->end(), no endinter, and subpath should be closed and no CLOSE_PATH
      add one more
      */
 template<typename PATH>
 typename Section<PATH>::size_type 
Section<PATH>::segments()const{//
      size_type sz =size();
      if(sz){--sz;}
      
      //for(const_iterator w= begin();w!=end();w++){
      //if((*w).second &CLOSE_PATH){--sz;}///duh...
      //}
      return sz;
 }
template<typename PATH>
ptrdiff_t Section<PATH>::get_maxstart()const{
  double minrad=coords::min_radius();
  ptrdiff_t maxstart=end()-begin();
if ((maxstart>0) && ((*(end()-1)).second &CLOSE_PATH)){maxstart--;}
 if (maxstart&& ((endinter && 
		  ((*(begin()+(maxstart-1))).first.get_distance(&endextra) <= minrad)) ||
		 ((!endinter) &&
		  ((*(begin()+(maxstart-1))).first.get_distance(&((*(subpath->begin())).first)) <=minrad)))){maxstart--;}
      return maxstart;
}


  template<typename PATH>
  void Section<PATH>::check_segment_start(ssize_t segstart)const{
    ssize_t maxstart=get_maxstart();
    if(segstart <0){
      if((segstart == -1)&& startinter){}
      else{
	std::ostringstream ostmp;
	ostmp<< "negative index: "<<segstart<<"\n"
	     <<(segstart==-1?"but no start intersection present"
		:"should be -1 for start_intersection(if present), or a normal positive index");
	throw xn_Error(ostmp.str());
      }
    }
    else if(segstart >= maxstart){
      std::ostringstream ostmp;
      ostmp<< "index "<<segstart<<" outside valid range ( -1 - "<<maxstart-1
	   <<")\nNB:l segment start on final coords,\nwhether(if endinter != NULL) endinter\nor otherwise (*(end()-1)).first";
      throw xn_Error(ostmp.str());
    }
  }

template<typename PATH>
 std::pair<const Segment<coords*>, ssize_t>  
Section<PATH>::get_segment_and_iter(ssize_t segstart, 
				    bool closepaths)const {
      const coords* firstone(NULL), *secondone(NULL);
      ssize_t endpos_is_nextstart( SEG_ERROR_RETURN);//error
      const_iterator starter=begin();
      bool at_start(false);
      check_segment_start(segstart);
      if((segstart == -1)&& startinter){
	  firstone= &startextra;
	  at_start=true;
	}
      else{//segstart is in valid range
	starter = begin()+segstart;
	if(!((*starter).second &(MOVE_CLICK|LINE_CLICK))){
	  throw xn_Error("must start segment at moveto() or lineto()");
	}
	firstone = &((*starter++).first);
      }
      if((starter !=end()) && 
	    (!((*starter).second &(LINE_CLICK|CLOSE_PATH)))){
	  std::ostringstream ostmp;
	  ostmp<< "BAD CLICKTYTPE:" << clicktype_as_string((*starter).second);
	  throw xn_Error(ostmp.str());
      }
      if(at_start && (starter !=end()) && 
	 (firstone->get_distance(&((*starter).first)) <= coords::min_radius())){starter++;  }
      if((endinter && (starter == (end()-1)) &&
	  (endextra.get_distance(&((*starter).first)) <= coords::min_radius())) ||
	 ((!endinter) && (starter == (end()-1)) &&
	      ((*starter).first.get_distance(&((*(subpath->begin())).first)) <= coords::min_radius()))){starter++;  }///skip to end of zero length  closing segment
      if((starter==end()) || ((*starter).second & CLOSE_PATH)){
	endpos_is_nextstart=SEG_LAST_RETURN;
	if(endinter){
	  secondone=&endextra;
	}
	else if((closepaths &&(starter==subpath->end())) || ((*starter).second & CLOSE_PATH)){
	  starter=subpath->begin();
	secondone= &((*starter).first);
	}
	else{throw xn_Error("no second coords remains to complete segment");}
      }
      else if((*starter).second & LINE_CLICK){
	secondone= &((*starter).first);
	endpos_is_nextstart=starter -begin();
      }
      if(!firstone || !secondone){
	throw xn_Error("!firstone || !secondone");
      }
      if(endpos_is_nextstart ==  SEG_ERROR_RETURN){
	throw xn_Error("nxt position not properly set");
      }
      ///unholy constery
      Segment<coords*> res1(const_cast<coords*>(firstone), const_cast<coords*>(secondone));
      return std::pair<const Segment<coords*>, ssize_t>(res1, endpos_is_nextstart);
    }



template<typename PATH>
std::pair<Segment<coords*>, ssize_t> 
Section<PATH>::get_segment_and_iter(ssize_t segstart,
				    bool closepaths){
      coords* firstone(NULL), *secondone(NULL);
      ssize_t endpos_is_nextstart( SEG_ERROR_RETURN);//error
      iterator starter=begin();
      bool at_start(false);
      check_segment_start(segstart);
      if((segstart == -1)&& startinter){
	firstone= &startextra;
	at_start=true;
      }	
      if(!firstone){//segsatart is in valid range
	starter = begin()+segstart;
	if(!((*starter).second &(MOVE_CLICK|LINE_CLICK))){
	  throw xn_Error("must start segment at moveto() or lineto()");
	}
	firstone = &((*starter).first);
	starter++;
      }
      //we have the first point . now find the second
      if((starter !=end()) && 
	 (!((*starter).second &(LINE_CLICK|CLOSE_PATH)))){
	std::ostringstream ostmp;
	ostmp<< "BAD CLICKTYTPE:" << clicktype_as_string((*starter).second);
	throw xn_Error(ostmp.str());
      }
      double minr=coords::min_radius();
      if(at_start && (starter !=end()) && 
	 (firstone->get_distance(&((*starter).first)) <= minr)){starter++;  }
      if((endinter && (starter == (end()-1)) &&
	  (endextra.get_distance(&((*starter).first)) <= minr)) ||
	 ((!endinter) && (starter == (end()-1)) &&
	  ((*starter).first.get_distance(&((*(subpath->begin())).first)) <= minr))){starter++;  }///skip to end of zero length  closing segment
      if((starter==end()) || ((*starter).second & CLOSE_PATH)){
	endpos_is_nextstart=SEG_LAST_RETURN;
	if(endinter){
	  secondone=&endextra;
	}
	else if((closepaths &&(starter==subpath->end())) || ((*starter).second & CLOSE_PATH)){
	  starter=subpath->begin();
	  secondone= &((*starter).first);
	}
	else{throw xn_Error("no second coords remains to complete segment");}
      }
      else if((*starter).second & LINE_CLICK){
	secondone= &((*starter).first);
	endpos_is_nextstart=starter -begin();
      }
      if(!firstone || !secondone){
	throw xn_Error("!firstone || !secondone");
      }
      if(endpos_is_nextstart ==  SEG_ERROR_RETURN){
	throw xn_Error("nxt position not properly set");
      }
#if CHECK_SUBPATH_SECTION
      //debug_print(std::cerr);
#endif
      Segment<coords*> res1(firstone, secondone);
      return std::pair<Segment<coords*>, ssize_t>(res1, endpos_is_nextstart);
    }

 template<typename PATH>
 std::pair<coords, Linear_function<double> > 
   Section<PATH>::perpendicular_at(typename Section<PATH>::signed_size_type pos)const{
      Segment<coords*> segnow= segment_at(pos);
      return segnow.create_perpendicular_and_midpoint();
    }

template<typename PATH>
  int Section<PATH>::crossings_to_outside()const{
      if (crossings < 0){
	std::ostringstream ostmp;
	ostmp<< "crossings not properly inited: value must be >= 0, not "
	     <<crossings<<"\nthe offending section @ "<<this;
	throw xn_Error(ostmp.str());
      }
      return crossings;
    }
  ///@todo if they intersect both ends, this is ambiguous,
  ///need to supply the intersection too
template<typename PATH>
bool Section<PATH>::junction_continues_direction(const Section<PATH>& fst,
						 const Section<PATH>& snd,
						 const Intersection<PATH>* the_inter){
  //Intersection<PATH>* inter = fst.get_common_intersection(snd);
  if(!(((the_inter== fst.startinter) ||(the_inter==fst.endinter)) &&
       ((the_inter== snd.startinter) ||(the_inter==snd.endinter)))){
    throw xn_Error("intersection not common to to the 2 Section passed into junction_continues_direction");
  }
  return (fst.whichend(the_inter) + snd.whichend(the_inter))==0;
}

template<typename PATH>
 typename Section<PATH>::iterator Section<PATH>::begin(){
      unsigned int off = get_start_offset();
      if(off || startinter){off++;}
      return subpath->begin()+off;
    }

    template<typename PATH>
 typename Section<PATH>::const_iterator Section<PATH>::begin()const{
      unsigned int off = get_start_offset();
      if(off|| startinter){off++;}
      return subpath->begin()+off;
    }

template<typename PATH>
typename Section<PATH>::iterator Section<PATH>::end(){
  if(!endinter){return subpath->end();}
  unsigned int off = get_end_offset() +1;
  return subpath->begin()+off;
    }
    
template<typename PATH>
typename Section<PATH>::const_iterator Section<PATH>::end()const{
      if(!endinter){return subpath->end();}
  unsigned int off = get_end_offset() +1;
      return subpath->begin()+off;
    }

template<typename PATH>
typename Section<PATH>::reverse_iterator Section<PATH>::rbegin(){
      return reverse_iterator(end());
    }

    template<typename PATH>
typename Section<PATH>::const_reverse_iterator Section<PATH>::rbegin()const{
      return const_reverse_iterator(end());
    }
    
template<typename PATH>
typename Section<PATH>::reverse_iterator Section<PATH>::rend(){
      return reverse_iterator(begin());
    }
    
template<typename PATH>
typename Section<PATH>::const_reverse_iterator Section<PATH>::rend()const{
      return const_reverse_iterator(begin());
    }

template<typename PATH>
std::pair<bool,int> Section<PATH>::is_outside(const std::deque<PATH*>& paths,
					      bool closepaths){
  ///find segment that will definitely be on the 'outside' OR shape
      int outside(POINTSIDE_ERROR);
      ssize_t segstart=0;
      if(startinter){segstart=-1;}
      while(segstart != SEG_LAST_RETURN){
	std::pair<const Segment<coords*>, ssize_t> nxtsegpair= get_segment_and_iter(segstart);
	const Segment<coords*>& nxtseg=nxtsegpair.first;
	segstart=nxtsegpair.second;
	int mleft(0), mright(0), mcentre(0);
      //Segment<coords> cursegment(nxtseg);
	std::pair<coords, Linear_function<double> > perp = nxtseg.create_perpendicular_and_midpoint();
#if CHECK_SUBPATH_SECTION
	 Segment<coords> testseg(nxtseg);
	  
#endif
	for(typename std::deque<PATH*>::const_iterator w=paths.begin();w!=paths.end();w++){
	  const PATH* otherpath = *w;
	  std::deque<coords> mine= otherpath->find_intersections(perp.second);
	  for(std::deque<coords>::iterator w = mine.begin(); w!= mine.end(); w++){
	    coords& pt= *w;
#if CHECK_SUBPATH_SECTION
#if XNDRAW_PEDANTIC
	  std::cerr<<do_FN_pointside_test(pt)<<std::endl;
	  if(!( FN_pointside_works(pt))){
	    throw xn_Error("error in pointside implementation- look at the printed results above");
	  }
	 if(!(test_segment_pointside(pt, testseg))){
	    throw xn_Error("error in segment pointside implementation- add code for it and then look at the printed results above");
	  }
#endif
#endif
	    int valtmp=-(nxtseg.pointside(pt));
	    if(!((valtmp == POINTSIDE_CENTRE)||(valtmp==POINTSIDE_LEFT) ||(valtmp==POINTSIDE_RIGHT))){
	      std::ostringstream ostmp;
	      ostmp <<"error from pointside() : rv = "<<valtmp;
	      throw xn_Error(ostmp.str());
	    }
	    if(valtmp==POINTSIDE_LEFT){mleft++;}
	    else if(valtmp==POINTSIDE_RIGHT){mright++;}
	    else{mcentre++;}
	    if((valtmp!=POINTSIDE_CENTRE) &&    (outside==POINTSIDE_ERROR)){
	      outside= valtmp;
	    }
	  }
	}
	if((!mleft) || (!mright)){
	  return std::pair<bool, int>(true, outside);
	}
	//	else if(mleft%2){return std::pair<bool, int>(false,POINTSIDE_ERROR); }
      }
      return std::pair<bool, int>(false,POINTSIDE_ERROR); 
}

template<typename PATH>
typename Section<PATH>::const_iterator  Section<PATH>::get_pathstart_iter()const{
      if(subpath->size()){
	const_iterator starter=subpath->begin();
	while((starter !=subpath->end()) && 
	      (!((*starter).second &(MOVE_CLICK)))){
	  if((*starter).second &(BEZIER_CLICK|BEZIER_HANDLE|LINE_CLICK|CLOSE_PATH)){
	    std::ostringstream ostmp;
	    ostmp<< "BAD CLICKTYTPE:" 
		 << clicktype_as_string((*starter).second);
	    throw xn_Error(ostmp.str());
	  }
	  else{
	    std::cerr<< "WARNING: ROGUE CLICK IN SECTION - this will become "
		     <<"error without a good rationale"
		     <<clicktype_as_string((*starter).second);
	  }
	  starter++;
	}
	if(starter ==subpath->end()){
	  throw xn_Error("phooie: end of underlying pathpath!");
	}
      return starter;
      }
	else{throw xn_Error("underlying path empty - and if it got here: 'blahhh'");}
    }


template<typename PATH>
typename Section<PATH>::iterator Section<PATH>::get_pathstart_iter(){
	if(subpath->size()){
	  iterator starter=subpath->begin();
	  while((starter !=subpath->end()) && 
		(!((*starter).second & MOVE_CLICK))){
	    if((*starter).second &(CLOSE_PATH|LINE_CLICK|BEZIER_CLICK|BEZIER_HANDLE)){
	      std::ostringstream ostmp;
	      ostmp<< "BAD CLICKTYPE:" 
		   << clicktype_as_string((*starter).second);
	      throw xn_Error(ostmp.str());
	    }
	    else{
	      std::cerr<< "WARNING: ROGUE CLICK IN SECTION - this will become "
		       <<"error without a good rationale"
		       <<clicktype_as_string((*starter).second);
	    }
	    starter++;
	  }
	  if(starter ==subpath->end()){
	      throw xn_Error("phooie: end of underlying pathpath!");
	  }
	  return starter;
	}
	else{ throw xn_Error("underlying path empty - 'blahhh'");}
}
 template<typename PATH>
  template<typename ITER>
 std::deque<Section<PATH>*> 
Section<PATH>::get_extra_adjacent_sections(ITER first,
					   ITER last, 
					   int flag_which)const{
   std::deque<Section<PATH>*> result;
   Intersection<PATH>* the_inter(NULL);
   size_type matchoffset(-1);
   if((!(flag_which & (SECTION_START|SECTION_END))) ||
      ((flag_which & SECTION_START) && (flag_which & SECTION_END))){
     throw xn_Error("one (not both) of SECTION_START ||SECTION_END required in flag pls"); 
   }
   if(!(flag_which & (SECTION_ABOVE|SECTION_BELOW))){
     throw xn_Error("need at least one of SECTION_ABOVE|SECTION_BELOW in flagwhich");
   }
   if(flag_which & SECTION_START){
     if(startinter){	the_inter=startinter;     }	
   }
   else if(endinter){    the_inter=endinter;   }
   if(!the_inter){throw xn_Error("no intersecrion: if get_adjacent section with same args return NULL, don't call this");}//an end section
   if(! the_inter->extras_size()){return result;}
   typename Intersection<PATH>::container_type pathoffs=the_inter->get_extras();
   typedef typename Intersection<PATH>::pathpair_iterator supernumary_iterator;
   for(supernumary_iterator w= pathoffs.begin(); w!= pathoffs.end();w++){
     PATH* thepath= (*w).first;
     matchoffset=(*w).second;
     bool looking=true;
     bool lookingboth=((flag_which & SECTION_BELOW) && (flag_which & SECTION_ABOVE));
     int which=flag_which;
     for(;(first!= last) && looking;first++){
       Section* candidate= &(*first);
       if(candidate->get_path() == thepath){
	 if(which & SECTION_BELOW){
	   if((candidate->endinter==the_inter)&&   
	      (candidate->get_end_offset() ==matchoffset)){
	     result.push_back(candidate);
	     if(!lookingboth){looking=false;}
	     else{lookingboth=false;}
	     which ^=SECTION_BELOW;
	   }
	 }
	 if(which & SECTION_ABOVE){
	   if((the_inter == candidate->startinter)&&   
	      (candidate->get_start_offset() ==matchoffset)){
	     result.push_back(candidate);
	     if(!lookingboth){looking=false;}
	     else{lookingboth=false;}
	     which ^=SECTION_ABOVE;
	   }
	 }
       }
     }
   }
   return result;
 }

  ///ignores supernumaries, if present(see choose_*(),
  ///or get_extra_adjacent_sections)
  template<typename PATH>
  template<typename ITER>
    Section<PATH>* Section<PATH>::get_adjacent_section(ITER first,
						       ITER last, 
						       int flag_which)const{
    Intersection<PATH>* the_inter(NULL);
    size_type matchoffset(-1);
    if(!(flag_which & (SECTION_START|SECTION_END))){
      throw xn_Error("SECTION_START ||SECTION_END in flag pls"); 
    }
    if(flag_which & SECTION_START){
      if(startinter){
	the_inter=startinter;
	matchoffset=get_start_offset();
      }	
    }
    else if(endinter){
      the_inter=endinter;
      matchoffset=get_end_offset();
    }
    //could return pathstart_section or pathend_section here
    if(!the_inter){return NULL;}//an end section
    if(flag_which & SECTION_CONTINUED){
      for(ITER w=first;w != last;w++){
	Section* candidate= &(*w);
	if( subpath== candidate->get_path()){
	  if(flag_which & SECTION_START){
	    if((the_inter==candidate->endinter) &&
	       (candidate->get_end_offset() == matchoffset)){
	      return candidate;
	    }
	  }
	  else if(flag_which & SECTION_END){
	    if((the_inter==candidate->startinter)&&
	       (candidate->get_start_offset() == matchoffset)){
	      return candidate;
	    }
	  }  
	}
      }
	throw xn_Error("SECTION_CONTINUED => no suitable SECTION in range supplied");
    }
    else if((flag_which & SECTION_ABOVE) ||(flag_which & SECTION_BELOW)){
      PATH* thepath = the_inter->get_alternate_path(get_p_inter(flag_which));
	matchoffset= the_inter->get_alternate_offset(get_p_inter(flag_which));
      
      for(;first!= last;first++){
	Section* candidate= &(*first);
	if(candidate->get_path() == thepath){
	  ///might have to rearrange this or maybe 
	  if(flag_which & SECTION_BELOW){
	    if((candidate->endinter==the_inter)&&   
	       (candidate->get_end_offset() ==matchoffset)){
	      //std::cerr<<"if(flag_which & SECTION_BELOW){\n(we connect to to its END)"<<std::endl;
	      //std::cerr<<"return ining candidate"<<candidate<<std::endl;
	      return candidate;
	    }
	  }
	  else if(flag_which & SECTION_ABOVE){
	    if((the_inter == candidate->startinter)&&   
	       (candidate->get_start_offset() ==matchoffset)){
	      //std::cerr<<"above: returning candidate"<<candidate<<std::endl;
	      return candidate;
	    }
	  }
	}
      }
      std::ostringstream ostmp;
      ostmp <<"no suitable SECTION in range supplied for flag "<<flag_which;
      throw xn_Error(ostmp.str());
    }
    throw xn_Error("no suitable SECTION in range supplied");
  }

}

#endif
