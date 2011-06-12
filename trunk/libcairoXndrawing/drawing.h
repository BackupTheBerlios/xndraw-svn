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




#ifndef LIBXNCAIRODRAWING_DRAWING_H
#define LIBXNCAIRODRAWING_DRAWING_H
#include "xndrawexceptions.h"

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include "io_defines.h"

#include <deque>
#include <iterator>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <functional>
#include "nodes.h"

#include "typedefs.h"
#include "pathproducer.h"
#include "pathqueueeater.h"
#include "drawingbase.h"
#include "pathshape.h"
#include "x3draw.h"


namespace xNDRAW{
 

  class Drawing: public Drawingbase
  {
 public:
    typedef void (Drawing::* paint_operation)();
     typedef void (Drawing::* line_operation)(const coords*);
    typedef size_t size_type;
    
  protected:
    
    ncoords point_data;
    deqC current;
    ncoords next;
    deqC future;
    
  public:

    Drawing( cairo_t* cr,cairo_surface_t* scratch, ncoords centrepoint= ncoords());
    
    Drawing( cairo_t* cr,cairo_surface_t* scratch, ncoords centrepoint, const simple_click_history& clicks);
    
    Drawing(Cairo::RefPtr<Cairo::Surface> target,cairo_surface_t* scratch, ncoords centrepoint);
    virtual ~Drawing(){}
    
    
    void reset_base(int point_no, bool draw_it= true);
    
    virtual void set_baseno(unsigned int x){reset_base(x);}             
    
    void insert_path(simple_click_history path, bool relative = true, int base = 0);
    void append_path(simple_click_history path, bool relative = true, int base = 0);
    void append_path(Cairo::Path& path){Cairo::Context::append_path(path);}
    void prepend_path(simple_click_history path, bool relative = true, int base = 0);
    
    simple_click_history cut_path(int pos, int count, bool relative= true);
    
    ncoords& get_next(){
      return next;
    }
    const ncoords& get_next()const{
      return next;
    }
    virtual    void   do_draw_path();
    virtual void do_draw_path(const ncoords& rotator);
        //void do_draw_to(click_pair_type
    //returns the old path
    virtual simple_click_history import_history(simple_click_history& path, coords* origin_ = NULL, int base = 0);
    
    virtual void get_extents(cairo_rectangle_t& drawrect, const coords* offset = NULL);
    void reset();
    nPathShape create_pathshape()const{
      return nPathShape(point_data, *history);
    }
    virtual bool path_within_bounds(const coords& offset);

  bool get_has_nodes(){ return false;}
  
virtual void set_source_offset_and_step(const coords& off,
				    size_type step=0, 
				    ColourRole role= COLOURROLE_STROKE){
  colourmanager.set_anchor(point_data);
      colourmanager.pattern_set_offset(off, role);
      colourmanager.pattern_set_rotation_step(step, role);
    }


    ncoords& get_point_data(){
      return point_data;
    }

 const ncoords& get_point_data()const{
      return point_data;
    }

friend std::ostream& operator<<(std::ostream& os, const Drawing& draw){
  os<< "Drawing at" << &draw << std::endl;
 os << "origin = " << draw.get_origin() << std::endl;
 os << "point_data = " << draw.get_point_data()<< std::endl;
 os << "next = " << draw.get_next()<< std::endl;
 return os;
}
    
    virtual cairo_t* draw_path_to_cairo(cairo_t* cr);

Drawing* dobj(){return this;}
  
  ncoords* get_last(){
    return &point_data;
  }
  
  void set_next(const coords* nxt){/*prefer this version*/
    coords centre(next.get_x(), next.get_y());
    next.set(centre , *nxt, next.get_base());
  }
    virtual void check_rotator_offset(const coords* offsett){
      if (offsett){
	throw xn_Error("no allowed offset in this class");
      }
    }
    void create_path_rotator(PathRotator::queue_type& q, PathRotator*& outref, const coords* offset=NULL);

  void stroke_preserve(){Cairo::Context::stroke_preserve();}
  
  void reset(int no);
    void set_next(ncoords* nxt);
    
    void set_point_data(ncoords* last);

 void set_point_data(const coords* last){
   point_data.set_radial( *last);
  }

   void on_init();

void set_line_cap(Cairo::LineCap lc){
    Cairo::Context::set_line_cap(lc);
  }
  double get_relsize();
  
  void fill(int x);
  void fill(){Cairo::Context::fill();}
  
  void flip(){} //mirror or rotate 

  void print_history();

    NCOORDS::coords get_origin() const{ return point_data.get_centre();}
 
  virtual void fill_history();
  virtual void fill_preserve_history();
virtual void stroke_history();
  virtual void stroke_preserve_history();
  virtual void line_to(float ex,float why){Cairo::Context::line_to(ex,why);}
  virtual void line_to(coords* point);
    //  virtual void line_to(coords* previous,coords* click, coords* offset);
virtual void curve_to(coords* h1, coords* h2, coords* pt);
    virtual void curve_to(double h1x, double h1y, double h2x, double h2y, double ptx, double pty);
    virtual void curve_to(coords* pt);
    virtual void line_to_history(coords* offset = NULL);
    inline void move_to(double ex, double why){
    Cairo::Context::move_to(ex, why);
  }
 
    inline void set_origin(float ex,float why){
      point_data.set(ex,why); // sets centre - bad f-name
      next.set(ex,why);
    }
    inline void set_origin(const coords& originclick){
      point_data.set(originclick.get_x(), originclick.get_y());// sets centre - bad f-name
      next.set(originclick.get_x(), originclick.get_y());
}
    
  void set_to_origin(double ex, double why);//fix the offset origin problem
  
  inline bool get_verbose(){return X3_draw::instance().get_verbose();}

  void set_to_origin(coords* pt){
    set_to_origin(pt->get_x(), pt->get_y());
  }

int get_baseno(){
  return point_data.get_base();
}



  void set(deqC& coordlist){/*keep compiler happy*/} 
  void move_to(coords* pt);



};
typedef Cairo::RefPtr<Drawingbase> dBasePtr;
typedef Cairo::RefPtr<Drawing> drawPtr;


}/*namespace xNDRAW*/


#endif /*DRAWING_H*/
