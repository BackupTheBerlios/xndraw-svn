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
   @file pathsection.h
   SECTIONS OF SOME PATHS BETWEEN THEIR INTERSECTIONS.
@todo: child class/more sophisticated Intersection, which handles more than 2 paths atr intersection
and sorts them according to the angle they make,
so if one looks for another section intersecting with it, 
it'll choose, say, the one with the most acute angle with it either to left or right
'same' with the crossings

*/

#ifndef NCOORDS_PATHSECTION_H
#define NCOORDS_PATHSECTION_H
#include "xndrawexceptions.h"

#if HAVE_CONFIG_H
#include "config.h"
#endif
#if CHECK_SUBPATH_SECTION
#define CONTAINER_H_VERBOSE 1
#endif
//remove_from_list()
#include "container.h"
//for JoinRank
#include "coordinatefunctors.h"
#include "bouncenav.h"
 ///@todo add to --debug switcheroo in build
#if CHECK_SUBPATH_SECTION
#define CHECK_SECTION_LOOP 1
#endif
#define XN_STREAMS_ERROR(streamjunk) do{		\
    std::ostringstream ostmp;			\
    ostmp << streamjunk;			\
    throw xn_Error(ostmp.str());		\
  }while(0) 

#include "clickhistory.h"

namespace NCOORDS{
  ///g++-4.4 can't find in container.h perhaps because of the overload below
   template<typename CONTAINER, typename OBJ>
    bool remove_from_list(CONTAINER& container, const OBJ& obj){
    size_t sz = container.size();
    for(typename CONTAINER::iterator w = container.begin(); w!= container.end();w++){
      if((*w)== obj){
	w=container.erase(w);
	if(w==container.end()){break;}
      }
    }
    return container.size() != sz;
  }


  template<typename CONTAINER, typename PATH>
bool remove_from_list(CONTAINER& container, const std::pair<Section<PATH>*,int>& obj);

  /**
should really be in pathsection.h
where to find various adjacent sections ...
At SECTION_START, the end of previous Section on same path(SECTION_START|SECTION_CONTINUED)
will be found, and at SECTION_END, the start of nextSection on same path(SECTION_END|SECTION_CONTINUED).
SECTION_ABOVE, whether at SECTION_START or END, will refer to the Section on the path
that crosses our Section's path at that intersection, after the intersection.(so we connect with its start)
SECTION_BELOW means, before the intersection (so we connect with its end)
@todo: make sure this  is really what's happening
   */
  enum SectionDirection{
    SECTION_ERROR =-1,
    SECTION_START=1,
    SECTION_END=SECTION_START << 1,
    SECTION_CONTINUED = SECTION_END << 1,
    SECTION_ABOVE = SECTION_CONTINUED<< 1,
    SECTION_BELOW = SECTION_ABOVE << 1,
    SECTION_START_ABOVE = SECTION_START|SECTION_ABOVE,
    SECTION_START_BELOW =  SECTION_START|SECTION_BELOW, 
    SECTION_START_CONTINUED = SECTION_START|SECTION_CONTINUED,
    SECTION_END_ABOVE = SECTION_END|SECTION_ABOVE,
    SECTION_END_BELOW =  SECTION_END|SECTION_BELOW, 
    SECTION_END_CONTINUED = SECTION_END|SECTION_CONTINUED,
    /*a 'separate' range - flags (no longer) used 4 output*/
    SECTION_USED=SECTION_BELOW<< 1,
    SECTION_BACKWARDS=SECTION_USED<<1
  };

  template< typename PATH>
  class SectionGroup; 

  /**
INFO TO pass into Section constructor .
  */
  enum { 
    INTERSECTION_ERROR=-1,
    INTERSECTION_NONE=0,
    /**FIRST PAIR in intersection*/
    INTERSECTION_FIRST=1, 
/**second PAIR in intersection*/
    INTERSECTION_SECOND=2,
    /** possible extra crossing path:  in supernumaries: realindex = gen_index-INTERSECTION_EXTRA
so Intersection<P>::get_path(n) will return supernumaries[n-INTERSECTION_EXTRA]*/
    INTERSECTION_EXTRA=3
  };

  template< typename T>
  struct FlagPointer:public std::pair<T*,int>{
    typedef std::pair<T*,int> base_type;
    using std::pair<T*,int>::first;
    using std::pair<T*,int>::second;


    FlagPointer(T* ptr=NULL, int flag=0):base_type(ptr,flag){}

    //T* operator*(){return first;}

    //const T*operator*()const{return first;}


  };
  template<typename T>
  bool operator==(const FlagPointer<T>& left, const FlagPointer<T>& right){
    return left.first = right.first;
  }

  
  //fwd decl in clickhistory.h
  //which is currently included
  //template <typename PATH>
  //class Intersection;

  //these operator<< should use template <typename CHAARTYPE, typename  TRAITS> also
  //using CTYPE_FACET macro for single characters, and the CHARTYPE*/string_type functions with it
  template <typename PATH>
  std::ostream& operator <<(std::ostream& os, const Intersection<PATH>& src);
template <typename PATH>
  std::ostream& operator <<(std::ostream& os, const Section<PATH>& src);
template <typename PATH>
bool operator==(const  Intersection<PATH>& left, const  Intersection<PATH>& right);
template <typename PATH>
bool operator==(const  Section<PATH>& left, const  Section<PATH>& right);

  template <typename PATH>
  bool operator<(const Section<PATH>& left, const Section<PATH>& right);
  

