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

#ifndef LIBCAIROXNDRAWING_PATTERNROSETTE_H
#define LIBCAIROXNDRAWING_PATTERNROSETTE_H

#include "exceptions.h"

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <iostream>
#include <cmath>
#include "npatterncoords.h"
#include "drawcolour.h"
#include "cairopath.h"

namespace NCOORDS{
  using xNDRAW::Draw_colour;
  class ColourPatternCoords: public npatterncoords{
  public:
    
    typedef std::pair<PatternColour, PatternColour> colourpair_type;
    typedef std::deque<colourpair_type> colour_container_type;
    
  protected:
    unsigned char colourcount;//actually colour pair count
    colour_container_type strokefills;
    double linewidth; //wrong -get lw in drawingbase?
    double scale_factor;
  public:
    ColourPatternCoords(const simple_click_history& path, 
			const coords& centre, 
			unsigned int n, 
			unsigned char colourcnt , 
			double lw=1.0, double sf=1.0);
    
    ColourPatternCoords(const simple_click_history& patternpath_, 
			unsigned int n =60, 
			unsigned char colourcnt = 3, 
			double angle=0.0,
			double lw = 1.0, double sf=1.0);
    //size_t get_
    void draw_sector_to_cairo(cairo_t* cr,unsigned int sector,
			      double abs_scale = 1.0,
			      double startoffset=0.0);
    void draw_sector_to_cairo(Cairo::RefPtr<Cairo::Context> cr,
			      unsigned int sector, 
			      double abs_scale = 1.0,
			      double startoffset=0.0);
    double get_linewidth()const;

    unsigned char get_colourcount()const{
      return colourcount;
    }

    void set_linewidth(double lw);
    void paint_to_cairo(cairo_t* cr, double abs_scale = 1.0);
    void paint_to_cairo(Cairo::RefPtr<Cairo::Context> cr, double abs_scale = 1.0);

    void init_colours();
    void randomise_colours();
    template <typename CONT>
      void set_colours(unsigned char cnt,const CONT& container){
      //deq or vector of Draw_colour
      //finish
    }

    void set_colours(unsigned char cnt, const colour_container_type& cols);
    virtual double get_start_angle(unsigned char ring)const{return 0.0;}
    std::pair<PatternColour, PatternColour> get_colourpair(unsigned char index)const;

    void set_colour(const PatternColour& source, unsigned char index, int role=COLOURROLE_STROKE);

  };


  class npatternrosette: public ColourPatternCoords{
  protected:
    static double min_scale_ratio;
    static double max_scale_ratio;
    double scale_ratio;
    double ring_angle;//relative rotation between one ring and the next
    double inner_radius;//area clipped out of middle
    unsigned char min_rings;//when it bumps up against another,whose line wins
    unsigned char max_rings;
   
  public:

    double get_ringscale(unsigned char ring)const;

    virtual double get_start_angle(unsigned char ring)const;
    void set_ringoffset_angle(double angle){
      //TODO: limits(is there a clamp<T> class?)
      ring_angle = angle;
    }

    double get_ringoffset_angle()const{
      return ring_angle;
    }

    double get_scale_ratio()const{
      return scale_ratio;
    }

    void set_scale_ratio(double scale){
      Clamp<double> theclamp(min_scale_ratio, max_scale_ratio);
      scale_ratio = theclamp(scale);
    }

    double get_scale_for_ring(unsigned char ring)const;

    unsigned char get_rings()const;
    unsigned char get_min_rings()const;



    void paint_to_cairo(cairo_t* cr, double abs_scale = 1.0);
    void paint_to_cairo(Cairo::RefPtr<Cairo::Context> cr, double abs_scale = 1.0);

    void fix_ring_angle();

    npatternrosette(const coords& origin, simple_click_history path, 
		    unsigned int n, 
		    double ratio = std::pow(XNDRAW_GOLDEN_MEAN_INVERSE, 0.5), 
		    double inner_cutoff = 50., 
		    double ringoffset_angle=0, //XNDRAW_GOLDEN_MEAN,
		    unsigned char rings_min=5,
		    unsigned char rings_max=8,
		    unsigned char colourcnt=3,
		    const colour_container_type* colours= NULL,
		    double base_linewidth = 1.0);

    npatternrosette(const ncoords& origin, simple_click_history path, 
		    double ratio = std::pow(XNDRAW_GOLDEN_MEAN_INVERSE, 0.5), 
		    double inner_cutoff = 50., 
		    double ringoffset_angle=0, //XNDRAW_GOLDEN_MEAN,
		    unsigned char rings_min=5,
		    unsigned char rings_max=8,
		    const colour_container_type* colours= NULL,
		    double base_linewidth = 1.0);

    double fix_scale(double sratio);

    static void set_min_ratio(double min);
    static void set_max_ratio(double max);

    double get_linewidth(unsigned char ringindex=0)const;

    virtual ~npatternrosette(){}
  };
}
#endif //LIBCAIROXNDRAWING_PATTERNROSETTE_H
  
