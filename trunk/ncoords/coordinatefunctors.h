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


#ifndef NCOORDS_COORDINATEFUNCTORS_H
#define NCOORDS_COORDINATEFUNCTORS_H
#include "config.h"
#include <functional>
#include <algorithm>
#include "peerrank.h"
#include "ncoords.h"


namespace NCOORDS{
enum CompareParam{
    COMPARE_ERROR=-1,
    COMPARE_LEFT=1,
    COMPARE_RIGHT=2
  };


  ///TODO: base class in peerrank that doesn't need a fn pointer
  template <typename ANGLER>
  class AngleRank: public std::binary_function<const ANGLER&, const ANGLER&,bool>{
  protected:
    const ANGLER& ranker;
    bool go_high;
  public:
    AngleRank(const ANGLER& angler, 
	      bool use_high_angle=true):
      ranker(angler), go_high(use_high_angle){}

    bool operator()(const ANGLER& left, const ANGLER& right)const{
      return  sort_pair(left, right);
    }

bool sort_pair(const ANGLER& left, const ANGLER& right)const{
      double a_left(ranker.get_angle(&left)), 
	a_right(ranker.get_angle(&right));
      return (a_left > a_right ? (go_high ? true : false) : (go_high ?false : true));
    }

    const ANGLER* choose(const ANGLER& left, const ANGLER& right)const{
      return (sort_pair(left, right) ? &left : &right);
    }

	ANGLER* choose(ANGLER& left, ANGLER& right)const{
	  return (sort_pair(left, right) ? &left : &right);
    }
	
  };

      template <typename ANGLER>
	ANGLER* choose_by_angle(const ANGLER& ranker,  
				ANGLER& left,	ANGLER& right,
				bool use_high_angle=true){
	AngleRank<ANGLER> angleranker(ranker, use_high_angle);
	return angleranker.choose(left, right);
      }
  template <typename ANGLER>
  class JoinRank: 
    public std::binary_function<const std::pair<ANGLER*,int>&, 
				const std::pair<ANGLER*,int>&,bool>{
  protected:
    const ANGLER& ranker;
    bool go_high;
  public:
    JoinRank(const ANGLER& angler, 
	      bool use_high_angle=true):
      ranker(angler), go_high(use_high_angle){}

    bool operator()(const std::pair<ANGLER*,int>& left,
		    const std::pair<ANGLER*,int>& right)const{
      return  sort_pair(left,right);
    }

  
bool sort_pair(const std::pair<ANGLER*,int>& left,
	       const std::pair<ANGLER*,int>& right)const{
      int lsecond(left.second) , rsecond(right.second);
      if(left.first ==&ranker){
	std::cerr<<"rejecting same section"<<std::endl;
	return false;
      }
      else if(right.first ==&ranker){return true;}
      double a_left(ranker.get_angle(*(left.first), lsecond)), 
	a_right(ranker.get_angle(*(right.first), rsecond));
      return (a_left > a_right ? 
	      (go_high ? true : false) : (go_high ? false : true));
    }

    const ANGLER* choose(const std::pair<ANGLER*,int>& left,
			 const std::pair<ANGLER*,int>& right)const{
      return (sort_pair(left, right) ? left.first : right.first);
    }

	ANGLER* choose(std::pair<ANGLER*,int>& left,
		       std::pair<ANGLER*,int>& right)const{
	  return (sort_pair(left, right) ? left.first : right.first);
    }

    bool is_ranker(const ANGLER& a)const{return &a == &ranker;}
	
  };

  template <typename ANGLER, typename GODKLASS>
  class LhsRhsJoinRank: 
    public JoinRank<ANGLER>{
  public:
    typedef JoinRank<ANGLER> base_type;
  protected:
    const GODKLASS& lhsrhsarbiter;
    int compare_flag;

  public:
    LhsRhsJoinRank(const GODKLASS& arbiter, const ANGLER& ranker, int cmpwhich, bool go_high=true)
      :base_type(ranker, go_high),
       lhsrhsarbiter(arbiter),
       compare_flag(cmpwhich){
#if CHECK_SUBPATH_SECTION
      if(!((compare_flag==COMPARE_LEFT)||(compare_flag==COMPARE_RIGHT))){
	throw xn_Error("cmpwhich should be COMPARE_LEFT or COMPARE_RIGHT");
      }
#endif
    }


    bool operator()(const std::pair<ANGLER*,int>& left,
		    const std::pair<ANGLER*,int>& right)const{
      return  sort_pair(left,right);
    }

    bool sort_pair(const std::pair<ANGLER*,int>& left,
		    const std::pair<ANGLER*,int>& right)const{
      if(base_type::is_ranker(*(left.first) )){
	std::cerr<<"rejecting same section"<<std::endl;
	return false;
      }
      else if(base_type::is_ranker(*(right.first))){return true;}
      if((compare_flag ==COMPARE_RIGHT) &&(!(left.first->crossings_to_outside()))){return false;}
      if((compare_flag ==COMPARE_LEFT) &&(left.first->crossings_to_outside())){return false;}
      if(lhsrhsarbiter.compare_type(*(left.first)) !=compare_flag){
	return false;
      }
      return base_type::sort_pair(left, right);
    }

const ANGLER* choose(const std::pair<ANGLER*,int>& left,
			 const std::pair<ANGLER*,int>& right)const{
      return (sort_pair(left, right) ? left.first : right.first);
    }

	ANGLER* choose(std::pair<ANGLER*,int>& left,
		       std::pair<ANGLER*,int>& right)const{
	  return (sort_pair(left, right) ? left.first : right.first);
    }

  };