  /**
     @class Intersection
     data defineing an intersection
     a pair of pairs, each holding a PATH*, and an offset to the point b4 the intersection.
     Used to delineate Section<PATH> instances
  */
  template <typename PATH>
  class Intersection:
    public std::pair<std::pair<PATH*, unsigned int>,
		     std::pair<PATH*, unsigned int> >{
  public:
    typedef PATH path_type;
    typedef typename path_type::size_type size_type;
    typedef typename path_type::iterator iterator;
    typedef typename path_type::const_iterator const_iterator;
    typedef std::pair<path_type*, unsigned int> path_pair_type;
    typedef std::pair<path_pair_type, path_pair_type> base_type;
    typedef std::deque<std::pair<path_type*, unsigned int> > container_type;
    typedef typename container_type::iterator pathpair_iterator;
    typedef typename container_type::const_iterator const_pathpair_iterator;
typedef std::deque<Section<PATH>* > section_container_type;
    // typedef typename section_container_type::iterator section_iterator;
    //typedef typename section_container_type::const_iterator const_section_iterator;
   
    using base_type::first;
    using base_type::second;
    
  protected:
    //no members except for those of base class
    //more lines that cross one or both intersecting segment within
    //coords::min_radius() of the intersection
    std::deque<std::pair<path_type*, unsigned int> > supernumaries;
  public:
    /**
@todo test to see why 'equal' intersections are appearing in the set
deal with intersections which coincide with an existing point in one(or even both paths
)here and in Section too
     */
    Intersection(base_type src):base_type(src){}
    coords operator()(){return get_intersection();}

    //friend std::ostream& operator <<(std::ostream& os, const Intersection<PATH>& src);
    std::pair<path_type*, unsigned int> get_supernumary(unsigned int index)const{
      if(supernumaries.size() <= index){
	throw xn_Error("index outside range");
      }
	return supernumaries[index];
    }
    container_type& get_extras(){return supernumaries;}
const container_type& get_extras()const{return supernumaries;}
    size_type extras_size()const{
      return supernumaries.size();
    }
    /**
how many paths cross at intersection
     */
    size_type pathcount()const{
 return supernumaries.size() +2;
    }


template<typename CONTAINER>
static void add_intersection(CONTAINER& outinters,
		      const Intersection& inter);

template<typename CONTAINER>
static void add_intersections(CONTAINER& outinters,
			      typename PATH::intersection_result_type rawdata);

    /**
currently may contain dupes:just naively copies
     */
    std::deque<PATH*> get_paths()const{
      std::deque<PATH*> res;
      res.push_back(first.first);
      res.push_back(second.first);
      for(const_pathpair_iterator w = supernumaries.begin(); w!= supernumaries.end();w++){
	res.push_back((*w).first);
      }
      return res;
    }

    bool uses_path(const PATH& path)const;

    /**
merge an intersection that is 'equal' to an existing one
with it
     */
void merge_intersection(const Intersection& inter);
    /**
@arg  whichpath =>INTERSECTION_FIRST or INTERSECTION_SECOND or
INTERSECTION_EXTRA + index to get something in supernumaries:
it's an error if there's no such index in supernumaries
@return => path which matches whichpath
     */
    path_type* get_path(int whichpath);
/**
@arg  whichpath =>INTERSECTION_FIRST or INTERSECTION_SECOND 
@return => path which doesn't match whichpath
ie: opposite path to what get_path() would return

     */
    path_type* get_alternate_path(int whichpath){
      whichpath = swap_whichflag(whichpath);
      return get_path(whichpath);
    }

    static int  swap_whichflag(int whichflag){
      switch( whichflag){
      case INTERSECTION_FIRST:
	return INTERSECTION_SECOND;
      case INTERSECTION_SECOND:
	return INTERSECTION_FIRST;
      default:
	return whichflag;
      }
      return whichflag;
    }

/**
@arg  whichpath =>INTERSECTION_FIRST or INTERSECTION_SECONDor
INTERSECTION_EXTRA + index to get something in supernumaries:
it's an error if there's no such index in supernumaries
     */
    const path_type* get_path(int whichpath)const;

    /**
       @arg  whichpath =>INTERSECTION_FIRST or INTERSECTION_SECOND
       @return => path which doesn't match whichpath
       ie: opposite path to what get_path() would return
    */
    const path_type* get_alternate_path(int whichpath)const{
      whichpath = swap_whichflag(whichpath);
      return get_path(whichpath);
    }
    /**
       @arg  whichpath =>INTERSECTION_FIRST or INTERSECTION_SECOND or
INTERSECTION_EXTRA + index to get something in supernumaries:
it's an error if there's no such index in supernumaries
    */
    size_type get_offset(int whichpath)const;
    /**
       @arg  whichpath =>INTERSECTION_FIRST or INTERSECTION_SECOND
    */
    size_type get_alternate_offset(int whichpath)const{
      whichpath = swap_whichflag(whichpath);
      return get_offset(whichpath);
    }
    /**
       orig clunky API - now should swap to using get_path(), get_alternate_path() etc
    */
    path_type* get_first_path(){  return first.first;}
    const path_type* get_first_path()const{ return first.first;}
    
    path_type* get_second_path(){return second.first;}
    const path_type* get_second_path()const{return second.first;}

    size_type get_first_offset()const{return first.second;}
    size_type get_second_offset()const{return second.second;}
    
    Segment<coords*> get_first_segment(){return get_intersecting_segment(first);}
    const Segment<coords*> get_first_segment()const{ return get_intersecting_segment(first);}
    
    Segment<coords*> get_second_segment(){return get_intersecting_segment(second);}
    const Segment<coords*> get_second_segment()const{return get_intersecting_segment(second);}
    /// only useful with first and second
    Segment<coords*> get_intersecting_segment(std::pair<path_type*, unsigned int>& source);
    const Segment<coords*> 
    get_intersecting_segment(const std::pair<path_type*, unsigned int>& source)const;

 

    coords get_intersection()const;
  };
  /**
     get intersection on PATH p, at location @code location @endcode, 
     and index @code   which @endcode .
   get intersection, with additional info to show which path(and especially), offset
in intersection should be used in the Sections created out of p .
@arg first => iterator to start of range of Intersections to choose
@arg last => sentinel iterator at end of range
@arg p => path on which we are finding the intersections(in order to create Section<PATH>s)
@arg location => offset on p at which to find the intersection
@arg which => index of which intersection at that location, if there are multiples
@return => a pair. rv.first is a pointer to an intersection, and rv.second
is either INTERSECTION_FIRST,  INTERSECTION_SECOND or INTERSECTION_NONE
   showing which of intersection.first and intersection.second
   has the actual offset into p, OR THAT THERE is no intersection(INTERSECTION_NONE):
ie we are at begin() or end() of p. throw an error if there is no intersection on p at location and which 
   @todo as static member of SectionGroup<PATH> ?

  */
  template <typename PATH, typename FWDITER>
  std::pair<Intersection<PATH>*, int> 
  find_intersection_at(FWDITER first, FWDITER last,
		       PATH* p, unsigned int location, 
		       unsigned int which);

  /**
describes a section of a PATH between two (possibly NULL) Intersection* .

     @class Section
     
     begin() and end() point to offsets within path
     if there is an intersectionbefore (ie begin() not at PATH::begin()) 
     the intersection point has to be included before.
     same story for intersection after
@todo think about adding a member data and/or method to tell if paths should be closed
  */
  template <typename PATH>
  class Section{
  public:
    //remember to account for intersections and so
    // a possible extra point 'before' begin() and 'after' end()-1
   
    typedef  PATH path_type;
    typedef  Intersection<PATH> intersection_type;
    typedef typename PATH::iterator iterator; //simple_click_history::iterator
    typedef typename PATH::const_iterator const_iterator;
    typedef typename PATH::reverse_iterator reverse_iterator; 
    typedef typename PATH::const_reverse_iterator const_reverse_iterator;
    typedef typename PATH::size_type size_type;
    typedef typename std::deque<Section >::const_iterator const_section_iterator;
    typedef typename std::deque<Section >::iterator section_iterator;
    typedef ssize_t signed_size_type;

    friend class SectionGroup<PATH>;
    friend class SectionShape<PATH>;
  
  protected:
    PATH* subpath;//we iterate over a 'section' of it
    Intersection<PATH>* startinter;
    Intersection<PATH>* endinter;
    int crossings, use_flags;
    int p_start, p_end; ///<which offsets in startinter and endinter to use
    //int outside;
    coords startextra, endextra;   
    static const size_type crossings_absmax;
    //static int crossings_now

  public:

    Section(PATH* p, std::pair<Intersection<PATH>*, int> l, 
	    std::pair<Intersection<PATH>* , int> r, int outcrossings=-1);
    //template<>
    friend std::ostream& operator << <PATH>(std::ostream& os, const Section<PATH>& src);

    ///@todo document use_flags
    void clear_use_flags(){use_flags=0;}
    void set_unused(){set_used(false);}
    bool used_for_output()const{return use_flags&SECTION_USED;}
    bool use_backwards()const{return use_flags&SECTION_BACKWARDS;}
    void set_used(bool used=true);
    bool marked()const{ return crossings >=0;}

    bool self_intersects()const{
      return startinter &&(startinter==endinter);
    }
    /*
   
    */
    static bool has_unmarked_sections(const std::deque<Section >& sections);
    static void add_path_sections(PATH* pat, 
				  const std::deque<Intersection<PATH>*>& pathinters,
				  const std::deque<unsigned int>& offsets,
				  const std::set<Intersection<PATH> >& intersections,
				  std::deque<Section >& outsections);

    int get_joinend(const intersection_type& inter)const{
      int res(0);
      if(&inter==startinter){res|=SECTION_START;}
      if(&inter==endinter){res|=SECTION_END;}
      if(!res){throw xn_Error("intersecton not used");}
      return res;
    }

    static std::deque<int> get_all_adjoining_sections_flags();
    /**gets the actual flags for this section.
       ie those only for which it actually has intersection(s)
    */
    std::deque<int> get_adjoining_sections_flags()const;
    std::deque<int> get_adjoining_sections_flags(int joinend)const;

   std::deque<int> get_closest_adjoining_sections_flags()const;
   std::deque<int> get_closest_adjoining_sections_flags(int joinend)const;
     /**
       @fn get_adjoining_sections
       create a deque<Section*> of Sections that adjoin the caller from  @code sections @endcode
       @arg sections a list of Sections
       @return the adjoining sectors :Section*s in rv returned only valid while sections alive
       TODO: use Set<Section> for the arg(and for the list of sections generally)
       will need operator==() for that
    */
    std::deque<Section*> 
    get_adjoining_sections(std::deque<Section >& sections)const;
    

