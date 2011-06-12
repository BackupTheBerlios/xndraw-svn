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

#include "imageview.h"
#include "cairopath.h" //CairoError

namespace xNDRAW{



Previewer::Previewer(cairo_surface_t* src, double zoomratio , double initzoom):
  source(Cairo::RefPtr<Cairo::Pattern>(new Cairo::Pattern(cairo_pattern_create_for_surface(src)), false)),
  zoom_ratio(zoomratio),
  init_ratio(initzoom)
{}

Previewer::Previewer(double zoomratio , double initzoom):source(Cairo::RefPtr<Cairo::Pattern>(NULL)),
								zoom_ratio(zoomratio),
								init_ratio(initzoom)
								{}

void Previewer::set_surface(cairo_surface_t* surf){
  source = Cairo::RefPtr<Cairo::Pattern>(new Cairo::Pattern(cairo_pattern_create_for_surface(surf)), false);
  if(imagecontext_gtk){
    imagecontext_gtk->set_source(source);
    imagecontext_gtk->paint();
  }
}
void Previewer::set_zoom_ratio(double newratio_x, double newratio_y){
Cairo::Matrix matrix;
 std::cerr << "zoom set to x = "<<newratio_x<< " and y to "<< newratio_y<<std::endl;
cairo_matrix_init_scale(&matrix, newratio_x, newratio_x);
if(imagecontext_gtk){
imagecontext_gtk->set_matrix(matrix);
 }
}

void Previewer::zoom_IN(){
  if(!imagecontext_gtk){
    return;
  }
  Cairo::Matrix matrix;
 imagecontext_gtk->get_matrix(matrix);
  cairo_matrix_scale(&matrix, zoom_ratio, zoom_ratio);
  imagecontext_gtk->set_matrix(matrix);
}

void Previewer::zoom_OUT(){
  if(!imagecontext_gtk){
    return;
  }
  Cairo::Matrix matrix;
 imagecontext_gtk->get_matrix(matrix);
  cairo_matrix_scale(&matrix, 1 / zoom_ratio, 1 / zoom_ratio);
  imagecontext_gtk->set_matrix(matrix);
}

bool Previewer::initialise(){
Glib::RefPtr<Gdk::Drawable> window = get_window(); 
  if(!window){
    return false;
  }
  GdkDrawable* surf = window->gobj();    
  cairo_t* cr = cairo_reference(gdk_cairo_create(surf));
  if(!cr){
    throw xn_Error("cairo_t* cr is NULL");
  }
  cairo_status_t status = cairo_status(cr);
  if (status != CAIRO_STATUS_SUCCESS){
    throw CairoError("cairo_t* cr has error:", status);
  }
  imagecontext_gtk = context_type(new Cairo::Context(cr, true));
  Cairo::Matrix matrix;
  cairo_matrix_init_scale(&matrix, init_ratio, init_ratio);
  imagecontext_gtk->set_matrix(matrix);
  imagecontext_gtk->set_source(source);
  imagecontext_gtk->paint();
  Glib::signal_timeout().connect(sigc::mem_fun(*this, &Previewer::paint), 1000);//once a second
  return true;
}
bool Previewer::on_expose_event(GdkEventExpose* e){
  Gtk::DrawingArea::on_expose_event(e);
  if(imagecontext_gtk){
    imagecontext_gtk->paint();
  }
  return true;
}

bool Previewer::paint(){
if(imagecontext_gtk){
    imagecontext_gtk->paint();
    return true;
  }
 return false;
}

int IconButton::buttoncount = 0;


void IconButton::create_pixbuf(const char* fname){
#ifdef GLIBMM_EXCEPTIONS_ENABLED
      //borrowed from GLIBMM_DOCS/examples/book/iconview
  try{
      icon = Gdk::Pixbuf::create_from_file(fname);
  }
  catch(const Glib::FileError& error){
    std::cerr<<"while attempting to create icon from file:" << fname
	     <<"\ncaught: Glib::FileError: " << error.what() << std::endl;
    icon = Glib::RefPtr<Gdk::Pixbuf>(NULL);
  }
#else
      std::auto_ptr<Glib::Error> error;
      icon = Gdk::Pixbuf::create_from_file(fname, error);
#endif //GLIBMM_EXCEPTIONS_ENABLED
      if(icon){

	Gtk::Image* image = Gtk::manage(new Gtk::Image(icon));
	set_image(*image);
      }
      else{
#ifdef GLIBMM_EXCEPTIONS_ENABLED
	//went bang already
#else
	std::cerr <<"Icon/button:An error occurred setting image fron file: "<< error.what()<<std::endl;
#endif
	Gtk::BuiltinStockID iconid;
	switch(buttoncount){
	case 1:
	  iconid = Gtk::Stock::MEDIA_PAUSE;
	  break;
	case 2:
	  iconid = Gtk::Stock::MEDIA_PLAY;
	  break;
	case 3:
	  iconid = Gtk::Stock::MEDIA_RECORD;
 break;
	case 4:
	  iconid = Gtk::Stock::MEDIA_REWIND;
	  break;
	case 5:
	  iconid = Gtk::Stock::MEDIA_FORWARD;
	  break;
	case 6:
	  iconid = Gtk::Stock::MEDIA_PREVIOUS;
	  break;
	default:
	  iconid = Gtk::Stock::MEDIA_NEXT;
	  break;
	}
	Gtk::Image* image = Gtk::manage(new Gtk::Image(iconid,Gtk::ICON_SIZE_SMALL_TOOLBAR ));
	set_image(*image);
      }
    }

}
