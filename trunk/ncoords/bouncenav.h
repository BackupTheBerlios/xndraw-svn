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
   @file bouncenav.h
   find the way to the 'outside' by bouncing
off (and not crossing) any paths encountered

*/

#ifndef NCOORDS_BOUNCENAV_H
#define NCOORDS_BOUNCENAV_H
#include "xndrawexceptions.h"

#if HAVE_CONFIG_H
#include "config.h"
#endif
#include <cmath>
#include "linesegment.h"
#include "coordinatefunctors.h"
namespace  NCOORDS{



  struct Bounceparms{ ///how deep to go, how many bounces b4 giving up
    size_t max_recursion;
    static size_t absmaxrecursion;
    ///how many bouncechildren to create
    size_t maxlaunches;
    ///how many bouncechildren to keep
    size_t keeplaunches;
    double randification; //how random direction 0-1
    ///range of directions will be:
    /// anglelimit - M_PI-rangelimit
    double anglelimit; 

    Bounceparms(size_t max_recurs=8,size_t keep_tries=2, 
		size_t launches=3,double howrand=0.1, 
		double anglelmt=M_PI/12./**<15 degrees*/);
    ///automatic copy constructor etc

    double angle_min()const{return anglelimit;}
    double angle_max()const{return M_PI - anglelimit;}
    double clamp_angle_value(double inangle)const;

  };

  template <typename SEG>
  class PointSort:
    public std::binary_function<const std::pair<coords, SEG>&, 
				const std::pair<coords, SEG>&, bool>{
  protected:
    int siderequired;
    SEG segment;
  public:
    PointSort(const SEG& src, int whichside):
      siderequired(whichside),
      segment(src){}
    bool operator()(const std::pair<coords, SEG>& left, const std::pair<coords, SEG>& right){
      int leftside = segment.pointside(left.first);
      if(leftside != siderequired){return false;}
      else if(segment.pointside(right.first)!= siderequired){return true;}
      coords midpoint=segment.get_midpoint();
      return left.first.get_distance(&midpoint) <right.first.get_distance(&midpoint) ;
    }
  };

  template <typename BOUNCE>
  class BounceSort:public std::binary_function<const BOUNCE&, const BOUNCE&, bool>{
  protected:
    const BOUNCE& ranker;
  public:
    BounceSort(const BOUNCE& r):ranker(r){}

    bool operator()(const BOUNCE& left, const BOUNCE& right){
      return ranker.rank(left, right);
    }

  };
template <typename BOUNCE>
class BounceSort<BOUNCE*>:public std::binary_function<const BOUNCE*, const BOUNCE*, bool>{
  protected:
    const BOUNCE& ranker;
  public:
    BounceSort(const BOUNCE& r):ranker(r){}

    bool operator()(const BOUNCE* left, const BOUNCE* right){
      return ranker.rank(*left, *right);
    }

  };


class Bounce{
public: 
  typedef std::map<size_t,bool>::iterator launchcount_iterator;
  typedef std::map<size_t,bool>::const_iterator const_launchcount_iterator;
protected:
  Bounce* grandparent;
  Bounce* parent;
  Bounceparms params;
  Segment<coords*> bouncepad;
  std::deque<std::pair<Linear_function<double>, Bounce*> > bouncelines;
  std::deque<Bounce> children;
  std::map<size_t,bool> launch_count;
  ///POINTSIDE_LEFT or POINTSIDE_RIGHT    
  int bouncedirection;
  ///how many bounces so far
  size_t recursions;
public:

   

  Bounce( const Segment<coords*>& bouncefrom, 
	  int camefrom);

  Bounce(Bounce* prent, 
	 const Segment<coords*>& bouncefrom);

  void initialise_launch_count();

  Bounce* get_valid_granny(Bounce* src);

  bool launches_exhausted()const;

  int can_recurse(){
    return params.max_recursion - recursions;
  }

  double get_distance(const Bounce& bounce)const;

  bool rank(const Bounce& left, const Bounce& right)const;
  //bool rank(const Bounce* left, const Bounce* right);

  void add_bouncechild(const Bounce& bounce){}///<@todo add if we want to keep(replacing one if necessary)

  Linear_function<double> generate_bouncefn(size_t launchno);

  double clamp_angle_value(double inangle)const{
    return params.clamp_angle_value(inangle);
  }
  bool is_grandparent()const{return grandparent==this;}

  template <typename PATH>
  bool no_escape(const PATH& path){
    Linear_function<double> bouncefn= generate_bouncefn(0);
    std::deque<std::pair<coords,Segment<coords*> > > intersectors = get_segments_for_pointside(path, bouncefn);
    return intersectors.size() %2;
  }

  template <typename PATH>
  bool no_escape(const PATH& path, size_t launchcount){
    Linear_function<double> bouncefn= generate_bouncefn(launchcount);
    std::deque<std::pair<coords,Segment<coords*> > > intersectors = get_segments_for_pointside(path, bouncefn);
    return intersectors.size() %2;
  }

template <typename PATH>
static std::pair<bool, bool> escapable(const Segment<coords*>& seg,
				       const PATH* path){
  Bounce lbounce(seg, POINTSIDE_LEFT);
  Bounce rbounce(seg, POINTSIDE_RIGHT);
  bool left=!(lbounce.no_escape(*path));
bool right = !(rbounce.no_escape(*path));
 return std::pair<bool, bool>(left, right);
}

