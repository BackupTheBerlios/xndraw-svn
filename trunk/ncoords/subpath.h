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



#ifndef X3DRAW_SUBPATH_H
#define X3DRAW_SUBPATH_H
#include "xndrawexceptions.h"

#if HAVE_CONFIG_H
#include "config.h"
#endif
#include "clickhistory.h"
#include "pathsection.h"

namespace NCOORDS{

  /**
an arbitrary subset of a path(ie simple_click_history) .

To facilitate path manipulations without unnecessary copying. 
Contains a pointer to the path and offsets into it
from which iterators are created, use begin(), end() etc
to get at the range.
In general use SubPath - a child class, which must have a move_to()
at the start, and end at the next, or path->end(),
whichever comes first,
instead of this.
@warning the path must stick around, and not be truncated
or bad things will happen
used for boolean operations => see Section<SubPath> which
makes path sections between intersections
   */
 class PathFragment{
  public:
    typedef simple_click_history path_type;
    typedef simple_click_history::const_iterator const_iterator;
    typedef simple_click_history::iterator iterator;
    typedef simple_click_history::const_reverse_iterator const_reverse_iterator;
    typedef simple_click_history::reverse_iterator reverse_iterator;
    //typedef ptrdiff_t 
    typedef simple_click_history::difference_type difference_type;
    typedef simple_click_history::size_type size_type;
  protected:
    //make const
    simple_click_history* path;
    difference_type first;
    difference_type last;
   //these allowed for now to allow copying into
   //STL_CONTAINER< PathFragment> at creation, so we don't
   //require deletion routines, but after that use a pointer
   //or reference from the container, and not copy
   //PathFragment& operator=(const PathFragment&);
   //PathFragment(const PathFragment&);
  public:
    template <typename ITERATOR>
    PathFragment(simple_click_history* p, ITERATOR start, ITERATOR finish):
      path(p){
      if(!path){throw xn_Error("path NULL");}
      first=start - path->begin();
      last= finish - path->begin();
      if(!(valid_range())){
	throw xn_Error("PathFragment(...)\niterators point to invalid range");
      }
    }

    ~PathFragment(){}
    bool is_subpath_of(const simple_click_history& p)const{
      return &p == path;
    }

    simple_click_history& complete_path(){return *path;}
    const simple_click_history& complete_path()const{return *path;}
    void get_extents(cRect& box)const;
    //maybe only const versions? - and constify path
    iterator begin();
    const_iterator begin()const;
    iterator end();
    const_iterator end()const;
    reverse_iterator rbegin();
    const_reverse_iterator rbegin()const;
    reverse_iterator rend();
    const_reverse_iterator rend()const;
    size_type size()const{ return last - first;}

    path_type as_path()const;

    bool valid_range()const;
    void throw_if_invalid()const;
    bool true_subpath()const;
    unsigned int segment_intersections(Segment<const coords*> seg)const;
    unsigned int self_intersections()const;

    std::pair<int, int> 
    get_intersection_count(const Linear_function<double>& pathfn, 
			   const coords& centre,
			   bool implicitly_closed = true)const;
  };
 
class SubPath: public PathFragment{
  public:
    typedef simple_click_history path_type;
    typedef simple_click_history::const_iterator const_iterator;
    typedef simple_click_history::iterator iterator;
    //typedef ptrdiff_t 
    typedef simple_click_history::difference_type difference_type;
  typedef Section<SubPath> section_type;
typedef SectionShape<SubPath> shape_type;
typedef Intersection<SubPath> intersection_type;

    typedef std::pair<SubPath*, unsigned int> path_offset_type;
    typedef std::deque<std::pair<path_offset_type,path_offset_type> > intersection_result_type;
  typedef std::deque<Section<SubPath> >::iterator section_iterator;
  protected:
  //lazy init
  ///the subpath must not change during its life
  ///its a type to iterate over a section of path without copying
  std::set<Intersection<SubPath> >* intersectioncache;
std::deque<Section<SubPath> >* sectioncache;

  std::deque<std::pair<SectionShape<SubPath> , int> >* shapescache;

  void destroy_caches(){
    if(intersectioncache){
      delete intersectioncache;
      intersectioncache=NULL;
    }
    if(sectioncache){
      delete sectioncache;
      sectioncache=NULL;
    }
if(shapescache){
      delete shapescache;
      shapescache=NULL;
    }
  }
  void clear_caches(){
    if(intersectioncache){
    intersectioncache->clear();
    }
    if(sectioncache){
    sectioncache->clear();
    }
    if(shapescache){
     shapescache->clear();
    }
  }
  public:
    
