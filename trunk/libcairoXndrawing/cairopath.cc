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


#include "cairopath.h"
#include <iterator>
#include "drawingbase.h"

namespace xNDRAW{
  using namespace NCOORDS;

void cairomatrix_transform(const cairo_matrix_t& matrix, Nodes& nodes){
 
   cairomatrix_transform(matrix, (ncoords&)nodes);
      if(nodes.size()){
	cairomatrix_transform(matrix,nodes.get_nodes());
      }
    }


  
  cRect cairo_rect_to_cRect(const cairo_rectangle_t& r){
    coords first(r.x, r.y), second(r.x + r.width, r.y +r.height);
    return cRect(first, second);
  }

  cairo_rectangle_t cRect_to_cairo_rect(const cRect& r){
    cairo_rectangle_t res;
    res.x = r.x();
    res.y = r.y();
    res.width= r.width();
    res.height = r.height();
    return res;
  }

cairo_matrix_t& ncoords_set_rotation_matrix(const ncoords& npts,
						       cairo_matrix_t& matrix, 
						       size_t step_index, 
						       bool invert, 
						       bool reverse){
    using namespace NCOORDS;
  double angle_(npts.get_step() * step_index);
  coord_state state= npts.get_state();
  int base = npts.get_base();
  if((base==3) &&(state & COORDS_UP)){
    if (reverse){
      angle_ = -angle_;//FLIP HERE(use no matrix version:faster)
    }
    if (invert){
      angle_ = angle_ + M_PI;
    }
  }
  //operations changed to reverse order(like for matrix with gl)
  //write a test, or find sample code
    cairo_matrix_init_translate(&matrix, npts.x, npts.y); //move centre to (0,0)
  cairo_matrix_rotate(&matrix,angle_);
  cairo_matrix_translate(&matrix, -npts.x, -npts.y);  //and back again
  return matrix;
}

  cairo_path_t* ncoords_rotate_path(const ncoords& npts,
				    cairo_path_t* path, 
				    size_t step_index, 
				    bool invert, bool reverse){
  double angle_( npts.get_step() * step_index);
  coord_state state = npts.get_state();
  coords centre(npts.x, npts.y);
  if(state){
    if (reverse){
      angle_ = -angle_;
    }
    if (invert){
      angle_ = angle_ + M_PI;
    }
  }
 cairo_path_t* result = path;
  if (!result ){
    throw xn_Error("NULL path");
  }
if(!result->data || !result->num_data){
    if(!result->data){std::cerr << "no allocated data"<< std::endl;}
    else{std::cerr << "num_data = 0!" << std::endl;}
    throw xn_Error("no data in path!");
  }
  cairo_path_data_t *data=NULL;
  for (int i=0; i < result->num_data; i += result->data[i].header.length){
    data = result->data + i;
    switch (data->header.type){
    case CAIRO_PATH_MOVE_TO:
    case CAIRO_PATH_LINE_TO:
      {
	centre.rotate(data[1].point.x, data[1].point.y, angle_);
	continue;
      }
    case CAIRO_PATH_CURVE_TO:
      {
	centre.rotate(data[1].point.x, data[1].point.y, angle_);
	centre.rotate(data[2].point.x, data[2].point.y, angle_);
	centre.rotate(data[3].point.x, data[3].point.y, angle_);
	continue;
      }
    default:
      {
	continue;
      }
    }
  }
  return result;
}
  //much slower than ncoords version(which use
  //year9 trigonometry)
  //maybe different if there hardware matrix, say in gl card
  cairo_path_t* ncoords_matrix_rotate_path(const ncoords& npts, 
					   cairo_path_t* path, 
					   size_t stepindex, 
					   bool invert, 
					   bool reverse){
  Cairo::Matrix matrix;
  ncoords_set_rotation_matrix(npts, matrix, stepindex, invert, reverse);
  return cairomatrix_transform_rv(matrix, path);
}


void cairomatrix_transform(const cairo_matrix_t& matrix, coords& pt){
  cairo_matrix_transform_point(&matrix, &(pt.x), &(pt.y));
  }
void cairomatrix_transform(const cairo_matrix_t& matrix, ncoords& npts){
  cairomatrix_transform_XY(matrix, (coords&)npts);
  coords rad(*(npts.get_radial()));
  cairomatrix_transform_XY(matrix,rad);
  npts.set_radial(rad);
  }
  void cairomatrix_transform(const cairo_matrix_t& matrix, NCOORDS::deqC& deq){
    for (deqC::iterator walk = deq.begin(); walk != deq.end(); walk++){
	cairomatrix_transform_XY(matrix, *(*walk));
      }
    }


cairo_path_t* cairomatrix_transform_rv(const Cairo::Matrix& matrix, cairo_path_t* inpath ){

  //cairo_matrix_t* matrix = mat.cobj();
  cairo_path_t* result = inpath;
  if (!result ){
    throw xn_Error("NULL path");
  }
  if(!result->data || !result->num_data){
    if(!result->data){std::cerr << "no allocated data"<< std::endl;}
    else{std::cerr << "num_data = 0!" << std::endl;}
    throw xn_Error("no data in path!");
  }
  cairo_path_data_t *data=NULL;
  for (int i=0; i < result->num_data; i += result->data[i].header.length){
    data = result->data + i;
    switch (data->header.type){
    case CAIRO_PATH_MOVE_TO:
    case CAIRO_PATH_LINE_TO:
      {
	cairo_matrix_transform_point(&matrix, &(data[1].point.x), &data[1].point.y);
	continue;
      }
    case CAIRO_PATH_CURVE_TO:
      {
	cairo_matrix_transform_point(&matrix, &(data[1].point.x), &data[1].point.y);
	cairo_matrix_transform_point(&matrix, &(data[2].point.x), &data[2].point.y);
	cairo_matrix_transform_point(&matrix, &(data[3].point.x), &data[3].point.y);
	continue;
      }
    default:
      {
	continue;
      }
    }
  }
  return result;
}

