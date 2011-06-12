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



#include "drawing.h"
#include "io_uncollide.h" // change for text_io
#include <iostream>
#include "pathproducer.h"


  
using namespace xNDRAW;
using namespace NCOORDS;

unsigned int Drawingbase::serialcount=0;  
//TODO constructor with coords

Drawing::Drawing( cairo_t* cr,cairo_surface_t* scratch, ncoords centrepoint) :
  Drawingbase(cr, scratch),
  point_data(centrepoint),
  next(centrepoint){
  coords radial = *point_data.get_radial();
  std::pair<coords, int> startspot(radial, MOVE_CLICK);
  draw_to(startspot);
}

Drawing::Drawing( cairo_t* cr,cairo_surface_t* scratch, ncoords centrepoint, const simple_click_history& clicks)
  :
  Drawingbase(cr, scratch, clicks),
  point_data(centrepoint),
  next(centrepoint){
  std::pair<coords, int> startspot( *(point_data.get_radial()),MOVE_CLICK) ;
  draw_to(startspot);
}

 Drawing::Drawing(Cairo::RefPtr<Cairo::Surface> target,cairo_surface_t* scratch, ncoords centrepoint):
   Drawingbase(cairo_create(target->cobj()), scratch),
   point_data(centrepoint),
  next(centrepoint){
 std::pair<coords, int> startspot( *(point_data.get_radial()),MOVE_CLICK) ;
  draw_to(startspot);
}


void Drawing::reset(int point_no){
  reset_base(point_no);
}

void Drawing::reset_base(int point_no, bool draw_it){
  point_data.set_baseno(point_no);
  next.set_baseno(point_no);
  point_data.fill_container(current);
  save();
  paint_white(true);
  if (draw_it) {
    draw_to(*history);
  }
  restore();
}


void Drawing::set_to_origin(double ex, double why){
  set_origin(ex,why);
  move_to(ex,why);
}


void Drawing::curve_to(coords* pt){
  line_to(pt);// a fudge for now - only for the path being created while mouse is down
}
void Drawing::curve_to(coords* h1, coords* h2, coords* pt){
  point_data = next;
  next.set_radial(*pt);
  ncoords nh1(point_data);
  ncoords nh2(point_data);
  nh1.set_radial(*h1);
  nh2.set_radial(*h2);
  deqC handl1_deq;
  deqC handl2_deq;
  nh1.fill_container(handl1_deq);
  nh2.fill_container(handl2_deq);
  point_data.fill_container(current);
  next.fill_container(future);
  deqC::iterator h1_iter = handl1_deq.begin();
  deqC::iterator h2_iter = handl2_deq.begin();
 deqC::iterator cur_iter = current.begin();
 for(deqC::iterator start = future.begin();start != future.end();start++, h1_iter++, h2_iter++, cur_iter++){
   move_to((*cur_iter)->x, (*cur_iter)->y);
   curve_to((*h1_iter)->x, (*h1_iter)->y, (*h2_iter)->x, (*h2_iter)->y, (*start)->x, (*start)->y);
 }
}
void Drawing::curve_to(double h1x, double h1y, double h2x, double h2y, double ptx, double pty){
  Drawingbase::curve_to( h1x, h1y, h2x, h2y, ptx, pty);
}


 void Drawing::move_to(coords* pt){
   point_data = next;
   next.set_radial(*pt);
 }

cairo_t* Drawing::draw_path_to_cairo(cairo_t* cr){
  typedef std::deque<simple_click_history>::const_iterator const_pathlist_iterator;
  PathRotator::queue_type q;
  PathRotator* m_rotator(NULL);
  coords* offsettt(NULL);
  create_path_rotator(q,  m_rotator,offsettt);
  if(!m_rotator){
    throw xn_Error("create_path_rotator returned NUKLL:");
  }
  path_container_type* m_paths= m_rotator->create_paths();
  for(const_path_container_iterator w = m_paths->begin();
      w != m_paths->end();w++){
    //const simple_click_history& p = *w;
    cairo_draw_clickhistory(cr, (*w).first);
    //source_transform_info() is ignorant of any flippng etc - should be subject
    //to those also, where whole path flipped etc?
    set_source_with_transform((*w).second, COLOURROLE_STROKE);
    cairo_stroke(cr);
  }
  delete m_paths;
  return cr;
}
//overload to offset and rotate in drawingset
void Drawing::do_draw_path(const ncoords& rotator){
  typedef PathRotator::queue_type queue_type;
  queue_type q_unused;  
  coords* offset(NULL);
  PathRotator* m_rotator(NULL);
  create_path_rotator( q_unused, m_rotator,offset);
 std::cerr<< "create_path_rotator() done\n";
 //if(!m_rotator){
 //   throw xn_Error("create_path_rotator returned NULL:");
 // }
 std::cerr<<"not NULL\n";
  path_container_type* m_paths= m_rotator->create_paths();
  if(!m_paths){
    throw xn_Error("m_rotator->create_paths(); faild");
  }
#ifdef DEBUG
  if(m_paths->first.size() != get_baseno()){
    throw xn_Error("wrong number of paths");
  }
#endif
for(const_path_container_iterator w = m_paths->begin();
      w != m_paths->end();w++){
  std::cerr << "iterloop"<<std::endl;
  //const simple_click_history& p = *w.first;
  cairo_draw_clickhistory(m_cobject, (*w).first);
    std::cerr<<"drew path on cairo\n";
    set_source_with_transform( (*w).second, COLOURROLE_STROKE);
    stroke_preserve();
    set_source_to_fill();
    fill();//flushes path
  }
  delete m_paths;
  check_status();
 
}
void  Drawing::create_path_rotator(PathRotator::queue_type& q,
				   PathRotator*& outref, const coords* offset){
      check_rotator_offset(offset);
      coords offs;
      if(offset){offs = *offset;}
      outref = new PathRotator(q, *history, point_data, offs);
    }