    /**
get those adjoining sections with 
     */
std::deque<Section*> 
    get_closest_adjoining_sections(std::deque<Section >& sections)const;
    /**
@return => a deque<std::pair<Section*, int> > containing sections 
that are attached at the end of the calling section indicated by joinend
and directly adjacent (ie a line drawn from the caller can cross them
without crossing anything. It ignores SECTION_CONTINUED sections 
and selects from the SECTION_ABOVE and _BELOW). Returnval.second indicates
the joinend for when they call this method in turn
@arg sectors => the list of sections to select from
@arg joinend => SECTION_START or SECTION_END => the end of `this' to which
the returned sections are attached
     */
    template <typename FWDITER>
std::deque<std::pair<Section<PATH>*, int> > 
    get_closest_adjoining_sections_with_joinend(FWDITER first, FWDITER last,
						int joinend);
    
    /**
@return => a deque<std::pair<Section*, int> > containing sections 
that are attached at the end of the calling section indicated by joinend
and directly adjacent (ie a line drawn from the caller can cross them
without crossing anything. It ignores SECTION_CONTINUED sections 
and selects from the SECTION_ABOVE and _BELOW). only those Section
where section.crossings == numcross are returned.Returnval.second indicates
the joinend for when they call this method in turn
@arg sectors => the list of sections to select from
@arg joinend => SECTION_START or SECTION_END => the end of `this' to which
the returned sections are attached
@arg numcross => number of crossings_to_outside on matching sections
@todo => signature to take a pair of FWDITER instead of sectors
    */    

template <typename FWDITER>
std::deque<std::pair<Section<PATH>*, int> > 
    get_closest_adjoining_sections_with_joinend(FWDITER first, FWDITER last,
    int joinend, int numcross);
    /**
if no escape possible, return false and NULL,
else a grandparent Bounce*(ie top of tree).remember to delete later, as it is possible to allocate a lot of memory(recursive bouncing!)when used
     */
    std::pair<bool,Bounce*> can_bounce_escape(const PATH& Oobstacle, size_t segwhich, 
					      int whichside)const{
      std::pair<Segment<coords*>,size_t> segpair=get_segment_and_iter(segwhich);
      Bounce bounce(segpair.first,whichside);
      if(bounce.no_escape(Oobstacle)){return std::pair<bool,Bounce*>(false, NULL);}
      return std::pair<bool,Bounce*>(true,new Bounce (segpair.first,whichside));
    } 
    
    /**
does Section have an adjoining section in the place indicated by flag
@arg flag=> eg SECTION_END|SECTION_ABOVE 
     */
    bool has_adjoining_section(int flag)const;

    /** 
	@fn smallest_adjacent_mark
	finds the smallest positive marked section, and returns it and the mark,
	or NULL and -1
    */
    std::pair<Section*, int> 
    smallest_adjacent_mark(std::deque<Section>& sections)const;
    /**
       @fn whichend
       at which terminus of section is an_inter
       @return -1 for start, 1 for end
    */
    int whichend(const Intersection<PATH>* an_inter)const;

    void add_to_path(simple_click_history& outp,
		     bool forwards,
		     bool first_section);

    static std::pair<Section*, int> reverse_joinend(std::pair<Section*, int> with_joinend){
      std::pair<Section*, int> tmp(with_joinend);
      swap_joinend_flag(tmp.second);
      return tmp;
    }
    /**
convenience to swap flag indicating end os Section 
intersection we might want is at
@arg joinend => SECTION_START or SECTION_END
@return => SECTION_END or SECTION_START
     */
    static int swap_joinend_flag(int joinend);

    ///show which half of startinter is the start  of Section
    ///@return =>INTERSECTION_FIRST,INTERSECTION_SECOND, or INTERSECTION_NONE
    int get_p_start()const{ return p_start;}
    ///show which half of endinter is the end  of Section
    ///@return =>INTERSECTION_FIRST,INTERSECTION_SECOND, or INTERSECTION_NONE
    int get_p_end()const{ return p_end;}
    ///which half(path and offset) of the intersection at the end indicated
    ///pertains to Section
    int get_p_inter(int whichend)const{
      if(whichend & SECTION_START){return p_start;}
      else if(whichend & SECTION_END){ return p_end;}
      throw xn_Error("whichend must contain SECTION_START or SECTION_END");
    }
    /**
    ///currently throws xn_Error on failure to find an intersection between the 2 sections
    ///actually get_common_intersection() throws the error
@arg fst => first section to compare
@arg snd => second section to compare
@arg common_inter => intersection to use in comparison(error if not common to fst and snd)
@todo was ambiguous without also supplying the Intersection we mean(since they might intersect twice)
Another weirdness is where both intersections are the same for each Section(ie its a 'twisted loop'
with a single Intersection where the closed path crosses itself - in this case, return true?)
    */
    static bool junction_continues_direction(const Section& fst,
					     const Section& snd,
					     const Intersection<PATH>* common_inter);
template<typename ITER>
 std::deque<Section*> get_extra_adjacent_sections(ITER first,
						  ITER last, 
						  int flag_which)const;

    template<typename ITER>
 bool is_adjacent(ITER first, ITER last,
		  const Section& other, int flag_which)const{
      if(!(&other)){throw xn_Error("other is ref to NULL pointer: ensure it is not NULL b4 calling Section::is_adjacent()");}
      Intersection<PATH>* theinter=const_cast<Intersection<PATH>*>(get_intersection(flag_which));
      if (!theinter){return false;}
      Section* adj1=get_adjacent_section(first, last, flag_which);
      if(adj1== &other){return true;}
      if(theinter->extras_size()){
	std::deque<Section*> extras= get_extra_adjacent_sections(first, last, flag_which);
	for(typename std::deque<Section*>::const_iterator w = extras.begin(); w!= extras.end();w++){
	  if(*w == &other){return true;}
	}
      }
      return false;
    }

    double get_angle(const Section& other, 
		     int joinflag=SECTION_END|SECTION_ABOVE)const;

    bool has_common_intersection(const Section& other)const;
    Intersection<PATH>* get_common_intersection(const Section& other)const;
    std::deque< Intersection<PATH>*> get_common_intersections(const Section& other)const;
    
    Section* get_pathstart_section(std::deque<Section>& sections)const;
    Section* get_pathend_section(std::deque<Section>& sections)const;
    /**
       find the Section at the start of subpath .
       @return => start Section of subpath
       @arg first => iterator to start of a collection of Section
       @arg last=> iterator to (one past) end of a collection of Section
       @todo find and replace any code using a different test 
       to in effect find start or end section
       eg @code
       if((subpath == sect->subpath)&& //OK
       (sect->begin() == subpath->begin())){  //unreliable
       //same goes with test for end()
       // do_something();
       }@endcode
    */
    template <typename ITER>
    Section* get_pathstart_section(ITER first, ITER last)const;

    /**
       get the Section at end of subpath .
       @return => end Section of subpath
       @arg first => iterator to start of a collection of Section
       @arg last=> iterator to (one past) end of a collection of Section
       @todo : to whom belongs the 'close_path' segment? - to end section!
       which 'actually' ends at subpath->begin()
       (although end() == subpath->end()).
       make sure a section ending at an intersection
       in the 'close_path section' behaves sanely
    */
    template <typename ITER>
    Section* get_pathend_section(ITER first, ITER last)const;
    /**
@return =>the section at end of path indicated by joinend
@arg joinend => SECTION_END or SECTION_START
@arg first => start of a range of Sections
@arg last =>end of  a range of Sections
     */
    template <typename ITER>
    Section* get_terminal_section(ITER first, ITER last, int joinend=SECTION_END)const;
     
    void set_backwards(const Section& adjoining, int joinend=SECTION_END);

    ///TODO: version taking 2 candidate sections
    /**
@arg first => start iterator to a container of Section
@arg last => sentinel at end of a container of Section
@arg flag => end of section to find connected section at: POINTSIDE_LEFT or POINTSIDE_RIGHT
@arg whichside => POINTSIDE_LEFT or POINTSIDE_RIGHT
@return => a Section*, and an int to show which end to look for next Section after it
     */
    template <typename ITER>
    std::pair<Section*, int> choose_adjacent_section(ITER first, 
				     ITER last,
				     int joinend, int whichside);
    