  bool rects_intersect(const cairo_rectangle_t& first, const cairo_rectangle_t&  second){
    coords firstTopL(first.x, first.y);
    coords secondTopL(second.x, second.y);
    coords firstBottomR(first.x +first.width, first.y+first.height);
    coords secondBottomR(second.x + second.width, second.y + second.height);
    cRect f(firstTopL, firstBottomR);
    cRect snd(secondTopL, secondBottomR);
    return f.intersect(snd);
  }


  bool operator==(const cairo_path_t& left, const cairo_path_t& right){
    
    Nearly_Equal<double> doubles_equal;
    for (int i=0; i < left.num_data; i += left.data[i].header.length){
      if(left.data[i].header.type != right.data[i].header.type){
	return false;
      }
      switch( left.data[i].header.type){
      case CAIRO_PATH_LINE_TO:
	
      case CAIRO_PATH_MOVE_TO:
	if (doubles_equal(left.data[i + 1].point.x ,right.data[i + 1].point.x) &&
	    doubles_equal(left.data[i + 1].point.y , right.data[i + 1].point.y)){ break;  }
	else { return false; }
      case CAIRO_PATH_CURVE_TO:
	
	for (int j = 1;j <4; j++){
	  if (!(doubles_equal(left.data[i + j].point.x ,right.data[i + j].point.x) &&
		doubles_equal(left.data[i + j].point.y , right.data[i + j].point.y))){ 
	    return false ;
	  }
	}
      }
    }
    return true;
  }



  //allocate path by a power of 2, for what it's worth

  size_t xn_path_buffersize(size_t clickcount){
    size_t the_default = XNDRAWPATH_START_BUFFERSIZE;
    size_t result = std::max(2 * clickcount, the_default);
    if(result > the_default){ 
      if(result == 2*the_default){
	return result;
      }
      while(result > the_default){
	the_default *= 2;
      }
      result = the_default;
    }
    return result;
  }

