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

#include "opsequence.h"

namespace xNDRAW{

DrawRecorder::DrawRecorder(Drawingbase* contex,
		 repository_type* store):
      trackable( contex),
	sequence_store(store){
	  if( trackable){
	    trackable->reference();
	  }
	}


 DrawRecorder::~DrawRecorder(){
if( trackable){
	trackable->unreference();
	}
      }

void DrawRecorder::set_sequence_store(repository_type* store){
	sequence_store=store;
      }

void DrawRecorder::new_sequence(){
	if(sequence_store){
	  sequence_store->push_back(container_type());
	  container_type& c_ref=*(sequence_store->end()-1);
	  size_type sz= draw_operations.size();
	  c_ref.swap(draw_operations);
	  if(draw_operations.size() || (c_ref.size() != sz)){
	    throw xn_Error ("opsequence swap() failed");
	  }
	}
      }

void DrawRecorder::set_methodowner(Drawingbase* d){
	Drawingbase* old =d;
	trackable=d;
	if(trackable){trackable->reference();}
	if(old){old->unreference();}
      }

DrawRecorder::const_iterator DrawRecorder::end()const{
      return draw_operations.end();
    }

    DrawRecorder::iterator DrawRecorder::end(){
      return draw_operations.end();
    }

 DrawRecorder::const_iterator  DrawRecorder::begin()const{
      return draw_operations.begin();
    }

     DrawRecorder::iterator  DrawRecorder::begin(){
      return draw_operations.begin();
    }

DrawRecorder::reference DrawRecorder::front(){
      return draw_operations.front();
    }

    DrawRecorder::const_reference DrawRecorder::front()const{
      return draw_operations.front();
    }

DrawRecorder::reference DrawRecorder::back(){
      return draw_operations.back();
    }

    DrawRecorder::const_reference DrawRecorder::back()const{
      return draw_operations.back();
    }

 void DrawRecorder::push_back(operation_type* op){
      draw_operations.push_back(op);
    }

void DrawRecorder::set_source_rgb(double r, double g, double b){
      operation_type* op =create_boundop(&Drawingbase::set_source_rgb, trackable, r, g,b);
      draw_operations.push_back(op);
    }

    void DrawRecorder::set_tolerance(double tolerance){
      operation_type* op =create_boundop(&Drawingbase::set_tolerance, trackable,tolerance );
      draw_operations.push_back(op);
    }

void DrawRecorder::set_fill_rule(Cairo::FillRule fill_rule){
      operation_type* op =create_boundop(&Drawingbase::set_fill_rule,   trackable,fill_rule );
      draw_operations.push_back(op);
    }

    void DrawRecorder::set_line_width(double lw){
      draw_operations.push_back(create_boundop(&Drawingbase::set_line_width, 
					       trackable, lw));
    }

void DrawRecorder::set_line_cap(Cairo::LineCap line_cap){
      operation_type* op =create_boundop(&Drawingbase::set_line_cap,   trackable,line_cap );
      draw_operations.push_back(op);
    }

    void DrawRecorder::set_line_join(Cairo::LineJoin line_join){
      operation_type* op =create_boundop(&Drawingbase::set_line_join,trackable,line_join );
      draw_operations.push_back(op);
    }

    void DrawRecorder::set_antialias(Cairo::Antialias antialias){
      operation_type* op =create_boundop(&Drawingbase::set_antialias,trackable,antialias );
      draw_operations.push_back(op);
    }

void DrawRecorder::unset_dash(){
      operation_type* op =create_boundop(&Drawingbase::unset_dash,   trackable);
      draw_operations.push_back(op);
    }

    void DrawRecorder::set_miter_limit(double limit){
      operation_type* op =create_boundop(&Drawingbase::set_miter_limit,trackable, limit );
      draw_operations.push_back(op);
    }

    void DrawRecorder::translate(double tx, double ty){
      operation_type* op =create_boundop(&Drawingbase::translate,   trackable, tx, ty );
      draw_operations.push_back(op);
    }

