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




#include "basecoords.h"
#include "coords.h"

namespace NCOORDS{

  double       basecoords::atom_limit = 5.0;

  bool click_selected(const std::pair<coords, int>& pt){
    return pt.second & CLICK_SELECTED;
  }
  bool drawing_click(const std::pair<coords, int>& pt){
    return pt.second & DRAWING_CLICK;
  }
bool move_click(const std::pair<coords, int>& pt){
    return pt.second & MOVE_CLICK;
  }

  bool handle_click(const std::pair<coords, int>& pt){
    return pt.second & BEZIER_HANDLE;
  }
  bool closepath_click(const std::pair<coords, int>& pt){
    return pt.second & CLOSE_PATH;
  }
  bool cairo_click(const std::pair<coords, int>& pt){
    return pt.second & DRAWING_ELEMENT;
  }
  bool is_drawing_element(PointerMode pointtype){return pointtype & DRAWING_ELEMENT;}
  
std::string clicktype_as_string(int cliktype){
  std::string s;
  if(cliktype==  NULL_CLICK){
    return "NULL_CLICK";
  }
  else if(cliktype & LINE_CLICK){
    s= "LINE_CLICK";
  }
  else if(cliktype &  MOVE_CLICK){
    s= "MOVE_CLICK";
  }
  else if(cliktype & BEZIER_CLICK){
    s= "BEZIER_CLICK";
  }
  else if(cliktype & BEZIER_HANDLE){
    s= "BEZIER_HANDLE";
  }
  else if(cliktype & CLOSE_PATH){
    s= "CLOSE_PATH";
  }
  else if(cliktype & ORIGIN_CLICK){
    s= "ORIGIN_CLICK";
  }
  else if(cliktype & OFFSET_CLICK){
    s= "OFFSET_CLICK";
  }
  else if(cliktype & XNDRAW_NO_CLICKDATA){
    s = "XNDRAW_NO_CLICKDATA";
  }
  else if(cliktype & XNDRAW_POINTER_SELECTCLICK){
    s = " XNDRAW_POINTER_SELECTCLICK";
  }
  else if(cliktype & XNDRAW_POINTER_SELECTBOX_BUTTONDOWN){
    s = " XNDRAW_POINTER_SELECTBOX_BUTTONDOWN";
  }
  else if(cliktype & XNDRAW_POINTER_SELECTBOX_DRAGMOTION){
    s = "XNDRAW_POINTER_SELECTBOX_DRAGMOTION";
  }
  else if(cliktype & XNDRAW_POINTER_SELECTBOX_BUTTONUP){
    s= " XNDRAW_POINTER_SELECTBOX_BUTTONUP(error)";
  }
  else if(cliktype & XNDRAW_POINTER_SELECTGRID){
    s= "XNDRAW_POINTER_SELECTGRID(error)";
  }
  if(cliktype & CLICK_TRANSFORM_START){
    s += " - CLICK_TRANSFORM_START";
  }
  if(cliktype & CLICK_TRANSFORM_CONTINUE ){
    s += " - CLICK_TRANSFORM_CONTINUE";
  }
  if(cliktype & CLICK_TRANSFORM_FINISH ){
    s += " = CLICK_TRANSFORM_FINISH";
  }
  if(cliktype & CLICK_TRANSLATE ){
    s += " = CLICK_TRANSLATE";
  }
  if(cliktype & CLICK_SCALE){
    s += " = CLICK_SCALE";
  }
  if(cliktype & CLICK_ROTATE){
    s += "CLICK_ROTATE";
  }
  if(cliktype & CLICK_ERROR){
    s += " - CLICK_ERROR";
  }
  if(cliktype & CLICK_SELECTED){
    s += " - CLICK_SELECTED";
  }
  return s;
}


  bool valid_drawing_element(PointerMode pointtype){
    if(pointtype & DRAWING_ELEMENT){
      int badflags = DRAWING_ELEMENT;
      if(pointtype & LINE_CLICK){badflags ^=LINE_CLICK;}
      else if (pointtype & MOVE_CLICK){badflags ^=MOVE_CLICK;}
      else if (pointtype & BEZIER_CLICK){badflags ^=BEZIER_CLICK;}
      else if (pointtype & BEZIER_HANDLE){badflags ^=BEZIER_HANDLE;}
      else if (pointtype & CLOSE_PATH){badflags ^=CLOSE_PATH;}
      else{ 
	throw xn_Error("definition of DRAWING_ELEMENT changed without update this function(NCOORDS::valid_drawing_element(PointerMode pointtype)");
      }
      return !(pointtype & badflags);
    }
    std::cerr<<" valid_drawing_element(): not a drawing element at all, instead :\n"
	     << clicktype_as_string(pointtype) <<std::endl;
    return false;
  }