    SubPath(simple_click_history* p, iterator start, iterator finish);
    template <typename ITERATOR>
    SubPath(simple_click_history* p, ITERATOR start, ITERATOR finish):
      PathFragment(p,start, finish),
      intersectioncache(NULL),
      sectioncache(NULL),
     shapescache(NULL)
    {    
      if(!true_subpath()){
	throw xn_Error("invalidSubPath");
      }
    }

    //default copy constructor ok
    ~SubPath(){
      destroy_caches();
    }
   
    void get_self_intersections(intersection_result_type& outcontainer);

  void get_intersections(intersection_result_type& outcontainer, SubPath& other, bool closepaths=true);

    void get_intersections(intersection_result_type& outcontainer, 
			   SubPath& other, unsigned int otherindex);

  ///should this be 'lazily' cached? 
  const std::deque<Section<SubPath> > as_sections()const;

  void create_intersectioncache(){
    intersectioncache=new std::set<intersection_type>;
intersection_result_type inters;
    get_self_intersections(inters);
    if(inters.size()){
      Intersection<SubPath>::add_intersections(*intersectioncache,inters);
    }
  }

protected:

  std::pair<section_iterator, int> find_outside_section(){
  //const SubPath* otherpath(NULL);
    if(!sectioncache){create_sectioncache();}
    if(!sectioncache->size()){
      throw xn_Error("emty subpath");
    }
      section_iterator first= sectioncache->begin();
      section_iterator last= sectioncache->end();
     
      for(;first != last; first++){
	const Section<SubPath>& current= *first;
	std::pair<bool, int> outside = current.is_outside(this);
	if(outside.first){
	  return std::pair<section_iterator, int>(first, outside.second);
	}
      }
      throw xn_Error("no section found outside?");
}


public:

  std::deque<Section<SubPath> > as_sections(){
    typedef std::deque<Section<SubPath> > result_type;
    result_type result;
    if(!intersectioncache){
      create_intersectioncache();
    }
    if(!intersectioncache){throw xn_Error("cache creation failure");}
    if(!(intersectioncache->size())){
      typedef std::pair<Intersection<SubPath>*, unsigned int> pr_type;
      Section<SubPath> mono(this, pr_type(NULL, 0), pr_type(NULL, end()-begin()));
      result.push_back(mono);
    }
    else{
      std::deque<Intersection<SubPath>*> ptr_inters;
      ptr_inters= to_ptr_container(intersectioncache->begin(), intersectioncache->end(), ptr_inters);
      std::deque<unsigned int> offsets;
      for(std::deque<Intersection<SubPath>*>::const_iterator qw=ptr_inters.begin(); qw!= ptr_inters.end();qw++){
	offsets.push_back((*qw)->get_first_offset());
	offsets.push_back((*qw)->get_second_offset());
      }
      //sort first?
      Section<SubPath>::add_path_sections(this,ptr_inters, offsets,*intersectioncache, result);
    }
    return result;
  }

  void create_sectioncache(){
    if(!sectioncache){
      sectioncache=new std::deque<section_type>;
      if(!sectioncache){
	throw xn_Error("out of memory");
      }
      *sectioncache=as_sections();
    }
  }

  void create_shapescache(){
typedef std::deque<std::pair<section_type*,int> > tmpshape_container;
  
  if(!sectioncache){create_sectioncache();}
  if(!(sectioncache->size())){throw xn_Error("empty subpath?");}
  else if(sectioncache->size()==1){
    std::pair<section_type*,int> outlineall(&(*(sectioncache->begin())), (int)SECTION_END);
    tmpshape_container shapesections;
    shapesections.push_back(outlineall);
  shape_type allshape(shapesections);

  shapescache->push_back(std::pair<shape_type,int>(allshape, 0));

  }
  else{
    //could skip the above and just make a SectionGroup?-but then problem(is_outside) goes recursif?
    //but a lot of the code can be factored out and called from both SectionGroup and SubPath

    throw xn_Error("finishme");
  }
}

  ///rename to create_shapescache
std::deque<SectionShape<SubPath>* > get_shapes(){
  std::deque<SectionShape<SubPath>* > result;
  if(!shapescache){create_shapescache();}
  for (std::deque<std::pair<SectionShape<SubPath>, int> >::iterator w =shapescache->begin();w!=shapescache->end();w++){
    result.push_back(&((*w).first));
  }

  return result;
  }

