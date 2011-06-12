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

#include "ratioflower.h"

namespace xNDRAW{

  RosyDraw::RosyDraw(Drawing* src):
    Drawing(src->get_visual(),
	    cairo_get_target(src->get_buffer()),
	    src->get_point_data() ),
    rosette(src->get_point_data(), src->get_history()){
    //saved_paths.swap(src->saved_paths);
    set_operator_scratch(Cairo::OPERATOR_SOURCE);
  }


  RosyDraw::RosyDraw(cairo_t* cr,cairo_surface_t* scratch, 
		     const npatternrosette& pattern):
    Drawing(cr,scratch,(const ncoords&)pattern),
    rosette(pattern){
set_operator_scratch(Cairo::OPERATOR_SOURCE);
    std::cerr <<"RosyDraw(cairo_t* cr,cairo_surface_t* scratch,\n\t\t\t"
	      <<"const npatternrosette& pattern)";
  }

void RosyDraw::do_draw_path(){
      rosette.clear_path();
      rosette.update_path_from_mirror(*history);
      draw_paint_rings();
    }
    void RosyDraw::do_draw_path(const ncoords& rotator){
      draw_to_path(rotator);
    }

 void  RosyDraw::set_line_width(double width){
      Drawing::set_line_width(width);
      rosette.set_linewidth(width);
    }

void RosyDraw::draw_to_path(){
      if(!buffer){throw xn_Error("not inited");}
      rosette.paint_to_cairo(buffer);
    }

    void  RosyDraw::draw_paint_rings(){
      for (unsigned char i =0; i< rosette.get_rings();i++){
	draw_scaled_ring(i);
      }
    }

NCOORDS::simple_click_history* RosyDraw::initialise_path(){
      history =Drawing::initialise_path();
      rosette.clear_path();
      return history;
    }

NCOORDS::simple_click_history* RosyDraw::initialise_path(const simple_click_history& path){
      history= Drawing::initialise_path(path);
      rosette.clear_path();
      rosette.update_path_from_mirror(*history);
      return history;
    }

void RosyDraw::assign_history(NCOORDS::simple_click_history* clicks){
      Drawing::assign_history(clicks);
      rosette.clear_path();
      rosette.update_path_from_mirror(*history);
    }
  void  RosyDraw::draw_to_path(const simple_click_history& path){

    rosette.update_path_from_mirror(path);
 if(!scratch){throw xn_Error("not inited");}
 //rosette.paint_to_cairo(buffer);
 Cairo::RefPtr<Cairo::Context> bufferref(new Cairo::Context(scratch));
draw_and_paint_path_to_context(bufferref);
 paint();
 if(!(&path == history)){
    rosette.update_path_from_mirror(*history);
 }
  }
void  RosyDraw::draw_to_path(const ncoords& rotator){
      rosette.update_path_from_mirror(*history);
      if(!scratch){throw xn_Error("not inited");}
      //rosette.paint_to_cairo(buffer);
      Cairo::RefPtr<Cairo::Context> bufferref(new Cairo::Context(scratch));
draw_and_paint_path_to_context(bufferref);
 paint();
    }

void RosyDraw::cleanup_history(){
      Drawing::cleanup_history();
      rosette.clear_path();
      rosette.update_path_from_mirror(*history);
    }
  void RosyDraw::draw_and_paint_path_to_context(Cairo::RefPtr<Cairo::Context> cr){
  for (unsigned int i=rosette.get_rings();i >0 ;--i){
    std::cerr<<"drawing ring no. "<< i <<std::endl;
    draw_and_paint_scaled_ring_to_context(cr, i);
  }
}

  npatternrosette& RosyDraw::get_rosette(){
    return rosette;
  }

const npatternrosette& RosyDraw::get_rosette()const {
    return rosette;
  }
    
