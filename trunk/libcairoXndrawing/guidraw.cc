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

#ifdef DEBUG
#define DEBUG_GUIDRAW 1
#endif

#include "guidraw.h"

#include "x3draw.h"
#include <climits>

using namespace NCOORDS;
using namespace xNDRAW;

#define MARCHINGANTSCALE 8

//separate drawing code to a separate file and have all classes that draw to a cairo surface use them
//point and path transformation code similarly abstracted
//drawing routine and path transform suitable for slotfunc

XndrawMap<int, std::string> GuiDraw::registry = GuiDraw::create_registry();

XndrawMap<int, std::string> GuiDraw::create_registry(){
  XndrawMap<int, std::string> mapresult;
  mapresult[XNGUI_CLICKPATH ] ="EDITLINE_COLOUR";
  mapresult[XNGUI_CLICKPATH | XNGUI_SELECTED] ="EDITLINE_COLOUR_SELECTED";
  mapresult[XNGUI_CLICKPOINT_LINE ] ="EDITPOINT_COLOUR";
  mapresult[XNGUI_CLICKPOINT_LINE | XNGUI_SELECTED] ="EDITPOINT_COLOUR_SELECTED";
  mapresult[XNGUI_CLICKPOINT_FILL ] ="EDITPOINT_FILL_COLOUR";
  mapresult[XNGUI_CLICKPOINT_FILL | XNGUI_SELECTED] ="EDITPOINT_FILL_COLOUR_SELECTED";
  mapresult[XNGUI_GRID_POINT_LINE ] ="MAPCOLOUR_POINT_LINE";
  mapresult[XNGUI_GRID_POINT_LINE  | XNGUI_SELECTED] ="MAPCOLOUR_POINT_LINE_SELECTED";
  mapresult[XNGUI_GRID_POINT_FILL ] ="MAPCOLOUR_POINT_FILL";
  mapresult[XNGUI_GRID_POINT_FILL | XNGUI_SELECTED] ="MAPCOLOUR_POINT_FILL_SELECTED";
  mapresult[XNGUI_HANDLE_LINE ] ="HANDLE_LINE_COLOUR";
  mapresult[XNGUI_HANDLE_LINE | XNGUI_SELECTED] = "HANDLE_LINE_COLOUR";
  mapresult[XNGUI_HANDLE_POINT_LINE ] ="HANDLE_POINT_COLOUR";
  mapresult[XNGUI_HANDLE_POINT_LINE | XNGUI_SELECTED] ="HANDLE_POINT_COLOUR_SELECTED";
  mapresult[XNGUI_HANDLE_POINT_FILL ] ="HANDLE_POINT_FILL_COLOUR";
  mapresult[XNGUI_HANDLE_POINT_FILL | XNGUI_SELECTED] ="HANDLE_POINT_FILL_COLOUR_SELECTED";
  mapresult[  XNGUI_MARCHING_ANTS ] ="SELECTION_MARCHING_ANTS";
  mapresult[  XNGUI_MARCHING_ANTS | XNGUI_SELECTED] ="SELECTION_MARCHING_ANTS";
  mapresult[  XNGUI_RTPREVIEW_PATH] ="RTPREVIEW_PATH";
  mapresult[  XNGUI_RTPREVIEW_BBOX] ="RTPREVIEW_BBOX";

  return  mapresult;
}
    

void GuiDraw::update_from_prefs(){
  Draw_colour  now_colour(0,0,0);
  std::string rolename;
  for(int i =0; i < XNGUI_COLOURS;i++){
    rolename = registry[i];
    if(rolename.size()){
      now_colour = XNDRAW_GET_PREF(rolename, now_colour);
      set_colour( i, now_colour);
    }
  }
}

void GuiDraw::draw_selectbox(cairo_t* context,const SelectBox& b){
  cairo_save(context);
  Draw_colour& antcolour = colours[XNGUI_MARCHING_ANTS];
  cairo_set_source_rgba(context, antcolour.r, antcolour.g,
			antcolour.b, antcolour.a);
  cairo_move_to(context, b.first_corner.x, b.first_corner.y);
  cairo_line_to(context, b.first_corner.x, b.last_corner.y);
  cairo_line_to(context, b.last_corner.x, b.last_corner.y);
  cairo_line_to(context, b.last_corner.x, b.first_corner.y);
  double darray[6];
  for (int i = 0; i < 6;i++){
    darray[i] = (std::rand()/ (double)INT_MAX) * MARCHINGANTSCALE;
  }
  cairo_set_dash(context, darray, 6, 0);
  cairo_stroke(context);
 
  cairo_restore(context);
}