  const std::deque<SectionShape<SubPath> > get_shapes()const{
    throw xn_Error("finishme");
  }
 /**
get the outside outline
   */
  SectionShape<SubPath> get_OR_shape()const{
throw xn_Error("finishme");
  }

    /**
from a deque of Intersections,
return those which involve this path.
@return Intersections of this path
     */
    std::deque<Intersection<SubPath>*> get_own_intersections(const std::set<Intersection<SubPath> >& intersectionlist)const;
 /**
from a deque of Intersections,
return a list of offsets where this path is intersected
@return a sorted std::deque<int> of the offsets with inteesections
     */

    std::deque<unsigned int> get_own_intersection_offsets(const std::deque<Intersection<SubPath>*>& intersectionlist)const;

    bool closed()const{
      if(size()<2){return false;}
      return (*(rbegin())).second & CLOSE_PATH;
    }

 
    bool point_within (const coords& pt, 
		       bool implicitlyclosed  = true, 
		       unsigned int segcount=256)const;
  int is_overlapping(const SubPath& other,
		     bool closedbydefault=true)const;
    bool intersects(const SubPath& other)const{
      return is_overlapping(other)== PATH_OVERLAP_INTERSECT;
    }

    Segment<coords*> get_segment(iterator w,
				 bool alwaysclosed=true);

    const Segment<coords*> get_segment(const_iterator w,
				       bool alwaysclosed=true)const ;

  std::pair<Segment<coords*>, iterator> get_segment_and_iter(iterator w,
				 bool alwaysclosed=true);

 std::pair<const Segment<coords*>, const_iterator> get_segment_and_iter(const_iterator w,
				 bool alwaysclosed=true)const;

void get_intersections(SubPath::intersection_result_type& outcontainer,
		       SubPath* src,
		       Segment<coords*>& seg, iterator b4, 
		       iterator after, bool alwaysclosed=true);

    std::deque<coords> find_intersections(Linear_function<double> fn,
					  bool implicitly_closed=true)const{
      std::deque<coords> res;
      return find_intersections(fn, res, implicitly_closed);
    }

std::deque<coords>& find_intersections(Linear_function<double> fn,
				       std::deque<coords>& result,
				       bool closed_implicitly=true)const{

   const_iterator w=begin();
   const_iterator guard=w;
   if(!closed_implicitly){guard= end();}
    bool started=false;
    while (w != guard || !started){
      started=true;
      SubPath& nonconst = const_cast<SubPath&>(*this);
      ///@todo compiler finding non-const version of get_segment_and_iter()
      /// and complaining about throwing away const
      /// method hasn't got sideeffects
      /// fixme : how to control this?
      std::pair<const Segment<coords*>,const_iterator> wpair=nonconst.get_segment_and_iter(w,closed_implicitly);
      if((*(wpair.second)).second & CLOSE_PATH){guard=begin();}
      else if((*(wpair.second)).second & (BEZIER_CLICK|BEZIER_HANDLE)){
	throw xn_Error("flatten path b4 get_intersections()");
      }
      if(wpair.first.function_intersects(fn)){
	  result.push_back(wpair.first.get_intersection(fn));
	}
      w=wpair.second;
    }
    return result;
    }

 std::deque<std::pair<coords, Segment<coords*> > >&
 find_intersections_with_segment(const Linear_function<double>& fn,
				 std::deque<std::pair<coords, Segment<coords*> > >& result,
				 bool closed_implicitly=true)const;

  ///lotta constcastery?
  std::deque<std::pair<coords, Segment<coords*> > >
 find_intersections_with_segment(const Linear_function<double>& fn,
				 bool closed_implicitly=true)const;
 /**combine 2 subpaths .
    see SectionGroup<PathTYPE>::combine_paths() 
refactored to better handle
PATH_COMBINE_DIFF, AS well as more than 2 input paths
(works on 2 containers of PathTYPE)
  @return number of subpaths after combine
  @arg other = other path to combine with
  @arg outpath => path to append to
  @arg flag => combination flag
  */
    int combine(SubPath& other, simple_click_history& outpath,
		int flag=PATH_COMBINE_OR);

  };

  std::ostream& test_segment_and_iter(std::ostream& os, const SubPath& p);
  std::ostream& test_segment_and_iter(std::ostream& os, const Section< SubPath>& p);


} //namespace NCOORDS


#endif