  void negative_clip(cairo_t* cr, const NCOORDS::simple_click_history& clippath, double xmax, double ymax){
    //copies current path,creates a clip path with  clippath cut out
    //then copies the path back
    cairo_surface_t* surf = cairo_get_target(cr);
    Cairo::Context context_ (cr);
    Cairo::RefPtr<Cairo::Context > context(&context_);
    Cairo::Path* oldpath= context->copy_path();
    context->begin_new_path();
    context->move_to(0,0);
    context->line_to(xmax,0);
    context->line_to(xmax,ymax);
    context->line_to(0, ymax);
    context->close_path();
    cairo_draw_clickhistory(context, clippath);
    context->close_path();
    context->clip();
    context->begin_new_path();
    context->append_path(*oldpath);
    //is oldpath memory leak?-how to free?
    //no lib function, and its no clear from headers how the path
    //was actually allocated
  }

  cairo_path_t* create_cairopath_raw(){
    cairo_path_t* result = new cairo_path_t;
    result->status = CAIRO_STATUS_SUCCESS;
    result->data = NULL;
    result->num_data = 0;
    return result;
  }

  void cairo_draw_clickhistory(Cairo::RefPtr<Cairo::Context> draw, const simple_click_history&  path){
    cairo_draw_clickhistory(draw->cobj(), path);
  }

  void cairo_draw_clickhistory(cairo_t* context, const simple_click_history&  path){
    register const coords* previous_handle(NULL);//2nd handle of previous bezier point
    if (!path.size()){return;}
    typedef simple_click_history::const_iterator path_iterator;
    path_iterator itt= path.begin();
    if(!(*itt).second == MOVE_CLICK){
      std::cerr << "WARNING: FIRST click should be MOVE_CLICK \ncalling 'move_to()' anyway"<<std::endl;
    }
    //TODO: move_to to accept bezier click(otherwise no curve out of first point)
    cairo_move_to(context, (*itt).first.x, (*itt).first.y);
    previous_handle= &((*itt).first);
    itt++;
    
    for(;itt != path.end(); itt++){
      const coords& pt= (*itt).first;
      NCOORDS::PointerMode ptflags= (NCOORDS::PointerMode)(*itt).second;
      if(!is_drawing_element(ptflags)){continue;}
      if(!valid_drawing_element(ptflags)){
	//std::ostringstream ostmp;
	XN_ERROR("ptflags invalid" <<clicktype_as_string(ptflags)<<" ("<<(int)ptflags<<")") ;
	  //throw xn_Error(ostmp.str());
      }
      if( ptflags & MOVE_CLICK){
	cairo_move_to(context, pt.x, pt.y);
	previous_handle = &pt;
#if DEBUG_CLICK
	if(ptflags != MOVE_CLICK){
	  std::cerr <<"extra flags on move click: "
		    << clicktype_as_string(ptflags - MOVE_CLICK)<< std::endl;
	}
#endif
      }
      else if( ptflags & LINE_CLICK){
	cairo_line_to(context, pt.x, pt.y);
	previous_handle = &pt;
	if(ptflags != LINE_CLICK){
	  std::cerr <<"extra flags on LINe click: "<< ptflags - LINE_CLICK<< std::endl;
	}
      }
      else if( ptflags & ORIGIN_CLICK){
	//cairo_move_to(context, pt.x, pt.y);
	std::cerr <<" ORIGIN_CLICK: not drawing it"<<std::endl;
	if(ptflags != ORIGIN_CLICK){
	  std::cerr <<"extra flags on LINe click: "<< ptflags - ORIGIN_CLICK<< std::endl;
	}
      }
      else if( ptflags & OFFSET_CLICK){
	//cairo_move_to(context, pt.x, pt.y);
	std::cerr <<" OFFSET_CLICK: not drawing it"<<std::endl;
	if(ptflags != OFFSET_CLICK){
	  std::cerr <<"extra flags on Ofset click: "<< ptflags - OFFSET_CLICK<< std::endl;
	}
      }
      else if(ptflags & CLOSE_PATH){
	cairo_close_path(context);
	if(ptflags != CLOSE_PATH){
	  std::cerr <<"extra flags on CLOSE_PATH: "<< ptflags - CLOSE_PATH<< std::endl;
	}
      }
      else  if( ptflags &  BEZIER_CLICK){
	if(ptflags != BEZIER_CLICK){
	  std::cerr <<"extra flags on bezier click: "<< ptflags - BEZIER_CLICK<< std::endl;
	}
	//TODO: draw bezier handlw in edit interface, around selected points
	if((itt+1) == path.end() || !( (*(itt + 1)).second & BEZIER_HANDLE)){
	  std::cerr<<"should be at least one bezier handle after a bezier click in simple_click_history\ntreating as if handles were identical with the point"<<std::endl;
	  cairo_curve_to(context, previous_handle->x, previous_handle->y, pt.x, pt.y, pt.x, pt.y);
	  previous_handle =&pt;
	}
	else if((itt+2) == path.end() ||!( (*(itt + 2)).second & BEZIER_HANDLE)){
	  std::cerr << "bezier with only one handle"<<std::endl;
	  const coords& handle = (*itt).first;
	  cairo_curve_to(context, previous_handle->x, previous_handle->y, handle.x, handle.y, pt.x, pt.y);
	  previous_handle =&pt;
	  itt++;
	}
	else{
	  const coords& handle1 =(*(itt+1)).first;
	  const coords& handle2 =(*(itt+2)).first;
	  cairo_curve_to(context, handle1.x, handle1.y,
			 handle2.x, handle2.y,   
			 pt.x, pt.y);
	  itt+=2;
	  previous_handle =&pt;
	}
      }
    }
  }
  
  


