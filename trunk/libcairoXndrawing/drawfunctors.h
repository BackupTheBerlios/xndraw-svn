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


#ifndef XNDRAW_DRAWFUNCTORS_H
#define XNDRAW_DRAWFUNCTORS_H

#include "xndrawexceptions.h"
#if HAVE_CONFIG_H
#include "config.h"
#endif

#include "config.h"
#include "drawingset.h"
#include "xndrawexceptions.h"
#include <algorithm>
#include <functional>
namespace xNDRAW{




  
  class draw_set_stroke_colour: public std::unary_function<Drawingbase*, void>{
    double r,g,b,a;
  public:
    draw_set_stroke_colour(double r_, double g_, double b_, double a_):r(r_), g(g_), b(b_), a(a_){}
    ~draw_set_stroke_colour(){}
    void operator()(Drawingbase* draw);
    
  };
  class draw_set_fill_colour: public std::unary_function<Drawingbase*, void>{
    double r,g,b,a;
  public:
  draw_set_fill_colour(double r_, double g_, double b_, double a_):r(r_), g(g_), b(b_), a(a_){}
    ~draw_set_fill_colour(){}
    void operator()(Drawingbase* draw);
    
  };
  
  class draw_source_to_fill: public std::unary_function<Drawingbase*, void>{    
  public:
    draw_source_to_fill(){}
    ~draw_source_to_fill(){}
    void operator()(Drawingbase* draw);
  };
  
  class draw_source_to_stroke: public std::unary_function<Drawingbase*, void>{    
  public:
    draw_source_to_stroke(){}
    ~draw_source_to_stroke(){}
    void operator()(Drawingbase* draw);
  };
  
  class draw_save: public std::unary_function<Drawingbase*, void>{    
  public:
    draw_save(){}
    ~draw_save(){}
    void operator()(Drawingbase* draw);
  };
  
  class draw_restore: public std::unary_function<Drawingbase*, void>{    
  public:
    draw_restore(){}
    ~draw_restore(){}
    void operator()(Drawingbase* draw);
  };
  
 
  
  class draw_insert_clickpath: public std::unary_function<Drawingbase*, void>{
    NCOORDS::simple_click_history path;
    bool relative;
    int base;
  public:
    draw_insert_clickpath(NCOORDS::simple_click_history path_, bool relative_ = true, int base_ = 0)
      :
      path (path_),
      relative(relative_),
      base(base_)
    {}
    ~draw_insert_clickpath(){}
    void operator()(Drawingbase* draw);
  };
  class draw_prepend_clickpath: public std::unary_function<Drawingbase*, void>{
    NCOORDS::simple_click_history path;
    bool relative;
    int base;
  public:
    draw_prepend_clickpath(NCOORDS::simple_click_history path_, bool relative_ = true, int base_ = 0)
      :
      path (path_),
      relative(relative_),
      base(base_)
    {}
    ~draw_prepend_clickpath(){}
    void operator()(Drawingbase* draw);      
  };
  
  class draw_append_clickpath: public std::unary_function<Drawingbase*, void>{
    NCOORDS::simple_click_history path;
    bool relative;
    int base;
  public:
    draw_append_clickpath(NCOORDS::simple_click_history path_, bool relative_ = true, int base_ = 0)
      :
      path (path_),
      relative(relative_),
      base(base_)
    {}
    ~draw_append_clickpath(){}
    void operator()(Drawingbase* draw);
  };
  
  class draw_line_to_history: public std::unary_function<Drawingbase*, void>{
  public:
    draw_line_to_history(){}
    ~draw_line_to_history(){}
    void operator()(Drawingbase* draw);
  };
  
  
  class draw_stroke_history: public std::unary_function<Drawingbase*, void>{
  public:
    draw_stroke_history(){}
    ~draw_stroke_history(){}
    
    void operator()(Drawingbase* draw);
  };
  
  
  
  class draw_stroke_preserve_history: public std::unary_function<Drawingbase*, void>{
  public:
    draw_stroke_preserve_history(){}
    ~draw_stroke_preserve_history(){}
    
    void operator()(Drawingbase* draw);
  };
  
  class draw_fill_preserve_history: public std::unary_function<Drawingbase*, void>{
  public:
    draw_fill_preserve_history(){}
    ~draw_fill_preserve_history(){}
    
    void operator()(Drawingbase* draw);
  };
  class draw_fill_history: public std::unary_function<Drawingbase*, void>{
  public:
    draw_fill_history(){}
    ~draw_fill_history(){}
    void operator()(Drawingbase* draw);
  };
  
  class draw_set_stroke_rgb: public std::unary_function<Drawingbase*, void>{
    double red;
    double green;
    double blue;
  public:
    draw_set_stroke_rgb(double r, double g, double b): red(r), green(g), blue(b) {}
    ~draw_set_stroke_rgb(){}
    void operator()(Drawingbase* draw);
  };
  class draw_set_fill_rgb: public std::unary_function<Drawingbase*, void>{
    double red;
    double green;
    double blue;
  public:
    draw_set_fill_rgb(double r, double g, double b): red(r), green(g), blue(b) {}
    ~draw_set_fill_rgb(){}
    void operator()(Drawingbase* draw);
  };

class draw_curve_to: public std::unary_function<Drawingbase*, void>{
  coords handle1;
  coords handle2;
  coords pt;
public:
  draw_curve_to(const coords& h1, const coords& h2, const coords& p): 
    handle1(h1), 
    handle2(h2),
    pt(p)
  {}
  void operator()(Drawingbase* draw){
  draw->curve_to(&handle1, &handle2, &pt);
  }
};
  
