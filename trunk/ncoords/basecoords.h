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
   @file basecoords.h
   base classes for coordinate types
*/

#ifndef BASECOORDS_H
#define BASECOORDS_H
//include appcommon/appcommonconfig.h first, reduce warning count
#include "xndrawexceptions.h"
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <iostream>
#include <cmath>
//
//#include "textio.h"
//#include "io_uncollide.h"

#include "preferences.h"
#include <set>

#define N_COUT(exp) std::cout << exp << std::endl;

namespace NCOORDS{

  enum coord_state{
    COORDS_BASESTATE =0,
    COORDS_UP =1,
    COORDS_SELECTED = COORDS_UP << 1
  };

  coord_state operator^=(coord_state& first, coord_state second);
  coord_state operator|=(coord_state& first, coord_state second);
  coord_state operator&=(coord_state& first, coord_state second);

  coord_state operator^(coord_state first, coord_state second);
  coord_state operator|(coord_state first, coord_state second);
  coord_state operator&(coord_state first, coord_state second);
  /**
     @enum PointerMode
     click types in drawing gui and paths(see NCOORDS::simple_click_history,
     as well as xNDRAW::Drawtool and children)
  */
  //from clickhistory.h
  //tODO: simplify
  //TODO: common prefix for all these PointerMode values
  enum PointerMode{
    XNDRAW_NO_CLICKDATA = 0,   
    LINE_CLICK =1,
    MOVE_CLICK =LINE_CLICK << 1,
    BEZIER_CLICK= MOVE_CLICK <<1,
    BEZIER_HANDLE=BEZIER_CLICK <<1,
    CLOSE_PATH=BEZIER_HANDLE << 1,
    DRAWING_CLICK =  (LINE_CLICK | MOVE_CLICK |BEZIER_CLICK),
    DRAWING_CLICK_HANDLE = (DRAWING_CLICK |BEZIER_HANDLE),
    DRAWING_ELEMENT = (DRAWING_CLICK_HANDLE | CLOSE_PATH),
    STRICTLY_FLAT_CLICK=LINE_CLICK | MOVE_CLICK  |CLOSE_PATH,
    ORIGIN_CLICK=CLOSE_PATH<< 1,
    OFFSET_CLICK = ORIGIN_CLICK<< 1,
    CLICK_SELECTED=OFFSET_CLICK<<1,
    //selectboxes only
    XNDRAW_POINTER_SELECTCLICK =CLICK_SELECTED<<1,
    XNDRAW_POINTER_SELECTBOX_BUTTONDOWN = XNDRAW_POINTER_SELECTCLICK << 1,
    XNDRAW_POINTER_SELECTBOX_DRAGMOTION = XNDRAW_POINTER_SELECTBOX_BUTTONDOWN <<1,
    XNDRAW_POINTER_SELECTBOX_BUTTONUP = XNDRAW_POINTER_SELECTBOX_DRAGMOTION  <<1,
    XNDRAW_POINTER_SELECTGRID = XNDRAW_POINTER_SELECTBOX_BUTTONUP <<1,
    CLICK_TRANSFORM_START = XNDRAW_POINTER_SELECTGRID << 1,
    CLICK_TRANSFORM_CONTINUE =CLICK_TRANSFORM_START <<1,
    CLICK_TRANSFORM_FINISH = CLICK_TRANSFORM_CONTINUE <<1,
    CLICK_TRANSLATE = CLICK_TRANSFORM_FINISH  <<1,
    CLICK_SCALE=CLICK_TRANSLATE <<1,
    CLICK_ROTATE = CLICK_SCALE <<1,

    //...
    CLICK_MARKED_4_DESTRUCTION = mMAXFLAG >> 3,
   
    NULL_CLICK = mMAXFLAG >> 2,
    XNDRAW_CLICK_BAD_DATA = mMAXFLAG >> 1,
    CLICK_ERROR= mMAXFLAG
  };
  //26 FLAG VALUES(JUN2010) too many! confusion!- WILL NEED uint_64t or whatever with 6 more, and an insane amount of tests to iterate over a path, if we expect these symbols to be used in one

  bool is_drawing_element(PointerMode pointtype);
  bool valid_drawing_element(PointerMode pointtype);
  std::string clicktype_as_string(int cliktype);

  class coords;//:public basecoords;

  template <typename T>
  class SelfRef{
  public:
    typedef T value_type;
    typedef T& reference;
    typedef T* pointer;
  protected:
    T& objref;
  public:
    SelfRef(T& self):objref(self){}

    reference as_reference(){return objref;}
    pointer as_pointer(){return &objref;}
    static reference create_reference(T& ptr){return ptr;}

  };

  template <typename T>
  class SelfRef<T*>{
  public:
    typedef T value_type;
    typedef T& reference;
    typedef T* pointer;
  protected:
    T* objref;
  public:
    SelfRef(T* self):objref(self){}

    reference as_reference(){return *objref;}
    pointer as_pointer(){return objref;}
    static reference create_reference(T* ptr){return *ptr;}

  };

