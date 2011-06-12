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

#include "patternrosette.h"

namespace NCOORDS{

  double npatternrosette::min_scale_ratio(1.01);
  double npatternrosette::max_scale_ratio(10.);
  void npatternrosette::set_min_ratio(double min){
    if(min <=1.0001 ){min = 1.0001;}
    else if (min > max_scale_ratio){min= max_scale_ratio;}
    min_scale_ratio = min;
  }

  void npatternrosette::set_max_ratio(double max){
    if(max > 100){max = 100.;}
    else if (max < min_scale_ratio){max = min_scale_ratio;}
    max_scale_ratio = max;
  }

  double npatternrosette::get_linewidth(unsigned char ringindex)const{
    double lw=ColourPatternCoords::get_linewidth();
    if(!ringindex){return lw;}
    return std::pow(scale_ratio,ringindex +1) * lw;
  }

  double npatternrosette::fix_scale(double sratio){
    if(sratio <0){sratio = 0-sratio;}
    if(sratio < 0.0000000001){sratio= XNDRAW_GOLDEN_MEAN_INVERSE;} //AVOID DIV BY ZERO
    if(sratio < 1){
      sratio = 1.0 /sratio;
    }
    if ( sratio <=min_scale_ratio  || sratio >= max_scale_ratio){
      std::cerr<<"scale outside settable range: setting "<<sratio<< " to "<< XNDRAW_GOLDEN_MEAN_INVERSE<< std::endl;
      sratio= XNDRAW_GOLDEN_MEAN_INVERSE;
    }
    return sratio;
  }
  double ColourPatternCoords::get_linewidth()const{
    return linewidth;
  }
  void ColourPatternCoords::set_linewidth(double lw){
    linewidth = lw;
  }

  std::pair<PatternColour, PatternColour> 
  ColourPatternCoords::get_colourpair(unsigned char index)const{
    if (index >= colourcount){
      throw xn_Error("out of range of colours");
    }
    if(colourcount > strokefills.size()){
      throw xn_Error("error initialising colours");
    }
    return *(strokefills.begin() + index);
  }

  double npatternrosette::get_ringscale(unsigned char ring)const{
    return std::pow(scale_ratio, double(ring));
  }

  double npatternrosette::get_scale_for_ring(unsigned char ring)const{
    return std::pow(scale_ratio, (double)ring) ;
  }

  unsigned char npatternrosette::get_rings()const{
    return max_rings;
  }

  unsigned char npatternrosette::get_min_rings()const{
    return min_rings;
  }

  double npatternrosette::get_start_angle(unsigned char ring)const{
    std::cerr <<"npatternrosette::get_start_angle("<<(int)ring
	      <<")=> \nring_angle: "<<ring_angle;
    double def_offset= (2*M_PI)/(get_n() *colourcount);
    // double scal= get_scale_for_ring(ring);
    double  res = ring_angle + (ring *(def_offset +ring_angle));
    //res *=scal;
    if(res > 2*M_PI){
      int mults = int(res /(2*M_PI));
      res -= mults * 2*M_PI;
    }
    else if(res <0){//which direction floor when x <0?
      while (res <0){res +=2*M_PI;}
    }
    std::cerr <<"\ndefault ffset: "<<def_offset<<"\nreturning: "
	      <<res <<std::endl;
    return res;
  }

  void npatternrosette::paint_to_cairo(cairo_t* cr, double abs_scale){
    for(unsigned char i=0; i < max_rings;i++){
      double scale = get_scale_for_ring(i) * abs_scale;
      ColourPatternCoords::paint_to_cairo(cr, scale);
    }
  }

  npatternrosette::npatternrosette(const coords& origin, simple_click_history path, 
				   unsigned int n, 
				   double ratio , 
				   double inner_cutoff , 
				   double ringoffset_angle,
				   unsigned char rings_min,
				   unsigned char rings_max,
				   unsigned char colourcnt,
				   const colour_container_type* colours,
				   double base_linewidth):
    ColourPatternCoords(path, origin, n, colourcnt, base_linewidth),
    scale_ratio(fix_scale(ratio)),
    ring_angle(ringoffset_angle),
    inner_radius(inner_cutoff),
    min_rings(rings_min),
    max_rings(rings_max){
    if (colours){
      set_colours(colourcnt, *colours);
    }
    fix_ring_angle();
  }

