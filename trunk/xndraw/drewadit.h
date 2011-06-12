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

#ifndef X3DRAW_DRAWEDITWIDGET_H
#define X3DRAW_DRAWEDITWIDGET_H

#include "xndrawexceptions.h"

#if HAVE_CONFIG_H
#include "config.h"
#endif


#include <gtkmm.h>
#include <iostream>
#include <cairomm/context.h>
#include <cairomm/refptr.h>
#include <gdk/gdkcairo.h>
#include "drawtool.h"
#include "clickhistory.h"
#include "ncoords/ncoords.h"
#include "drawingbase.h"
#include <deque>

namespace xNDRAW{
  //pretty flaky  
  class drawedit_widget: public Gtk::VBox{
  public:
typedef std::pair<coords*, int> click_pair_type;
    void draw_to(click_pair_type pointpair);
  protected:
    Gtk::DrawingArea area;
    Drawtool* clickpencil;
    NCOORDS::simple_click_history clicks;
    Cairo::Context* drawcontext;
    bool cairo_inited;
    bool draw_freehand;
    Gtk::HBox buttonbox;
    Gtk::Button init;
    Gtk::Button freedrawer;
    Gtk::Button emit_path_butt;
    std::deque<Gtk::TargetEntry> copyables;
    double current_r,current_g,current_b;
    cairo_surface_t* png_image;
  public:
    sigc::connection draw_connection;
    
    typedef sigc::signal<void, NCOORDS::simple_click_history> send_path_signal;
    send_path_signal m_path_send_signal;
    send_path_signal path_send_signal(){
      std::cout << "drawedit_widget::path_send_signal()" << std::endl;
      return m_path_send_signal;
    }

    typedef sigc::signal<void, drawedit_widget*> Constructed_signal;
    Constructed_signal constructed_signal; 
    Constructed_signal signal_constructed(){
return constructed_signal;
    }
    drawedit_widget(int width =400,int height =400);
    virtual ~drawedit_widget();

    void set_current_colour(double r, double g,double b){
      current_r =r;
      current_g =g;
      current_b = b;
    }
    
    static Glib::RefPtr<Gtk::Window> create_in_window(Gtk::Window* parent);//was static but didnt work
    
    double get_r()const { return current_r;}
    double get_g()const { return current_g;}
    double get_b()const { return current_b;}

    cairo_surface_t* get_background(){
      return png_image;
    }

    void send_path(){
      std::cout << "drew->path_send_signal().emit(clicks)" << std::endl;
      path_send_signal().emit(clicks);
    }
    
    void paint(){
      drawcontext->paint();
    }
    
    bool on_init();
    
    void set_freehand(bool free=true);
    void set_free(){
      set_freehand();
    }
    bool get_freehand(){
      return draw_freehand;
    }
    
    void line_to(coords* pt);
    
  NCOORDS::simple_click_history& get_history(){
    return clicks;
  }
  NCOORDS::simple_click_history copy_history(){
    return clicks;
  }
  void move_to(coords* pt){
    drawcontext->move_to(pt->get_x(),pt->get_y());
  }

  bool on_button_release(GdkEventButton* event){
    std::cout<< "button_released" << std::endl;
    clickpencil->on_drawbutton_release(event);
    return true;
  }
  bool on_button_press(GdkEventButton* event){
    std::cout<< "button_pressed" << std::endl;
    clickpencil->on_drawbutton_press(event);
    return true;
  }
  Drawtool& get_pencil(){
    return *clickpencil;
  }
  void init_connections();
  
  NCOORDS::simple_click_history* swap_paths( NCOORDS::simple_click_history& in){
    NCOORDS::simple_click_history* out = &(get_history());
    clicks = in;
    return out;
  }

    Cairo::RefPtr<Cairo::Context> get_cairo(){
      if(!drawcontext){ std::cerr<< "drawxcontext is uninitialised" << std::endl;}
      return Cairo::RefPtr<Cairo::Context>(drawcontext);
    }
  
  
  void set_source(Cairo::RefPtr<Cairo::Surface> sourcesurface){
    if (cairo_inited){
      drawcontext->set_source(sourcesurface,0,0);
    }
  }
  
  
  bool on_expose_event(GdkEventExpose* event);
    bool has_cairo(){return cairo_inited;}
  };
 
}

#endif /*X3DRAW_DRAWEDITWIDGET_H*/