  cairo_path_t* create_cairopath(size_t sz){
    cairo_path_t* result = create_cairopath_raw();
    result->data = create_pathdata(sz);
    if (!(result->data)){
      destroy_cairopath(result);
      throw xn_Error("out of memory");
    }
    return result;
  }

  cairo_path_t* join_cairopaths(cairo_path_t* dest, cairo_path_t* addpath, bool destroypaths){
    bool startdest_closed = false;
    if(!dest || !addpath){
      throw xn_Error("NULL path");
    }
    cairo_path_t* newdest = create_cairopath(dest->num_data + addpath->num_data);
    if(!newdest){
      throw xn_Error("out of memory?");
    }
    cairo_path_data_t* newdata = newdest->data;
    for (int i=0; i < dest->num_data; i += dest->data[i].header.length){
      switch( dest->data[i].header.type){
      case CAIRO_PATH_LINE_TO:
	if(i ==0){
	  throw xn_Error("line must start with move_to");
	}
      case CAIRO_PATH_MOVE_TO:
	for (int j = 0;j <2; j++){
	  if(!j){
	    newdata[newdest->num_data].header.type = dest->data[i].header.type;
	    newdata[newdest->num_data++].header.length = 2;
	  }
	  else{
	    newdata[newdest->num_data].point.x = dest->data[i +1].point.x;
	    newdata[newdest->num_data++].point.y = dest->data[i +1].point.y;
	  }
	}
	break;
      case CAIRO_PATH_CURVE_TO:
	if(i ==0){
	  throw xn_Error("line must start with move_to");
	}
	for (int j = 0;j <4; j++){
	  if(!j){
	    newdata[newdest->num_data].header.type = dest->data[i].header.type;
	    newdata[newdest->num_data++].header.length = 4;
	  }
	  else{
	    newdata[newdest->num_data].point.x = dest->data[i +j].point.x;
	    newdata[newdest->num_data++].point.y = dest->data[i +j].point.y;
	  }
	}
      case CAIRO_PATH_CLOSE_PATH:
	if(i ==0){
	  throw xn_Error("line must start with move_to");
	}
	if (i == dest->num_data - 1){
	  startdest_closed = true;
	}
	newdata[newdest->num_data].header.type = dest->data[i].header.type;
	newdata[newdest->num_data++].header.length = 1;
      }
    }
    for (int i=0; i < addpath->num_data; i += addpath->data[i].header.length){
      switch( addpath->data[i].header.type){
      case CAIRO_PATH_LINE_TO:
	if(i ==0 && startdest_closed){
	  throw xn_Error("subpath must start with move_to");
	}
      case CAIRO_PATH_MOVE_TO:
	for (int j = 0;j <2; j++){
	  if(!j){
	    newdata[newdest->num_data].header.type = addpath->data[i].header.type;
	    newdata[newdest->num_data++].header.length = 2;
	  }
	  else{
	    newdata[newdest->num_data].point.x = addpath->data[i +1].point.x;
	    newdata[newdest->num_data++].point.y = addpath->data[i +1].point.y;
	  }
	}
	break;
      case CAIRO_PATH_CURVE_TO:
	if(i ==0 && startdest_closed){
	  throw xn_Error("subpath must start with move_to");
	}
	for (int j = 0;j <4; j++){
	  if(!j){
	    newdata[newdest->num_data].header.type = addpath->data[i].header.type;
	    newdata[newdest->num_data++].header.length = 4;
	  }
	  else{
	    newdata[newdest->num_data].point.x = addpath->data[i +j].point.x;
	    newdata[newdest->num_data++].point.y = addpath->data[i +j].point.y;
	  }
	}
      case CAIRO_PATH_CLOSE_PATH:
	if(i ==0 && startdest_closed){
	  throw xn_Error("subpath must start with move_to");
	}
	newdata[newdest->num_data].header.type = addpath->data[i].header.type;
	newdata[newdest->num_data++].header.length = 1;
      }
    }
    cairo_path_t*olddest =dest;
    dest = newdest;
    if(destroypaths){
      destroy_cairopath(addpath);
    }
    destroy_cairopath(olddest);
    return dest;
  }