    /**
@todo fixme: this is not correct
count will vary depending if last point is a CLOSE_PATH
when we assume path always closed
     */
    size_type segments()const;
    ///signed type index: -1 means startintersection
#define SEG_ERROR_RETURN -3
#define SEG_LAST_RETURN -2 ///<LAST SEGMENT IN SECTION
    /**
get the segment starting at 'index' segstart .

    latest attempt at reliable way to iterate over segments in section 
    @return => a pair: rv.first is the actual Segment
    rv.second is the position of the second pair<coords, int> in the section
if (rv.second ==  SEG_LAST_RETURN ) this is the final segment
@arg segstart => position to find the first coorsds at in Section
start with a value of either -1 (if startinter is present), or 0
    ///and keep feeding back rv.second until it is SEG_LAST_RETURN
*/
    std::pair<Segment<coords*>, ssize_t> get_segment_and_iter(ssize_t segstart,
							      bool closepaths=true);

    /**
get the segment starting at 'index' segstart .

    latest attempt at reliable way to iterate over segments in section 
    @return => a pair: rv.first is the actual Segment
    rv.second is the position of the second pair<coords, int> in the section
if (rv.second ==  SEG_LAST_RETURN ) this is the final segment
@arg segstart => position to find the first coorsds at in Section
start with a value of either -1 (if startinter is present), or 0
    ///and keep feeding back rv.second until it is SEG_LAST_RETURN

@arg closepaths => always will be true so this arg might as well disappear
unless we plan to make this work for unclosed lines
     */

    std::pair<const Segment<coords*>, ssize_t> get_segment_and_iter(ssize_t segstart, 
       							    bool closepaths=true)const;
  protected:
    ptrdiff_t get_maxstart()const;
    void check_segment_start(ssize_t)const;
  public:
    /**
get an iterator to first moveto in subpath .
     */
    iterator get_pathstart_iter();

    /**
get a const_iterator to first moveto in subpath .
     */
    const_iterator get_pathstart_iter()const;


  
    std::pair<coords, Linear_function<double> > 
    perpendicular_at(signed_size_type pos)const;
  
    ///make sure the 2 segment_at() stay in sync
    const Segment<coords*> segment_at(signed_size_type pos)const;
    Segment<coords*> segment_at(signed_size_type pos);

    int crossings_to_outside()const;

    void set_crossings(int howmany){
      if((howmany > (int)crossings_absmax) || (howmany <0)){
	std::ostringstream ostmp;
	ostmp << "crossings set to invalid value: "<<howmany<<"\nmax is "
	      <<crossings_absmax;
	throw xn_Error(ostmp.str());
      }
      crossings = howmany;
    }

    /**
       is section on the outside edge?.
       @return  std::pair<bool, int> =>bool =>is it outside?
       => int -what side is outside if it is indeed on outside
       POINTSIDE_LEFT or POINTSIDE_RIGHT
       ONLY meaningful if the bool is true
       warning : may be false negatives
       on some sections
       idea is iterate over the sections,
       find an outside section, and then iterate over Sections again
       to work out where the rest fit in?
       deq version  checks if Section is inside a collection of subpaths, paths etc
       and is likely to give more false negs again - its plays it safe until finding
       a section uneqivocally on the outside
       TODOtest framework to find wrinkles/test alternatives
       something to more definitively show if some section is on the outside of 
       composite shape(including in an 'outside' OR shape hole)
       see Bounce(also not definitive)
 
    */
    std::pair<bool,int> is_outside(const std::deque<PATH*>& paths, 
				   bool closepaths=true);
    /**
@todo: test in case where otherpath==subpath
     */
    std::pair<bool,int> is_outside(const PATH* otherpath, 
				   bool closepaths=true)const;

    std::pair<bool,int> can_bounce_outside(const std::deque<PATH*>& paths, 
				   bool closepaths=true)const;
 std::pair<bool,int> can_bounce_outside(const PATH* otherpath, 
				   bool closepaths=true)const;

    bool is_inside(const PATH* otherpath, 
		   bool closepaths=true)const;
 bool is_inside(const std::deque<PATH*>& paths, 
		   bool closepaths=true)const;

   
    /**
       @return size(number of pts) of section, including any intersection(s)
    */
    size_type size()const;
    /**the points in the section in human readable form*/
    std::ostream& human_points(std::ostream& os)const;
    /**streams info on the intersections and path, 
       and the output of human_points*/
    std::ostream& debug_print(std::ostream& os)const;

    PATH* get_path(){return subpath;}
    const PATH* get_path()const {return subpath;}
 /**
@arg joinend =>SECTION_START or SECTION_END
    */
Intersection<PATH>* get_intersection(int joinend){
  if(joinend & SECTION_START){return startinter;}
  else if(joinend & SECTION_END){return endinter;}
  throw xn_Error("bad joinend - should contain SECTION_START or SECTION_END");
}
    /**
@arg joinend =>SECTION_START or SECTION_END
    */
const Intersection<PATH>* get_intersection(int joinend)const{
  if(joinend & SECTION_START){return startinter;}
  else if(joinend & SECTION_END){return endinter;}
  throw xn_Error("bad joinend - should contain SECTION_START or SECTION_END");
}

template <typename ITER>
std::deque<std::pair<Section*, int> > 
get_adjacent_candidates(ITER first, ITER last, 
			int  joinend);

    /**
    

@return => a pair of the Section*, and an int, 
signifying the end opposite to that 
thru which it connects to the caller at joinend. 
ie the next joinend to be searched
    */
 template <typename ITER>
   std::pair<Section*, int> choose_section_by_angle(ITER first, ITER last, 
						    int  joinend, bool go_high);

    Intersection<PATH>* get_start_intersection(){return startinter;}
    const Intersection<PATH>* get_start_intersection()const{return startinter;}
    Intersection<PATH>* get_end_intersection(){return endinter;}
    const Intersection<PATH>* get_end_intersection()const{return endinter;}

    unsigned int get_end_offset()const;
    unsigned int get_start_offset()const;

    //iterators
    //remember the possibility of preceding/next intersections
    //at end of path
    iterator begin();
    const_iterator begin()const;

    iterator end();
    const_iterator end()const;
    reverse_iterator rbegin();
    const_reverse_iterator rbegin()const;
    reverse_iterator rend();
    const_reverse_iterator rend()const;
    //end of iterators
    ////////
    bool has_start_intersection()const{return startinter;}
    bool has_end_intersection()const{return endinter;}

    /**
@return => does section have intersection at the end indicated by joinend
@arg joinend -> SECTION_START or SECTION_END
     */
    bool has_intersection(int joinend)const{return get_intersection(joinend);}
    /**
@return => is inter either startinter or endinter
@arg inter => the intersection to test for
     */
    bool contains_intersection(const intersection_type& inter)const{
      return (startinter==&inter) || (endinter==&inter);
    }
    coords get_start_intersection_coords()const;
    coords get_end_intersection_coords()const;
    ///seems unnecessary
    template < typename FWDITER>
    std::pair<Section<PATH>*, int> choose_unused_section(FWDITER first, FWDITER last, int joinend, int outside ){
      if(first == last){
	throw xn_Error("empty container");
      }
      JoinRank<Section<PATH> > sorter(*this, (outside == POINTSIDE_LEFT ? false :true));
      std::deque<std::pair<Section<PATH>*, int> > tmp(first, last);
      std::sort(tmp.begin(), tmp.end(), sorter);
      return tmp[0];
    }

    bool uses_intersection(const intersection_type& inter)const{
      if(!(&inter)){return false;}
      return (startinter == &inter) || (endinter ==&inter);
    }
    /**

       get the point next to intersection
       @return a @code coords @endcode
       /// @arg adjacent_section => the section point will come from
       /// @arg flag_which => SECTION_ABOVE|SECTION_END or so
       */
    coords get_adjacent_point(Section* adjacent_section,
			      int flag_which)const;
    /**
@return => the first Segment of path
note that if the Segment from the start intersection to the adjacent point 
is too short it will use the point after that as 2nd point

     */
    Segment<coords*> get_start_segment();
/**
@return => the first Segment of path
note that if the Segment from the start intersection to the adjacent point 
is too short it will use the point after that as 2nd point

     */
    const Segment<coords*> get_start_segment()const;
/**
@return => the last Segment of path
note that if the Segment from the end intersection to the adjacent point 
is too short it will use the point b4 that as 1st point

     */
    Segment<coords*> get_end_segment();
/**
@return => the last Segment of path
note that if the Segment from the end intersection to the adjacent point 
is too short it will use the point before that as 1st point

     */
    const Segment<coords*> get_end_segment()const;

