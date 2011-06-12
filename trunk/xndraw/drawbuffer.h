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

#ifndef XNDRAW_DRAWBUFFER_H
#define XNDRAW_DRAWBUFFER_H
#include "xndrawexceptions.h"
#if HAVE_CONFIG_H
#include "config.h"
#endif


#include <gtkmm.h>
#include <cmath>
#include <cairo.h>
#include "drawingset.h"
#include "drawtool.h"
//#include "drewadit.h"

namespace xNDRAW{
  
  class LockingBuffer:public CairoBuffer{
  public:
    typedef bool (*expose_event_handler)(Gtk::Widget& , Gtk::Allocation&);
    typedef Cairo::RefPtr<Cairo::Context> context_pointer_type;

  protected:
    Glib::Mutex mutex;
    expose_event_handler expose_handler;
    Cairo::Matrix source_transformation;
  public:
    LockingBuffer(unsigned int width, unsigned int height, cairo_format_t format =CAIRO_FORMAT_ARGB32 ):CairoBuffer(width, height, format){}
    virtual ~LockingBuffer(){}
  protected:
    cairo_surface_t* get_surface(){
      return CairoBuffer::get_surface();
    }
  public:
    //void update_from_source(Drawingbase* source);
    
    void update_from_source(Drawingbase* source, double scale=0);
    void update_from_source(cairo_surface_t* source, double scale=0){
Glib::Mutex::Lock lock(mutex);
 Cairo::Matrix matrix;
 // imagecontext_gtk->get_matrix(matrix);
 //cairo_matrix_scale(&matrix, zoom_ratio, zoom_ratio);
 //imagecontext_gtk->set_matrix(matrix);
      cairo_surface_t* surf = get_surface();
      cairo_pattern_t* buf_pattern= cairo_pattern_create_for_surface (source);
      cairo_t* cr= cairo_create(surf);
      cairo_save(cr);
      cairo_set_source(cr,buf_pattern);
      cairo_paint(cr);
      cairo_pattern_destroy(buf_pattern);
      cairo_restore(cr);
    }
    void update_from_source(Drawingbase* source, 
			    Cairo::Matrix* transform);
    void update_from_source(cairo_surface_t* source, 
			    Cairo::Matrix* transform);
    bool on_expose_event(Gtk::Widget& widget, Gtk::Allocation& exposedarea);
    bool paint_dest(context_pointer_type cairo,Gtk::Allocation& alloc);
    bool trylock(){
      bool res= mutex.trylock();
      return res;
    }
    bool paint_dest(cairo_t* cr, Gtk::Allocation& alloc){
      //Glib::Mutex::Lock lock(mutex);
      if(!trylock()){return false;}

      cairo_surface_t* surf = get_surface();
      cairo_pattern_t* buf_pattern= cairo_pattern_create_for_surface (surf);
      cairo_save(cr);
      cairo_set_source(cr,buf_pattern);
      if(!cairo_status(cr) ==CAIRO_STATUS_SUCCESS){
	mutex.unlock();
	throw xn_Error("error setting pattern as source");
      }
      double x(alloc.get_x()), y(alloc.get_y()), 
	w(alloc.get_width()), h(alloc.get_height());
      cairo_move_to(cr, x,y);
      cairo_line_to(cr,x+w,y);
      cairo_line_to(cr, x+w, y+h);
      cairo_line_to(cr, x, y+h);
      cairo_close_path(cr);
      cairo_fill(cr);
      cairo_restore(cr);
      mutex.unlock();    
      return true;
    }
  };
  
  class DrawBuffer:public  Glib::Object/*or public: Gdk::Drawable?*/{
    Gtk::Widget* buffered;
    int wide;
    int high;
    int stride;
    cairo_format_t format;
    unsigned char* data_buffer;
    EditMode drawoption;
 
   
    Cairo::RefPtr<Cairo::ImageSurface> surf;

    DrawBuffer(Gtk::Widget& pingW,  
	       double min = 2.0,
	       cairo_format_t format = CAIRO_FORMAT_ARGB32);
    //    drawbuffer(drawedit_widget& pingW, coords* origin, coords* radial, NCOORDS::simple_click_history path,int pt_no=3, double min = 2.0);
    virtual ~DrawBuffer();
    unsigned int get_bpp();
   
    void on_coordsdata_save();
    
    
    surfPtr  on_alloc_buffer(Gtk::Widget& buffd, cairo_format_t format = CAIRO_FORMAT_ARGB32 );  
    
    inline Cairo::RefPtr<Cairo::ImageSurface>& get_surf(){
      return surf;
    }

    inline Cairo::RefPtr<Cairo::SurfacePattern> get_surf_pattern(){
      return Cairo::SurfacePattern::create(Cairo::RefPtr<Cairo::Surface>(new Cairo::Surface(surf->cobj(), false)));
    }

    void paint_white();
    void clear_to_background();
        
    void on_fill_maybe();

    inline int get_width() const {
      return wide;
    }

    inline int get_height() const {
      return high;
    }
    
 
  };
}/*namespace xNDRAW*/

#endif/*DRAWBUFFER_H*/