  coord_state operator^=(coord_state& first, coord_state second){
    first=(coord_state)(first ^ second);
    return first;
  }
  
  coord_state operator|=(coord_state& first, coord_state second){
    first=(coord_state)(first | second);
    return first;
  }
  
  coord_state operator&=(coord_state& first, coord_state second){
    first= (first & second);
    return first;
  }
  coord_state operator|(coord_state first, coord_state second){
    int subres=(int)first | (int)second;
   return (coord_state) subres ;
  }
coord_state operator^(coord_state first, coord_state second){
  int subres=(int)first ^ (int)second;
   return (coord_state) subres ;
   
  }
  coord_state operator&(coord_state first, coord_state second){
    int subres=(int)first & (int)second;
   return (coord_state) subres ;
  }
  basecoords::basecoords(coord_state st ):
    state(st)
  {
#ifdef COUNT_COORDS
    serial_no = ++total_count;
    ++c_count;
	  
#endif // COUNT_COORDS
  }

  basecoords::basecoords(const basecoords& src)
    :
    state(src.state)
  {
#ifdef COUNT_COORDS
    total_count++;
    ++c_count;
	    
#endif // COUNT_COORDS
  }

  basecoords& basecoords::operator=(const basecoords& src){
    if(&src != this){
      state = src.state;
    }
    return *this;
  }


#ifdef COUNT_COORDS
  unsigned int basecoords::c_count = 0;
  unsigned int basecoords::d_count = 0;
  unsigned int basecoords::total_count = 0;
#endif // COUNT_COORDS

  double basecoords::get_atomlimit(){
    return atom_limit;
  }

  void basecoords::set_atomlimit(double atom){
    atom_limit = atom;
  }

  coord_state basecoords::get_state ()const{
    return state;
  }
  bool basecoords::selected()const{
    return state & COORDS_SELECTED;
  }
  void basecoords::select(){
    if (!selected()){
      int stat = state | COORDS_SELECTED;
      state = (coord_state)stat;
    }
  }

  void basecoords::deselect(){
    if (selected()){
      int stat = state ^ COORDS_SELECTED;
      state = (coord_state)stat;
    }
  }

  void basecoords::set_updownstate(int value){
    set_stateflag(COORDS_UP, (bool)value);
  }

  coord_state basecoords::flip_stateflag(coord_state stat){
    int s_tmp = state ^ stat;
    state =(coord_state) s_tmp;
    return state;
  }

  coord_state basecoords::unset_stateflag(coord_state stat){
    if(state & stat){
      int s_tmp = state ^ stat;
      state =(coord_state) s_tmp;
    }
    return state;
  }

  coord_state basecoords::set_stateflag(coord_state stat, bool value){
    if(value){
      set_stateflag(stat);
    }
    else{ unset_stateflag(stat);}
    return state;
  }

  coord_state basecoords::set_state(coord_state stat){
    state = stat;
    return state;
  }
  coord_state basecoords::set_stateflag(coord_state stat){
    int s_tmp =      state | stat;
    state = (coord_state)s_tmp;
    return state;
  }

  void basecoords::transfer_selection_to_pathclick(std::pair<coords, int>& pointpair,
						   bool XORed, 
						   bool clear_source){
    if(pointpair.first.selected()){
      if( XORed){
	pointpair.second ^=CLICK_SELECTED;
      }
      else{
	pointpair.second |=CLICK_SELECTED;
      }
      if(clear_source){
	pointpair.first.deselect();
      }
    }
  }

  void basecoords::transfer_selection_from_pathclick(std::pair<coords, int>& pointpair,
						     bool XORed , 
						     bool clear_source){
    if(pointpair.second &CLICK_SELECTED ){
      if( XORed & pointpair.first.selected()){	 
	pointpair.first.deselect();
      }
      else{pointpair.first.select();}
      if(clear_source){
	pointpair.second ^= CLICK_SELECTED;
      }
    }
  }


  basecoords::~basecoords(){
#ifdef COUNT_COORDS
    --total_count ;
    d_count++;
#endif // COUNT_COORDS
  }	

}//namespace NCOORDS