void Drawing::do_draw_path(){//point already in history
  std::cout<< " ,,,\nDrawing::do_draw_path()"<< std::endl;
  try{
  do_draw_path(point_data);
  }
  catch(const std::exception& e){
    std::cerr << "do_draw_path thrrew an exception: "<<e.what()<<std::endl;
    throw e;
  }
}

void Drawing::line_to(coords* point){
point_data = next;
  next.set_radial(* point);
  point_data.fill_container(current);
  next.fill_container(future);
  deqC_iter p_walk = current.begin();
  for(deqC_iter f_walk = future.begin(); f_walk!=future.end(); p_walk++, f_walk++){
    coords* move = *p_walk ;
    coords* line = *f_walk ;
    move_to(move->get_x(), move->get_y());
    line_to(line->get_x(), line->get_y());
  }
}



void Drawing::stroke_history(){  
  do_draw_path();
  save();
  set_source_to_stroke();
  cairo_stroke(m_cobject);
  paint();
  restore();
  
#ifdef DEBUG
  std::cerr <<"DEBUG:fill_history()THE drawingbase* OBJECT: ( at "
	    <<this <<"):\n"<<*this <<"TODO:pass painting slot into (AT LEAST)the most expensive drawing ops, so we don't to wait till iter is complete to start fill/stroking to clear some points from the cairo onject"<<std::endl;
#endif //DEBUG
}

void Drawing::fill_history(){  
  do_draw_path();
  save();
  set_source_to_fill();
  fill();
  paint();
  restore(); 
#ifdef DEBUG
  std::cerr <<"DEBUG:stroke_history()THE drawingbase* OBJECT: ( at "
	    <<this <<"):\n"<<*this <<std::endl;
#endif
}

void Drawing::fill_preserve_history(){
  do_draw_path();
  save();
  set_source_to_fill();
  fill_preserve();
  paint();
  restore();
}

void Drawing::stroke_preserve_history(){
  do_draw_path();
  save();
  set_source_to_stroke();
  stroke_preserve();
  paint();
  restore();
}

void Drawing::line_to_history(coords* offset){
  //maybe should implement the offset
  
  stroke_preserve_history();
}


void Drawing::fill(int x){
  fill_history();
}

NCOORDS::simple_click_history Drawing::import_history(NCOORDS::simple_click_history& path, coords* origin_ , int base){
      if (base == 0){
	base = get_baseno();
      }
      else{set_baseno(base);}
      simple_click_history old_history = get_history();
      if (origin_ == NULL){ throw xn_Error("NULL pointer passed");}
      set_to_origin(origin_);
      set_history(&path);
      const coords& temp = (*(path.end()-1)).first;
      move_to(temp.x, temp.y);
      return old_history;
    }

void Drawing::set_next(ncoords* nxt){
  if (point_data.get_centre() != nxt->get_centre()){
    throw xn_Error("Houston we have a problem!\norigins don't match");
  }
  next.set(*nxt);
}

void Drawing::set_point_data(ncoords* last){
  if (point_data.get_centre() != last->get_centre()){
    throw xn_Error("Houston we have a problem!\norigins don't match");
  }
  point_data.set(*last);
}
   
    void Drawing::insert_path(NCOORDS::simple_click_history path, bool relative , int base ){
      get_history().insert_path(path, relative);
      if(base!= 0){set_baseno(base);}
      begin_new_path();
      paint_white(true);
      do_draw_path();
    }

 void Drawing::append_path(NCOORDS::simple_click_history path, bool relative , int base ){
   get_history().append_path(path, relative);
      if(base!= 0){set_baseno(base);}
      stroke();
      paint_white(true);
       do_draw_path();
    }

    void Drawing::prepend_path(NCOORDS::simple_click_history path, bool relative , int base ){
      history->prepend_path(path, relative);
      if(base!= 0){set_baseno(base);}
      stroke();
      paint_white(true);
      line_to_history();
    }

NCOORDS::simple_click_history Drawing::cut_path(int pos, int count, bool relative){
  NCOORDS::simple_click_history result= history->cut_path(pos,count,relative);
      stroke();
      paint_white(true);
       do_draw_path();
      return result;
    }


void Drawing::get_extents(cairo_rectangle_t& drawrect, const coords* offset){
  cRect tmprect;
  coords centred, offs(point_data.get_state());
  ncoords nctmp(point_data);
  if(offset){
    // offset -= (coords&)point_data
    offs= *offset;
  }
  PathRotator::queue_type q_unused;
  PathRotator r(q_unused, *history, nctmp, offs);

  path_container_type* res= r.create_paths();
  cRect result;
  for(const_path_container_iterator iter = res->begin(); iter != res->end(); iter++){
    cRect tmpresult;
    (*iter).first.get_extents(tmpresult);
    result += tmpresult;
  }
  
  drawrect= cRect_to_cairo_rect(result);
}
 

bool Drawing::path_within_bounds(const coords& offset){
  cairo_rectangle_t pathbounds;
  cairo_surface_t* b_surface = cairo_get_target(buffer);
  cairo_rectangle_t imagebounds = {0, 0, 
				   cairo_image_surface_get_width(b_surface), 
				   cairo_image_surface_get_height(b_surface)};
  this->get_extents(pathbounds, &offset);
  return NCOORDS::rects_intersect(imagebounds, pathbounds);
}