void GuiDraw::draw_grid(cairo_t* context, const Nodedeq& nodes){
  cairo_save(context);
  Draw_colour& gridlinecolour = colours[XNGUI_GRID_POINT_LINE];
  Draw_colour& gridfillcolour = colours[XNGUI_GRID_POINT_FILL];

  cairo_set_source_rgba(context, gridfillcolour.r, gridfillcolour.g,
			gridfillcolour.b, gridfillcolour.a);
  for(Nodedeq::const_iterator iter = nodes.begin(); iter != nodes.end(); iter++){
    draw_editcoords(context, *(*iter), pathattributes.pointwidth);
  }
  cairo_fill_preserve(context);
  cairo_set_source_rgba(context, gridlinecolour.r, gridlinecolour.g,
			gridlinecolour.b, gridlinecolour.a);
  cairo_stroke(context);
  cairo_restore(context);
}

void GuiDraw::stroke_with_colour(cairo_t* context, 
				 const Draw_colour& strokecolour,
				 bool preserve_path){
  cairo_save(context);
  cairo_set_source_rgba(context, strokecolour.r, strokecolour.g,
			strokecolour.b, strokecolour.a);
  if(preserve_path){
    cairo_stroke_preserve(context);
  }
  else{ cairo_stroke(context);}
  cairo_restore(context);
}
void GuiDraw::fill_with_colour(cairo_t* context, 
			       const Draw_colour& strokecolour,
			       bool preserve_path){
  cairo_save(context);
  cairo_set_source_rgba(context, strokecolour.r, strokecolour.g,
			strokecolour.b, strokecolour.a);
  if(preserve_path){
    cairo_fill_preserve(context);
  }
  else{ cairo_fill(context);}
  cairo_restore(context);
}

void GuiDraw::set_prefs(){
  std::string rolename;
  for(int i =0; i < XNGUI_COLOURS;i++){
    rolename = registry[i];
    if(rolename.size()){
      XNDRAW_SET_PREF(rolename, colours[i]);
    }
  }
}
void GuiDraw::on_preview_scale(cairo_t* context,
			       const simple_click_history& path,
			       const std::pair<coords, coords>& arg){
  cairo_save(context);
  simple_click_history tmp= path;
  tmp.scale(arg.first, &(arg.second));
  
  stroke_path_and_bbox(context,tmp, colours[XNGUI_RTPREVIEW_PATH], colours[XNGUI_RTPREVIEW_BBOX]);
  cairo_restore(context);
}
void GuiDraw::on_preview_scale_centred(cairo_t* context,
				       const simple_click_history& path,
				       const coords& arg){
  cairo_save(context);
  simple_click_history tmp= path;
  tmp *= arg;
  
  stroke_path_and_bbox(context,tmp, colours[XNGUI_RTPREVIEW_PATH], colours[XNGUI_RTPREVIEW_BBOX]);
  cairo_restore(context);
}
//draw_path(context,tmp);
//draw_path_scaled(context, path, arg);
//Draw_colour d=colours[XNGUI_RTPREVIEW_PATH];
//   cairo_set_source_rgba(context, d.r, d.g, d.b, d.a);
//   cairo_stroke(context);
//   d=colours[XNGUI_RTPREVIEW_BBOX];
//cairo_set_source_rgba(context, d.r, d.g, d.b, d.a);
// draw_path_bbox(context, path);
//cairo_stroke(context);
  
void GuiDraw::stroke_path_and_bbox(cairo_t* context,
				   const simple_click_history& path,
				   const Draw_colour& line_col,
				   const Draw_colour& box_col){
  cairo_save(context);
  draw_path(context, path);
  cairo_set_source_rgba(context,line_col.r, line_col.g, line_col.b, line_col.a);
  cairo_stroke(context);
  draw_path_bbox(context, path);
  cairo_set_source_rgba(context,box_col.r, box_col.g, box_col.b, box_col.a);
  cairo_stroke(context);
  cairo_restore(context);
}

