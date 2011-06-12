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


#ifndef TYPEDEFS_H
#define TYPEDEFS_H
#include "config.h"
#include "nodes.h"
#include "nodesmap.h"
#include "clickhistory.h"
#include <set>
#include <deque>
#include <iterator>
//#include <cairomm/context.h>
//#include <cairomm/surface.h>



namespace xNDRAW{

  

  typedef NCOORDS::xn_Error xn_Error;

//cairo refptrs



 typedef NCOORDS::coords coords;
 typedef NCOORDS::ncoords ncoords;
   
 typedef NCOORDS::Nodes Nodes;
 typedef NCOORDS::simple_click_history simple_click_history;

typedef double ratio_t;
 typedef NCOORDS::Nodedeq Nodedeq;

 typedef NCOORDS::deqC deqC;
typedef deqC::iterator deqC_iter;


 typedef std::pair<coords*, int> click_pair_type;
 typedef std::deque<click_pair_type> click_history_t; 



;
 

// typedef std::multimap<int, NCOORDS::ncoords*> MapN;

//typedef MapN::iterator MapN_iter;

}




#endif /*TYPEDEFS_H*/
