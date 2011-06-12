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



#ifndef NCOORDS_BEZIERCOORDS_H
#define NCOORDS_BEZIERCOORDS_H

#include "xndrawexceptions.h"
#if HAVE_CONFIG_H
#include "config.h" 
#endif


#include "coords.h"
namespace NCOORDS{
class bez_coords: public coords{


  coords B1;
coords B2;
 public:
 bez_coords(){ }
 bez_coords(coords* pt,coords* handle1,coords* handle2):coords(*pt),B1(*handle1), B2(*handle2){ }

   virtual ~bez_coords(){ }

 coords* get_point(){
   return (coords*)this;
 }
 const coords* get_B1() const{
   return &B1;
 }
 const NCOORDS::coords* get_B2() const{
   return &B2;
 }

coords* get_B1() {
   return &B1;
 }
 NCOORDS::coords* get_B2() {
   return &B2;
 }
 void set(double x, double y);

 inline void set_B1(coords* b1){
  B1 = *b1;
 }
 inline void set_B2(coords* b2){
 B2 = *b2;
 }
 void set(coords* pt, coords*b1 = NULL, coords*b2 = NULL);

  void set(const bez_coords& bez){
    coords::set(bez);
    B1.set(*(bez.get_B1()));
    B2.set(*(bez.get_B2()));
  }

virtual bez_coords&  rotate(bez_coords* satellite, double angle);

  };//class bez_coords

 

}//namespace NCOORDS;

#endif//NCOORDS_BEZIERCOORDS_H