  void RosyDraw::draw_and_paint_scaled_ring_to_context(Cairo::RefPtr<Cairo::Context> context,
					     unsigned char ring){
      if(ring> rosette.get_rings()){throw xn_Error("out of range");}
      double scale = rosette.get_ringscale(ring);
      context->save();;
      context->begin_new_path();
      context->set_line_width(get_line_width() * scale);
      unsigned char count = rosette.get_n();
      for(unsigned char n= 0; n< count;n++){
	unsigned char colourindex = n % rosette.get_colourcount();
	std::cerr<<"n : "<< (int)count
		 <<"colourindex : "<<(int)colourindex << std::endl;
	std::pair<PatternColour, PatternColour> cpair=rosette.get_colourpair(colourindex);
	if(!((n+10) % 10)){std::cerr<<"rosette.draw_sector_to_cairo"<<std::endl;}
	rosette.draw_sector_to_cairo(context, n, scale, rosette.get_start_angle(ring));
	context->close_path();
	Cairo::RefPtr<Cairo::Pattern> fillpattern= cpair.second.create_pattern();
	context->set_source(fillpattern);
	context->fill_preserve();
      Cairo::RefPtr<Cairo::Pattern> strokepattern= cpair.first.create_pattern();
      context->set_source(strokepattern);
     
      context->stroke();
    }
    context->restore();
 
    }
   void RosyDraw::draw_scaled_ring(unsigned char ring){
      if(ring> rosette.get_rings()){throw xn_Error("out of range");}
      double scale = rosette.get_ringscale(ring);
      cairo_save(buffer);
      cairo_new_path(buffer);
      cairo_set_line_width(buffer, get_line_width() * scale);
      unsigned char count = rosette.get_n();
      for(unsigned char n= 0; n< count;n++){
	unsigned char colourindex = n % rosette.get_colourcount();
	std::pair<PatternColour, PatternColour> cpair=rosette.get_colourpair(n);
	//error was, using n and not ring in rosette.get_start_angle(ring)
	//TODO fix ColourPatternCoords::paint_to_cairo along same lines
	rosette.draw_sector_to_cairo(buffer, n, scale, rosette.get_start_angle(ring));
	cairo_close_path(buffer);
	Cairo::RefPtr<Cairo::Pattern> fillpattern= cpair.second.create_pattern();
	cairo_set_source(buffer, fillpattern->cobj());
cairo_fill_preserve(buffer);
      Cairo::RefPtr<Cairo::Pattern> strokepattern= cpair.first.create_pattern();
      cairo_set_source(buffer, strokepattern->cobj());
     
      cairo_stroke(buffer);
    }
    cairo_restore(buffer);
 
    }



  Ratioflower::Ratioflower(cairo_t* cr,
		cairo_surface_t* scratch, 
		const Colourfacet& face ,
		size_t iters_per_ring, 
		double scal , double innerrad , 
		size_t rings ):Drawing(cr, scratch),
				 facet(face), scaleratio( scal), 
				 innerradius(innerrad), ringcount(rings){
      coords centre(0,0), radius(0,innerradius);
point_data = next =  ncoords(centre, radius, facet.get_variations()* iters_per_ring);

    }
  Ratioflower::Ratioflower(const std::pair<cairo_t*, cairo_surface_t*>& cr_pair,
		size_t iters_per_ring , 
		double scal , 
		double innerrad , 
		size_t rings ):
      Drawing(cr_pair.first, cr_pair.second),
      scaleratio( scal), innerradius(innerrad), 
      ringcount(rings){
      size_type numcolours = facet.get_variations();
      std::cerr <<"number of colours: "<< numcolours<<std::endl;
      if(numcolours < 2){
	throw xn_Error("not enough colours");
      }
      coords centre(0,0), radius(0,innerradius);
      point_data = next =  ncoords(centre, radius, numcolours* iters_per_ring);
    }

void Ratioflower::set_path(const simple_click_history& newpath){facet.path = newpath;}
    simple_click_history& Ratioflower::get_path(){return facet.path;}
    const	simple_click_history& Ratioflower::get_path()const{return facet.path;}


}
