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


#ifndef LIBCAIROXNDRAWING_PAINTCONTEXT_H
#define LIBCAIROXNDRAWING_PAINTCONTEXT_H 1

#include "exceptions.h"

#if HAVE_CONFIG_H
#include "config.h"
#endif



#include <cairomm/context.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "imagefile.h"
#include "drawcolour.h"

namespace xNDRAW{
class PaintContext;
 
  bool operator !(const PaintContext&);

class PaintContext{
public:
  typedef void(*paint_method_type)(cairo_t*);
operator bool()const;
  friend bool operator !(const PaintContext&);
protected:
  PatternColour* colour;
Cairo::RefPtr<Cairo::Context> context;
  paint_method_type paint_method;
  FileCommandParams filepostcommand;

public:
  PaintContext(PatternColour* col, Cairo::RefPtr<Cairo::Context> dest,
	       paint_method_type paint_me = &cairo_stroke);
  PaintContext();
PaintContext(PatternColour* col, cairo_t* dest,
	       paint_method_type paint_me = &cairo_stroke);
  
  PaintContext(const PaintContext& paintsrc);

  PaintContext& operator=(const PaintContext& paintsrc);

  ~PaintContext(){delete colour;}

  Cairo::RefPtr<Cairo::Context> get_context()const{return context;}

  void set_colour(PatternColour* c);
  void set_context(Cairo::RefPtr<Cairo::Context> cont);
 
  void set_paint_method(paint_method_type p);

    void set_source_transformation(SourceTransformInfo info);

    void operator()();

  void set_post_command(const FileCommandParams params){
    filepostcommand=  params;
  }

  bool do_post_command()const{
    return filepostcommand.do_post_command();
}
 bool do_pre_command()const{
    return filepostcommand.do_pre_command();
}
};
  


} //namespace xNDRAW

#endif
