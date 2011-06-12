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


#include "npatterncoords.h"

using namespace NCOORDS;


npatterncoords::npatterncoords(coords* centr, coords* radia, int n, double angle,double skew_radians):
  ncoords(*centr, *radia,n), 
  skew(skew_radians),
  start_angle(angle)
{}

npatterncoords::npatterncoords(const simple_click_history& patternpath_, 
			       int n , double angle,double skew_radians):
  ncoords((*(patternpath_.begin())).first, (*(patternpath_.end()-1)).first, n),
  skew(skew_radians),
  start_angle(angle),
  patternpath(patternpath_)
{}

npatterncoords::npatterncoords(const simple_click_history& patternpath_,
			       const coords& offset_, int n , 
			       double angle,double skew_radians):
   ncoords(offset_, coords(offset_.x+100,offset_.y+100), n),
   
   skew(skew_radians),
   start_angle(angle),
   patternpath(patternpath_ + offset_){
   //double offset_angle = get_angle(&offset);
 //double pattern_angle = get_angle(&((*(patternpath.begin() + 1)).first));
//get_angle(*(end() -1).first)?
 //patternpath += offset;
  //patternpath.rotate_around_begin(offset_angle + skew -pattern_angle); 
  ncoords::set((coords&)(*this), (*(patternpath.end()-1)).first, n);
 } 


npatterncoords& npatterncoords::operator+=(const coords& added){
   ncoords::translate(&added);
   patternpath += added;
  
   return *this;
 }

simple_click_history npatterncoords::draw_path(int sector,
double startangle){
    /**
@param  sector :0-n where n is the ncoords rotation count
given by get_n()*/
  unsigned int n = get_n();
    sector = sector % n;
    simple_click_history res(patternpath);
    if(!sector){return res;}
    coords ctr = get_centre();
    return res.rotate(ctr, ((2 * M_PI / n) *sector) + startangle);
}
