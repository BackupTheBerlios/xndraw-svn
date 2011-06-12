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

#ifndef XNDRAW_PINGBUFFER_H
#define XNDRAW_PINGBUFFER_H
#include "xndrawexceptions.h"

#if HAVE_CONFIG_H
#include "config.h"
#endif

//#include "hframe.h"
#include "drawbuffer.h"
#include "ping_window.h"
#include "drawdeque.h"


//TODO: add drawbuffer->get_draw() to Paintable* draw in initialisation
namespace xNDRAW{
  class h_frame;
  
  class ping_buffered: public ping_window{
    h_frame* container_widget;
   
    double minimum;
    
    
  public:
    ping_buffered(h_frame& parent, int base_no=3, double min=2.0) ;
     
   virtual ~ping_buffered() ;
   void init();
   
 Gtk::Window* get_parent_window(){
   return (Gtk::Window*)container_widget;
 }
virtual bool on_motion_what(GdkEventMotion* event){
  static int n=0;//this value not important from threadsafety pov//just to ration print statements
  if(n++ >300){
    n=0;
      std::cout<<"ping_window::on_motion_what(GdkEventMotion* event)" << std::endl;
  }
      return true;
    }
    virtual void on_draw_import_paths(std::string fname);
   void on_clip_test(); //rewrite to work out how clipping path works
  
    void on_svgpathsread(std::string fname);
   void on_preinit_coordset();
 
 double get_movement_threshhold(){
   return minimum;
 }

 void on_coordsdata_read(); //need a parser class
//overloads   
    void save_history_as_datastream();
   void read_history_from_datastream();
    void init_drawbuffer();

    bool load_background();

};
}
#endif /*XNDRAW_PINGBUFFER_H*/