  template <typename T>
  class SelfRef<const T*>{
  public:
    typedef T value_type;
    typedef const T& reference;
    typedef const T* pointer;
  protected:
    const T* objref;
  public:
    SelfRef(T* self):objref(self){}

    reference as_reference(){return *objref;}
    pointer as_pointer(){return objref;}
    static reference create_reference(const T* ptr){return *ptr;}

  };

  //used by pathproducer.h => PointPredicate
  bool click_selected(const std::pair<coords, int>& pt);
  bool drawing_click(const std::pair<coords, int>& pt);
  bool move_click(const std::pair<coords, int>& pt);
  bool handle_click(const std::pair<coords, int>& pt);
  bool closepath_click(const std::pair<coords, int>& pt);
  bool cairo_click(const std::pair<coords, int>& pt);
  //returns false or throws an xn_Error
  //(perhaps add a throw declaration)
  //if std::isnan(f) or std::isinf(f)
  //maybe should aDD check for 'subnormal'(meaning?) et al?
  //set quiet to true(and dothrow to false)to suppress error output
  //otherwise returns true
  template<typename FLOATTYPE>
  bool check_nan_and_inf(FLOATTYPE f , bool dothrow=true, bool quiet = false){
    bool val_nan(false), val_inf(false);
    if(std::isnan(f)){ val_nan=true;}
    if(std::isinf(f)){ val_inf=true;}
    if(val_nan||val_inf){
      std::string errstring= (dothrow? "ERROR: ":"WARNING: ");
      errstring +="value is ";
      errstring += ( val_nan? "NaN -not a number": "infinite");
      if(dothrow){
	throw xn_Error(errstring.c_str());
      }
      if(!quiet){
	std::cerr<<errstring<<std::endl;
      }
      return false;
    }
    return true;
  }

  template<typename T>
  class threshold: public std::unary_function<const T&, bool>{
    T value;
  public:
    threshold(const T& val) : value(val) {}
    bool operator()(const T& v) const { return v > value;}
    void reset(const T& newvalue){value = newvalue;}
    T get_value()const{return  value;}

    operator T()const{return value;}
  };

  template<typename XYCOORDTYPE>
  class distance_threshold:public std::binary_function<const XYCOORDTYPE& , const XYCOORDTYPE&, bool>{
    double value;
  public:
    distance_threshold(double val =0.1) : value(val) {}
    bool operator() (const XYCOORDTYPE& A, const XYCOORDTYPE& B)const{
      return std::sqrt(((A.x - B.x)*(A.x - B.x)) + ((A.y - B.y)*(A.y - B.y))) > value;
    }
    distance_threshold& operator=(double newval){
      value = newval;
    }

    void reset(double newvalue){value = newvalue;}
    double get_value()const{return  value;}
 

  };

  template <typename STATEFUL>
  struct NcoordSelection{
    typedef std::set<STATEFUL*> container_type;
    typedef typename container_type::iterator iterator;
    typedef typename container_type::const_iterator const_iterator;

    std::set<STATEFUL*> selection;

    NcoordSelection(){}
    template<typename ITER>
    NcoordSelection(ITER first, ITER last){
      for(;first != last; first++){
	select(*first);
      }
    }

    void select(STATEFUL* point){
      point->set_stateflag(COORDS_SELECTED);
      selection.insert(point);
    }

    void deselect(STATEFUL* point){
      iterator iter = selection.find(point);
      if(point == selection.end()){
	throw XNSelectionError("point not in (this) selection");
      }
      point->unset_stateflag(COORDS_SELECTED);
      selection.erase(iter);
    }

  };
  

  class basecoords{
  protected:
#ifdef COUNT_COORDS
    static unsigned int c_count;
    static unsigned int d_count;
    static unsigned int total_count;
    unsigned int serial_no; 
#endif // COUNT_COORDS
    coord_state state;
    static double atom_limit;
    
    
  public:
    virtual ~basecoords();
    basecoords(coord_state st = COORDS_BASESTATE);
    basecoords(const basecoords& src);
    basecoords& operator=(const basecoords& src);

    coord_state get_state ()const;
    bool selected()const;
    void select();
    void deselect();

    static void transfer_selection_to_pathclick(std::pair<coords, int>& pointpair,
						bool XORed =false, 
						bool clear_source=true);

    static void transfer_selection_from_pathclick(std::pair<coords, int>& pointpair,
						  bool XORed =false, 
						  bool clear_source=true);

    static	double get_atomlimit();
    static	  void set_atomlimit(double atom);

#ifdef COUNT_COORDS
    unsigned int get_Ccount()const{return c_count;}
    unsigned int get_Dcount()const{return d_count;}
    unsigned int get_totalcount()const{return total_count;}
    unsigned int get_serialno()const{return serial_no;}
#endif // COUNT_COORDS

    coord_state set_state(coord_state stat);
    coord_state set_stateflag(coord_state stat);
    coord_state set_stateflag(coord_state stat, bool value);
    coord_state unset_stateflag(coord_state stat);

    void set_updownstate(int value);
    coord_state flip_stateflag(coord_state stat);
     
  };
};



#endif /*BASECOORDS_H*/