  template <typename PATH>
  std::deque<std::pair<coords,Segment<coords*> > > 
  get_segments_for_pointside(const PATH& path, 
			     Linear_function<double> bouncefn){
    typedef std::pair<coords,Segment<coords*> > value_type;
    typedef std::deque< value_type > result_type;
    typedef  result_type::iterator result_iterator;
    result_type result;
    std::deque<std::pair<coords, Segment<coords*> > > intersectors=path.find_intersections_with_segment(bouncefn);
    std::cerr<<"get_segments_for_pointside()\ncount of raw intersections: "
	     <<intersectors.size()<<std::endl;
    for(result_iterator w= intersectors.begin();w!=intersectors.end();w++){
      int mside = bouncepad.pointside((*w).first);
      if(mside==bouncedirection){
	result.push_back(*w);
      }
    }
    std::cerr<<"count in direction of "<<pointside_as_string(bouncedirection)
	     <<": "<<result.size();
    return result;
  }

  template <typename PATH>
  static  bool escape(const PATH& Oobstacle,
		      const Segment<coords*>& launchpad,
		      int bounceside){
    Bounce bounce(launchpad, bounceside);
    if(bounce.no_escape(Oobstacle)){return false;}
    //return std::pair<bool,Bounce*>(true,new Bounce (segpair.first,whichside));
    Bounce* curbounce(&bounce);
    while(!(curbounce->no_escape(Oobstacle))){
      std::pair<bool, Bounce*> nxtbounce= bounce.next_bounce(Oobstacle);
      if(nxtbounce.first){return true;}
      curbounce= nxtbounce.second;
      if(!curbounce){return false;}
    }
    return false;
  }

  std::deque<Bounce*> collect_free_bounces(){
    if(grandparent!=this){return grandparent->collect_free_bounces();}
    std::deque<Bounce*> res;
    collect_free_bounces(res);
    return res;
  }

  Bounce* best_candidate(){
    if(!children.size()){return this;}
    std::deque<Bounce*> tmp;
    for(std::deque<Bounce>::iterator w = children.begin(); w!= children.end();w++){
      Bounce* childcand = (*w).best_candidate();
      if(childcand){tmp.push_back(childcand);}
    }
    if((can_recurse()>=0) && (!(launches_exhausted()))){
      tmp.push_back(this);
    }
    if(tmp.size()){
      BounceSort<Bounce*> bouncesorter(*grandparent);
      std::sort(tmp.begin(), tmp.end(), bouncesorter);
      return tmp[0];
    }
    return NULL;
  }

  size_t choose_launchno()const{
    std::deque<size_t> possib;
    for (std::map<size_t, bool>::const_iterator wmap= launch_count.begin(); 
	 wmap!= launch_count.end();wmap++){
      if(!(*wmap).second){possib.push_back((*wmap).first);}
    }
    return possib[std::rand() %possib.size()];
  } 
  template <typename PATH>
  std::pair<bool, Bounce*> next_bounce(const PATH& path){
    Bounce* b_launcher= grandparent->best_candidate();
    size_t launch= b_launcher->choose_launchno();
    if( b_launcher &&(!(b_launcher->no_escape(path, launch)))){
      return b_launcher->bounce_to_freedom(launch, path);
    }
    return std::pair<bool, Bounce*> (false, NULL);
    }

  ///inefficient approach if repeated with almost same content
 std::deque<Bounce*>& collect_free_bounces(std::deque<Bounce*>& collection){
   if(!launches_exhausted()){collection.push_back(this);}
   for(std::deque<Bounce>::iterator w= children.begin(); w!= children.end();w++){
     (*w).collect_free_bounces(collection);
   }
    return collection;
  }

  template <typename PATH>  
  std::pair<bool, Bounce*> bounce_to_freedom(size_t launchno, const PATH& path){
    if((launchno>= launch_count.size())|| launches_exhausted()){
      throw xn_Error("launchno out of range or launches exhausted,");
    }
    Linear_function<double> bouncefn= generate_bouncefn(launchno);
    std::deque<std::pair<coords,Segment<coords*> > > intersectors = get_segments_for_pointside(path, bouncefn);
    if(intersectors.size() %2){//no escape
      throw xn_Error("No escape! call if(!no_escape(path)){... first");//or throw error
    }
    if(!intersectors.size()){return  std::pair<bool, Bounce*>(true, NULL);}
    else if(!(can_recurse())){return  std::pair<bool, Bounce*>(false, NULL);}//bouncedout
    PointSort<Segment<coords*> > segsorter(bouncepad, bouncedirection);
    std::sort(intersectors.begin(), intersectors.end(), segsorter);
    Bounce nxtbounce(this, intersectors[0].second);
     Bounce * p_nxtbounce=add_bounce(nxtbounce,bouncefn,launchno); 
     //which should replacing child in children and in bouncelines as required
    return std::pair<bool, Bounce*>(false, p_nxtbounce);
    }

  Bounce* add_bounce(const Bounce& bouncemaybe, Linear_function<double> fn,int launchno){
children.push_back(bouncemaybe);
 Bounce* p_nxtbounce(&(*(children.end()-1)));
 if(!launch_count[launchno]){ launch_count[launchno]=true;}
 else{throw xn_Error("this bounce done!");}
 bouncelines.push_back(std::pair<Linear_function<double>, Bounce*>(fn, p_nxtbounce));
 return p_nxtbounce;
  }

  coords bouncepoint()const{return bouncepad.get_midpoint();}

  double bounce_aggregate()const;

  double average_bounce()const{
    if(!recursions){return 0;}
    return bounce_aggregate() /(double)recursions;
  }


  double last_hop_distance()const;

    double last_hop_gained()const;
    
      };
  }

#endif
