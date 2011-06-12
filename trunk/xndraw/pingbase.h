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

#ifndef X3DRAW_PINGBASE_H
#define X3DRAW_PINGBASE_H
#include "xndrawexceptions.h"

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <gtkmm.h>
#include <gdk/gdkcairo.h>
#include <iostream>
//#include <set>
#include "x3draw.h"

//#include "drawtool.h"
#include "drawbuffer.h"

//#include "opsequence.h"


//#include "typedefs.h"

namespace xNDRAW{

  //  cairo_t* xndraw_get_cairo_from_gtkvisual(Gtk::Widget& widget);
  enum Pingbufferflags{
    PINGBUFFER_RESIZE_LARGER=1,
    PINGBUFFER_RESIZE_SMALLER =2,
    PINGBUFFER_RESIZE_ALWAYS =PINGBUFFER_RESIZE_LARGER |PINGBUFFER_RESIZE_SMALLER,
    PINGBUFFER_RESIZE_NEVER= 4
  };



  class ping_base : public Gtk::DrawingArea{
  protected:

    CairoBuffer buffer;
    Pingbufferflags bufferflag;
  ping_base(): buffer(XNDRAW_GET_PREF("IMAGEBUFFER_WIDTH", 0u),
		      XNDRAW_GET_PREF("IMAGEBUFFER_HEIGHT",0u), 
		      CAIRO_FORMAT_ARGB32),
      bufferflag(PINGBUFFER_RESIZE_LARGER)
      {}
  public:
  ping_base(unsigned int bufferwidth,unsigned int bufferheight, 
	    cairo_format_t bufferformat,
	    Pingbufferflags flags=PINGBUFFER_RESIZE_LARGER):
    buffer(bufferwidth, bufferheight, bufferformat),
      bufferflag(flags){}
  protected:
    //overload to instantiate
    virtual ~ ping_base(){}
  public:
    virtual void on_transform_signal(Cairo::Matrix& matr){}
    
    virtual void set_source(Cairo::RefPtr<Cairo::Surface>& src){}
  protected:
    virtual void reallocate_drawbuffer(Gtk::Allocation& alloc);
  public:
    
    surfPtr open_background(std::string& filename){
      return Cairo::ImageSurface::create_from_png(filename.c_str());
    }
    virtual void on_draw_import_paths(std::string fname){
      std::cerr<<"base function:  void on_draw_import_paths(std::string fname){"<<std::endl;
    }
    virtual   bool get_verbose(){
      return X3_draw::instance().get_verbose();
    }
    virtual bool save_nodes(std::string& filename){
      return false;
    }    
    cairo_t*  get_gdkcairo();//cairo from gdk(ie the window onscren)

    Cairo::RefPtr<Cairo::Context> get_gdkcairo_ref();

    cairo_surface_t* get_buffersurface(){
      return buffer.get_surface(); 
    }
    Cairo::RefPtr<Cairo::Surface> get_buffer_surface_ref();
    Cairo::RefPtr<Cairo::Context> get_buffer_context();

    CairoBuffer& get_buffer();
    const CairoBuffer& get_buffer()const;
    virtual void set_source_from_buffer(Cairo::RefPtr<Cairo::Context> buffercontext =Cairo::RefPtr<Cairo::Context>(NULL));
    virtual void paint_buffer_to_window();
    bool needs_resize(const Gtk::Allocation& alloc) const;
    
    virtual   void clear_cairo(){}

    virtual void clear_buffer_to_bg();

    virtual void on_drawtool_signal_coords(click_pair_type pointpair){}
    virtual void on_drawtool_signal_origin(click_pair_type pointpair){}
    virtual void on_selecttool_signal_coords(click_pair_type pointpair){}

    virtual void on_size_allocate(Gtk::Allocation& alloc);
    virtual   bool on_expose_event(GdkEventExpose* event);
    virtual bool on_motion_what(GdkEventMotion* event){
      return false;
    }
    virtual bool on_motion_notify_event(GdkEventMotion* event){
      return Gtk::DrawingArea::on_motion_notify_event(event);
    }
    
  };/*class ping_base*/

} //namespace xNDRAW

#endif
