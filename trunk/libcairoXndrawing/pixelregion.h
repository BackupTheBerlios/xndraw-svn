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


#ifndef XNDRAW_PIXELREGION_H
#define XNDRAW_PIXELREGION_H

#include "exceptions.h"
#if HAVE_CONFIG_H
#include "config.h"
#endif

#include "coords.h"
#include "drawcolour.h"
#include "blockalloc.h"

#define PIXELREGION_DEFAULT_WIDE \
((int)(std::sqrt(BLOCKALLOC::Blockalloc<xNDRAW::Pixel, 4096>::blocksize()/sizeof(xNDRAW::Pixel))))
#define PIXELREGION_DEFAULT_HIGH PIXELREGION_DEFAULT_WIDE

namespace xNDRAW{
  using namespace BLOCKALLOC;


  class PixelRegion{
  public:
    typedef size_t size_type;
    typedef BlockContainer<Pixel, 4096> container_type;

    protected:
    size_type width;
    size_type height;

    
    BlockContainer<Pixel, 4096>  block;//or child class
    public:

        PixelRegion():width(0), height(0){}
    //change to take a Cairo::ImageSurface,
    //and get its bitmap(so we know the buffer width/height
       PixelRegion(Cairo::RefPtr<Cairo::Surface>   imagebuffer, const NCOORDS::coords& offset, 
		size_type w = PIXELREGION_DEFAULT_WIDE,
		size_type h = PIXELREGION_DEFAULT_HIGH):
      width(w),
      height(h)
    {get_pixels( imagebuffer);}

    void get_pixels(Cairo::RefPtr<Cairo::Surface>   imagebuffer){
	throw xn_Error("unimplemted");
      }


  };

} //namespace xNDRAW{

#endif // XNDRAW_DRAWCOLOUR_H