  class coords_2_ncoords: public std::unary_function<NCOORDS::ncoords*,NCOORDS::coords*>{
    NCOORDS::coords* origin;
    int base;
  public:
    coords_2_ncoords(NCOORDS::coords* origin_, int base_ = 3):origin(origin_), base(base_){}
    ~coords_2_ncoords(){}
    
    NCOORDS::ncoords* operator()(NCOORDS::coords* pt){
      NCOORDS::ncoords* poxy = new NCOORDS::ncoords(*origin, *pt,base);
      return  poxy ;
    }
  };
  
  class coordspair_2_ncoordspair: public std::unary_function<std::pair<NCOORDS::ncoords*,int>*, std::pair<NCOORDS::coords*,int>* >{
    NCOORDS::coords* origin;
    int base;
  public:
    coordspair_2_ncoordspair(NCOORDS::coords* origin_, int base_ = 3):origin(origin_), base(base_){}
    ~coordspair_2_ncoordspair(){}
    
    std::pair<NCOORDS::ncoords*,int>* operator()(std::pair<NCOORDS::coords*,int>* ptpair){
      NCOORDS::ncoords* poxy = new NCOORDS::ncoords(*origin,*(ptpair->first), base);
      return new std::pair<NCOORDS::ncoords*,int>(poxy, ptpair->second);
    }
  };
  
  
  class coordspair_2_ncoordspair_ref: public std::unary_function<std::pair<NCOORDS::ncoords*,int>, std::pair<NCOORDS::coords,int>&>{
    NCOORDS::coords origin;
    //std::pair<NCOORDS::ncoords*,int>* proxy;
    int base;
  public:
    coordspair_2_ncoordspair_ref(const NCOORDS::coords& origin_,int base_ = 3):origin(origin_),base(base_){}
    ~coordspair_2_ncoordspair_ref(){}
    
    std::pair<NCOORDS::ncoords*,int> operator()(std::pair<NCOORDS::coords,int>& ptpair){
      NCOORDS::ncoords* poxy = new NCOORDS::ncoords(origin,ptpair.first, base);
      return std::pair<NCOORDS::ncoords*,int>(poxy, ptpair.second);
    }
  };
  
  
  
  class translate_coords: public std::unary_function<NCOORDS::coords&, void>{
    NCOORDS::coords offset;
  public:
    translate_coords(const NCOORDS::coords& offset_):offset(offset_){}
    ~translate_coords(){}
    void operator()(NCOORDS::coords& pt){
      pt += offset;
    }
  };
  
  class translate_coordspair: public std::unary_function<std::pair<NCOORDS::coords,int>, void >{
    NCOORDS::coords offset;
  public:
    translate_coordspair(const NCOORDS::coords& offset_):offset(offset_){}
    virtual ~translate_coordspair(){}
    void operator()(std::pair<NCOORDS::coords ,int> pt)const{
      pt.first += offset;
    }
  };

template <class COORDSTYPE>
class translate_COORDSTYPE_pair: public std::unary_function<std::pair<COORDSTYPE*,int> , void>{
  COORDSTYPE offset;
public:
  translate_COORDSTYPE_pair(COORDSTYPE  offset_):offset(offset_){}
  virtual ~translate_COORDSTYPE_pair(){}
  void operator()(std::pair<COORDSTYPE*, int> pt){
    *pt.first += offset;
  }
};

class relativiser:public std::unary_function<coords*, coords*>{
  coords adjustment;
public:
  relativiser(const coords& diff):adjustment(diff){}
  ~relativiser(){}
  coords* operator()(coords* input){
    *input += adjustment;
    return input;
  }

};

  class pair_relativiser:public std::unary_function<std::pair<coords ,int>, std::pair<coords ,int> >{
  coords adjustment;
public:
  pair_relativiser(const coords& diff):adjustment(diff){}
  ~pair_relativiser(){}

  std::pair<coords,int>& operator()(std::pair<coords,int>& input){
    input.first += adjustment;
  return input;
  }

};
template <class COORDSTYPE>
class COORDSTYPE_pair_relativiser:public std::unary_function<std::pair<COORDSTYPE*,int>, std::pair<COORDSTYPE*,int> >{
  COORDSTYPE adjustment;
public:
  COORDSTYPE_pair_relativiser(COORDSTYPE* diff):adjustment(*diff){}
  ~COORDSTYPE_pair_relativiser(){}
  std::pair<COORDSTYPE*,int> operator()(std::pair<COORDSTYPE*,int> input){
    return std::pair<COORDSTYPE*,int>(input.first->add(adjustment), input.second);
  }

};

 class coords_equals : public std::binary_function<coords*, coords*, bool>{
	      int failcount;
	    public:
	      coords_equals() : failcount(0) {}
		bool operator()(coords* left, coords* right){
		  if (*left != *right){failcount++;}
		  return *left == *right;
		}
		void reset() {failcount = 0;}
		
		int get_failcount(){return failcount;}
	    };

	    class coords_unequals : public std::binary_function<coords*, coords*, bool>{
	      int failcount;
	    public:
	      coords_unequals() : failcount(0) {}
		bool operator()(coords* left, coords* right){
		  if (*left == *right){failcount++;}
		  return *left != *right;
		}
		void reset() {failcount = 0;}
		
		int get_failcount(){return failcount;}
		
	    };

}

#endif/*NCOORDS_COORDINATEFUNCTORS_H*/