    Segment<coords*> get_adjacent_segment(Section* adjacent_section,
					  int flag_which)const;

    //TODO: const version?
    template<typename ITER>
    Section* get_adjacent_section(ITER first, ITER last, int flag_which)const;

 template <typename ITER>
  void print_choices(ITER first, ITER last, std::ostream& os, int joinend, int side){
    if((joinend!=SECTION_END)&&(joinend!=SECTION_START)){
      os <<"joinend not equal to SECTION_END or SECTION_START"<<std::endl;
    }
    if(!(has_intersection(joinend))){
      os <<"no intersection this end, so only 1 possible choice"<<std::endl;
      return;
    }
      std::pair<Section*, int> choice= choose_adjacent_section(first, last, joinend, side);
      int choicelocn(SECTION_ERROR),antichoicelocn(SECTION_ERROR),
	choicelocn2(SECTION_ERROR),antichoicelocn2(SECTION_ERROR);
      //if(choice.second==SECTION_END){choicelocn |= SECTION_ABOVE;}
      //else if(choice.second==SECTION_START){choicelocn |= SECTION_BELOW;}
      std::pair<Section*, int> antichoice= choose_adjacent_section(first, last, joinend, -side);
      std::deque<std::pair<Section*, int> > chooselist=get_adjacent_candidates(first, last, joinend);
      for(typename std::deque<std::pair<Section*, int> >::const_iterator w = chooselist.begin();w != chooselist.end(); w++){
	if((*w).first == choice.first){
	  if(choicelocn==SECTION_ERROR){choicelocn=(*w).second;}
	  else{
	    os <<"another copy of same section(the chosen one - "
	       <<choice.first<<") in list"<<std::endl;
	    choicelocn2=(*w).second;
	    if(choicelocn==choicelocn2){os <<"same topological info as previuos one:ERROR\n";}
	  }
	}
	if((*w).first == antichoice.first){
	  if(antichoicelocn==SECTION_ERROR){antichoicelocn=(*w).second;}
	  else{os <<"another copy of same section(the unchosen one - "
		  <<antichoice.first<<" in list"<<std::endl;
	    antichoicelocn2=(*w).second;
	    if(antichoicelocn==antichoicelocn2){os <<"same topological info as previuos one:ERROR\n";}
	  }
	}
      //int antichoicelocn=joinend;
      //if(choice.second==SECTION_END){antichoicelocn |= SECTION_ABOVE;}
      //else if(choice.second==SECTION_START){antichoicelocn |= SECTION_BELOW;}
      os<<"choose_adjacent_section() for joinend: "
	<<(joinend==SECTION_END?"SECTION_END" :(joinend==SECTION_START ?"SECTION_START" : "???????"))
	<<"\npointside: "<<pointside_as_string(side) <<"\nchoice is : "<<choice.first<<"new joinend at "
	<<(choice.second ==SECTION_END?"SECTION_END" :(choice.second==SECTION_START ?"SECTION_START" : "???????"))
	<<"\n angle is "<<get_angle(*(choice.first), choicelocn);
      if(choicelocn2 != SECTION_ERROR){
	os << "other angle: "<<get_angle(*(choice.first), choicelocn2);
      }
os <<"\nantichoice is : "<<antichoice.first<<"new joinend at "
	<<(antichoice.second ==SECTION_END?"SECTION_END" :(antichoice.second==SECTION_START ?"SECTION_START" : "???????"))
	<<"\n angle is "<<get_angle(*(antichoice.first), antichoicelocn)<<std::endl;
    }
   if(antichoicelocn2 != SECTION_ERROR){
	os << "other angle: "<<get_angle(*(antichoice.first), antichoicelocn2);
      }
 }
  };
  template< typename PATH >
  std::ostream& operator<<(std::ostream& os, const SectionGroup<PATH>& src);

template< typename PATH >
bool operator==(const SectionShape<PATH>& left,
		const SectionShape<PATH>& right);

template< typename PATH >
std::ostream& operator<<(std::ostream& os, const SectionShape<PATH>& src);

template< typename PATH >
class SectionShape{
public:
    typedef PATH path_type;
  typedef Section<PATH> section_type;
  //typedef std::deque<section_type*> container_type;
  //needs joinend, at least for OR path
  typedef std::deque<std::pair<section_type*, int> > container_type;
  typedef typename container_type::iterator iterator;
typedef typename container_type::const_iterator const_iterator;
typedef typename container_type::reverse_iterator reverse_iterator;
typedef typename container_type::const_reverse_iterator const_reverse_iterator;
  typedef typename container_type::size_type size_type;
  
  typedef Intersection<PATH> intersection_type;

protected:
  container_type outlines;
  void check_crossings()const;

  void validate_outlines(const container_type& incontainer)const{
    const_iterator first(incontainer.begin()), last(incontainer.end());
    if(pair_first_contains_dupes(first, last)){
      std::ostringstream ostmp;
      ostmp <<"duplicate_sections in shape!"<<std::endl;
      std::pair<const_iterator, const_iterator> dupes =get_next_pair_first_dupe(first, last);
      while(dupes.first != last){
	ostmp<<"dupe at positions "<<dupes.first-first<< " and "<< dupes.second-first
	     <<"\nthe duplicate Section:\n"<<*((*(dupes.first)).first);
	dupes=get_next_pair_first_dupe(dupes.first, last);
      }
      std::string s_err=ostmp.str();
      std::cerr<<s_err<<std::endl;
      throw xn_Error(s_err.c_str());
    }
  }
public:
  friend class SectionGroup<PATH>;
  
  SectionShape(const container_type& sectoutlines):
    outlines(sectoutlines){
#if CHECK_SUBPATH_SECTION
    check_crossings();
    validate_outlines(outlines);
#endif
  }

  SectionShape(const SectionShape& src):outlines(src.outlines){
    std::cerr<<"SectionShape::SectionShape(const SectionShape& src)\n";
#if CHECK_SUBPATH_SECTION
    //check_crossings();
#endif
  }

  SectionShape& operator=(const SectionShape& src){
  if(&src != this){
    outlines=src.outlines;
#if CHECK_SUBPATH_SECTION
    //check_crossings();
#endif
  }
  return *this;
}

  int get_joinend(const section_type& sector)const{
    for(const_iterator w=outlines.begin();w!= outlines.end();w++){
      if((*w).first == &sector){return (*w).second;}
    }
    throw xn_Error("shape doesn't use section");
  }

  template <typename SHAPEITER>
  static int shapes_get_joinend(SHAPEITER first, SHAPEITER last, 
				const section_type& sector){
    for(;first !=last;first++){
      if((*first).uses_outline(sector)){
	return (*first).get_joinend(sector);
      }
    }
    throw xn_Error("shapes don't use section");
  }

  /**
shapes sharing one or more section with this
   */
  template <typename SHAPEITER>
  std::deque<SectionShape> get_contiguous_shapes(SHAPEITER first,
						  SHAPEITER last);
  /**
@return => is the intersection part of any of the Section in outlines?
@arg inter => the intersection to look for
   */
  bool uses_intersection(const intersection_type& inter)const{
    for(const_iterator w = outlines.begin(); w != outlines.end();w++){
      if((*w).first->contains_intersection(inter)){return true;}
    }
    return false;
  }

  bool uses_outline(const Section<PATH>& section)const{
    for(const_iterator w = outlines.begin(); w!= outlines.end();w++){
      if(&section == (*w).first){return true;}
    }
    return false;
  }
  
  template <typename SHAPEITER>
  static bool shapes_use_outline(SHAPEITER first, SHAPEITER last, 
				 const Section<PATH>& section){
    for(;first !=last;first++){
      if((*first).uses_outline(section)){return true;}
    }
    return false;
  }
  /**
put the Sections in outlines that are before outlines.begin() + (howmany%outlines.size()) at the end of outlines so that outlines now start at what was outlines.begin() + (howmany%outlines.size())
   */