//todo: return a path to draw bbox of
void GuiDraw::draw_path_scaled(cairo_t* context,
			       const simple_click_history& path,
			       const std::pair<coords, coords>& arg){
  simple_click_history tmp= path;
  tmp.scale(arg.first, &(arg.second));
  draw_path(context,tmp);
}
void GuiDraw::on_preview_rotate_centred(cairo_t* context,
			       const NCOORDS::simple_click_history& path,
			       double  arg){
  cairo_save(context);
  NCOORDS::simple_click_history tmp= path;
  tmp.rotate( arg);
  stroke_path_and_bbox(context,tmp, colours[XNGUI_RTPREVIEW_PATH], colours[XNGUI_RTPREVIEW_BBOX]);
  cairo_restore(context);
}
void GuiDraw::draw_path_bbox(cairo_t* context,
			     const simple_click_history& path){
  cRect drawrect;
  path.get_extents(drawrect);
  cairo_move_to(context, drawrect.x(), drawrect.y());
  cairo_line_to(context, drawrect.xmax(), drawrect.y());
  cairo_line_to(context, drawrect.xmax(), drawrect.ymax());
  cairo_line_to(context, drawrect.x() , drawrect.ymax());
  cairo_close_path(context);
}

void GuiDraw::on_preview_rotate(cairo_t* context,
				const simple_click_history& path,
				const	    std::pair<double, coords>& arg){
  cairo_save(context);
  simple_click_history tmp= path;
  tmp.rotate(arg.second, arg.first);
  stroke_path_and_bbox(context,tmp, colours[XNGUI_RTPREVIEW_PATH], colours[XNGUI_RTPREVIEW_BBOX]);
  cairo_restore(context);
}
void GuiDraw::draw_path_rotated(cairo_t* context,
				const simple_click_history& path,
				const	std::pair<double, coords>& arg){
  simple_click_history tmp= path;
  tmp.rotate(arg.second, arg.first);
  draw_path(context,tmp);
}

void GuiDraw::draw_path_rotated(cairo_t* context,
				const simple_click_history& path,
				double arg){
  simple_click_history tmp= path;
  tmp.rotate( arg);
  draw_path(context,tmp);
}

void GuiDraw::draw_path_translated(cairo_t* context,
				   const simple_click_history& path,
				   const std::pair<coords, int>& arg){
  simple_click_history transpath= path + arg.first;
  draw_path(context,transpath);
}

void GuiDraw::on_preview_translate(cairo_t* context,
				   const simple_click_history& path,
				   const std::pair<coords, int>& arg){
  cairo_save(context);
  simple_click_history transpath= path + arg.first;
  stroke_path_and_bbox(context,transpath, colours[XNGUI_RTPREVIEW_PATH], colours[XNGUI_RTPREVIEW_BBOX]);
													  cairo_restore(context);
}

void GuiDraw::draw_editcoords(cairo_t* context, const coords& pt, double pointsize)const{
  //pointsize refers to the width of a rendered point
  //no stroke or fill here, draw as many as we can before making 
  //expensive stroke_preserve() and then fill() calls
  cairo_move_to( context, pt.x -(pointsize / 2.0), pt.y-(pointsize / 2.0));
  cairo_rel_line_to(context, pointsize, 0);
  cairo_rel_line_to(context, 0, pointsize);
  cairo_rel_line_to(context, -pointsize, 0);
  cairo_rel_line_to(context, 0, -pointsize);
  cairo_close_path(context);
}

void GuiDraw::draw_editcoords_path(cairo_t* context, const NCOORDS::simple_click_history& path){ //wrap in cairo_save()/cairo_restore() pair
  ///point outline and point fill, selected/unselected
  double scale= pathattributes.pointwidth;
  Draw_colour& reg_linecolour = colours[XNGUI_CLICKPOINT_LINE];
  Draw_colour& selected_linecolour = colours[XNGUI_CLICKPOINT_LINE +1];
  Draw_colour& reg_fillcolour = colours[XNGUI_CLICKPOINT_FILL];
  Draw_colour& selected_fillcolour = colours[XNGUI_CLICKPOINT_FILL +1];
  //2 loops, first unselected, then selected
  cairo_set_source_rgba(context, reg_fillcolour.r, reg_fillcolour.g, 
			reg_fillcolour.b, reg_fillcolour.a);
  cairo_set_line_width(context, pathattributes.pointlinewidth);
  for (NCOORDS::simple_click_history::const_iterator itt = path.begin(); itt != path.end(); itt++){
    if(!(*itt).first.selected()){
      draw_editcoords(context, (*itt).first, scale); 
    }
  }
  cairo_fill_preserve(context);
  cairo_set_source_rgba(context, reg_linecolour.r, reg_linecolour.g, 
			reg_linecolour.b, reg_linecolour.a);
  cairo_stroke(context); //clears path in context
  for (NCOORDS::simple_click_history::const_iterator itt2 = path.begin(); itt2 != path.end(); itt2++){
    if((*itt2).first.selected()){
      draw_editcoords(context, (*itt2).first, scale); 
    }
  }
  cairo_set_source_rgba(context, selected_fillcolour.r, selected_fillcolour.g, 
			selected_fillcolour.b, selected_fillcolour.a);
  cairo_fill_preserve(context);
  cairo_set_source_rgba(context, selected_linecolour.r, selected_linecolour.g, 
			selected_linecolour.b, selected_linecolour.a);
  cairo_stroke(context); //clears path in context
  //cairo_restore(context);
}