 void DrawRecorder::scale(double sx, double sy){
      operation_type* op =create_boundop(&Drawingbase::scale,   trackable, sx, sy );
      draw_operations.push_back(op);
    }

    void DrawRecorder::rotate(double angle_radians){
      operation_type* op =create_boundop(&Drawingbase::rotate,   trackable, angle_radians );
      draw_operations.push_back(op);
    }

  void DrawRecorder::rotate_degrees(double angle_degres){
      operation_type* op =create_boundop(&Drawingbase::rotate_degrees,   trackable, angle_degres );
      draw_operations.push_back(op);
    }
  
    void DrawRecorder::transform(const Cairo::Matrix& matrix){
      operation_type* op =create_boundop(&Drawingbase::transform__mm, trackable  , matrix );
      draw_operations.push_back(op);
    }

    void DrawRecorder::transform(const cairo_matrix_t& matrix){
      Drawingbase*d=trackable->dobj();
      operation_type* op =new BoundOp<void,Drawingbase,const cairo_matrix_t&, void, void>(	&Drawingbase::transform__cobj,   d , matrix );
      draw_operations.push_back(op);
    }

void DrawRecorder::set_matrix(const Cairo::Matrix& matrix){
      operation_type* op =create_boundop(&Drawingbase::set_matrix__mm, trackable, matrix );
      draw_operations.push_back(op);
    }

    void DrawRecorder::set_matrix(const cairo_matrix_t& matrix){
      operation_type* op =new BoundOp<void,Drawingbase,const cairo_matrix_t&, void, void>(	&Drawingbase::set_matrix__cobj,   trackable, matrix );

      draw_operations.push_back(op);
    }

void DrawRecorder::set_identity_matrix(){
      operation_type* op =create_boundop(&Drawingbase::set_identity_matrix,   trackable );
      draw_operations.push_back(op);
    }

void DrawRecorder::mask(const Cairo::RefPtr<const Cairo::Pattern>& pattern){
	 typedef const Cairo::RefPtr<const Cairo::Pattern>& const_pattern_reference;
	 typedef BoundOp<void,Drawingbase,const_pattern_reference,void, void> local_op_type;
       operation_type* op=new local_op_type(&Drawingbase::mask,   trackable,pattern);
       draw_operations.push_back(op);
       }

void DrawRecorder::append_path(const Cairo::Path& path){
typedef BoundOp<void,Drawingbase,const Cairo::Path&,void, void> local_op_type;
 operation_type* op= new local_op_type(&Drawingbase::append_path,  trackable, path);
      draw_operations.push_back(op);
      }

void DrawRecorder::set_source(const Cairo::RefPtr<const Cairo::Pattern>& source){
typedef BoundOp<void,Drawingbase,
    const Cairo::RefPtr<const Cairo::Pattern>&, void, void> local_op_type;
      operation_type* op =  new local_op_type(&Drawingbase::set_patternsource,
					      trackable->dobj()  , source);
      //operation_type* op=create_boundop(&Drawingbase::set_patternsource, trackable->dobj()  , source);
      draw_operations.push_back(op);
    }

    void DrawRecorder::push_front(operation_type* op){
      draw_operations.push_back(op);
    }