  void rotate(int howfar){
    howfar = howfar % outlines.size();
    if(howfar){
      iterator mark=outlines.begin()+howfar;
      container_type outoutlines(mark, outlines.end());
      for(const_iterator w= outlines.begin(); w != mark;w++){
	outoutlines.push_back(*w);
      }
      outlines= outoutlines;
    }
  }
  /**
reverse the order of the sections
   */
  //fix when joinend added to type
  void reverse(){
    container_type outoutlines;
    for(const_reverse_iterator w= outlines.rbegin(); w!= outlines.rend();w++){
      outoutlines.push_back(section_type::reverse_joinend(*w));
    }
    outlines = outoutlines;
  }

friend bool operator==<PATH>(const SectionShape& left, const SectionShape& right);

friend std::ostream& operator<< <PATH>(std::ostream& os, const SectionShape& src);

  /**
@return => how many lines to cross to get to the outside
   */
  int crossings_to_outside()const;

  /**
shapes sharing an intersection, but not a Section
can use to find pairs of shapes to combine at output
  */
template <typename SHAPEITER>
std::deque<SectionShape> get_shapes_that_intersect(SHAPEITER first,
						    SHAPEITER last)const;

  bool intersects(const SectionShape& shape)const;

bool is_contiguous(const SectionShape& shape)const;

  simple_click_history& add_to_path(simple_click_history& outpath);

 

};

  /**
@class SectionGroup<PATH>
  ///create from a set of intersecting paths
used by simple_click_history::combine()
@todo: solve problem of too short segments, by merging an offending intersection
with a preexisting adjacent point(more problematic when the source paths each have point too close)
add a start and end adjuster that can possibly move the offsets one in at either end(of course it must have the points - always keep the 2 intersections, or if only one inter, one point also. if still too short, elide away the whole section
and Intersection class that can handle more than 2 paths intersecting, which will change mark_adjoining to only mark the 'closest' pair of paths directly adjacent in any cycle. Also, need to determine where the intersection is actually at
  */
  template< typename PATH >
  class SectionGroup{
  public:
    typedef PATH path_type;
    typedef std::set<FlagPointer<PATH> > path_set_type;
    typedef Intersection<PATH> intersection_type;
    typedef Section<PATH> section_type;
    typedef SectionShape<PATH> shape_type;
    typedef std::set<Intersection<PATH> > intersection_container_type;
    typedef std::deque<shape_type> shape_container_type;
    typedef typename path_set_type::iterator iterator;
    typedef typename path_set_type::const_iterator const_iterator;
    typedef typename Section<PATH>::iterator sub_iterator;
    typedef typename Section<PATH>::const_iterator const_sub_iterator;
    typedef typename Section<PATH>::reverse_iterator reverse_sub_iterator;
    typedef typename Section<PATH>::const_reverse_iterator const_reverse_sub_iterator;

    typedef typename intersection_container_type::iterator intersection_iterator;
    typedef typename intersection_container_type::const_iterator const_intersection_iterator;
    typedef typename intersection_container_type::size_type size_type;
    typedef typename std::deque<SectionShape<PATH> >::iterator shape_iterator;
    typedef typename std::deque<SectionShape<PATH> >::const_iterator const_shape_iterator;
    typedef typename std::deque<Section<PATH> >::iterator section_iterator;
    typedef typename std::deque<Section<PATH> >::const_iterator const_section_iterator;
    typedef typename std::deque<Section<PATH> >::reverse_iterator reverse_section_iterator;
    typedef typename std::deque<Section<PATH> >::const_reverse_iterator const_reverse_section_iterator;
    typedef LhsRhsJoinRank<Section<PATH> ,SectionGroup<PATH>  > diff_rank_type;
    
  protected:
    std::deque<Section<PATH> > sections;
    std::deque<SectionShape<PATH> > shapes;
    std::deque<SectionShape<PATH> > OR_shapes;
    std::deque<SectionShape<PATH> > DIFF_shapes;//only used with DIFF
    std::set<Intersection<PATH> > intersections;
    
    std::set<FlagPointer<PATH> > paths;
    ///set by mark_sections()
    int  groupside;
  public:
    SectionGroup(const std::set<FlagPointer<PATH> >& pathset):paths(pathset),
							      groupside(POINTSIDE_ERROR){
      create_sections();
    }

    virtual ~SectionGroup(){}
  protected:
    SectionGroup(const SectionGroup&);
    SectionGroup& operator=(const SectionGroup&);
  public:
    friend std::ostream& operator<< <PATH>(std::ostream& os, const SectionGroup<PATH>& src);

    static  std::deque<std::set<FlagPointer<PATH> > > 
    create_pathsets(const std::deque<FlagPointer<PATH> >& paths);
    /**
@return => whether we only iterate around a single 'crossings' value
on output. PATH_COMBINE_OR only uses sections with a crossings
of 0 - ie the outline, and with AND and NOTAND(ie XOR) we find the sections
that go round shapes with either odd or even crossings values
and there are both recursive and non-recursive variants possible
(and we'll need a travelling salesman routine to find a way to combine them in a way were each section get visited once
*/
    static bool does_loop_crossings_on_output(int flag= PATH_COMBINE_OR);

    static int get_startcrossings(int flag= PATH_COMBINE_OR);
    /**
@arg inter => the intersection to check
@return=> does the intersection contain a left and a right path
*/
      bool is_DIFF_intersection(const intersection_type& inter)const{
       std::deque<PATH*> interpaths=inter.get_paths();
      bool have_left(false), have_right(false);
      for (typename std::deque<PATH*>::const_iterator w= interpaths.begin();
	   w != interpaths.end(); w++){
	if(compare_type(*w) == COMPARE_LEFT){have_left=true;}
	else if(compare_type(*w) == COMPARE_RIGHT){have_right=true;}
	else{throw xn_Error("invalid compare_type flag, should be COMPARE_LEFT or COMPARE_RIGHT");}
      }
      //throw xn_Error("fixme");
      return have_left && have_right;
    }

    std::deque<intersection_type*> get_DIFF_intersections(){
      std::deque<intersection_type*> res;
      ///get all the intersections which have a left and a right path
      for(intersection_iterator w = intersections.begin();w != intersections.end(); w++){
	if(is_DIFF_intersection(*w)){
	  res.push_back(const_cast<intersection_type*>(&(*w)));
	}
      }
      return res;
    }

    bool OR_shapes_contain(const intersection_type& inter)const{
      for(const_shape_iterator w = OR_shapes.begin(); w!= OR_shapes.end();w++){
	if((*w).uses_intersection(inter)){return true;}
      }
      return false;
    }
    void add_OR_shape(const NCOORDS::SectionShape<PATH>&);

    bool cutter_breaks_surface(const intersection_type& interpt)const{
      SectionGroup<PATH>* caller=const_cast<SectionGroup<PATH>*>(this);
      std::deque<Section<PATH>*> lhs_sections = caller->get_intersecting_sections(interpt, COMPARE_LEFT);
      std::deque<Section<PATH>*> rhs_sections = caller->get_intersecting_sections(interpt, COMPARE_RIGHT);
      int lhscount(0), rhscount(0);
      typedef typename std::deque<Section<PATH>*>::const_iterator psection_iterator;
      for(psection_iterator left1 = lhs_sections.begin(); left1 != lhs_sections.end();left1++){
	if((*left1)->crossings == 0){	  lhscount++;}
      }
      for(psection_iterator right1 = rhs_sections.begin(); right1 != rhs_sections.end();right1++){
	if((*right1)->crossings == 0){	  rhscount++;}
      }
      return (lhscount==1) && (rhscount==1);
    }
    /**
does a path self-cross here
     */
    bool is_twist_intersection(const intersection_type& interpt)const{
      std::deque<Section<PATH>*> sectors = const_cast<SectionGroup*>(this)->get_intersecting_sections(interpt, COMPARE_LEFT|COMPARE_RIGHT);
      typedef typename std::deque<Section<PATH>*>::const_iterator psection_iterator;
      int count(0); 
      for(psection_iterator left1 = sectors.begin(); left1 != sectors.end();left1++){
	if((*left1)->crossings == 0){	  count++;}
      }
      return count==4;
    }
    std::pair<section_type*,int> choose_initial_DIFF_section(const intersection_type& inter){
      std::deque<std::pair<Section<PATH>*, int> > lhs_sections = get_intersecting_sections_with_joinend(inter, COMPARE_LEFT);
      if(!(lhs_sections.size())){throw xn_Error("no lhs sections at intersection");}
      typedef typename std::deque<std::pair<Section<PATH>*,int> >::iterator psection_iterator;
       for(psection_iterator lefty = lhs_sections.begin(); lefty != lhs_sections.end();lefty++){
	 if((*lefty).first->crossings==0){return *lefty;}
       }
       throw xn_Error("no lhs sections breaking surface at intersection");
    }

