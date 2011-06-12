#include "subpath.h"


namespace NCOORDS{

unsigned int PathFragment::self_intersections()const{
    if(size()<4){return 0;}
    unsigned int res(0);
    const_iterator w = begin();
    while (w != end()  &&(!((*w).second & MOVE_CLICK))){w++;}
    w++;
    bool pclosed=false;
    for(;w!=end();w++){
      const coords& prev= (*(w-1)).first;
      const coords* thisn(NULL);
      while(w != end() && !(drawing_click(*w) 
			    || (*w).second & CLOSE_PATH)){w++;}
      if((*w).second & CLOSE_PATH){
	const_iterator p= begin();
	while (p != end()  &&(!((*p).second & MOVE_CLICK))){p++;}
	if(p != end()){
	  thisn=&((*p).first);
	  pclosed=true;
	}
	else{throw xn_Error("not enough points");}
      }
      else{
	thisn=&((*w).first);
      }
      Segment<const coords*> seg(&prev, thisn);
	
      res+=segment_intersections(seg);
	
    }
    if(!pclosed){
      const std::pair<coords,int>& last = *(w-1);
      if(last.second& LINE_CLICK){
	const coords& prev=last.first;
	const coords* thisn=NULL;
	w=begin();
	while (w!= end()  &&(!((*w).second & MOVE_CLICK))){w++;}
	if(w != end()){
	  thisn=&((*w).first);
	    
	}
	else{throw xn_Error("not enough points");}
	Segment<const coords*> seglast(&prev, thisn);      
	res += segment_intersections(seglast);
      }
    }
    return res;
  }

unsigned int PathFragment::segment_intersections(Segment<const coords*> seg)const{
    if(size()<2){return 0;}
    unsigned int res(0);
    const_iterator w = begin();
    while (w != end()  &&(!((*w).second & MOVE_CLICK))){w++;}
    w++;
    bool pclosed=false;
    for(;w!=end();w++){
      const coords& prev= (*(w-1)).first;
      const coords* thisn(NULL);
      while(w != end() && !(drawing_click(*w) 
			    || (*w).second & CLOSE_PATH)){w++;}
      if((*w).second & CLOSE_PATH){
	const_iterator p= begin();
	while (p != end()  &&(!((*p).second & MOVE_CLICK))){p++;}
	if(p != end()){
	  thisn=&((*p).first);
	  pclosed=true;
	}
	else{throw xn_Error("not enough points");}
      }
      else{
	thisn=&((*w).first);
      }
      Segment<const coords*> segmine(&prev, thisn);
      //same and adjoining segments will always intersect
      if(!((&prev == seg.first)||(thisn == seg.second)
	   ||(thisn ==seg.first) || (&prev == seg.second))){

	if(segmine.lines_cross(seg)){
	  res++;
	}
      }
    }
    if(!pclosed){
      const std::pair<coords,int>& last = *(w-1);
      if(last.second& LINE_CLICK){
	const coords& prev=last.first;
	const coords* thisn=NULL;
	w=begin();
	while (w!= end()  &&(!((*w).second & MOVE_CLICK))){w++;}
	if(w != end()){
	  thisn=&((*w).first);
	    
	}
	else{throw xn_Error("not enough points");}
	Segment<const coords*> seglast(&prev, thisn);  
	//same and adjoining segments will always intersect
	if(!((&prev == seg.first)||(thisn == seg.second)
	     ||(thisn ==seg.first) || (&prev == seg.second))){
	  if(seglast.lines_cross(seg)){
	    res++;
	  }
	}
	  
      }
    }
    return res;
  }

  simple_click_history::iterator PathFragment::begin(){
    throw_if_invalid();
    iterator out(path->begin());
    return out + first;
  }

  simple_click_history::const_iterator PathFragment::begin()const{
    throw_if_invalid();
    return path->begin() + first;
  }

  simple_click_history::const_reverse_iterator PathFragment::rbegin()const{
    throw_if_invalid();
    return const_reverse_iterator(end());
  }

  simple_click_history::reverse_iterator PathFragment::rbegin(){
    throw_if_invalid();
    return reverse_iterator(end());
  }