  void DrawRecorder::rel_line_to(double dx, double dy){
 operation_type* op=create_boundop(&Drawingbase::rel_line_to,   trackable, dx, dy);
    draw_operations.push_back(op);
    }
void DrawRecorder::restore(){
 operation_type* op=create_boundop(&Drawingbase::restore,   trackable);
    push_back(op);
 }

void DrawRecorder::save(){
 operation_type* op=create_boundop(&Drawingbase::save,   trackable);
    push_back(op);
 }

void DrawRecorder::set_operator(Cairo::Operator oper){
 operation_type* op_tp=create_boundop(&Drawingbase::set_operator,   trackable, oper);
    draw_operations.push_back(op_tp);
 }

void DrawRecorder::rel_move_to(double dx, double dy){
 operation_type* op=create_boundop(&Drawingbase::rel_move_to,   trackable, dx, dy);
    draw_operations.push_back(op);
    }

void DrawRecorder::close_path(){
 operation_type* op=create_boundop(&Drawingbase::close_path,   trackable);
    draw_operations.push_back(op);
    }

void DrawRecorder::paint_with_alpha(double alpha){
 operation_type* op=create_boundop(&Drawingbase::paint_with_alpha,   trackable,
			    alpha);
    draw_operations.push_back(op);
    }

void DrawRecorder::fill_preserve(){
 operation_type* op=create_boundop(&Drawingbase::fill_preserve,   trackable);
    draw_operations.push_back(op);
    }

void DrawRecorder::show_page(){
 operation_type* op=create_boundop(&Drawingbase::show_page,   trackable);
    draw_operations.push_back(op);
    }

void  DrawRecorder::reset_clip(){
 operation_type* op=create_boundop(&Drawingbase::reset_clip,   trackable);
    draw_operations.push_back(op);
    }

void DrawRecorder::push_group(){
 operation_type* op=create_boundop(&Drawingbase::push_group,   trackable);
    draw_operations.push_back(op);
    }

void  DrawRecorder::push_group_with_content(Cairo::Content content){
    draw_operations.push_back(create_boundop(&Drawingbase::push_group_with_content, trackable,content));
  }

void DrawRecorder::pop_group_to_source(){
    draw_operations.push_back(create_boundop(&Drawingbase::pop_group_to_source, trackable));
  }

void 	DrawRecorder::redraw_line (bool paint_to_bg){
    operation_type* op=create_boundop(&Drawingbase::redraw_line,   
			       trackable,paint_to_bg);
    draw_operations.push_back(op);
  }

 void DrawRecorder::clip_preserve(){
 operation_type* op=create_boundop(&Drawingbase::clip_preserve,   trackable);
    draw_operations.push_back(op);
    }

void 	DrawRecorder::stroke_with_pattern (std::string pattername){
operation_type* op=create_boundop(&Drawingbase::stroke_with_pattern, 
			   trackable,pattername);
    draw_operations.push_back(op);
  }
  //needs specialisation
  //<void, TP, const arg1&, void, void>
 void 	DrawRecorder::add_png_pattern (const std::string &fname){
    operation_type* op=create_boundop<Drawingbase, const std::string&>(&Drawingbase::add_png_pattern, 
			   trackable,fname);
    draw_operations.push_back(op);
  }

void 	DrawRecorder::cleanup_history (){
operation_type* op=create_boundop(&Drawingbase::cleanup_history, 
			   trackable);
    draw_operations.push_back(op);
  }

void 	DrawRecorder::cleanup_histories (){
operation_type* op=create_boundop(&Drawingbase::cleanup_histories, 
			   trackable);
    draw_operations.push_back(op);
  }
//needs specialisation
  //<void, TP, const arg1&, void, void>
void 	DrawRecorder::paint_rect (const cairo_rectangle_t &r){
  operation_type* op=create_boundop<Drawingbase, const cairo_rectangle_t&>(&Drawingbase::paint_rect, 
			   trackable, r);
    draw_operations.push_back(op);
  }

void 	 DrawRecorder::set_baseno (int n){
operation_type* op=create_boundop(&Drawingbase::set_baseno, 
			   trackable, n);
    draw_operations.push_back(op);
  }

void 	DrawRecorder::stroke_history (){
operation_type* op=create_boundop(&Drawingbase::stroke_history, 
			   trackable);
    draw_operations.push_back(op);
  }

void 	DrawRecorder::stroke_preserve_history (){
operation_type* op=create_boundop(&Drawingbase::stroke_preserve_history, 
			   trackable);
    draw_operations.push_back(op);
  }

