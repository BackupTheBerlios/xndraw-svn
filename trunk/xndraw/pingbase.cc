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


#include "pingbase.h"

namespace xNDRAW{
  cairo_t* ping_base::get_gdkcairo(){
Glib::RefPtr<Gdk::Drawable> window = get_window(); 
  if(!window){
    throw xn_Error("Gdk::Window* is NULL - ping_window not actualised and visible yet");
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
  std::cerr <<"refcount of new gdkcairo:"
	    <<cairo_get_reference_count(cr)<<std::endl;
  return cr;
  }

 Cairo::RefPtr<Cairo::Context> ping_base::get_gdkcairo_ref(){
   cairo_t* cr= get_gdkcairo();
      Cairo::Context* p_cr= new Cairo::Context(cr);
      if(!p_cr){
	throw xn_Error(" new Cairo::Context NULL");
      }
      return Cairo::RefPtr<Cairo::Context>(p_cr);
    }

void ping_base::reallocate_drawbuffer(Gtk::Allocation& alloc){
  
  //fix - really call the function
//danger - if some other object has a pointer to internal data
  std::cerr <<"reallocate_drawbuffer called"<<std::endl;
  //set width/height/stride on cbuffer   
  //buffer.allocate_surface();
  // set draws buffer to NULL
  // determine draws type, get it's history, asnd whatwever
  //delete it and the surface from buffer
  
  //buffer.reallocate();

}
bool ping_base::needs_resize(const Gtk::Allocation& alloc)const{
    if(( bufferflag & PINGBUFFER_RESIZE_NEVER)||( bufferflag ==0)){
      return false;
    }
    if(bufferflag & PINGBUFFER_RESIZE_LARGER){
      if((size_t)alloc.get_width() > (size_t)buffer.get_width()
	 || (size_t)alloc.get_height() > (size_t)buffer.get_height()){
	return true;
      }
      //fall into next condition
    }
    if( bufferflag & PINGBUFFER_RESIZE_SMALLER){
	if((size_t)alloc.get_width() < (size_t)buffer.get_width()
	   || (size_t)alloc.get_height() < (size_t)buffer.get_height()){
	  return true;
	}
      }
    return false;
}

CairoBuffer& ping_base::get_buffer() { return  buffer;}
const CairoBuffer& ping_base::get_buffer()const { return  buffer;}

Cairo::RefPtr<Cairo::Context> ping_base::get_buffer_context(){
 cairo_surface_t* surface = get_buffersurface();
  if(!surface){throw xn_Error("NULL :cairo_surface_t* surface = get_buffersurface();");}
Cairo::Surface* p_surf = new Cairo::ImageSurface(surface);
 if(!p_surf){
   throw xn_Error("new Cairo::Surface(surface) returbed NULL");
 }
Cairo::RefPtr<Cairo::Surface> ref_surf(p_surf);
 return Cairo::Context::create(ref_surf);
 }

  Cairo::RefPtr<Cairo::Surface> ping_base::get_buffer_surface_ref(){
 cairo_surface_t* surface = get_buffersurface();
 if(!surface){throw xn_Error("NULL :cairo_surface_t* surface = get_buffersurface();");}
Cairo::Surface* p_surf = new Cairo::Surface(surface);
 if(!p_surf){
   throw xn_Error("new Cairo::Surface(surface) returbed NULL");
 }
 return Cairo::RefPtr<Cairo::Surface>(p_surf);
 }
bool ping_base::on_expose_event(GdkEventExpose* event){
  return Gtk::DrawingArea::on_expose_event(event);
}

void ping_base::on_size_allocate(Gtk::Allocation& alloc){
  //size_t suppress signed/unsigned warning
  if(needs_resize(alloc)){
    //TODO: determine if there are still references to the cairo
    reallocate_drawbuffer(alloc);
  }
  Gtk::DrawingArea::on_size_allocate(alloc);
}

void ping_base::clear_buffer_to_bg(){
Cairo::RefPtr<Cairo::Context> bufo=get_buffer_context();
 if(!bufo){throw xn_Error("bad buffer provides null context");}
 bufo->save();
 bufo->set_source_rgb(1,1,1);
 bufo->paint();
 bufo->restore();
    }
  void ping_base::paint_buffer_to_window(){
if(!get_is_drawable()){return;}//nothing ready
Cairo::RefPtr<Cairo::Surface> surf= get_buffer_surface_ref();
 if(!surf){throw xn_Error("buffersurface NULL");}
PatternColour buffer_output(surf);
    Cairo::RefPtr<Cairo::Pattern> patternsource=  buffer_output.create_pattern();
    Cairo::RefPtr<Cairo::Context> context= get_gdkcairo_ref();
    if(!context){throw xn_Error("cairo NULL");}
    context->save();
    context->set_source(patternsource);
    context->paint();
    context->restore();
  }
    
  void ping_base::set_source_from_buffer(Cairo::RefPtr<Cairo::Context> buffercontext){
    if(!get_is_drawable()){return;}//nothing ready
    Cairo::RefPtr<Cairo::Surface> surf(NULL);
    std::cerr <<"ping_base::set_source_from_buffer()"<<std::endl;
    if(buffercontext){
      surf=       buffercontext->get_target();	
    }
    else{
      surf = get_buffer_surface_ref();
    }
    if(!surf){throw xn_Error("buffercontext.get_target() => NULL");}
    //Cairo::RefPtr<Cairo::Pattern> pattern = Cairo::SurfacePattern::create(surf);
    PatternColour buffer_output(surf);
    Cairo::RefPtr<Cairo::Pattern> patternsource=  buffer_output.create_pattern();
    Cairo::RefPtr<Cairo::Context> context= get_gdkcairo_ref();
    if(!context){throw xn_Error("cairo NULL");}
    context->set_source(patternsource);
  }

} //namespace xNDRAW
