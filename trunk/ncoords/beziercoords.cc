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

#include "beziercoords.h"

using namespace NCOORDS;

void bez_coords::set( coords* pt, coords* b1,coords* b2){
  if (!pt){
  throw xn_Error("pt itself cqannot be NULL");
}
coords::set(*pt);
  if(b1 && b2){

  set_B1(b1);
  set_B2(b2);
    return;
  }
  if(b1){
 set_B1(b1);
 coords t(pt->add((*b1) * -1.0));
 set_B2(&t);
  
    return;
  }
  set_B1(pt);
  set_B2(pt);
}
 bez_coords&  bez_coords::rotate(bez_coords* satellite, double angle){
  bez_coords temp(*satellite);
  coords point_ =  coords::rotate(&temp, angle);
  coords b1(*satellite->get_B1());
  coords b2(*satellite->get_B2());
  coords::rotate(&b1,angle);
 coords::rotate(&b2,angle);
 satellite->set(&point_, &b1, &b2);
 return *satellite;
}

void bez_coords::set(double x, double y){
  coords::set(x,y);
}