 void 	DrawRecorder::fill_preserve_history (){
operation_type* op=create_boundop(&Drawingbase::fill_preserve_history, 
			   trackable);
    draw_operations.push_back(op);
  }

void 	DrawRecorder::adjust_stroke_alpha (double a){
operation_type* op=create_boundop(&Drawingbase::adjust_stroke_alpha, 
			   trackable, a);
    draw_operations.push_back(op);
  }
//needs specialisation
  //<void, TP, arg1*, void, void>
void 	DrawRecorder::set_to_origin (NCOORDS::coords * pt){
operation_type* op=create_boundop(&Drawingbase::set_to_origin, 
			   trackable, pt);
    draw_operations.push_back(op);
  }
//needs specialisation
  //<void, TP, const arg1&, arg2, void>
void 	DrawRecorder::add_pattern (const std::string &nm, 
		     Cairo::RefPtr< Cairo::Pattern > pattern){
  operation_type* op=create_boundop<Drawingbase, const std::string &, 
                             Cairo::RefPtr< Cairo::Pattern > >(&Drawingbase::add_pattern,  trackable, nm, pattern);
    draw_operations.push_back(op);
  }
//needs specialisation
  //<void, TP, const arg1&, void, void>
void 	DrawRecorder::save_history (const simple_click_history &path){
  operation_type* op=create_boundop<Drawingbase, const simple_click_history &>(&Drawingbase::save_history, 
			   trackable, path);
    draw_operations.push_back(op);
  }

void 	DrawRecorder::clear_history (){
operation_type* op=create_boundop(&Drawingbase::clear_history, 
			   trackable);
    draw_operations.push_back(op);
  }
//needs specialisation(or no use)
  //<void, TP, arg1*, void, void>
void 	DrawRecorder::set_history (NCOORDS::simple_click_history *clicks){
operation_type* op=create_boundop(&Drawingbase::set_history, 
			   trackable, clicks);
    draw_operations.push_back(op);
  }

void 		DrawRecorder::really_clean_path (){
operation_type* op=create_boundop(&Drawingbase::really_clean_path, 
			   trackable);
    draw_operations.push_back(op);
  }

void 	DrawRecorder::clear_cairopath (){
operation_type* op=create_boundop(&Drawingbase::clear_cairopath, 
			   trackable);
    draw_operations.push_back(op);
  }

void 	DrawRecorder::fill_history (){
operation_type* op=create_boundop(&Drawingbase::fill_history, 
			   trackable);
    draw_operations.push_back(op);
  }

void 	DrawRecorder::paint_white (bool paint_buffer){
operation_type* op=create_boundop(&Drawingbase::paint_white, 
			   trackable, paint_buffer);
    draw_operations.push_back(op);
  }
//needs specialisation
  //<void, TP, arg1&, void, void>
void 	DrawRecorder::do_draw_to (std::pair< coords, int > &location){
  operation_type* op=create_boundop<Drawingbase, std::pair< coords, int > & >(&Drawingbase::do_draw_to, 
			   trackable, location);
    draw_operations.push_back(op);
  }

void DrawRecorder::clip(){
 operation_type* op=create_boundop(&Drawingbase::clip,   trackable);
    draw_operations.push_back(op);
    }

void DrawRecorder::copy_page(){
 operation_type* op=create_boundop(&Drawingbase::copy_page,   trackable);
    draw_operations.push_back(op);
    }

void DrawRecorder::fill(){
 operation_type* op=create_boundop(&Drawingbase::fill,   trackable);
    draw_operations.push_back(op);
    }


void DrawRecorder::stroke_preserve(){
 operation_type* op=create_boundop(&Drawingbase::stroke_preserve,   trackable);
    draw_operations.push_back(op);
    }

 void DrawRecorder::stroke(){
    operation_type* op=create_boundop(&Drawingbase::stroke,   trackable);
    draw_operations.push_back(op);
    }


void  DrawRecorder::paint(){
 operation_type* op=create_boundop(&Drawingbase::paint,   trackable);
    draw_operations.push_back(op);
    }

}
