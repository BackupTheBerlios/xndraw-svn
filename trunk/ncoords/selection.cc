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



#include "selection.h"

using namespace NCOORDS;

GraphicElementBase::size_type GraphicElementBase::element_count =0;
using namespace NCOORDS;
/*
Selection::Selection(NCOORDS::simple_click_history& line, bool select_all){
     for(NCOORDS::simple_click_history::iterator walk = line.begin(); walk != line.end(); walk++){
       insert(end(),std::pair<bool, Point_>(select_all,*walk));
     }
}

     std::vector<NCOORDS::coords*> Selection::get_selected(){
    std::vector<NCOORDS::coords*> result;
    SelectionMap  map(*this);
    std::pair<std::multimap<bool,Point_>::iterator, std::multimap<bool,Point_>::iterator> range = map.equal_range(true);
 for (;range.first != range.second; (range.first)++){
   selectablePoint_ x = *(range.first);
   result.push_back(x.second.get_coords());
 }
 return result;
  }
    
int Selection::find_closest(coords& selector){
      int n = -1;
      iterator walk = begin();
      if (walk == end()){return n;}
      coords* closest = (*walk).second.get_coords();
      double min_dist = selector.get_distance(closest);    
      for (int i = 0; walk != end(); walk++){

	double this_dist = selector.get_distance((*walk).second.get_coords());    
	if( this_dist < min_dist){
	  n = i;
	}
	i++;
      }
      return n;
    }

void Selection::add_selection(NCOORDS::coords* nearest, bool add_to_selection){
  iterator walk = begin();
  if (! add_to_selection){
    for(; walk != end(); walk++){
      (*walk).first  = false;
    }
  }
  int ordinal = find_closest(*nearest);
  if (ordinal < 0){
      std::cerr << "empty map: no points to select from!!!!" << std::endl;
      return;
  }
  walk =  begin();
  if((*(walk + ordinal)).first == true){
    (*(walk + ordinal)).first =  false;
  }
  else {
    (*(walk +  ordinal)).first = true;
  }
}
*/ 