    std::pair<Section<PATH>*,int> choose_rhs_section(Section<PATH>* section, int joinend){
      return choose_section_by_angle_and_compare(section,  joinend,COMPARE_RIGHT);
    }

std::pair<Section<PATH>*,int> choose_lhs_section(Section<PATH>* section, int joinend){
      return choose_section_by_angle_and_compare(section,  joinend,COMPARE_LEFT);
    }

    std::pair<Section<PATH>*,int>
    choose_section_by_angle_and_compare(Section<PATH>* section, int joinend, int compare){
      std::deque<std::pair<Section<PATH>*,int> > candidates = section->get_adjacent_candidates(sections.begin(),
											       sections.end(),
											       joinend);
    Section<PATH>* continuer=section->get_adjacent_section(sections.begin(),sections.end(),
							   joinend|SECTION_CONTINUED);
    int continueflag=(joinend==SECTION_END? SECTION_ABOVE:SECTION_BELOW);
    candidates.push_back(std::pair<Section<PATH>*,int>(continuer,joinend|continueflag));//CHEAT
    bool go_high=true;
    if(((compare==COMPARE_LEFT) &&(groupside==POINTSIDE_LEFT))||
       ((compare==COMPARE_RIGHT) &&(groupside==POINTSIDE_RIGHT))){go_high=false;}
    diff_rank_type sorter( *this,*section, compare,go_high);
    std::sort(candidates.begin(), candidates.end(), sorter);
     std::pair<Section<PATH>*,int>preres =candidates[0];
if(preres.second& SECTION_ABOVE){preres.second=SECTION_END;}
      else{preres.second=SECTION_START;}
      if(!preres.first){throw xn_Error("choose_section_by_angle() found a NULL section");}
      return preres;
    }
      
      void create_DIFF_shapes(){
	using NAMESPACE_APPCOMMON;
      //if too problematic OR left and right first: all intersections will be 'outside'
      std::deque<intersection_type*> diffinters=get_DIFF_intersections();
      std::deque<intersection_type*> outer, inner;
      typedef typename std::deque<intersection_type*>::iterator diff_iterator;
      for(diff_iterator w = diffinters.begin();w!= diffinters.end(); w++){
	if(OR_shapes_contain(*(*w))){  outer.push_back(*w);}
	else{inner.push_back(*w);}
      }
      int /*inside(-groupside),*/ outside(groupside);
     while(outer.size()){
       intersection_type* cur_inter=*(outer.begin());
       remove_from_list(outer,cur_inter);
       std::pair<Section<PATH>*,int> startsectionpair(choose_initial_DIFF_section(*cur_inter));
       int joinend=startsectionpair.second;
       Section<PATH>* startsection=startsectionpair.first;
       Section<PATH>* cursection(NULL);
       std::deque<std::pair<Section<PATH>*,int> > outlines;
       outlines.push_back(startsectionpair);
       while(cursection != startsection){
	 if(!cursection){cursection=startsection;}
	 std::pair<Section<PATH>*,int> nxt(NULL, SECTION_ERROR);
	 if(!cur_inter){
	   if(joinend==SECTION_END){
	     cursection=cursection->get_pathstart_section(sections.begin(),sections.end());
	   }
	   else{cursection=cursection->get_pathend_section(sections.begin(),sections.end());}
	 }
	 else{
	   if(compare_type(*cursection)==COMPARE_LEFT){
	     if(cutter_breaks_surface(*cur_inter)){
	       nxt=choose_rhs_section(cursection,joinend);
	     }
	     else{
	       nxt= cursection->choose_adjacent_section(sections.begin(),sections.end(),joinend,outside);
	     }
	   }
	   else if(cutter_breaks_surface(*cur_inter)){
	     nxt=choose_lhs_section(cursection, joinend);
	   }
	   else{ nxt=choose_rhs_section(cursection,joinend);}
	   joinend=nxt.second;
	   cursection= nxt.first;
	 }
	 outlines.push_back(std::pair<section_type*, int>(cursection, joinend));
	 cur_inter= cursection->get_intersection(joinend);
	 remove_from_list(outer,cur_inter);
       }
       SectionShape<PATH> shape(outlines);
       DIFF_shapes.push_back(shape);
     }
      }

    static void add_to_setcollection(const FlagPointer<PATH>& path,
				     std::deque<std::set<FlagPointer<PATH> > >& outcollection);
    /**
@return =>get the shapes from which the output path will be generated
@arg combine_flag => one of PATH_COMBINE_OR, PATH_COMBINE_DIFF, PATH_COMBINE_AND or PATH_COMBINE_NOTAND
@arg recursive => whether to operate recursively:\n ie are we only looking for a single 'crossings' value or crossval, crossval+2...crossval+2n
     */
    std::deque<SectionShape<PATH>* > get_output_shapes(int combine_flag, 
						      bool recursive=false);

    std::deque<SectionShape<PATH>* > generate_output_shapes(int combine_flag, bool recursive);

    ///wrong - fix in light of get_output_shapes, with new signature
    std::pair<Section<PATH>*, int> choose_output_section(Section<PATH>& starter, int joinend,
							 int outside, int combine_flag){
      int numcross=starter.crossings;
      //version of this to take flag not numcross
      if(combine_flag==PATH_COMBINE_OR){
	std::deque<std::pair<Section<PATH>*, int> > possibles=starter.get_closest_adjoining_sections_with_joinend(sections.begin(), sections.end(),  joinend,0 );
	if(!possibles.size()){throw xn_Error("get_closest_adjoining_sections_with_joinend return empty container");}
	return starter.choose_unused_section(possibles.begin(), possibles.end(), joinend, -outside );
      }
      else{throw xn_Error("do other flag values, finish SectionShape");}
    }

    ///max value of crossings among sections currently
    int max_crossings()const;
    std::deque<Section<PATH>*> sections_needing_shapes(int crosscnt);
    bool need_shapes()const;
    void add_shape(const SectionShape<PATH>& shape){
      if((!has_shape(shape))&&
	 (!has_OR_shape(shape))){shapes.push_back(shape);}
    }

bool has_OR_shape(const SectionShape<PATH>& shape)const{
  if(OR_shapes.size()<=1){return false;}
      for (const_shape_iterator w = OR_shapes.begin()+1; w!= OR_shapes.end();w++){
	if (shape == *w){return true;} 
      }
      return false;
    }

    bool has_shape(const SectionShape<PATH>& shape)const{
      for (const_shape_iterator w = shapes.begin(); w!= shapes.end();w++){
	if (shape == *w){return true;} 
      }
      return false;
    }
    /**
creates the internal shapes, which determines
how many lines to cross to get out of each
     */
    void create_shapes();
    void check_shapes()const;
  
    /**
@arg crossingcnt => if (crossincnt <0), return all 'unused' sections
else return those where(crossingcnt == sector.crossings)
@return => a deque<Section*> of sections for which(used_for_output()==false)
     */
    std::deque<Section<PATH>*> get_unused_sections(int crossingcnt =-1);

   
    
    /**
       find all sections adjoining markfrom, and for each, if unmarked, set crossings  to markfrom.crossings+1
     */
    void mark_adjacent_sections(Section<PATH>* markfrom);

    /**
       is sect from the left or right parameter passed into the toplevel calling function(only of importance when the output flag is PATH_COMBINE_DIFF - where a path is subtracted from another)
    */
    int compare_type(const Section<PATH>& sect)const;

    int compare_type(const PATH* p)const;
    ///misnamed - find_intersections() better
    void get_intersections();
  protected:
 
    void add_intersection(const Intersection<PATH>& inter);

    Section<PATH>* find_first_output_section(      int flag= PATH_COMBINE_OR);