  class draw_set_operator: public std::unary_function<Drawingbase*, void>{
    Cairo::Operator op;
  public:
    draw_set_operator(Cairo::Operator op_): op(op_) {}
    ~draw_set_operator(){}
    void operator()(Drawingbase* draw);
  };
  
  class draw_set_linecap: public std::unary_function<Drawingbase*, void>{
    Cairo::LineCap linecap;
  public:
    draw_set_linecap(Cairo::LineCap lc): linecap(lc){}
    ~draw_set_linecap(){}
    void operator()(Drawingbase* draw);
  };
  
  class draw_set_fillrule: public std::unary_function<Drawingbase*,void>{
    Cairo::FillRule fillrule;
  public:
    draw_set_fillrule(Cairo::FillRule rule) : fillrule(rule){}
    ~ draw_set_fillrule(){}
    void operator()(Drawingbase* draw);
  };
  
  class draw_set_linejoin: public std::unary_function<Drawingbase*,void >{
    Cairo::LineJoin linejoin;
  public:
    draw_set_linejoin(Cairo::LineJoin join):linejoin(join) {}
    ~ draw_set_linejoin(){}
    void operator()(Drawingbase* draw);
  };
  
  class draw_clear_history:  public std::unary_function<Drawingbase*, void>{
  public:
    draw_clear_history() {}
    ~draw_clear_history() {}
    void operator()(Drawingbase* draw);
  };
  
  class draw_adjust_stroke_alpha: public std::unary_function<Drawingbase*, void>{
    double alpha;
  public:
    draw_adjust_stroke_alpha(double alph):alpha(alph) {}
    ~ draw_adjust_stroke_alpha() {}
    void operator()(Drawingbase* draw);
    
  };
  class draw_set_linewidth: public std::unary_function<Drawingbase*, void>{
    double linewidth;
  public:
    draw_set_linewidth(double width):linewidth(width){}
    ~draw_set_linewidth() {}
    void operator()(Drawingbase* draw);
    
    
  };
  
  class draw_save_png: public std::unary_function<Drawingbase*, void>{
    int count;
    std::string filename;
  public:
    draw_save_png(std::string fname):count(0) ,filename(fname){}
    ~draw_save_png(){}
    void operator()(Drawingbase* draw);
  };
  
  class draw_moveto_coords:public std::unary_function<Drawingbase*, void>{
    NCOORDS::coords* pt;
  public:
    draw_moveto_coords(NCOORDS::coords* point): pt(point){}
    ~draw_moveto_coords(){}
    void operator()(Drawingbase* draw);
    
  };
  
  class draw_lineto_coords:public std::unary_function<Drawingbase*, void>{
    
    NCOORDS::coords* pt;
  public:
    draw_lineto_coords(NCOORDS::coords* point): pt(point){
      std::cout << "line_to_functor CTor " << std::endl;
    }
    ~draw_lineto_coords(){}
    
    void operator()(Drawingbase* draw);
    
  };/*class draw_lineto_coords*/
  
  class draw_setorigin_coords :public std::unary_function<Drawingbase*, void>{
    NCOORDS::coords* pt;
  public:
    draw_setorigin_coords(NCOORDS::coords* point): pt(point){}
    ~draw_setorigin_coords(){}
    
    void operator()(Drawingbase* draw);
  };
  
  
  class draw_fill_functor:public std::unary_function<Drawingbase*, void>{
  public:
    draw_fill_functor(){}
    ~draw_fill_functor(){}   
    void operator()(Drawingbase* draw);    
  };

  class draw_fillpreserve_functor:public std::unary_function<Drawingbase*, void>{
  public:
    draw_fillpreserve_functor(){}
    ~draw_fillpreserve_functor(){}    
    void operator()(Drawingbase* draw);    
  };

  class draw_stroke_functor:public std::unary_function<Drawingbase*, void>{
  public:
    draw_stroke_functor(){}
    ~draw_stroke_functor(){}   
    void operator()(Drawingbase* draw);
  };
    
    class draw_set_baseno:  public std::unary_function<Drawingbase*, void>{
      int newbase;
  public:
      draw_set_baseno(int base):newbase(base){}
      ~draw_set_baseno(){}
 void operator()(Drawingbase* draw);
  };

  class draw_strokepreserve_functor:public std::unary_function<Drawingbase*, void>{
  public:
    draw_strokepreserve_functor(){}
    ~draw_strokepreserve_functor(){}
    
    void operator()(Drawingbase* draw);
    
    
  };
  
  class draw_set_flipped: public std::unary_function<Drawingbase*, void>{
    bool flip;
  public:
    draw_set_flipped(bool flip_):flip(flip_){}
    void operator()(Drawingbase* draw){
      // per Drawingset would be better
      //DERIVE A PROJECT SETTINGS CLASS
      //from Preferences
      XNDRAW_SET_PREF("DRAWINGSET_DO_INVERT",flip);
    }
  };
  //unused - consider removing
class move_line_iterfunc: 
    public std::binary_function< coords*, drawPtr&, void>
  {
coords point;

public:

move_line_iterfunc(coords* seed) : point(*seed) {}

~move_line_iterfunc(){}

void 
operator()(coords* nxt, drawPtr& draw)
{
  draw->move_to(&point);
draw->line_to(nxt);
 point.set(*nxt);
}
};

class line_iterfunc: 
    public std::binary_function< coords*, drawPtr&, void>
  {
public:

line_iterfunc(){}

~line_iterfunc(){}

void 
operator()(coords* nxt, drawPtr& draw)
{
draw->line_to(nxt);
}

};


}/*namespace  xNDRAW*/

#endif /* XNDRAW_DRAWFUNCTORS_H*/