  void copy_cairopath(cairo_path_t* src, cairo_path_t* dest){
    if (src->status != CAIRO_STATUS_SUCCESS){
      throw CairoError("src in error state", src->status);
    }
    if(!dest){
      dest = new_copy_cairopath(src);
      return;
    }
    if (dest->status != CAIRO_STATUS_SUCCESS){
      throw CairoError("dest in error state", dest->status);
    }
    if (dest->num_data < src->num_data){
      destroy_cairopath(dest);
      dest = new_copy_cairopath(src);
    }
    else{
      dest->num_data = 0;
      int datalength = 0;
      for (int i=0; i < src->num_data; i += src->data[i].header.length){
	if(i ==0 && src->data[i].header.type != CAIRO_PATH_MOVE_TO){
	  throw xn_Error("line must start with move_to");
	}
	switch( src->data[i].header.type){
	case CAIRO_PATH_LINE_TO:
	case CAIRO_PATH_MOVE_TO:
	  datalength= 2;
	  break;
	case CAIRO_PATH_CURVE_TO:
	  datalength = 4;
	  break;
	case CAIRO_PATH_CLOSE_PATH:
	  datalength = 1;
	  break;
	}
	for (int j = 0;j <datalength; j++){
	  if(!j){
	    dest->data[dest->num_data].header.type = src->data[i].header.type;
	    dest->data[dest->num_data++].header.length = datalength;
	  }
	  else{
	    dest->data[dest->num_data].point.x = src->data[i +j].point.x;
	    dest->data[dest->num_data++].point.y = src->data[i +j].point.y;
	  }
	}
      }   
    }
  }

  cairo_path_t* new_copy_cairopath(cairo_path_t* src){
    if (src->status != CAIRO_STATUS_SUCCESS){
      throw CairoError("src in error state", src->status);
    }
    cairo_path_t* newcopy = create_cairopath(src->num_data);
    cairo_path_data_t* dest = newcopy->data;
    for (int i=0; i < src->num_data; i += src->data[i].header.length){
      for(int j=0; j < src->data[i].header.length; j++){
	if(!j){
	  dest[newcopy->num_data].header.type = src->data[i].header.type;
	  dest[newcopy->num_data++].header.length = src->data[i].header.length;
	}
	else{
	  dest[newcopy->num_data].point.x = src->data[i +j].point.x;
	  dest[newcopy->num_data++].point.y = src->data[i +j].point.y;
	}
      }
    }
    return newcopy;
  }

  cairo_path_data_t* create_pathdata(size_t sz){
    cairo_path_data_t* data = new cairo_path_data_t[sz];// or use malloc/free?
    return data;
  }

