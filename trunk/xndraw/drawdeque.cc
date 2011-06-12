//-*- c++ -*-

/*  Copyright (C) 2006-2011 John O'Driscoll
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "drawdeque.h"

using namespace xNDRAW;
using namespace NCOORDS;

void drawdeque::adjust_stroke_alpha(double alpha){
   draw_adjust_stroke_alpha functor(alpha);
for_each(begin(), end(), functor);
std::cout << "drawdeque:adjust_stroke_alpha(" << alpha << ")" << std::endl;
 }

void drawdeque::reinit_drawset(Nodes& map){
  for(iterator walk = begin();walk != end();walk++){
    (*walk)->check_type (std::cout, "in drawdeque::reinit_drawset... before");
    Drawingset* drawset = dynamic_cast<Drawingset*>(*walk);
    if(drawset){
      drawset->reset_nodes(map);
    }
    else{
      Drawing* draw = dynamic_cast<Drawing*>(*walk);
      Drawingset* newdraw  =	 new Drawingset(draw,map);
    }    
    (*walk)->check_type(std::cout, "in drawdeque::init_drawset... after");
  }
}

void drawdeque::init_drawset(const NCOORDS::Nodes& map){

  for(drawdeque::iterator walk = begin();walk != end();walk++){
    // Nodedeq map_copy(map);
    
    (*walk)->check_type (std::cout, "in drawdeque::init_drawset... before");
    if((*walk)->get_has_nodes()){
      continue;
    }
    Drawing* draw = dynamic_cast<Drawing*>(*walk);
    if (!draw){
      (*walk)->check_type(std::cerr, "not a Drawing but rather...   -->");
      throw xn_Error("not a drawing");
    }
    Drawingset* newdraw  =	 new Drawingset(draw,map);
    erase(walk);//what happens?
    insert(newdraw) ;
    (*walk)->check_type(std::cout, "in drawdeque::init_drawset... after");
  }
}

void drawdeque::set_stroke_colour(double r, double g, double b){
  COUT("drawdeque::set_stroke_colour(double r, double g, double b)");
  draw_set_stroke_rgb functor(r,g,b);
  std::for_each(begin(),end(), functor);	 
	} 

void drawdeque::set_stroke_colour(double r, double g, double b, double a){
  draw_set_stroke_colour functor(r,g,b,a);
  std::for_each(begin(),end(), functor); 
	}

void drawdeque::set_baseno(int base_){
      draw_set_baseno functor( base_);
std::for_each(begin(),end(), functor); 
	}

void drawdeque::set_fill_colour(double r, double g, double b){
draw_set_fill_rgb functor(r,g,b);
std::for_each(begin(),end(), functor);
}

void drawdeque::set_fill_colour(double r, double g, double b, double a){
draw_set_fill_colour functor(r,g,b,a);
std::for_each(begin(),end(), functor); 
}



drawdeque::~drawdeque(){
  std::cout << "virtual ~drawdeque(){" << std::endl;
  int i = 0;
  for (iterator walk = begin(); walk != end(); walk++){
    std::cout <<"how many context's in drawdeque?...: i = " << ++i << std::endl;
  }
}

void drawdeque::set_queue (std::queue<coords*>* queue){
	  pencilqueue = queue;
	}

void drawdeque::line_to(NCOORDS::coords* point){
  {std::cout << "virtual void line_to(NCOORDS::coords* point){" << std::endl;}
  draw_lineto_coords functor(point);
  for_each(begin(), end(), functor); 
}

void drawdeque::fill_history(){
  draw_fill_history functor;
  for_each(begin(), end(), functor);	 
}

void drawdeque::fill_preserve(){
	  draw_fillpreserve_functor  functor;
	  for_each(begin(), end(), functor);	  
	}

void drawdeque::stroke(){
	  draw_stroke_functor functor ;
	  for_each(begin(), end(), functor);	 
	}

void drawdeque::fill(){
	  draw_fill_functor functor;
	  for_each(begin(), end(), functor);	 
	}

bool drawdeque::save_nodes(std::string& filename){
  for(iterator walk = begin();walk != end();walk++){
    Drawingset* draw = dynamic_cast<Drawingset*>(*walk);
    if(!draw){
      continue;
    }
    if (draw->save_nodes(filename)){
      return true;
    }
  }
  return false;
}
void drawdeque::stroke_preserve(){
	  draw_strokepreserve_functor functor;
	  for_each(begin(), end(), functor);	  
	}

void drawdeque::set_to_origin(NCOORDS::coords* pt){
	  draw_setorigin_coords functor (pt);
	  for_each(begin(), end(), functor);	 
	}

void drawdeque::add(drawPtr& draw){
	  insert(draw->dobj());
	}

void drawdeque::add(Drawingbase* draw){
	 insert(draw);
	}

void drawdeque::add(Cairo::RefPtr<Drawingset> drawset){
	  insert(drawset->dobj());
	}

void drawdeque::save_to_png(std::string filename){
	  draw_save_png functor(filename);
	  for_each(begin(), end(), functor);	 
	}

void drawdeque::clear_history(){
	  draw_clear_history functor;
	  for_each(begin(),end(), functor);	 
	}
void drawdeque::set_linewidth(double width){
  draw_set_linewidth functor(width);
for_each(begin(),end(), functor);
}

void drawdeque::set_line_join(Cairo::LineJoin j){
	  draw_set_linejoin functor(j);
	  for_each(begin(),end(), functor);	 
	}

void drawdeque::set_fill_rule(Cairo::FillRule rule){
	  draw_set_fillrule functor (rule);
	  for_each(begin(),end(), functor);	 
	}

void drawdeque::set_line_cap(Cairo::LineCap cap){
	  draw_set_linecap functor (cap);
	  for_each(begin(),end(), functor);	  
	}

void drawdeque::set_operator(Cairo::Operator op){
	  draw_set_operator functor (op); 
	  for_each(begin(),end(), functor);	 
	}

void drawdeque::line_to_history(){
	  draw_line_to_history functor ;
	  for_each(begin(),end(), functor);	 
	}

void drawdeque::append_path(NCOORDS::simple_click_history path_, bool relative_ , int base_){
	  draw_append_clickpath functor(path_,relative_,base_);
	  for_each(begin(),end(), functor);	 
	}

void drawdeque::prepend_path(NCOORDS::simple_click_history path_, bool relative_ , int base_){
	  draw_prepend_clickpath functor (path_,relative_,base_);
	  for_each(begin(),end(), functor);	  
	}

void drawdeque::insert_path(NCOORDS::simple_click_history path_, bool relative_ , int base_ ){
	  draw_insert_clickpath functor(path_,relative_,base_);
	  for_each(begin(),end(), functor);	 
	}



void drawdeque::move_to(NCOORDS::coords* pt){
	  draw_moveto_coords functor (pt);
	  std::for_each(begin(),end(),functor);
	}

void drawdeque::stroke_preserve_history(){
draw_stroke_preserve_history functor ;
std::for_each(begin(),end(),functor);
    }

void drawdeque::stroke_history(){
draw_stroke_history functor ;
std::for_each(begin(),end(),functor);
    }

void drawdeque::fill_preserve_history(){
  draw_fill_preserve_history functor;
std::for_each(begin(),end(),functor);
    }

void drawdeque::save(){
draw_save functor;
std::for_each(begin(),end(),functor);
}

void drawdeque::restore(){
draw_restore functor;
std::for_each(begin(),end(),functor);
    }

void drawdeque::set_source_to_fill(){
      draw_source_to_fill functor;
std::for_each(begin(),end(),functor);
    }

void drawdeque::set_source_to_stroke(){
draw_source_to_stroke functor;
std::for_each(begin(),end(),functor);
}


std::queue<NCOORDS::coords*>& drawdeque::get_queue(){
	  return *pencilqueue;
}	

void drawdeque::set_pencil(Drawtool* tool){
	  pencil = tool;
	  pencilqueue= &(tool->get_queue());
	}

Drawtool& drawdeque::get_pencil(){
	  return *pencil;
	}


void drawdeque::set_flipped(bool flip){
  draw_set_flipped functor(flip);
      std::for_each(begin(),end(), functor);
    }
