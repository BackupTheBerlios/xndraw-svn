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



#ifndef NCOORDS_SELECTION_H
#define NCOORDS_SELECTION_H
#include "xndrawexceptions.h"
#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <glibmm/object.h>
#include <map>
#include "clickhistory.h"
#include "preferences.h"

namespace NCOORDS{
  //not going to work without upcasting
  //can't define operator-> here as vfunc, as it would be template function
  struct SelectBox: public cRect{

  bool active;
    double drag_tolerance;
  coords& first_corner;
  coords& last_corner;

    SelectBox(double tol = 3.0):active(false), 
				drag_tolerance(tol),
first_corner(topL), last_corner(bottomR){}
    SelectBox(const coords& first, const coords& second):
      cRect (first, second),
active(false),
first_corner(topL), 
last_corner(bottomR){}

    double get_draggable_tolerance()const{
      return drag_tolerance;
    }
    void set_draggable_tolerance(double val){
      drag_tolerance = val;
    }
};

  //not much use
  struct GraphicElementBase{
    typedef std::string::size_type size_type;
    std::string name;
    const size_type ID;
    int state;
    static size_type element_count;

    GraphicElementBase():ID(++element_count){}
    GraphicElementBase(const std::string& nm):name(nm), ID(++element_count){}
    virtual ~GraphicElementBase(){}

    operator bool()const{return initialised();}
    void set_name(const std::string& nm){name = nm;}
    std::string get_name()const{ return name;}
    virtual bool initialised()const{return false;}
    virtual std::string get_typehint()const{return "";}

    virtual int get_state()const{return state;}
    size_type get_ID()const{return ID;}
  };

template <typename GRAPHICDATA>
class GraphicElement: public GraphicElementBase
{
protected:
  GRAPHICDATA* data;
  GraphicElementBase* parent;
  std::deque<GraphicElementBase*> children;
public:
  GraphicElement( GRAPHICDATA* d = NULL):data(d), parent(NULL){}
  GraphicElement( GRAPHICDATA* d, const std::string nm):GraphicElementBase(nm),data(d), parent(NULL){}
virtual bool initialised()const{return data;}
  virtual std::string get_typehint()const{return get_valueType(*data);}

  GRAPHICDATA* operator*(){return data;}
const GRAPHICDATA* operator*()const {return data;}
GRAPHICDATA* operator->(){return data;}
const GRAPHICDATA* operator->()const{return data;}

    };

  class Selection: public std::set<coords*>{

  public:
template <class ITERATOR>
Selection(ITERATOR start, ITERATOR finish){
  for(;start !=finish; start++){
    insert(*start);
  }
}
    //default copy ctor ok
  };

  //thus far this is crap

  /**
  
 
   *  template<class COORDSTYPE>
   *class Selection:public std::multimap<bool, std::pair<COORDSTYPE*, int> >{
   * typedef std::pair<bool, std::pair<COORDSTYPE*, int> > Selectable_;
   *
   * public:
   *
   *
   *Selection(NCOORDS::clickin_history<COORDSTYPE> input_line, bool select_all = false);
   *
   *std::vector<COORDSTYPE*> get_selected();
   *virtual ~Selection(){}
   * };*
   *
   *class simple_selection:public Selection<NCOORDS::coords>{
   *public:
   *simple_selection(NCOORDS::simple_click_history& line, bool select_all = false){
   *  for(NCOORDS::simple_click_history::iterator walk = line.begin(); walk != line.end(); walk++){
   *insert(Selectable_(false,*walk));
   *  }
   *}
   * };
 **

  struct Point_ :public std::pair<NCOORDS::coords*, int>{
    typedef Point_* pointer;

    Point_(coords* pt , int x):std::pair<NCOORDS::coords*, int>(pt,x){}
    Point_(const std::pair<NCOORDS::coords*, int> point):std::pair<NCOORDS::coords*, int>(point){}
    Point_(const Point_& point):std::pair<NCOORDS::coords*, int>(point){}
    Point_ operator=(const Point_& point){
      if(&point != this){
	//Point_ result(point.first,point.second);
	first = point.first;
	second = point.second;
      }
      return *this;
    }
 
    virtual ~Point_(){}
    coords* get_coords(){return first;}
  };
  typedef std::pair<bool, Point_> selectablePoint_;
  class Selection: public std::deque<std::pair<bool, Point_> >{
    
  public:
    Selection(){}
    virtual ~Selection(){}
    Selection(NCOORDS::simple_click_history& line, bool select_all = false);
    
    virtual std::vector<NCOORDS::coords*> get_selected();

    int find_closest(coords& selector);// position in map

    virtual void add_selection(NCOORDS::coords* nearest, bool add_to_selection = false);

    std::deque<std::pair<bool, Point_> >* get_underlying(){
      return (std::deque<std::pair<bool, Point_> >*)(this);
    }


  }; 

  class SelectionMap: public std::multimap<bool, Point_>{
  public:
    SelectionMap(const Selection& sel){
      for (Selection::const_iterator walk = sel.begin();walk != sel.end();walk++){
	insert(end(),*walk);
    }
  }


};
  */
}
#endif/*XNDRAW_SELECTION_H*/
    
