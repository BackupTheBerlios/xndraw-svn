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


#ifndef XNDRAW_IODEFINES_H
#define XNDRAW_IODEFINES_H

#include "config.h"
//#include "x3draw.h"
#include "clickpopper_threads/io_uncollide.h"

namespace xNDRAW{

#define xNDRAW_IS_STRING(exp) No_iostream_collision::get_instance().print_string(exp);

#define STREAMOUT(ostringstream) xNDRAW::No_iostream_collision::get_instance().std_out(ostringstream)
}

#endif//XNDRAW_IODEFINES_H
