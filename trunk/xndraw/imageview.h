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


/**
 * @file imageview.h
 * widgets with images
 */


#ifndef    XNDRAW_IMAGEVIEW_H
#define   XNDRAW_IMAGEVIEW_H
#include "exceptions.h"
#if HAVE_CONFIG_H
#include "config.h"
#endif


#include <gtkmm.h>
#include <cairomm/context.h>
#include "xndrawexceptions.h"
#include "typedefs.h"

namespace xNDRAW{
  
  using namespace NCOORDS;
  
  template <class WINDOWTYPE>
    class Xndraw_Windowfactory;
  class Previewer :public Gtk::DrawingArea{
  public:
    typedef Cairo::RefPtr<Cairo::Context> context_type;
    typedef  Cairo::RefPtr<Cairo::Pattern > pattern_type;
    
  protected: 
    pattern_type source;
    double zoom_ratio;
    double init_ratio;
    context_type imagecontext_gtk;
   
  public:
    Previewer(cairo_surface_t* src, double zoomratio = 1.25, double initzoom = 0.15);
    Previewer(double zoomratio = 1.25, double initzoom = 0.15);
    
    friend class Xndraw_Windowfactory<Previewer>;
    bool initialise(); 
    
    void set_surface(cairo_surface_t* surf);
    bool on_expose_event(GdkEventExpose* e);
    bool on_drag_scale(const cairo_rectangle_t& box);
    bool on_scalebutton_press(GdkEventButton* e);
    bool on_scalebutton_release(GdkEventButton* e);
    void set_zoom_ratio(double newratio_x, double newratio_y);
    
    void zoom_IN();
    void zoom_OUT();
    bool paint();
  }; //class Previewer
  
  template <class WINDOWTYPE>
    class Xndraw_Windowfactory{
  public:
    Xndraw_Windowfactory(){}
  };
  /*
//some alternatives
  const Gtk::BuiltinStockID 	MEDIA_FORWARD
const Gtk::BuiltinStockID 	MEDIA_NEXT
const Gtk::BuiltinStockID 	MEDIA_PAUSE
const Gtk::BuiltinStockID 	MEDIA_PLAY
const Gtk::BuiltinStockID 	MEDIA_PREVIOUS
const Gtk::BuiltinStockID 	MEDIA_RECORD
const Gtk::BuiltinStockID 	MEDIA_REWIND
ie
Gtk::Stock::MEDIA_PAUSE; etc
sizes
Gtk::ICON_SIZE_SMALL_TOOLBAR 	
Gtk::ICON_SIZE_LARGE_TOOLBAR 
  */


  class IconButton:public Gtk::Button{
    Glib::RefPtr<Gdk::Pixbuf> icon;
    int ID; //tool ids SIMPLE_CLICK etc
    static int buttoncount;
    //Gtk::Image* image;
  public:
    virtual ~IconButton(){buttoncount--;}
    IconButton():icon(NULL), ID(-1){buttoncount++;}
      IconButton(const char* iconfname, int id):icon(NULL), ID(id){buttoncount++;
      create_pixbuf(iconfname);
    }

	bool initialised()const {return ID > 0;}
	int get_ID()const{return ID;}
	void set(const char* iconfname, int id){
create_pixbuf(iconfname);
 ID = id;
    }

	void set_slot(sigc::slot0<void> slot){
	  signal_clicked().connect(slot);
	}

  protected:
	void create_pixbuf(const char* fname);

  };
} //namespace xNDRAW

#endif //XNDRAW_IMAGEVIEW_H