  void npatternrosette::fix_ring_angle(){
    double ra=ring_angle;
    if(ra < 0){ra = -ra;}
    if(ra < 1e-8){
      ring_angle= XNDRAW_GOLDEN_MEAN_INVERSE / (double)get_base();
    }
  }
  npatternrosette::npatternrosette(const ncoords& origin, simple_click_history path, 
				   double ratio, 
				   double inner_cutoff , 
				   double ringoffset_angle,
				   unsigned char rings_min,
				   unsigned char rings_max,
				   const colour_container_type* colours,
				   double base_linewidth ):
    ColourPatternCoords(path, origin,  origin.get_n(), origin.get_n() / 20,base_linewidth),
    scale_ratio(fix_scale(ratio)),
    ring_angle(ringoffset_angle),
    inner_radius(inner_cutoff),
    min_rings(rings_min),
    max_rings(rings_max){
    if (colours){
      set_colours( origin.get_n(), *colours);
    }
    fix_ring_angle();
  }

  void ColourPatternCoords::set_colour(const PatternColour& source, 
				       unsigned char index, int role){
    if (index >= colourcount){
      throw xn_Error("out of range");
    }
    if(strokefills.size() < colourcount){init_colours();}
    if(strokefills.size() < colourcount){throw xn_Error("something went wrong");}
    std::pair<PatternColour,PatternColour>& prref=*(strokefills.begin()+index);
    switch(role){
    case COLOURROLE_STROKE:
      prref.first= source;
      break;

    case COLOURROLE_FILL:
      prref.second = source;
      break;
    default:
      throw xn_Error("role not correct");
    }
  }

  void ColourPatternCoords::paint_to_cairo(cairo_t* cr, double abs_scale){
    std::cerr<<"ColourPatternCoords::paint_to_cairo(cairo_t* cr,double abs_scale)\nscale :"<<abs_scale<< std::endl;
    cairo_save(cr);
    //BODY GOES HERE
    cairo_new_path(cr);
    cairo_set_line_width(cr, linewidth * abs_scale);
    for(unsigned char n= 0; n< get_n(); n++){
      unsigned char colourindex = n % colourcount;
      colourpair_type cpair = *(strokefills.begin()+ colourindex);
      draw_sector_to_cairo(cr, n, abs_scale, get_start_angle(n));
      cairo_close_path(cr);
      Cairo::RefPtr<Cairo::Pattern> fillpattern= cpair.second.create_pattern();
      cairo_set_source(cr, fillpattern->cobj());
      cairo_fill_preserve(cr);
      Cairo::RefPtr<Cairo::Pattern> strokepattern= cpair.first.create_pattern();
      cairo_set_source(cr, strokepattern->cobj());
     
      cairo_stroke(cr);
    }
    cairo_restore(cr);
  }

  void ColourPatternCoords::set_colours(unsigned char cnt, const ColourPatternCoords::colour_container_type& cols){
    if(cnt >=3){
      colourcount = cnt;
    }
    strokefills= cols;
    if (strokefills.size()< colourcount){
      init_colours();
    }
  }
 
