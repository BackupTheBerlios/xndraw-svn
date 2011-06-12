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


#ifndef XNDRAW_BUFFERALLOC_H
#define XNDRAW_BUFFERALLOC_H

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <glibmm/object.h>
#include <cairomm/context.h>

namespace xNDRAW{
  
  class refbase_{
  public:
    virtual void reference() const= 0;   
    virtual void unreference() const= 0;
    virtual ~refbase_()=0;    
  };
  
  class Referenced: public refbase_{
    mutable int refcount_;
  public:
    virtual void reference ()const;
    virtual void unreference ()const;
    Referenced() : refcount_(0){reference();}
    Referenced(Referenced& src) : refcount_(src.refcount_) {reference();}
    virtual	~Referenced(){}
    Referenced operator=(Referenced& src){
      refcount_ = src.refcount_;
      return *this;
    }
  };
  
  

  class ImageBuffer: public Glib::Object{
  protected:
    unsigned int width;
    unsigned int height;
    unsigned int bpp;
    size_t buffersize;
    
    unsigned char* data_buffer;

  public:
    //TODO: clean up reallocation function here and CairoBuffer -
    //already exists-problem is anyone externally holding a caiero surface
    //depending on one
virtual    size_t calc_buffersize()const;
    //at the least size aligned to a four kbyte boundary
    static size_t optimise_buffersize(size_t request, size_t page_size=4096);

    //default OK
    //ImageBuffer(const ImageBuffer&); ie copy the pointer
    //ImageBuffer& operator=(const ImageBuffer&);

    ImageBuffer(unsigned int  wide, unsigned int high, unsigned int bpp_ = 32);

    ~ImageBuffer();
    void reallocate();
    void allocate();
    void really_delete_buffer(unsigned char* buf=NULL);
    unsigned char* get_data();

    unsigned int get_width()const{
      return width;
    }

 unsigned int get_height()const{
      return height;
    }

    void resize(unsigned int wide, unsigned int high, bool force = false);

  };

  unsigned int get_bpp_from_cairo_format(cairo_format_t format);
  
  class CairoBuffer: public ImageBuffer{
    cairo_surface_t* surface;
    unsigned int stride;
    cairo_format_t format;

  public:
    CairoBuffer(unsigned int width, unsigned int height, cairo_format_t format =CAIRO_FORMAT_ARGB32 );
    ~CairoBuffer();

    void allocate();
    size_t calc_buffersize()const;
    void reallocate_surface();
    void allocate_surface(cairo_format_t format=CAIRO_FORMAT_ARGB32);
    void really_destroy_surface(cairo_surface_t* s=NULL);
    cairo_surface_t* get_surface();
    void resize(unsigned int, unsigned int, bool unused=true);
    cairo_surface_t*  create_surface();


  };
 
}


#endif