  simple_click_history::iterator PathFragment::end(){
    ///@todo : move throw_if_invalid checks inside
    ///#ifdef SOMETHING_DEBUG
    /// ONCE WE ARE CONFIDENT IN THIS, AND WRINKLES removed - botox
    ///main problem seemed to be last was one less than required 
    ///so that size() was returneing REALSZ -1,
    ///so a zero length PathFragment returned size_t(-1)
    /// and end() and rbegin() were similarly affected
    throw_if_invalid();
    return path->begin() + last;
  }

  simple_click_history::const_iterator PathFragment::end()const{
    throw_if_invalid();
    return path->begin() + last;
  }

  simple_click_history::reverse_iterator PathFragment::rend(){
    throw_if_invalid();
    return reverse_iterator(begin());
  }

  simple_click_history::const_reverse_iterator PathFragment::rend()const{
    throw_if_invalid();
    return const_reverse_iterator(begin());
  }

bool PathFragment::true_subpath()const{
    if(!size()){
      std::cerr<<"PathFragment::true_subpath(): failure => size()==0"
	       <<std::endl;
      return false;}
    const_iterator w= begin();
    if(!((*w).second & MOVE_CLICK)){
      std::cerr<<"PathFragment::true_subpath(): failure => doesn't start with MOVE_CLICK, but "<< clicktype_as_string((*w).second) <<std::endl;
return false;}
    w++;
    for(; w!= end(); w++){
      if(move_click(*w)){
  std::cerr<<"PathFragment::true_subpath(): failure => 2nd MOVE_CLICK IN subPATH\nlocation index: "<< w-begin()<<"\nsubpath size: "<<size()<<std::endl;
return false;}
    }
    if((path->end()- end()) >0){
      w= end();
      if(!move_click(*w)){
std::cerr<<"PathFragment::true_subpath(): failure => not followed by next MOVE_CLICK, BUT "<< clicktype_as_string((*w).second)<<std::endl;
//return false;
}
    }
    return true;
  }


void PathFragment::get_extents(cRect& outval)const{
    const_iterator w= begin();
    while(w!= end() && !drawing_click(*w) ){w++;}
    if(w==end()){
      std::cerr << "empty subpath: get_extents() returning {0,0,0,0}\n//Logic Error:should have a internal value to indicate no extent or location whatever"<< std::endl;
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

std::pair<int, int> PathFragment::get_intersection_count(const Linear_function<double>& pathfn, 
							   const coords& centre,
							   bool implicitlyclosed)const{
    std::deque<coords> hits;
    simple_click_history subpath=as_path();
    subpath = subpath.flatten_copy();
    return subpath.get_intersection_count(pathfn,centre,implicitlyclosed);
  }

  simple_click_history PathFragment::as_path()const{
    path_type result;
    for(const_iterator w= begin(); w!= end();w++){
      result.push_back(*w);
    }
    return result;
  }

bool PathFragment::valid_range()const{
    if(first >= last){return false;}
    const_iterator no1 = path->begin() + first;
    const_iterator no2 = path->begin() + last;
    return path->valid_iterator(no1) &&path->valid_iterator(no2);
  }

 void PathFragment::throw_if_invalid()const{
    if(!valid_range()){
      std::ostringstream ostmp;
	ostmp<<"SubPath: range_invalid: first ="<<first <<"\nlast = "<<last
	     <<"\npath size = "<<path->size();
	throw xn_Error(ostmp.str());
    }
  }
  /**
@todo write throw_if_invalid() checks into operator=() and copy constructor
if we really need
Occasional path blows up with invalid range later, making it hard to determine the bad value was originally created
   */
SubPath::SubPath(simple_click_history* p, iterator start, iterator finish):
    PathFragment(p,start, finish){    
 #if CHECK_SUBPATH_SECTION
  std::cerr<< "Subpath(): printing point types:\n";
  for(iterator checkstart=begin();checkstart != end();checkstart++){
    std::cerr<<clicktype_as_string((*checkstart).second)<<std::endl;
  }
  std::cerr<<"\t(((((((yyyyy)))))))))))))"<<std::endl;
#endif
    if(!true_subpath()){
      throw xn_Error("invalidSubPath");
    }
#if CHECK_SUBPATH_SECTION
    throw_if_invalid();
#endif
  }

 void SubPath::get_intersections(intersection_result_type& outcontainer, SubPath& other, bool alwaysclosed){
    cRect box, otherbox;
    get_extents(box);
    other.get_extents(otherbox);
    if(!box.overlap(otherbox)){return;}
    iterator w = begin();
    if((*w).second & (BEZIER_CLICK|BEZIER_HANDLE)){
      throw xn_Error("flatten path b4 get_intersections()");
    }
    iterator guard=w;
    if(!alwaysclosed){guard=end();}
    bool started=false;
    while (w != guard || !started){
      started=true;
      std::pair<Segment<coords*>,iterator> wpair=get_segment_and_iter(w,alwaysclosed);
      if((*(wpair.second)).second & CLOSE_PATH){guard=begin();}
      else if((*(wpair.second)).second & (BEZIER_CLICK|BEZIER_HANDLE)){
	throw xn_Error("flatten path b4 get_intersections()");
      }
      other.get_intersections(outcontainer, this,wpair.first, w, wpair.second, alwaysclosed);
      w=wpair.second;
    }
#if CHECK_SUBPATH_SECTION
    std::cerr<<"Outcontainer.size(): "<<outcontainer.size()<<std::endl;
    if(alwaysclosed &&(&other != this)&&(outcontainer.size()%2)){
      throw xn_Error("odd no of intersections from a closed path");
    }
#endif
 }

  /*  while(othercount < othersz && (! (*(other.begin()+othercount)).second &(LINE_CLICK|MOVE_CLICK))){
      othercount++;
    }
    for(;othercount < othersz;othercount++){
      int flag=(*(other.begin()+othercount)).second;
      if(flag &(LINE_CLICK|MOVE_CLICK)){
	get_intersections(outcontainer, other, othercount);
      }
      else if(flag & (BEZIER_CLICK | BEZIER_HANDLE)){
	throw xn_Error("get_intersections(): flatten() path first");
      }
    }
  }
  */

  int SubPath::combine(SubPath& other, simple_click_history& outpath,
		       int flag){
    outpath = SectionGroup<SubPath>::combine_paths(*this, other, flag);
    //should count how many subpaths here
    //better, change SectionGroup<SubPath>::combine_paths signature
    //to return the number of output subpaths
    int result=0;
  return result;
  }

  std::deque<Intersection<SubPath>*> 
  SubPath::get_own_intersections(const std::set<Intersection<SubPath> >& intersectionlist)const{
      std::deque<Intersection<SubPath>*> result;
      typedef std::set<Intersection<SubPath> >::const_iterator inter_iterator;
      for(inter_iterator w=intersectionlist.begin();
	  w!=intersectionlist.end(); w++){
	if((*w).uses_path(*this)){
	  result.push_back(const_cast<Intersection<SubPath>*>(&(*w)));
	}
      }
      return result;
    }

   std::deque<unsigned int> SubPath::get_own_intersection_offsets(const std::deque<Intersection<SubPath>*>& intersectionlist)const{
      std::deque<unsigned int> result;
      typedef std::deque<Intersection<SubPath>*>::const_iterator inter_iterator;
      for(inter_iterator w=intersectionlist.begin();
	  w!=intersectionlist.end(); w++){
	if((*w)->get_first_path() ==this){
	  result.push_back((*w)->get_first_offset());
	}
	if((*w)->get_second_path() ==this){
	  result.push_back((*w)->get_second_offset());
	}
      }
      //order from lowest to highest
      //may be multiple instances of same value
      std::sort(result.begin(), result.end());
      return result;
    }
 
void SubPath::get_self_intersections(std::deque<std::pair<std::pair<SubPath*, unsigned int> ,std::pair<SubPath*, unsigned int> > >& outcontainer){
    if(size()>=4){
      get_intersections(outcontainer, *this);
    }
    return;
  }

  void SubPath::get_intersections(SubPath::intersection_result_type& outcontainer,
				  SubPath* src,
				  Segment<coords*>& seg, iterator b4, 
				  iterator after, bool alwaysclosed){
    std::pair<SubPath*, unsigned int> srcinfo(src, b4 - src->begin());
    iterator w = begin();
    iterator guard=w;
    if(!alwaysclosed){guard=end();}
    bool started=false;
#if CHECK_SUBPATH_SECTION
    int counter=0;
#endif
    while (w != guard || !started){
      started=true;
      if((*w).second & (BEZIER_CLICK|BEZIER_HANDLE)){
	throw xn_Error("flatten path b4 get_intersections()");
      }
      std::pair<Segment<coords*>,iterator> wpair=get_segment_and_iter(w,alwaysclosed);
#if CHECK_SUBPATH_SECTION
#if XNDRAW_VERBOSE
      if((counter % 200) ==99){
		std::cerr<<"comparing segments\nours: "<<wpair.first<<"\ntheirs: "<< seg<<std::endl;
      }
      counter++;
#endif
#endif
      if((w==b4)||(w== after) || (wpair.second==b4)||(wpair.second== after)){
	w=wpair.second;continue;
      }
      if((*(wpair.second)).second & CLOSE_PATH){guard=begin();}
      else if((*(wpair.second)).second & (BEZIER_CLICK|BEZIER_HANDLE)){
	throw xn_Error("flatten path b4 get_intersections()");
      }
      if(wpair.first.lines_cross(seg)){
#if CHECK_SUBPATH_SECTION
	std::cerr<<"lines_cross! ours:\n"<<wpair.first<<"\nthe other seg:\n"<<seg<<std::endl;
#endif
	std::pair<SubPath*, unsigned int> mine(this, w-begin());
	std::pair<std::pair<SubPath*, unsigned int>,std::pair<SubPath*, unsigned int> > both(mine, srcinfo);
	outcontainer.push_back(both);
      }
      if(w==wpair.second){
	throw xn_Error("iterator not advancing");
      }
      w=wpair.second;
    }
  }
  /**
@todo back to front aswell as broken
iterate through the two paths with get_segment_and_iter()
   */
void SubPath::get_intersections(SubPath::intersection_result_type& outcontainer,
				  SubPath& other, unsigned int otherindex){
    //TODO: make sure we aren't checking any point against itself
    std::pair<SubPath*, unsigned int> otherres(&other, otherindex);
    unsigned int sz(size()-1),count(0);
    unsigned int othercount= otherindex;
    // throw xn_Error("FIXME: incomplete");
    std::pair<coords,int>& otherpr = *(other.begin() + otherindex);
    if(!(otherpr.second & (MOVE_CLICK|LINE_CLICK))){
      throw xn_Error("bad index");
    }
    while(othercount < other.size()){
      if(!((*(other.begin() + othercount)).second & LINE_CLICK)){
	othercount++;
      }
      else{
	break;
      }
    }
    if(!((*(other.begin() + othercount)).second & LINE_CLICK)){
      throw xn_Error("can't find second point");
    }
    std::pair<coords,int>& othernext=*(other.begin() + othercount);
    Segment<coords*> segother(&(otherpr.first), &(othernext.first));
    //const_iterator w = begin();
    while (count < sz){
      if(!((*(begin() + count)).second & MOVE_CLICK)){count++;}
      else{break;}
    }
    unsigned int index= count;
    bool pclosed=false;
    while(count <sz){
      coords& prev= (*(begin()+count)).first;
      coords* thisn(NULL);
      while(count <sz && !((drawing_click(*(begin() + count) ))
			   || (*(begin() + count)).second & CLOSE_PATH)){
	count++;
      }
      if((*(begin() + count)).second & CLOSE_PATH){
	iterator p= begin();
	while (p != end()  &&(!((*p).second & MOVE_CLICK))){p++;}
	if(p != end()){
	  thisn=&((*p).first);
	  pclosed=true;
	}
	else{throw xn_Error("not enough points");}
      }
      else{
	thisn=&((*(begin()+ count)).first);
      }
      Segment<coords*> seg(&prev, thisn);
      if(!((segother.first == seg.first)||(segother.first == seg.second)
	   ||(segother.second == seg.first)|| (segother.second == seg.second))){
	//Segment<coords> tmpseg(prev , * thisn);
	if(segother.lines_cross(seg)){
	  std::pair<SubPath*, unsigned int> mine(this, index);
	  std::pair<std::pair<SubPath*, unsigned int>,std::pair<SubPath*, unsigned int> > both(mine, otherres);
	  outcontainer.push_back(both);
	}
      }
    }
    if(!pclosed){
      std::pair<coords,int>& last = *(begin() +count);
      if(last.second& LINE_CLICK){
	coords& prev=last.first;
	coords* thisn=NULL;
	iterator w=begin();
	while (w!= end()  &&(!((*w).second & MOVE_CLICK))){w++;}
	if(w != end()){
	  thisn=&((*w).first);
	}
	else{throw xn_Error("not enough points");}
	Segment<coords*> seglast(&prev, thisn); 
	if(!((segother.first == seglast.first)||(segother.first == seglast.second)
	     ||(segother.second == seglast.first)|| (segother.second == seglast.second))){
	  //Segment<coords> tmpseg(prev , * thisn);
	  if(segother.lines_cross(seglast)){
	    std::pair<SubPath*, unsigned int > res(this, count);
	    std::pair<std::pair<SubPath*, unsigned int >, std::pair<SubPath*, unsigned int > > both(res, otherres);
	    outcontainer.push_back(both);
	  }	 
	}
      }
    }  
  }

  /*Segment<coords*> SubPath::get_segment(iterator w,
					bool alwaysclosed){
std::pair<Segment<coords*>, iterator> res= get_segment_and_iter(w, alwaysclosed);
    return res.first;
    }

  const Segment<coords*> SubPath::get_segment(const_iterator w,
					      bool alwaysclosed)const {
    std::pair<const Segment<coords*>, const_iterator> res= get_segment_and_iter(w, alwaysclosed);
    return res.first;
  }
  */
  std::ostream& test_segment_and_iter(std::ostream& os, const Section< SubPath>& p){
    typedef SubPath::const_iterator const_iterator;
    typedef SubPath::iterator iterator;
    os<<"+++++++++++++\ntest_section_segment_and_iter(sectionijoi)\npoints from begin() to end";
    for(const_iterator q=p.begin();q!=p.end();q++){
      os <<(*q).first<<std::endl;
    }
    ssize_t w(0), guard(SEG_LAST_RETURN);
    if(p.has_start_intersection()){w=-1;}
    os <<"const seg and iter:\n";
    while(w!=guard){
      std::pair<const Segment<coords*>, ssize_t> constpr = p.get_segment_and_iter(w, true);
      w=constpr.second;
      os << constpr.first<<std::endl;
    }
    os <<"nonconst version:\n";
    Section< SubPath>& notconst= const_cast<Section<SubPath>&>(p);
  w=0;
  if(p.has_start_intersection()){w=-1;}
  while(w!=guard){
    std::pair<Segment<coords*>, ssize_t> pr = notconst.get_segment_and_iter(w, true);
    w=pr.second;
    os << pr.first<<std::endl;
  }
  return os;
}

  std::ostream& test_segment_and_iter(std::ostream& os, const SubPath& p){
    //this seems to work
    typedef SubPath::const_iterator const_iterator;
    typedef SubPath::iterator iterator;

    os<<"+++++++++++++\ntest_segment_and_iter(subpath)\npoints from begin() to end";
    for(const_iterator q=p.begin();q!=p.end();q++){
      os <<(*q).first<<std::endl;
    }
    os <<"const seg and iter:\n";
    const_iterator w(p.begin()), guard(p.begin());
    bool started=false;
    while((w!=guard) ||!started){
      started=true;
      std::pair<const Segment<coords*>, const_iterator> constpr = p.get_segment_and_iter(w, true);
      w=constpr.second;
      os << constpr.first<<std::endl;
    }
    os <<"nonconst version:\n";
    SubPath& notconst= const_cast<SubPath&>(p);
    iterator wnot(notconst.begin()), notguard(notconst.begin());
    started = false;
    while((wnot!=notguard) ||!started){
      started=true;
      std::pair<Segment<coords*>, iterator> pr = notconst.get_segment_and_iter(wnot, true);
      wnot=pr.second;
      os << pr.first<<std::endl;
    }
    return os<<"*****end test ***********"<<std::endl;
  }
  std::pair<Segment<coords*>, SubPath::iterator> 
  SubPath::get_segment_and_iter(SubPath::iterator w,
				bool alwaysclosed){
    iterator guard=w;
    std::pair<coords, int>& thisn= *w;
    w++;
    if(!(thisn.second & (MOVE_CLICK|LINE_CLICK))){
      throw xn_Error("wrong click: must be MOVE_CLICK|LINE_CLICK");
    }
    double minrad= coords::min_radius();
    coords* firstpt=&(thisn.first);
    if((w!=end()) &&((!((*w).second &(MOVE_CLICK|LINE_CLICK|CLOSE_PATH))))){
      throw xn_Error("bad click in flat subpath");
    }
    for(;(w!=end()) &&((*w).second &(MOVE_CLICK|LINE_CLICK))&& 
	  (thisn.first.get_distance(&((*w).first)) <= minrad);w++){
      if(w==guard){throw xn_Error("proceeded around loop?");}
    }
    if((*w).second &CLOSE_PATH){w=begin();}
    else if(w==end()){
      if(alwaysclosed){w=begin();}
      else{throw xn_Error("past end of segments");}
    }
    while(((*w).second &(MOVE_CLICK|LINE_CLICK)) &&
	  (firstpt->get_distance(&((*w).first))<=minrad)){
      if(w==guard){throw xn_Error("proceeded around loop?");}
      w++;
    }
    if((alwaysclosed &&((*w).second &(MOVE_CLICK|LINE_CLICK)) && (w==end()-1) && 
	((*w).first.get_distance(&((*(begin())).first))<= minrad)) ||
       ((w==end()-2) && ((*(w+1)).second & CLOSE_PATH) &&
	((*w).first.get_distance(&((*(begin())).first))<= minrad))){w=begin();}
    coords* nextn= &((*w).first);
    return std::pair<Segment<coords*>, iterator>(Segment<coords*>(firstpt, nextn),w);
  }


std::deque<std::pair<coords, Segment<coords*> > >
SubPath::find_intersections_with_segment(const Linear_function<double>& fn,
					 bool closed_implicitly)const{
  typedef std::pair<coords, Segment<coords*> > value_type;
  typedef std::deque<value_type> result_type;
  result_type result;
  return find_intersections_with_segment(fn, result, closed_implicitly);
}

std::deque<std::pair<coords, Segment<coords*> > >&
SubPath::find_intersections_with_segment(const Linear_function<double>& fn,
					 std::deque<std::pair<coords, Segment<coords*> > >& result,
					 bool closed_implicitly)const{
typedef std::pair<coords, Segment<coords*> > value_type;
  if(size()<2){return result;}
  const_iterator w=begin();
  const_iterator guard=w;
  if(!closed_implicitly){guard=end();}
  bool started=false;
  while (w != guard || !started){
    started=true;
    SubPath& nonconst = const_cast<SubPath&>(*this);
    ///@todo fix so compiler not finding non-const version of get_segment_and_iter()
    std::pair<const Segment<coords*>,const_iterator> wpair=nonconst.get_segment_and_iter(w,closed_implicitly);
    if((*(wpair.second)).second & CLOSE_PATH){guard=begin();}
    else if((*(wpair.second)).second & (BEZIER_CLICK|BEZIER_HANDLE)){
      throw xn_Error("flatten path b4 get_intersections()");
    }
    if(wpair.first.function_intersects(fn)){
      result.push_back(value_type(wpair.first.get_intersection(fn),wpair.first));
    }
    w=wpair.second;
  }
#if CHECK_SUBPATH_SECTION
  if(closed_implicitly && (result.size()%2)){
    std::cerr<<"size of intersection collection: "<<result.size()<<"\n should be an even number or 0 where path is closed"<<std::endl;
    throw xn_Error("odd count of intersections in closed path!");
  }
#endif
  return result;
}

 std::pair<const Segment<coords*>, SubPath::const_iterator> 
 SubPath::get_segment_and_iter(SubPath::const_iterator w,
			       bool alwaysclosed)const{
    const_iterator guard=w;
    const std::pair<coords, int>& thisn= *w;
    w++;
    if(!(thisn.second & (MOVE_CLICK|LINE_CLICK))){
      throw xn_Error("wrong click: must be MOVE_CLICK|LINE_CLICK");
    }
    double minrad=coords::min_radius();
    const coords* firstpt=&(thisn.first);
    if((w!=end()) &&((!((*w).second &(MOVE_CLICK|LINE_CLICK|CLOSE_PATH))))){
      throw xn_Error("bad click in flat subpath");
    }
    for(;(w!=end()) &&((*w).second &(MOVE_CLICK|LINE_CLICK))&& 
	  (thisn.first.get_distance(&((*w).first)) <= minrad);w++){
      if(w==guard){throw xn_Error("proceeded around loop?");}
    }
    if((*w).second &CLOSE_PATH){w=begin();}
    else if(w==end()){
      if(alwaysclosed){w=begin();}
      else{throw xn_Error("past end of segments");}
    }
    while((!((*w).second &(MOVE_CLICK|LINE_CLICK))) ||
	  (firstpt->get_distance(&((*w).first))<= minrad)){
      if(w==guard){throw xn_Error("proceeded around loop?");}
      w++;
    }
    if((alwaysclosed &&((*w).second &(MOVE_CLICK|LINE_CLICK)) && (w==end()-1) && 
	((*w).first.get_distance(&((*(begin())).first))<= minrad)) ||
       ((w==end()-2) && ((*(w+1)).second & CLOSE_PATH) &&
	((*w).first.get_distance(&((*(begin())).first))<= minrad))){w=begin();}
    const coords* nextn= &((*w).first);
    const Segment<coords*> result(const_cast<coords*>(firstpt), const_cast<coords*>(nextn));
    return std::pair<const Segment<coords*>, const_iterator>(result,w);
 }

  int SubPath::is_overlapping(const SubPath& other, bool alwaysclosed)const{
    cRect box, obox;
    get_extents(box);
    other.get_extents(obox);
    if(!box.overlap(obox)){return PATH_OVERLAP_NONE;}
    ////////////////////
    const_iterator w = begin();
    const_iterator guard=w;
    if(!alwaysclosed){guard=end();}
    bool started=false;
    while (w != guard || !started){
      started=true;
      if((*w).second & (BEZIER_CLICK|BEZIER_HANDLE)){
	throw xn_Error("flatten path b4 get_intersections()");
      }
      const_iterator other_w = other.begin();
      const_iterator otherguard=other_w;
      if(!alwaysclosed){otherguard=other.end();}
      bool otherstarted=false;
      SubPath* nonconst= const_cast<SubPath*>(this);
      std::pair<const Segment<coords*>,const_iterator> wpair=nonconst->get_segment_and_iter(w,alwaysclosed);
      while (other_w != otherguard || !otherstarted){
	otherstarted=true;
	if((*other_w).second & (BEZIER_CLICK|BEZIER_HANDLE)){
	  throw xn_Error("flatten path b4 get_intersections()");
	}
	//fixme shouldn't be calling nonconst method ... why?
	SubPath& nonconst2 = const_cast<SubPath&>(other);
	std::pair<const Segment<coords*>,const_iterator> otherpair=nonconst2.get_segment_and_iter(other_w,alwaysclosed);
	if(wpair.first.lines_cross(otherpair.first)){
	  return PATH_OVERLAP_INTERSECT;
	}
	other_w=otherpair.second;
      }
      w=wpair.second;
    }
      /////////////////////////////////
    ///@todo =>this sucks below
    w = other.begin();
    while(w != other.end() && !drawing_click(*w)){w++;}
    //if(w==other.end()
    bool inside(false), outside(false);
    for(;w !=other.end();w++){
      //int flag= (*w).second;
      if(point_within((*w).first)){	inside= true;}
      else{    outside=true;}
    }
    if(inside &&!outside){return PATH_OVERLAP_INSIDE;}
    w=begin();
    while(w != end() && !drawing_click(*w)){w++;}
    for(;w!=end();w++){
      int flag= (*w).second;
      if(flag & BEZIER_CLICK || flag & BEZIER_HANDLE){
	throw xn_Error("SubPath::is_overlapping: flatten path first");
      }
      if(other.point_within((*w).first)){return PATH_OVERLAP_OUTSIDE;}
      else{return PATH_OVERLAP_NONE;}
    }
    return PATH_OVERLAP_NONE;
  }

bool SubPath::point_within (const coords& pt, 
			      bool implicitlyclosed , 
			      unsigned int segcount)const{
    cRect box;
    get_extents(box);
    if(!pt.is_in_box(box)){  return false;  }
    size_type sz= size();
    if(!sz){return false;}
    else if(sz <=2){return false;}//point could be on_path()
    else{
      if(!(implicitlyclosed|| closed() )){return false;}
      RadialLine_Generator gen(pt,  segcount);
      for(unsigned int i=0; i< segcount; i++){
	Linear_function<double>  fn = gen.get_nth_path(i);
	std::pair<int, int> intersections = get_intersection_count(fn, pt, implicitlyclosed);
	if((intersections.first %2)&& (intersections.second%2)){return true;}
	else if((!(intersections.first)) || (!(intersections.second))){return false;}
	///@todo : this wrong -  need OR_ed path , aand maybe any internal shapes and/or 
	//count of self-intersections to make sense of all these counts
	if(implicitlyclosed && ((!(intersections.first %2)) ||(!(intersections.second%2)))){
	  return false;
	}
      }
    }
    return true;
  }

} //namespace NCOORDS
