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

#ifndef XNDRAW_XNSIGNALS_H
#define XNDRAW_XNSIGNALS_H



namespace xNDRAW{
enum Connections{
    XNDRAW_CONNECT_INITIALISATION = 1,
    XNDRAW_CONNECT_COORDS_DRAW = 2,
    XNDRAW_CONNECT_COORDS_FREEDRAW =3,
    XNDRAW_CONNECT_COORDS_SELECT = 4,
    XNDRAW_SIGNAL_PATH_TRANSLATE=5,
    XNDRAW_SIGNAL_PATH_TRANSLATE_PREVIEW=6,
    XNDRAW_SIGNAL_PATH_ROTATE=7,
    XNDRAW_SIGNAL_PATH_ROTATE_PREVIEW=8,
    XNDRAW_SIGNAL_PATH_SCALE=9,
    XNDRAW_SIGNAL_PATH_SCALE_PREVIEW=10
};

  enum EditMode{
    XNDRAW_EDIT_DRAW = 1,
    XNDRAW_EDIT_SELECT = XNDRAW_EDIT_DRAW << 1, 
    XNDRAW_EDIT_VIEW  = XNDRAW_EDIT_SELECT<< 1, 
 
    XNDRAW_DRAWING_SIMPLE_CLICK  = XNDRAW_EDIT_VIEW<< 1,
    XNDRAW_DRAWING_FREEHAND  = XNDRAW_DRAWING_SIMPLE_CLICK<< 1,
   XNDRAW_DRAWING_BEZIER  = XNDRAW_DRAWING_FREEHAND<< 1,
    XNDRAW_SELECT_SIMPLE_CLICK  =  XNDRAW_DRAWING_BEZIER<< 1,
    XNDRAW_SELECT_BOX =  XNDRAW_SELECT_SIMPLE_CLICK<< 1
  };
  
} 


#endif