  cairo_path_data_t* reallocate_pathdata(cairo_path_t* path , size_t newsz){
    cairo_path_data_t* olddata = path->data;
    cairo_path_data_t* newdata = path->data = new cairo_path_data_t[newsz];
    for (int i = 0; i < path->num_data; i+= path->data[i].header.length){
      for(int j = 0;j < path->data[i].header.length; j++){
	if(!j){
	  newdata[i].header.type = olddata[i].header.type;
	  newdata[i].header.length = olddata[i].header.length;
	}
	else{
	  newdata[i +j].point.x = olddata[i +j].point.x;
	  newdata[i +j].point.y = olddata[i +j].point.y;
	}
      }
    }
    destroy_pathdata(olddata);
    return path->data;
  }

  void destroy_cairopath(cairo_path_t* crpath){
    if(crpath){
      if (crpath->num_data && crpath->data){
	destroy_pathdata(crpath->data);
      }
      delete crpath;
      crpath = NULL;
    }
  }

  void destroy_pathdata(cairo_path_data_t* data_destroy){
    if(data_destroy){
      delete[] data_destroy;
      data_destroy = NULL;
    }
  }

  
  cairo_path_data_t* set_pathdata_header(cairo_path_data_t* data, cairo_path_data_type_t typ, int length){
    if(!data){
      throw xn_Error("pointer to path data NULL");
    }
    data->header.type=typ;
    data->header.length = length;
    return data;
  }

  size_t count_cairodata_4copy(const cairo_path_t* src){
    size_t result(0);
    //cairo_path_data_t data = src->data;
    for (int i=0; i < src->num_data; i += src->data[i].header.length){
      switch( src->data[i].header.type){
      case CAIRO_PATH_MOVE_TO:
      case CAIRO_PATH_LINE_TO:
	{
	  result +=2;   continue;
	}
      case CAIRO_PATH_CURVE_TO:
	{
	  result +=4;   continue;
	}
      case CAIRO_PATH_CLOSE_PATH:
	{
	  result++;   continue;
	}
      }
    }
    return result;
  }

  cairo_path_data_t* set_pathdata_point(cairo_path_data_t* data, const coords& pt){
    if(!data){
      throw xn_Error("pointer to path data NULL");
    }
    data->point.x = pt.get_x();
    data->point.y = pt.get_y();
    return data;
  }
  cairo_path_t* XndrawPath::transform_inplace(Cairo::Matrix& matrix, cairo_path_t* inpath ){
    //cairo_matrix_t* matrix = mat.cobj();
    cairo_path_t* result = inpath;
    if (!result){
      result = this->cobj();
    }
    result=  cairomatrix_transform_rv(matrix, result);
    return result;
  }
    
  NCOORDS::simple_click_history* XndrawPath::to_click_history(){
    cairo_path_t* path_cobj = cobj();
    if (!path_cobj || (path_cobj->num_data==0)){
      return NULL;
    }
    //caller responsible 4 deleting this
    simple_click_history* result = new simple_click_history;
    cairo_path_data_t *data=NULL;
    coords tmp;
    for (int i=0; i < path_cobj->num_data; i += path_cobj->data[i].header.length) { 
      data = &path_cobj->data[i];
    
      switch (data->header.type) {
      case CAIRO_PATH_MOVE_TO:
	tmp.set(data[1].point.x,data[1].point.y);
	result->push_back(&tmp, MOVE_CLICK);
	continue;
      case CAIRO_PATH_LINE_TO:
	tmp.set(data[1].point.x,data[1].point.y);
	result->push_back(&tmp,LINE_CLICK);
	continue;
      case CAIRO_PATH_CURVE_TO:
	//do_curve_to_things --DATA[0] is the header, DATA[1] AND[2]ARE THE HANDLES,[3] IS THE PT! ..I think...
	//THE HANDLES determine the curve between this 
	//and precedingline/move/bez click; 
	//ie data[1] is the previous point's handle heading towards this point, 
	//data[2] is this points handle towards the previous pt,
	//so clickhistory's arrangement with the 2 handles after is 'wrong'
	tmp.set(data[3].point.x,data[3].point.y);
	result->push_back(&tmp, BEZIER_CLICK);
	tmp.set(data[1].point.x,data[1].point.y);
	result->push_back(&tmp,BEZIER_HANDLE);
	tmp.set(data[2].point.x,data[2].point.y);
	result->push_back(&tmp,BEZIER_HANDLE);
	continue;
      case CAIRO_PATH_CLOSE_PATH:
  
	result->push_back(&tmp,CLOSE_PATH);
	//do_close_path_things();
	continue;
      }
    }
    return result;
  }
  XndrawPath::XndrawPath(const XndrawPath& src) : c_path(new_copy_cairopath(src.c_path)), xndraw_owns_path(true)
    {}
  XndrawPath::XndrawPath(const simple_click_history& clicks):
    c_path(XndrawPath::cairo_from_history(clicks)),
    xndraw_owns_path(true)
      {}

