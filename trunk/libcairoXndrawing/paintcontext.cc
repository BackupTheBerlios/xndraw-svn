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

#include "paintcontext.h"

namespace xNDRAW{

 void PaintContext::set_colour(PatternColour* c){
    PatternColour* old=colour;
    colour=c;
    if(old){delete old;}
  }
void PaintContext::set_context(Cairo::RefPtr<Cairo::Context> cont){
    context = cont;
}
  PaintContext::operator bool()const{
    return colour && context;
  }
void PaintContext::set_source_transformation(SourceTransformInfo info){
    if(!colour){
      throw xn_Error("colour not initialised");
    }
    colour->set_transform(info);
  }

void PaintContext::operator()(){
    context->save();
    Cairo::RefPtr<Cairo::Pattern> pattern = colour->create_pattern();
    context->set_source(pattern);
    (*paint_method)(context->cobj());
    context->restore();
    if(do_post_command()){
      filepostcommand(context);
    }
  }

void PaintContext::set_paint_method(paint_method_type p){
    paint_method = p;
  }

  bool operator !(const PaintContext& p){
    bool res=p;
    return !res;
  }
 PaintContext::PaintContext(PatternColour* col, Cairo::RefPtr<Cairo::Context> dest,
	       paint_method_type paint_me ):
    colour(col),
         context(dest),
    paint_method(paint_me){}

PaintContext::PaintContext(PatternColour* col, cairo_t* dest,
	     paint_method_type paint_me ):
  colour(col),
  context(Cairo::RefPtr<Cairo::Context>(new Cairo::Context(dest))),
  paint_method(paint_me){}

  //default ctorMUST BE INITIALISED B4 use: copying etc will cause an error otherwise
  PaintContext::PaintContext():
colour(NULL), context(NULL),paint_method( &cairo_stroke){}

PaintContext::PaintContext(const PaintContext& paintsrc):
    colour(NULL),
    context(paintsrc.context),
    paint_method( paintsrc.paint_method),
    filepostcommand(paintsrc.filepostcommand){
    if(paintsrc.colour){
      colour = paintsrc.colour->copy();
    }
    else{throw xn_Error("src colour NULL");    }
  }

PaintContext& PaintContext::operator=(const PaintContext& paintsrc){
    if (&paintsrc != this){
      PatternColour* old = colour;
      if (paintsrc.colour){
	colour = paintsrc.colour->copy();
      }
      else{throw xn_Error("src colour NULL");    }
      if(old){
	delete old;
      }
      filepostcommand=paintsrc.filepostcommand;
      context = paintsrc.context;
      paint_method= paintsrc.paint_method;
    }
    return *this;
  }


} //NAMESPACE Xndraw
