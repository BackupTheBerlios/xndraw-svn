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

#include "drawbuffer.h" 
#include "nodes.h"
#include "drawingset.h"

using namespace xNDRAW;
using namespace NCOORDS;


  DrawBuffer::~DrawBuffer(){
    std::cout << "~~~drawbuffer()\n...down plughole" << std::endl;
    
    delete[] data_buffer;
    data_buffer = NULL;
  }  

  Cairo::RefPtr<Cairo::Surface> DrawBuffer::on_alloc_buffer(Gtk::Widget& buffd, cairo_format_t format ){//add a format arg
    buffered = &buffd;
    Gtk::Allocation  allOK = buffered->get_allocation();
    wide = allOK.get_width()*2;
    high = allOK.get_height()*2;
    stride = (cairo_format_stride_for_width(format, wide)); 
    unsigned char* oldbuffer= data_buffer;
    //delete data_buffer;
    data_buffer= new unsigned char[stride * high];
    Cairo::RefPtr<Cairo::ImageSurface> oldsurf = surf;
    //cairo_surface_destroy(surf->cobj());
    if(oldbuffer){
      delete[] oldbuffer;
      oldbuffer = NULL;
    }
    cairo_surface_t* surf_ =cairo_image_surface_create_for_data(data_buffer, CAIRO_FORMAT_ARGB32,wide,high, stride);
    Cairo::ImageSurface* surfy= new Cairo::ImageSurface(surf_, false);
    surf =  Cairo::RefPtr<Cairo::ImageSurface>(surfy);
    if(oldsurf &&oldsurf->cobj()){
      unsigned int refcount = cairo_surface_get_reference_count(oldsurf->cobj());
      for (unsigned int i = 0; i < refcount; i++){
	cairo_surface_destroy(surf->cobj());
      }
    }
    return surf;
  } 

  DrawBuffer::DrawBuffer(Gtk::Widget& buffd,
			 double min, 
			 cairo_format_t f): buffered(&buffd), 
    wide(buffd.get_width()*2),
    high(buffd.get_height()*2),
    stride(cairo_format_stride_for_width(f, wide)),
    format(f),
    drawoption(XNDRAW_DRAWING_SIMPLE_CLICK){
      data_buffer = new unsigned char[stride * high];
      cairo_surface_t* d_surf = cairo_image_surface_create_for_data(data_buffer,format,wide,high, stride);
      Cairo::ImageSurface* surfptr =new Cairo::ImageSurface(d_surf,false);
      surf = Cairo::RefPtr<Cairo::ImageSurface>(surfptr);
#ifdef DEBUG
      std::cout << "exiting drawbuffer constructor"  << std::endl;
#endif //DEBUG
    }


  void DrawBuffer::on_coordsdata_save(){
    std::cout << "####\n  # \ndummy function-void on_coordsdata_save() FIXME\n  # \n ####" << std::endl;
  }

