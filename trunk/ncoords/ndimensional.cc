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

#include "ndimensional.h"

int main(int c, char** v){
double g[4] = {0.,0.,0.,1.};
 NCOORDS::XNglarray<4,0> single(g);
 NCOORDS::XNglarray<4,4> multi(g);
 NCOORDS::VertexData<double,  4, 0> pong(g);
 std::cerr << "element at(2): " << *(pong.at(2)) << " @ " << pong.at(2); 
return 0;
}
