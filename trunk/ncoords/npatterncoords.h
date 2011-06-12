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


#ifndef NCOORDS_NPATTERNCOORDS_H
#define NCOORDS_NPATTERNCOORDS_H

#include "xndrawexceptions.h"

#if HAVE_CONFIG_H
#include "config.h"
#endif
#include "clickhistory.h"

namespace NCOORDS{

class npatterncoords: public ncoords{
  //TODO: skew
protected:
  double skew;//?
  double start_angle;
  simple_click_history patternpath;//could wrap a pointer for xtra flrxibility
public:
  npatterncoords(coords* centr, coords* radia, int n=60, double start_angle=0., double skew = 0.);
  virtual ~npatterncoords(){}
  npatterncoords(const simple_click_history& patternpath_, int n = 60, double start_angle=0., double skew = 0.);

 npatterncoords(const simple_click_history& patternpath_,
		const coords& offset_, int n =60,double start_angle= 0.0, double skew_radians=0.0);

 npatterncoords& operator+=(const coords& added);

  virtual simple_click_history draw_path(int sector,
					 double starrtangle = 0.0);

  void update_path_from_mirror(const simple_click_history& mirror){
    simple_click_history::size_type sz= patternpath.size();

    if (sz < mirror.size()){
      for(simple_click_history::const_iterator w = mirror.begin()+sz;w !=mirror.end();w++){
	patternpath.push_back(*w);
      }
    }
  }

  void clear_path(){patternpath.clear();}
 /**

  *@param added the offset to add
  *
  */
};

  

}

#endif /*NCOORDS_NPATTERNCOORDS_H*/
