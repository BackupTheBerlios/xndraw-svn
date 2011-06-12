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
#include "drewadit.h"

using namespace xNDRAW;
using namespace NCOORDS;

drawedit_widget::drawedit_widget(int width,int height) : drawcontext(NULL), cairo_inited(false),draw_freehand(false),init("init",false),freedrawer("draw\nfreehand", false), emit_path_butt("emit_path",false),current_r(0.0),current_g(1.0),current_b(0.0), png_image(NULL){
    pack_start(area);
    area.set_size_request(width,height);
    pack_start( buttonbox);
    buttonbox.pack_start(init);
    buttonbox.pack_start(freedrawer);
 buttonbox.pack_start(emit_path_butt);
 emit_path_butt.set_events(Gdk::BUTTON_PRESS_MASK);
 emit_path_butt.signal_clicked().connect(sigc::mem_fun(*this , &drawedit_widget::send_path));
clickpencil = new Drawtool(this); 
 copyables.push_back( Gtk::TargetEntry("SIMPLE_HISTORY"));
 drag_source_set(copyables, Gdk::BUTTON2_MASK , Gdk::ACTION_COPY );
 show_all_children();
 show(); 
}


drawedit_widget::~drawedit_widget(){
delete clickpencil;
clickpencil = NULL;
clicks.clear();
delete drawcontext;
drawcontext = NULL;
}

 bool drawedit_widget::on_init(){
   if (cairo_inited== false){
   std::cout << "on_init called" << std::endl;
Glib::RefPtr<Gdk::Drawable> window =area.get_window(); 
    GdkDrawable* g_surf = window->gobj();
    cairo_t* cr = gdk_cairo_create(g_surf);
    drawcontext = new Cairo::Context(cr, false);
    cairo_inited= true;
    drawcontext->move_to(0.0,0.0);
    drawcontext->set_source_rgb(1,1,1);
    drawcontext->paint();
    drawcontext->set_source_rgb(current_r,current_g,current_b);
    drawcontext->save();
    init_connections();
    signal_constructed().emit(this);
   }
    return false;
 }


  void drawedit_widget::set_freehand(bool free){
std::cout << "set_freehand" << std::endl;
    draw_freehand = free;
    Drawtool* temppencil = clickpencil;
    clickpencil = new Freedrawtool(this);
    delete temppencil;
    temppencil = NULL;
    draw_connection.disconnect();
    draw_connection=clickpencil->coords_signal_draw().connect(sigc::mem_fun(*this, &drawedit_widget::draw_to));
  }

void drawedit_widget::draw_to(click_pair_type pointpair){
  //Drawingbase* draw = drawcontext;
    coords* pt = pointpair.first;
    if (clicks.begin() == clicks.end()){
      drawcontext->move_to(pt->get_x(),pt->get_y());
      clicks.push_back(pt);
      return;
    }
    if (pointpair.second == LINE_CLICK){
      drawcontext->line_to(pt->get_x(),pt->get_y());
    }
    else if (pointpair.second == MOVE_CLICK){
      drawcontext->move_to(pt->get_x(),pt->get_y());
    }
    else{
      throw xn_Error("not supported yet");
    }
  }
 void drawedit_widget::line_to(coords* pt){
std::cout << "void line_to(coords* pt){" << std::endl;
if (clicks.begin() == clicks.end()){
   drawcontext->move_to(pt->get_x(),pt->get_y());
clicks.push_back(pt);
 return;
 }
   drawcontext->line_to(pt->get_x(),pt->get_y());
   clicks.push_back(pt);
   drawcontext->stroke_preserve();
 }

  void drawedit_widget::init_connections(){
  draw_connection = clickpencil->coords_signal_draw().connect(sigc::mem_fun(*this, &drawedit_widget::draw_to));
  freedrawer.signal_clicked().connect(sigc::mem_fun(*this, &drawedit_widget::set_free));
  }



  Glib::RefPtr<Gtk::Window> drawedit_widget::create_in_window(Gtk::Window* parent){
    Gtk::Window* win = new Gtk::Window;
    win->set_transient_for(*parent);
  drawedit_widget* widge = new drawedit_widget;
  win->add(*widge);
  widge->show();
  win->set_events(Gdk::ALL_EVENTS_MASK);
  win->signal_button_release_event().connect(sigc::mem_fun(*widge, &drawedit_widget::on_button_release), false);
win->signal_button_press_event().connect(sigc::mem_fun(*widge, &drawedit_widget::on_button_press), false);
  win->show();
  Glib::signal_idle().connect(sigc::mem_fun(*widge, &drawedit_widget::on_init),false);
  return Glib::RefPtr<Gtk::Window>(win);
  }

  bool drawedit_widget::on_expose_event(GdkEventExpose* event){
    //Gtk::Allocation alloc= get_allocation();
    Gtk::VBox::on_expose_event(event);
    /**blanks screen - maybe call buttons exposeevent instead
     *
     */  
if(cairo_inited){   
 drawcontext->restore();
 //drawcontext->set_source_rgb(1,1,1);
      drawcontext->rectangle(event->area.x, event->area.y, event->area.width, event->area.height);
      drawcontext->fill();
      drawcontext->save();
	drawcontext->set_source_rgb(current_r,current_g,current_b);
      if (clicks.begin() != clicks.end()){
	NCOORDS::simple_click_history::iterator walk = clicks.begin();
	drawcontext->move_to((*walk).first.x, (*walk).first.y);
	for (;walk != clicks.end();++walk){
	  drawcontext->line_to((*walk).first.x, (*walk).first.y);
	}
      }
    }
    return true;
  }
