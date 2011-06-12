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


#ifndef XNDRAW_CAIROPATH_H
#define XNDRAW_CAIROPATH_H

#include "exceptions.h"
#if HAVE_CONFIG_H
#include "config.h"
#endif


#include <cairomm/path.h>
#include <cairomm/context.h>
#include "xndrawexceptions.h"
#include "xncairoerror.h"
#include "clickhistory.h"
#include <algorithm>
#include "typedefs.h"

#define XNDRAWPATH_START_BUFFERSIZE 64ul
#define XN_PATH_SIZE(count)     xn_path_buffersize(count)


namespace xNDRAW{

typedef Cairo::RefPtr<Cairo::Context> cairoPtr;
  typedef Cairo::RefPtr<Cairo::ImageSurface> imagePtr;
typedef Cairo::RefPtr<Cairo::Surface> surfPtr;
typedef Cairo::RefPtr<Cairo::SvgSurface> svgPtr;

  class Drawingbase;
  class Drawing;
  class Drawingset;

  //bezier code is wrong//drawtool mostly to blame?
  //cairo_curve_to()'s control points refer to the line segment,
  //so the first pair come from the previous point

bool rects_intersect(const cairo_rectangle_t& first, 
		     const cairo_rectangle_t&  second);

template<typename X_Y>
  X_Y& cairomatrix_transform_XY(const cairo_matrix_t& matrix, X_Y& point){
    cairo_matrix_transform_point(&matrix, &point.x, &point.y);
    return point;
  }

  void cairomatrix_transform(const cairo_matrix_t& matrix, coords& pt);
  void cairomatrix_transform(const cairo_matrix_t& matrix, ncoords& npts);
  void cairomatrix_transform(const cairo_matrix_t& matrix, NCOORDS::deqC& deq);
  void cairomatrix_transform(const cairo_matrix_t& matrix, Nodes& nodes);


  cairo_matrix_t& ncoords_set_rotation_matrix(const ncoords& npts,
						       cairo_matrix_t& matrix, 
						       size_t step_index, 
						       bool invert, 
					      bool reverse); 



//something with the matrix here was broken - 
//need to undestand how to chain together matrix transformations
//trying ordering backwards
//write some tests to understand the problem, and prevent regression when fixed
//in any case using the matrix to rotate path is vslow
  cairo_rectangle_t cRect_to_cairo_rect(const NCOORDS::cRect& r);
  NCOORDS::cRect cairo_rect_to_cRect(const cairo_rectangle_t& r);

  cairo_path_t* ncoords_matrix_rotate_path(const ncoords& npts,
					   cairo_path_t* path, 
					   size_t stepindex, 
					   bool invert, 
					   bool reverse);

  simple_click_history flatten_path_copy(const simple_click_history& src);
  void set_path_flattener();
 void cairo_draw_clickhistory(cairo_t* context, 
			      const simple_click_history&  path);
  void cairo_draw_clickhistory(Cairo::RefPtr<Cairo::Context> draw, 
			       const simple_click_history&  path);

cairo_path_t* cairomatrix_transform_rv(const Cairo::Matrix& matrix, 
				    cairo_path_t* inpath );

  void negative_clip(cairo_t* cr, const NCOORDS::simple_click_history& clippath, double xmax=2048., double ymax = 2048.);
  

  bool operator==(const cairo_path_t& left, const cairo_path_t& right);

  cairo_path_t* create_cairopath_raw();
  cairo_path_t* create_cairopath(size_t sz = XNDRAWPATH_START_BUFFERSIZE);
  cairo_path_t* join_cairopaths(cairo_path_t* dest, cairo_path_t* addpath, bool destroypath = false);
  cairo_path_t* new_copy_cairopath(cairo_path_t* src);
  void copy_cairopath(cairo_path_t* src, cairo_path_t* dest);
    size_t count_cairodata_4copy(const cairo_path_t* src);
  
  void destroy_cairopath(cairo_path_t* crpath);
  cairo_path_data_t* create_pathdata(size_t size = XNDRAWPATH_START_BUFFERSIZE);

  void destroy_pathdata(cairo_path_data_t* data_destroy);
  size_t xn_path_buffersize(size_t clickcount);


  cairo_path_data_t* reallocate_pathdata(cairo_path_t* path , size_t newsz = XNDRAWPATH_START_BUFFERSIZE );
  
  
  cairo_path_data_t* set_pathdata_header(cairo_path_data_t* data, cairo_path_data_type_t typ, int length);
  
  cairo_path_data_t* set_pathdata_point(cairo_path_data_t* data, const coords& pt);

  //cairo_path_t* path_from_history
  //very slow compared to usind simple_click_history - too much point copying  
  class XndrawPath{
  protected:
    typedef cairo_path_t cobject;
    cairo_path_t* c_path;
    bool xndraw_owns_path;
  public:
    XndrawPath(const XndrawPath& src);//create new copy of c_path;
    XndrawPath(cairo_path_t* path_, bool allocated_by_xndraw = true):
      c_path(path_),
      xndraw_owns_path(allocated_by_xndraw){}

    XndrawPath(const NCOORDS::simple_click_history& clicks);
    
  virtual ~XndrawPath(){
      destroy_c_path(c_path, xndraw_owns_path);
    }

    XndrawPath& operator=(const XndrawPath& src);//create new copy ofc_path;
  protected:
    void destroy_c_path(cairo_path_t* path, bool ownedby_x);
  public:

    static cairo_path_t* cairo_from_history(const simple_click_history& clicks);

    const cairo_path_t* cobj()const;
    cairo_path_t* cobj();

    cairo_path_t* create(NCOORDS::simple_click_history& clicks);
    simple_click_history* to_click_history();
    void rotate(const NCOORDS::coords& centre_, double angle_/*radians*/);
    void transform(const cairo_matrix_t& matrix);
    cairo_path_t* transform_inplace(Cairo::Matrix& matrix, cairo_path_t* inpath = NULL);
  };

  
}/*namespace xNDRAW*/

#endif/* XNDRAW_CAIROPATH_H*/