    /**
       create the sections for PATH pat.
       WARNING: internal use
       @arg pat the path
       @arg pathinters a subset of intersections with this pat involved
       got using pat->get_own_intersections()
       @arg offsets the offsets into pat from that subset
    */

    void add_path_sections(PATH* pat, 
			   const std::deque<Intersection<PATH>*>& pathinters,
			   const std::deque<unsigned int>& offsets);

    static std::deque<PATH*> flagpointers_get_paths(const std::deque<FlagPointer<PATH> >& flaggedpaths);

    static std::deque<PATH*> flagpointers_get_paths(const std::set<FlagPointer<PATH> >& flaggedpaths);

    bool has_unmarked_sections()const{
      return Section<PATH>::has_unmarked_sections(sections);
    }

    std::deque<Section<PATH>*> get_marked_sections(int markwanted=0){
      std::deque<Section<PATH>*> res;
      for(section_iterator w=sections.begin();w!= sections.end(); w++){
	if((*w).crossings==markwanted){
	  res.push_back(&(*w));
	}
      }
      return res;
    }

  bool external_section(const section_type& sector)const{
    if(!(OR_shapes.size())){throw xn_Error("mark_sections() first");}
    return OR_shapes[0].uses_outline(sector);
  }

bool hole_section(const section_type& sector)const{
  if(sector.crossings !=0){return false;}
  return !external_section(sector);
}

std::deque<const Section<PATH>*> get_marked_sections(int markwanted=0)const{
  std::deque<const Section<PATH>*> res;
  for(const_section_iterator w=sections.begin();w!= sections.end(); w++){
    if((*w).crossings==markwanted){
      res.push_back(&(*w));
    }
  }
  return res;
}
    /**
       set crossings for all the sections that surround 
       a contiguous undivided area
       error if any are found to have smaller positive crossings than markfrom
       call repeatedly from create_shapes()
    */
    
    void mark_shapelet(const Section<PATH>& markfrom);

    std::deque<PATH*> get_paths()const{
      return flagpointers_get_paths(paths);
    }

    //std::pair<Section<PATH>*, bool> 
    //find_next_output_section( Section<PATH>& sectstarter,
    //			      int flag,
    //			  bool previous_forwards);
    /**
       @fn add_to_path(simple_click_history& outpath,
       int flag,
       int whichside_outside)
       iterate through the sections to create a path to match the flag
       (PATH_COMBINE_*  ie OR, AND, NOTAND and DIFF
       @arg outpath path to write output to
       @arg flag => PATH_COMBINE_*  ie OR, AND, NOTAND and DIFF
       @arg whichside_outside => result of mark sections()
    */
    int add_to_path( simple_click_history& outpath,
		     int flag,
		     int whichside_outside);
    /**
@return => number of sections without crossings marked
     */
    size_type unmarked_section_count()const{
      size_type count(0);
      for (const_section_iterator w = sections.begin();
	   w!= sections.end();w++){
	if(!(*w).marked()){count++;}
      }
      return count;
    }
    /**
@return => get the sections ending at inter which are on the side of the operator as indicated by cmp_lhsrhs
@arg inter => the intersection searched for
@arg cmp_lhsrhs => COMPARE_LEFT || COMPARE_RIGHT || (COMPARE_LEFT | COMPARE_RIGHT)
     */
    std::deque<Section<PATH>*> get_intersecting_sections(const intersection_type& inter,
						  int cmp_lhsrhs){
      if(!(&inter)){throw xn_Error("intersection must not be NULL");}
      if(!((cmp_lhsrhs&(COMPARE_LEFT | COMPARE_RIGHT)))){
	throw xn_Error("invalid flag: COMPARE_LEFT | COMPARE_RIGHT only");
      }
      std::deque<Section<PATH>*>  result;
      bool use_all= (cmp_lhsrhs == (COMPARE_LEFT | COMPARE_RIGHT));
      for(section_iterator first = sections.begin();first != sections.end();first++){
	if(((*first).uses_intersection(inter)) &&
	   ((use_all || (compare_type((*first).get_path()) & cmp_lhsrhs)))){
	  result.push_back(&(*first));
	}
      }
      return result;
    }
    std::deque<std::pair<Section<PATH>*,int> > get_intersecting_sections_with_joinend(const intersection_type& inter,
						  int cmp_lhsrhs){
      if(!(&inter)){throw xn_Error("intersection must not be NULL");}
      if(!((cmp_lhsrhs&(COMPARE_LEFT | COMPARE_RIGHT)))){
	throw xn_Error("invalid flag: COMPARE_LEFT | COMPARE_RIGHT only");
      }
      std::deque<std::pair<Section<PATH>*, int> >  result;
      bool use_all= (cmp_lhsrhs == (COMPARE_LEFT | COMPARE_RIGHT));
      for(section_iterator first = sections.begin();first != sections.end();first++){
	if(((*first).uses_intersection(inter)) &&
	   ((use_all || (compare_type((*first).get_path()) & cmp_lhsrhs)))){
	  result.push_back(std::pair<section_type*,int>(&(*first),(*first).get_joinend(inter)) );
	}
      }
      return result;
    }

    bool section_exhausted_for_shapes(const Section<PATH>& section, 
				 int numcross)const{
      int count(0);
      for(const_shape_iterator w= shapes.begin();w!= shapes.end();w++){
	if((*w).uses_outline(section)){
	  count++;
	  if((!numcross) ||(count==2)){return true;}
	}
      }
      return false;
    }

    size_type shapes_to_get(const Section<PATH>& section, 
				 int numcross=-1)const{
      if(numcross <0){numcross=section.crossings;}
      if(numcross <0){throw xn_Error("can't get numcross : -1");}
      size_type required=2;
      if(!numcross){
	// && contains_pair_first(OR_shapes[0].begin(), OR_shapes[0].end(), &section)){
	required=1;
      }
      for(const_shape_iterator w= shapes.begin();w!= shapes.end();w++){
	if((*w).uses_outline(section)){
	  if(required){required--;}
	  else{throw xn_Error("excess shapes for Section");}
	}
      }
    }
    std::pair<section_iterator, int> find_outside_section();
  std::deque<std::pair<Section<PATH>*, int> > collect_outside_sections();
    /**
for debug purposes, and perhaps not totally reliable
     */
    std::deque<Section<PATH>*> collect_inside_sections();
  public:
    int mark_sections();

    void create_sections();
    /*
      static std::set<SectionGroup> create(FWDITER first, FWDITER last){
      std::set<SectionGroup> result;

      return result;
      }
    */
    static std::deque<SectionGroup*> create(const std::deque<FlagPointer<PATH> >& paths);

    static std::deque<FlagPointer<PATH> > create_flagged_group(std::deque<PATH>& pathsorig, int paramsyntaxflag);
    
    static std::deque<FlagPointer<PATH> > create_flagged_group(const std::deque<PATH*>& pathsorig, int paramsyntaxflag);
    /**
       @WARNING: paths in pathleft and pathsright must all directly or indirectly intersect(TODO: or either contain one or more of the others, or be contained by one or more of the others). behaviour undfined otherwise
       @todo: handle curious cases, like the path that is completely outside all the other sources, but is contained by their ORed combination: 2 passes? better error handling
       @arg pathsleft => paths on the left side of the invoking operator or method
       @arg pathsright => paths on the right side of the invoking operator or method
       @arg flag => PATH_COMBINE_OR, PATH_COMBINE_AND, PATH_COMBINE_NOTAND, or PATH_COMBINE_DIFF
       @return => the resulting path(simple_click_history)
    */

    static std::deque<FlagPointer<PATH> > 
    create_path_container(const std::deque<PATH*>& pathsleft,
			  const std::deque<PATH*>& pathsright);


    static simple_click_history combine_paths(const PATH& left, const PATH& right, int flag){
      std::deque<PATH*> leftp, rightp;
      leftp.push_back(const_cast<PATH*>(&left));
      rightp.push_back(const_cast<PATH*>(&right));
      return combine_paths(leftp, rightp, flag);
    }
    static simple_click_history combine_paths(const std::deque<PATH*>& pathsleft,
					      const std::deque<PATH*>& pathsright,
					      int flag);

  };


}//namespace NCOORDS

#endif
//template implementations
#include "pathsection.tcc"