  void ColourPatternCoords::paint_to_cairo(Cairo::RefPtr<Cairo::Context> cr,
					   double abs_scale){
    std::cerr<<"ColourPatternCoords::paint_to_cairo(Cairo::RefPtr<Cairo::Context> cr,double abs_scale)\nscale :"<<abs_scale <<std::endl;
    cr->save();
    //BODY GOES HERE
    cr->begin_new_path();

    cr->set_line_width(linewidth * abs_scale);
    for(unsigned char n= 0; n< get_n(); n++){
      unsigned char colourindex = n % colourcount;
      colourpair_type cpair = *(strokefills.begin()+ colourindex);
      draw_sector_to_cairo(cr, n, abs_scale, get_start_angle(n));
      cr->close_path();
      Cairo::RefPtr<Cairo::Pattern> fillpattern= cpair.second.create_pattern();
      cr->set_source(fillpattern);
      cr->fill_preserve();
      Cairo::RefPtr<Cairo::Pattern> strokepattern= cpair.first.create_pattern(); 
      cr->set_source(strokepattern);
      cr->stroke();
    }
    cr->restore();
  }
  void ColourPatternCoords::draw_sector_to_cairo(cairo_t* cr,
						 unsigned int sector,
						 double abs_scale,
double startoffset){
    simple_click_history path  = draw_path(sector, startoffset);
    if(path.size()<patternpath.size()){
      throw xn_Error("path got lost");
    }
    cRect box;
    path.get_extents(box);
    std::cerr<<"Path rotated\nx : "<<box.x()<<"\ty : "<<box.y()
	     <<"\nwidth: "<<box.width()<< "\theight: "<< box.height()
	     <<std::endl;
    if( abs_scale !=1.0){
      coords centre(x, y);
      path.scale(abs_scale, abs_scale,&centre, false); 
      path.get_extents(box);
      std::cerr<<"Path rotatedand scaled \nx : "<<box.x()<<"\ty : "<<box.y()
	       <<"\nwidth: "<<box.width()<< "\theight: "<< box.height()
	       <<std::endl;
    }
    cairo_draw_clickhistory(cr, path);
  }

  void ColourPatternCoords::draw_sector_to_cairo(Cairo::RefPtr<Cairo::Context> cr,unsigned int sector, double abs_scale,
double startoffset){
    //fix with startoffset
    simple_click_history outpath  = draw_path(sector, startoffset);
    if(!((sector +10) %10)){
      cRect box;
      outpath.get_extents(box);
      std::cerr <<"path no: "<<sector<<"...\tlength: "<< outpath.size()
		<<"\nlocation: "<< box.x()<<", "<<box.y()<<"\nsize : "
		<< box.width()<<" x "<< box.height()<<std::endl;
    }
    if(abs_scale != 1.0){
      coords centre(x, y);
      outpath.scale(abs_scale, abs_scale,&centre, false); 
      if(!((sector +10) %10)){
	cRect box;
	outpath.get_extents(box);
	std::cerr <<"aftr scaling: "<< box.x()<<", "<<box.y()<<"\nsize : "
		  << box.width()<<" x "<< box.height()<<std::endl;
      }
    }
    cairo_draw_clickhistory(cr, outpath);
  }

  ColourPatternCoords::ColourPatternCoords(const simple_click_history& patternpath_, 
					   unsigned int n , 
					   unsigned char colourcnt, 
					   double angle, double lw, double sf):
    npatterncoords(patternpath_, n, angle),
    colourcount(colourcnt),
    linewidth(lw),
    scale_factor(sf)
  {
    init_colours();
  }

  void ColourPatternCoords::randomise_colours(){
    init_colours();
  }


  void ColourPatternCoords::init_colours(){
    if (colourcount <3){colourcount =3;}
    if(strokefills.size() < colourcount){    
      for (unsigned char n =strokefills.size();n < colourcount;n++){
	Draw_colour strok, fil;
	strok.randomise();
	fil.randomise();
	PatternColour strokecol(strok);
	PatternColour filcol(fil);
	std::pair<PatternColour,PatternColour>  strokefill( strokecol,filcol) ;
	strokefills.push_back(strokefill);
      }
    }
  }

     

  
  ColourPatternCoords::ColourPatternCoords(const simple_click_history& path, 
					   const coords& centre, 
					   unsigned int n, 
					   unsigned char colourcnt ,
					   double lw, double sf):
    npatterncoords(path, n, 0.),
    colourcount(colourcnt),
    linewidth(lw),
    scale_factor(sf){
    std::cerr <<"colourcount: "<<colourcount <<std::endl;
    set(centre.x, centre.y);
    init_colours();
  }






}
