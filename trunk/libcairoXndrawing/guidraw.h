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



/**
 * @file guidraw.h
 * drawing lines and points for the user interface(as against output)
 */

#ifndef LIBCAIROXNDRAWING_GUIDRAW_H
#define LIBCAIROXNDRAWING_GUIDRAW_H

#include "exceptions.h"

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include "drawcolour.h"
#include <cairo.h>
#include "clickhistory.h"
#include "nodes.h"
#include "xndrawexceptions.h"
#include "typedefs.h"
#include "selection.h"

namespace xNDRAW{

  enum xnViewportinfo{
    XNVIEWPORT_TO_USER =1,
XNVIEWPORT_TO_DEVICE=2
  };


  /**
  //indexes into GuiDraw::colours got from these values 
  // use '|' or '+' to add XNGUI_SELECTED
  //<pseudocode>
  //GuiDraw gp;
  //Draw_colour dc= produce_some_lurid_colour();;
  //gp.set_colour(XNGUI_CLICKPATH |XNGUI_SELECTED, dc);
  // note to hackers/maintainers: enum vars -apart from XNGUI_SELECTED - must be postive and _even_: 
  // ie leave that low bit alone
  //and remeber to expand the colours array as more vars defined
  */
#define XNGUI_COLOURS 24
enum GuiColours{

  XNGUI_CLICKPATH =0,
  XNGUI_SELECTED=1,
  XNGUI_CLICKPOINT_LINE=2,
  
  XNGUI_CLICKPOINT_FILL=4,
  XNGUI_GRID_POINT_LINE = 6,
  XNGUI_GRID_POINT_FILL = 8,
  XNGUI_HANDLE_LINE = 10,
  XNGUI_HANDLE_POINT_LINE=12,
  XNGUI_HANDLE_POINT_FILL =14,
  XNGUI_MARCHING_ANTS =16,
  XNGUI_RTPREVIEW_PATH = 18,
XNGUI_RTPREVIEW_BBOX=20
};

 struct GuiDrawAttributes{
double linewidth;
    double pointwidth;
   double pointlinewidth;
   //TODO: clamp minimum values
   GuiDrawAttributes(double lwide =1.0, 
		     double pwide= 3.0, 
		     double pline=0.6):
     linewidth(lwide),
     pointwidth(pwide),
     pointlinewidth(pline)
   {}

   void scale(double scaleval){
     linewidth *= scaleval;
pointwidth*= scaleval;
pointlinewidth*= scaleval;
   }
     };

  class DrawViewport:public std::binary_function<cairo_t*, const std::pair<coords*, int>&,coords> {
  public:
    //to screen units
    //important - only set the transform matrix for the actual screen cairo
    //(the cairo provided by gdk-cairo:ie Drawingbase::visual)
    // if you expect the drawing gui to work
    coords& to_user_space(cairo_t* context, coords& pt){
      cairo_device_to_user(context, &(pt.x), &(pt.y));
      return pt;
    }
    //ie to the units of the actual artwork
    coords& to_device_space(cairo_t* context, coords& pt){
      cairo_user_to_device(context, &(pt.x), &(pt.y));
return pt;
    }
    coords operator()(cairo_t* context, const std::pair<coords*, int>& pt){
      coords result(*(pt.first) );
#ifdef DEBUG
      if((pt.second & XNVIEWPORT_TO_DEVICE)&&(pt.second & XNVIEWPORT_TO_USER)){
	throw xn_Error("DrawViewport:XNVIEWPORT_TO_DEVICE and XNVIEWPORT_TO_USER both set");
      }
#endif
      if(pt.second & XNVIEWPORT_TO_DEVICE){
	return to_device_space(context,result);
      }
      else if(pt.second & XNVIEWPORT_TO_USER){
	return to_user_space(context,result);
      }
      else{throw xn_Error("DrawViewport:must be XNVIEWPORT_TO_DEVICE or XNVIEWPORT_TO_USER");}
    }
  };

  class GuiDraw{
  protected:
    Draw_colour colours[XNGUI_COLOURS];
    GuiDrawAttributes pathattributes;
    GuiDrawAttributes handleattributes;
    //simple_click_history* editpreview;
    //this static member is problematic if a python module wrapping this lib
    //made, if python itself compiled with a C (not C++) compiler
    static XndrawMap<int, std::string> registry;
  public:
    GuiDraw(double linescale= 1., double handlescale= 1.);
   
    void update_from_prefs();
    void set_prefs();
    void set_pref(int which);
    /**more trouble than is worth
    template <typename TRANSFORMER>
    void draw_path_with_transformation(cairo_t* context,
				       const simple_click_history& path,
				       typename TRANSFORMER::value_type& arg);

    */
    void draw_path_translated(cairo_t* context,
			      const simple_click_history& path,
			      const std::pair<coords, int>& arg);

    void on_preview_translate(cairo_t* context,
			      const simple_click_history& path,
			      const std::pair<coords, int>& arg);

void on_preview_scale(cairo_t* context,
			      const simple_click_history& path,
			      const std::pair<coords, coords>& arg);
    static void draw_path_bbox(cairo_t* context,
			       const simple_click_history& path);
static void stroke_path_and_bbox(cairo_t* context,
				   const simple_click_history& path,
				   const Draw_colour& line_col,
				 const Draw_colour& box_col);
void on_preview_scale_centred(cairo_t* context,
			       const simple_click_history& path,
				       const coords& arg);

void draw_path_scaled(cairo_t* context,
			      const simple_click_history& path,
			      const std::pair<coords, coords>& arg);
    void on_preview_rotate(cairo_t* context,
			      const simple_click_history& path,
			      const std::pair<double, coords>& arg);
void on_preview_rotate_centred(cairo_t* context,
			      const simple_click_history& path,
			      double  arg);
void draw_path_rotated(cairo_t* context,
			      const simple_click_history& path,
			      const std::pair<double, coords>& arg);
void draw_path_rotated(cairo_t* context,
			      const simple_click_history& path,
			      double arg);


    static  XndrawMap<int, std::string> create_registry();
    void set_colour(int colour_to_set, const Draw_colour& dc){
      if(colour_to_set >= XNGUI_COLOURS || colour_to_set < 0){
	throw xn_Error("colour index out of range");
      }
      colours[colour_to_set] = dc;
    }
    //TODO:offset versions for the Pleasure of  PathPreview
 void draw_editcoords(cairo_t* context, const coords& pt, double scale= 1.0)const;
  void draw_editcoords_path(cairo_t* context, const NCOORDS::simple_click_history& path);
  void draw_editline(cairo_t* context, const NCOORDS::simple_click_history& path);
    //does no colouring/stroking etc, just sets path
static void draw_path(cairo_t* context, 
	       const NCOORDS::simple_click_history& path);
void stroke_with_colour(cairo_t* context, 
			const Draw_colour& strokecolour,
			bool preserve_path=false);

void fill_with_colour(cairo_t* context, 
			const Draw_colour& strokecolour,
			bool preserve_path=false);



    void draw_selectbox(cairo_t* context,const NCOORDS::SelectBox& b);
 void draw_grid(cairo_t* context, const Nodedeq& nodes);


  }; //class GuiDraw


} //namespace xNDRAW


#endif // LIBCAIROXNDRAWING_GUIDRAW_H
