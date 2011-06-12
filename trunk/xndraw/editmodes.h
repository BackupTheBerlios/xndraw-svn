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

#ifndef XNDRAW_EDITMODES_H
#define XNDRAW_EDITMODES_H
#include "config.h"
#include "defines.h"
#include <glibmm/object.h>

namespace xNDRAW{
  //unused
  

class Mode :public Glib::Object{
typedef int Modality;
Modality mode;

public:
Mode(Modality mode_):mode(mode_){}
virtual ~Mode() = 0;

  Modality get_mode();
 

};
}/*namespace xNDRAW*/

#endif/*XNDRAW_EDITMODES_H*/