  cairo_path_t* XndrawPath::cobj(){
    return c_path;
  }

  const cairo_path_t* XndrawPath::cobj()const{
    return c_path;
  }
  void set_path_flattener(){
    NCOORDS::simple_click_history::set_flattener(&flatten_path_copy);
  }

  simple_click_history flatten_path_copy(const simple_click_history& src){
    cRect box;
    src.get_extents(box); 
    cairo_surface_t* surface=cairo_image_surface_create(CAIRO_FORMAT_ARGB32, box.width(), box.height());;
    if(!surface){throw xn_Error("cairo_image_surface_create() failes");}
    cairo_t* flattener = cairo_create(surface);
    if(!flattener){
      throw xn_Error("cairo NULL");
    }
    cairo_draw_clickhistory(flattener, src);
    simple_click_history res;    
    cairo_path_t* cairopath = cairo_copy_path_flat(flattener);
    //todo: check status
    XndrawPath p(cairopath, false);
    simple_click_history* p_tmp= p.to_click_history();//why pointer?
    if(!p_tmp){
      throw xn_Error("to_click_history(); returned NULL");
    }
    res = *p_tmp;
    delete p_tmp;
    cairo_surface_destroy(surface);
    cairo_destroy(flattener);
    return res;
  }

  cairo_path_t* XndrawPath::cairo_from_history(const simple_click_history& clicks){
    std::cerr << "cairo_from_history : size of history: "<< clicks.size()<<std::endl;
     unsigned long iters =clicks.size();
    size_t buffersize =XN_PATH_SIZE(iters);
    cairo_path_t* rawpath = create_cairopath(buffersize);
    coords theclick, handle1, handle2;
   if(rawpath->data){
      for (int i = 0; i < iters; i++){
	cairo_path_data_t* data =rawpath->data + rawpath->num_data;
	if(rawpath->num_data + 4 >= buffersize){
	  buffersize *= 2;
	  data= reallocate_pathdata(rawpath, buffersize) + rawpath->num_data;
	}
	int ptflags = clicks[i].second;
	if(ptflags & ORIGIN_CLICK  || ptflags & OFFSET_CLICK){
	  std::cerr<<"ptflags & ORIGIN_CLICK  || ptflags & OFFSET_CLICK => nothing doing"<<std::endl;
	}
	else if(ptflags & MOVE_CLICK){
	  set_pathdata_header(data++, CAIRO_PATH_MOVE_TO, 2);
	  data->point.x= clicks[i].first.x;
	  data->point.y= clicks[i].first.y;
	  rawpath->num_data += 2;
	}
	else if(ptflags & LINE_CLICK){
	  set_pathdata_header(data++, CAIRO_PATH_LINE_TO, 2);
	  data->point.x= clicks[i].first.x;
	  data->point.y= clicks[i].first.y;
	  rawpath->num_data += 2;
	}
	else if(ptflags & 	 BEZIER_CLICK){
	  set_pathdata_header(data++, CAIRO_PATH_CURVE_TO, 4);
	  //handles before clicks in cairo::path
	  theclick = clicks[i++].first;
	  if (!(clicks[i].second & BEZIER_HANDLE )){
	    handle1 = theclick;
	    handle2= theclick;
	  }
	  else if (!(clicks[i+1].second & BEZIER_HANDLE)){
	    handle1 = clicks[i].first;
	    handle2= clicks[i++].first;//ahead by one more
	  }
	  else{
	    handle1 = clicks[i++].first;
	    handle2= clicks[i++].first;//ahead by two more
	  }
	  data->point.x= handle1.x;
	  (data++)->point.y= handle1.y;
	  data->point.x= handle2.x;
	  (data++)->point.y= handle2.y;
	  data->point.x= theclick.x;
	  data->point.y= theclick.y;
	  rawpath->num_data += 4;	
	}
	else if(ptflags &    BEZIER_HANDLE){
	  rawpath->status =CAIRO_STATUS_INVALID_CONTENT;
	  throw xn_Error("BEZIER_CLICK must precede BEZIER_HANDLE \n CAIRO_STATUS_INVALID_CONTENT set");
	}
	else if(ptflags &  CLOSE_PATH){
	  set_pathdata_header(data++, CAIRO_PATH_CLOSE_PATH, 1);
	  rawpath->num_data++;
	}
	else {
	  rawpath->status =CAIRO_STATUS_INVALID_FORMAT;
	  throw xn_Error("unknown clicktype: CAIRO_STATUS_INVALID_FORMAT set");
	}  
      }
    }
    if(rawpath){
      if(!rawpath->data){ std::cerr <<" rawpath created without data"<<std::endl; }
      std::cerr <<" rawpath->num_data :"<<rawpath->num_data <<std::endl;
    }
    else{ std::cerr <<" rawpath NULL"<<std::endl;}
    return rawpath;
  }