void GuiDraw::draw_path(cairo_t* context, 
			const NCOORDS::simple_click_history& path){
  coords pt;
  if(path.size()){
    pt = (*(path.begin())).first;
  }
  bool previous_selected(false),pt_selected(false), 
    next_selected(false), last_state(false), current_state(false);
  for (NCOORDS::simple_click_history::const_iterator itt = path.begin(); itt != path.end(); itt++){
    pt =  (*itt).first;
    int ptflags= (*itt).second;
    if(ptflags &  MOVE_CLICK){
      cairo_move_to(context, (*itt).first.x, (*itt).first.y);
    }
    else if(ptflags &  LINE_CLICK){
      cairo_line_to(context, pt.x, pt.y);
    }
    else if(ptflags &  BEZIER_CLICK){
      //this bezier code works _correctly_
      //TODO: draw bezier handlw in edit interface, around selected points
      if((itt+1)==path.end() || (!((*(itt + 1)).second & BEZIER_HANDLE)) ){
	std::cerr<< "WARNING:NO BEZIER_HANDLE with BEZIER_CLICK"<<std::endl;
	bool curvedrawn(false);

	for(int i=0;(i <3) && (itt- (i -1)!= path.begin());i++){
	  if((*(itt -i)).second & (MOVE_CLICK | LINE_CLICK | BEZIER_CLICK)){
	    const coords & handle1=(*(itt -i)).first;
	    cairo_curve_to(context, handle1.x, handle1.y, pt.x, pt.y, pt.x, pt.y);
	    curvedrawn = true;
	  }
	}
	if(!curvedrawn){
	  //give up looking for the real handle1
	  cairo_curve_to(context, pt.x, pt.y, pt.x, pt.y, pt.x, pt.y);
	}
      }
      else if((itt+2)==path.end() || (!((*(itt + 2)).second & BEZIER_HANDLE)) ){
	std::cerr<< "WARNING: only onwe BEZIER_HANDLE with BEZIER_CLICK"
		 <<std::endl;
	const coords& handle=(*(itt + 1)).first;
	cairo_curve_to(context, handle.x, handle.y, pt.x, pt.y, pt.x, pt.y);
	itt++;
      }
      else{
	const coords& handle1 = (*(itt + 1)).first;
	const coords& handle2 = (*(itt + 2)).first;
	cairo_curve_to(context, handle1.x, handle1.y, 
		       handle2.x, handle2.y, pt.x, pt.y);
	itt += 2;
      }
    }
    else if(ptflags & CLOSE_PATH){
#ifdef DEBUG_GUIDRAW
      std::cerr <<"path closed: guidraw not showing it - TODO: preference to cointrol this"<<std::endl; 
#endif
    }
    else if(ptflags & BEZIER_HANDLE){
      std::cerr << "BEZIER_HANDLE not following BEZIER_CLICK:ignoring"
		<< std::endl;
    }
    else{
      std::cerr << "unknown point type:"<<(*itt).second
		<<"\n... ignoring it ..."<<std::endl;
    }     
  }
}


