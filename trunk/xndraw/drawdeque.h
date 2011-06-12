//-*- c++ -*-

/*  Copyright (C) 2006-2011 John O'Driscoll
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef  XNDRAW_DRAWDEQUE_H
#define  XNDRAW_DRAWDEQUE_H
#include "xndrawexceptions.h"

#if HAVE_CONFIG_H
#include "config.h"
#endif


#include <queue>
#include <deque>
#include <set>
#include <string>
#include <sstream>
#include <glibmm/thread.h>
#include "drawingset.h"
#include "drawtool.h"

#include "defines.h"
#include "drawfunctors.h"


namespace xNDRAW{
  
  class drawdeque: public std::set<Drawingbase*, compare_serial<Drawingbase> >{
    std::queue<coords*>* pencilqueue;
    Drawtool* pencil;
    
    Glib::Thread* click_receiver_thread;
  public:
    drawdeque(): pencilqueue(NULL), pencil(NULL),  click_receiver_thread(NULL){std::cout << "drawdeque() CTor" << std::endl;}
    drawdeque(drawPtr& draw): pencilqueue(NULL) , pencil(NULL), click_receiver_thread(NULL) {insert(draw->dobj());}
    virtual ~drawdeque();
	
    void  set_queue (std::queue<coords*>* queue);
    std::queue<coords*>& get_queue();

    virtual  void set_pencil(Drawtool* tool);
	
    Drawtool& get_pencil();

	
    void line_to(NCOORDS::coords* point);

    void curve_to(NCOORDS::coords* handle1,  NCOORDS::coords* handle2, NCOORDS::coords* point){
      draw_curve_to functor(*handle1, *handle2, *point);
      for_each(begin(), end(), functor);
    }
    void fill_history();

    bool save_nodes(std::string& filename);
  	
    void fill();
	
    void stroke();
	
	  void fill_preserve();
	
    void stroke_preserve();
	
    void set_to_origin(NCOORDS::coords* pt);
	
    void add(drawPtr& draw);
    void add(Drawingbase* draw);
    void add(Cairo::RefPtr<Drawingset> drawset);
    void save_to_png(std::string filename);
    void set_linewidth(double linewidth);
	void adjust_stroke_alpha(double alpha);
	
    void clear_history();
	
   void init_drawset(const NCOORDS::Nodes& map);
    void reinit_drawset(NCOORDS::Nodes& map);
    void set_line_join(Cairo::LineJoin j);

    void set_fill_rule(Cairo::FillRule rule);

    void set_line_cap(Cairo::LineCap cap);

    void set_operator(Cairo::Operator op);
		
    void line_to_history();
		
    void append_path(NCOORDS::simple_click_history path_, bool relative_ = true, int base_ = 0);

    void prepend_path(NCOORDS::simple_click_history path_, bool relative_ = true, int base_ = 0);
	
    void insert_path(NCOORDS::simple_click_history path_, bool relative_ = true, int base_ = 0);
	
    void init_threads();
	
	  void move_to(NCOORDS::coords* pt);
	
    void stroke_preserve_history();
   
    void stroke_history();

    void fill_preserve_history();

    void save();
    void restore();

    void set_source_to_fill();

    void set_source_to_stroke();

    void set_stroke_colour(double r, double g, double b);
    void set_stroke_colour(double r, double g, double b, double a);

    void set_baseno(int base_);

    void set_fill_colour(double r, double g, double b);

    void set_fill_colour(double r, double g, double b, double a);

    

    void set_flipped(bool flip = true);

  };/*class drawdeque*/

  


}//namespace xNDRAW
#endif /*XNDRAW_DRAWDEQUE_H*/