  XndrawPath& XndrawPath::operator=(const XndrawPath& src){
    if(&src != this){
      cairo_path_t* oldpath = this->c_path;
      bool owned_olpath = xndraw_owns_path;
      if (!src.c_path){ this->c_path = NULL; }
      else{
	this->c_path = new_copy_cairopath(src.c_path);
	if(!this->c_path){
	  throw xn_Error("out of memory");
	}
	this->xndraw_owns_path = true;
      }
      destroy_c_path(oldpath, owned_olpath);
    }
    return *this;
  }

  void XndrawPath::destroy_c_path(cairo_path_t* path, bool ownedby_x){
    if (path){
      if(ownedby_x){
	destroy_cairopath(path);
      }
      else{
	cairo_path_destroy(path);
      }
      path = NULL;
    }
  }
  void XndrawPath::rotate(const NCOORDS::coords& centre_, double angle_/*radians*/){
    //fix == use matrix
    cairo_matrix_t matrix;
    //test this to make sure the behaviour is right
    cairo_matrix_init_translate(&matrix,  -centre_.x, -centre_.y);
    cairo_matrix_init_rotate(&matrix, angle_);
    cairo_matrix_translate(&matrix, centre_.x, centre_.y);
    this->transform(matrix);
  }

  void XndrawPath::transform(const cairo_matrix_t& matrix){
    cairo_path_t* path = cobj();
    if (path->status != CAIRO_STATUS_SUCCESS){
      if (path->status == CAIRO_STATUS_NO_MEMORY){
	throw xn_Error("out of memory");
      }
      else{
	throw xn_Error("path in error state - TODO:handler to determine the error");
      }
    }
    cairo_path_data_t *data = path->data;
    if(!path->num_data){
      return;
    }
    for (int i=0; i < path->num_data; i += data->header.length){
      data = data + i;
      switch (data->header.type) {
      case (CAIRO_PATH_MOVE_TO||CAIRO_PATH_LINE_TO):
	{
	  cairo_matrix_transform_point(&matrix, &(data[i+1].point.x), &(data[i+1].point.y));
	  continue;
	}
      case CAIRO_PATH_CURVE_TO:
	{
	  cairo_matrix_transform_point(&matrix, &(data[i+1].point.x), &(data[i+1].point.y));
	  cairo_matrix_transform_point(&matrix, &(data[i+2].point.x), &(data[i+2].point.y));
	  cairo_matrix_transform_point(&matrix, &(data[i+3].point.x), &(data[i+3].point.y));
	  continue;
	}
      default:
	{
	  continue;
	}
      }
    }
  }

}//namespace xNDRAW