void GuiDraw::draw_editline(cairo_t* context, const NCOORDS::simple_click_history& path){
  //cairo_save(context);
  coords pt;
  if(path.size()){
    pt = (*(path.begin())).first;
  }
  bool previous_selected(false),pt_selected(false), 
    next_selected(false), last_state(false), current_state(false);
  Draw_colour& reg_colour = colours[XNGUI_CLICKPATH];
  Draw_colour& selected_colour = colours[XNGUI_CLICKPATH +1];
  cairo_set_source_rgba(context, reg_colour.r, reg_colour.g, 
			reg_colour.b, reg_colour.a);
  //std::cerr <<"pathattributes.linewidth" <<pathattributes.linewidth<<std::endl;
  cairo_set_line_width(context, std::max(pathattributes.linewidth, 1.));
  for (NCOORDS::simple_click_history::const_iterator itt = path.begin(); itt != path.end(); itt++){
    
    last_state = current_state;
    previous_selected= pt_selected;
    pt_selected = (*itt).first.selected();
    if((itt + 1) != path.end()){
      next_selected = (*(itt +1)).first.selected();
    }
    else{next_selected = false;}
    current_state = previous_selected || pt_selected ||  next_selected ;
    if(current_state != last_state){
      cairo_stroke(context);
      cairo_move_to(context, pt.x, pt.y);
      if(current_state){
	cairo_set_source_rgba(context,selected_colour.r, selected_colour.g,
			      selected_colour.b, selected_colour.a);
      }
      else{
	cairo_set_source_rgba(context, reg_colour.r, reg_colour.g, 
			      reg_colour.b, reg_colour.a);
      }
    }
    pt =  (*itt).first;
    int ptflags= (*itt).second;
    if(ptflags &  MOVE_CLICK){
      cairo_move_to(context, (*itt).first.x, (*itt).first.y);
    }
    else if(ptflags &  LINE_CLICK){
      cairo_line_to(context, pt.x, pt.y);
    }
    else if(ptflags &  BEZIER_CLICK){
      //this bezier code works _correctly_
      //TODO: draw bezier handlw in edit interface, around selected points
      if((itt+1)==path.end() || (!((*(itt + 1)).second & BEZIER_HANDLE)) ){
	std::cerr<< "WARNING:NO BEZIER_HANDLE with BEZIER_CLICK"<<std::endl;
	bool curvedrawn(false);

	for(int i=0;(i <3) && (itt- (i -1)!= path.begin());i++){
	  if((*(itt -i)).second & (MOVE_CLICK | LINE_CLICK | BEZIER_CLICK)){
	    const coords & handle1=(*(itt -i)).first;
	    cairo_curve_to(context, handle1.x, handle1.y, pt.x, pt.y, pt.x, pt.y);
	    curvedrawn = true;
	  }
	}
	if(!curvedrawn){
	  //give up looking for the real handle1
	  cairo_curve_to(context, pt.x, pt.y, pt.x, pt.y, pt.x, pt.y);
	}
      }
      else if((itt+2)==path.end() || (!((*(itt + 2)).second & BEZIER_HANDLE)) ){
	std::cerr<< "WARNING: only onwe BEZIER_HANDLE with BEZIER_CLICK"
		 <<std::endl;
	const coords& handle=(*(itt + 1)).first;
	cairo_curve_to(context, handle.x, handle.y, pt.x, pt.y, pt.x, pt.y);
	itt++;
      }
      else{
	const coords& handle1 = (*(itt + 1)).first;
	const coords& handle2 = (*(itt + 2)).first;
	cairo_curve_to(context, handle1.x, handle1.y, 
		       handle2.x, handle2.y, pt.x, pt.y);
	itt += 2;
      }
    }
    else if(ptflags & CLOSE_PATH){
#ifdef DEBUG_GUIDRAW
      std::cerr <<"path closed: guidraw not showing it - TODO: preference to cointrol this"<<std::endl; 
#endif
    }
    else if(ptflags & BEZIER_HANDLE){
      std::cerr << "BEZIER_HANDLE not following BEZIER_CLICK:ignoring"
		<< std::endl;
    }
    else{
      std::cerr << "unknown point type:"<<(*itt).second
		<<"\n... ignoring it ..."<<std::endl;
    }     
  }

  cairo_stroke(context);
  //cairo_restore(context);
}


void GuiDraw::set_pref(int i){
  std::string rolename= registry[i];
  if(rolename.size()){
    XNDRAW_SET_PREF(rolename, colours[i]);
  }
}

GuiDraw::GuiDraw(double linescale , double handlescale){
  pathattributes.scale(linescale);
  handleattributes.scale(handlescale);
  update_from_prefs();
}
      
      
	
