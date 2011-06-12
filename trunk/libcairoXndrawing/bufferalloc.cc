// -*- C++ -*-

/* Copyright (C)2006-2011 John O'Driscoll - biggerbumme@ yahoo.co.in
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#include "xncairoerror.h"
#include "bufferalloc.h"

namespace xNDRAW{

refbase_::~refbase_() {}

unsigned int get_bpp_from_cairo_format(cairo_format_t format){
  if (format == CAIRO_FORMAT_ARGB32 ) return 32;
  if (format == CAIRO_FORMAT_RGB24 ) return 24;
  if (format == CAIRO_FORMAT_A8 ) return 8;
  if (format == CAIRO_FORMAT_A1 ) return 1;
  throw xn_Error("format unknown");
}


ImageBuffer::ImageBuffer(unsigned int  high, unsigned int wide, unsigned int bpp_):
  width(wide),
  height(high),
  bpp(bpp_),
  buffersize (calc_buffersize()),
  data_buffer(NULL){
  if(width){
    this->allocate();
  }
}

size_t ImageBuffer::calc_buffersize()const{
  size_t res=0;
  if (bpp ==32 || (bpp !=1 && !(width % 8))){
    res= width * height * bpp / 8;//result divisible by 4
  }
  else{
    size_t realwide = ((width /8)+1) * 8;
    size_t res= realwide * height * bpp / 8; //bytes
  }
  return optimise_buffersize(res);
}
size_t CairoBuffer::calc_buffersize()const{
  size_t res = optimise_buffersize(stride * height);
  return res;

}

size_t ImageBuffer::optimise_buffersize(size_t request, size_t page_size){
  //align to swapper memory page size,
  //since even 200x200 bitmap=5000bytes, and who uses bitmaps?
  if(request % page_size){
    request = ((request/page_size) + 1)* page_size;
  }
  return request;
}

ImageBuffer::~ImageBuffer(){
  really_delete_buffer();
}


    
void ImageBuffer::allocate(){
  unsigned char* oldbuffer = data_buffer;
  buffersize = calc_buffersize();
  data_buffer = new unsigned char[buffersize];
  if (!data_buffer){
    data_buffer = oldbuffer; //cleaned up in destructor?
    throw xn_Error("no memory to allocate Imagebugger  -, er, Buffer");
  }

}
void ImageBuffer::reallocate(){
  unsigned char* oldbuffer = data_buffer;
  allocate();
  if(oldbuffer){
    really_delete_buffer(oldbuffer);
  }
}

void CairoBuffer::resize(unsigned int wide, unsigned int high, bool force ){
  width = wide;
  height = high;
  stride=cairo_format_stride_for_width(format, wide);
  if(buffersize < calc_buffersize()){
    buffersize = calc_buffersize();
    reallocate_surface();
  }
}
void ImageBuffer::resize(unsigned int wide, unsigned int high, bool force ){
  width = wide;
  height = high;
  if(force || buffersize < calc_buffersize()){
    //pass a value and set 'buffersize' in allocate()?
    buffersize =  calc_buffersize();
    this->reallocate();
  }
}

unsigned char* ImageBuffer::get_data(){
  return data_buffer;
}

CairoBuffer::CairoBuffer(unsigned int width, unsigned int height, cairo_format_t form ):
  ImageBuffer(width,height,get_bpp_from_cairo_format(form)),
  surface(NULL),
  stride(cairo_format_stride_for_width(form, width)),
  format(form){
  allocate_surface(form);
  }

CairoBuffer::~CairoBuffer(){
  really_destroy_surface(surface);
  if(data_buffer){
    delete[] data_buffer;;
    data_buffer =NULL;
  }
}

void ImageBuffer::really_delete_buffer(unsigned char* buf){
  bool deleting_data_buffer(false);
  if(!buf){
    buf=data_buffer;
    deleting_data_buffer=true;
  }
  if(buf){
    delete[] buf;
  }
  if(deleting_data_buffer){
    data_buffer =NULL;
  }
}

void CairoBuffer::really_destroy_surface(cairo_surface_t* s){

  if( s && cairo_surface_status(s)==CAIRO_STATUS_SUCCESS){
    unsigned int refcount = cairo_surface_get_reference_count(s);
    for (unsigned int i = 0; i < refcount; i++){
      cairo_surface_destroy(s);
    }
  }
}
  cairo_surface_t* CairoBuffer::get_surface(){
    if(!surface){
      throw xn_Error("NULL");
    }
    return surface;
  }
  void CairoBuffer::reallocate_surface(){
    unsigned char* oldbuffer =data_buffer;;
    bool did_alloc(false), do_alloc(false);
    cairo_surface_t* oldsurface = surface;
    if(buffersize < calc_buffersize()){
      buffersize = calc_buffersize();
      do_alloc=true;
    }
    if(!data_buffer||do_alloc){
      allocate();
      did_alloc = true;
    }
    else if(surface 
	    && cairo_surface_status(surface) == CAIRO_STATUS_SUCCESS){return;}
    surface =  create_surface();
    if(did_alloc){
      if(oldsurface){
	really_destroy_surface(oldsurface);
      }
      if(oldbuffer){
	really_delete_buffer(oldbuffer);
      }
    }
  }
  cairo_surface_t*  CairoBuffer::create_surface(){

    cairo_surface_t* surf = cairo_surface_reference(cairo_image_surface_create_for_data (data_buffer, format,  width, height, stride));//phew!
    if(!surf || cairo_surface_status(surf) != CAIRO_STATUS_SUCCESS){
      throw CairoError("error creating Surface for allocated buffer!", 
		       cairo_surface_status(surf));
    }
    std::cerr<<"surface refcount after cairo_surface_reference(cairo_image_surface_create_for_data (...): "
	     << cairo_surface_get_reference_count(surf)<<std::endl;
    return surf;
  }

  void CairoBuffer::allocate_surface(cairo_format_t format){
    if(surface ){
      throw xn_Error("call reallocate surface() if surface already exists");
    }
    if(data_buffer){really_delete_buffer();}//in case ImageBuffer allocated one, delete it
    if(buffersize != calc_buffersize()){
      buffersize = calc_buffersize();
    }
    allocate();
    surface = create_surface();
  }

  void CairoBuffer::allocate(){
    ImageBuffer::allocate();
  }

  void Referenced::reference()const{
    ++refcount_;
  }

  void Referenced::unreference()const{
    if (!(--refcount_)){
      delete this;
    }
  }





}
