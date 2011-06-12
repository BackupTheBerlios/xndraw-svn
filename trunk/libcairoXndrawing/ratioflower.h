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
 * @file ratioflower.h
 * emphasis pattern seen in roman mosaics
 */

#ifndef LIBXNCAIRODRAWING_RATIOFLOWER_H
#define LIBXNCAIRODRAWING_RATIOFLOWER_H
#include "xndrawexceptions.h"

#if HAVE_CONFIG_H
#include "config.h"
#endif
#include "drawing.h"
#include "xndrawexceptions.h"
#include "patternrosette.h"

namespace xNDRAW{

  class BitIndexError: public xn_Error{
    size_t indexmax;
    size_t value;
  public:
    BitIndexError(size_t maxx, size_t v):xn_Error("wha?"),indexmax(maxx), value(v){
      errr=errorname() + "index out of range";
    }
    char* what(){
      std::ostringstream ostmp;
      ostmp<< "BitIndexError: index out of range\nmax index: " << indexmax
	   << "\nvalue given: "<<value;
      std::string s_what = ostmp.str();
      return strdup(s_what.c_str());
    }
  };


  template<typename UNSIGNED>
  class BitField{
  public:
    typedef size_t size_type;

  protected:
    UNSIGNED bits;
  public:
    BitField():bits(0){}
    virtual ~ BitField(){}

    //automatic ctors ok
    virtual size_type size()const {return sizeof(UNSIGNED) * 8;}

    bool check_index(size_t someindex)const{
      if(someindex >= size()){
	throw BitIndexError(size(),someindex);
      }
      return true;
    }

    bool get_bit( size_type bitindex)const{
      check_index(bitindex );
      return (bits & (1 << bitindex));
    }
    
    void set_bit( size_type bitindex, bool value=true){
      check_index(bitindex );
      if (value){
	bits |= (1 << bitindex);
      }
      else if(bits & (1 << bitindex)){
	bits ^= (1 << bitindex);
      }
    }
    
    void toggle_bit(size_type bitindex){
      check_index(bitindex );
      bits ^= (1 << bitindex);
    }
  };

 

  template <typename UNSIGNED>
  class LargeBitField: public BitField<UNSIGNED>{
  public:
    typedef BitField<UNSIGNED> base_type ;

  protected:
    std::deque<base_type > bitfields;

  public:

    virtual ~LargeBitField(){}

    virtual size_t size(){
      size_t fieldsize = base_type::size();
      return fieldsize * (bitfields.size()+1);
    }
  protected:
    base_type& get_field(size_t fieldindex){
      if(!fieldindex){
	return *this;
      }
      else if(fieldindex <= bitfields.size()){
	fieldindex--;
	return *(bitfields.begin() +fieldindex);
      }
      throw  BitIndexError(size(),fieldindex * base_type::size());
    }
  public:
    bool get_bit(size_t bitindex){
      size_t basesz = base_type::size();
      size_t fieldindex=bitindex/basesz;
      base_type& field= get_field(fieldindex);
      size_t baseindex = bitindex - (fieldindex * basesz);
      return field.get_bit(baseindex);
    }

    void set_bit(size_t bitindex, bool value = true){
      size_t basesz = base_type::size();
      size_t fieldindex=bitindex/basesz;
      base_type& field= get_field(fieldindex);
      size_t baseindex = bitindex - (fieldindex * basesz);
      field.set_bit(baseindex, value);
    }

    void toggle_bit(size_t bitindex){
      size_t basesz = base_type::size();
      size_t fieldindex=bitindex/basesz;
      base_type& field= get_field(fieldindex);
      size_t baseindex = bitindex - (fieldindex * basesz);
      field.toggle_bit(baseindex);
    }


    void set_size(size_t newsize){
      if (newsize <= size()){return;} 
      size_t bitfieldsz = base_type::size();
      //size_t bitfieldcount = bitfields.size() +1;
      size_t cursize = size();
      size_t newfields = ((newsize - cursize)/ bitfieldsz) +1;
      for(size_t i = 0; i < newfields;i++){
	bitfields.push_back(BitField<UNSIGNED>());
      }
    }


  };

  class RosyDraw: public Drawing{//Drawing* in ping_window
    //TODO: png file output via a separate buffer,
    //with all drawn offset, so the centre is in the centre
    ///////////////////
    //Drawing bug - last painted segments not clipped , which breaks the patterns illusion
    //unfortuneatly cairo clipping regions can only be intersected with a path,
    //no union and so on
    //ideally you could add(union) a bunch of the paths(say the first 45degrees of each ring iteration)
    //then create a clip region by setting this as the path and then intersecting with an unrestricted clip
  protected:
    npatternrosette rosette;

    //fix scale etc handlers to operate on rosette
  public:
    RosyDraw(cairo_t* cr,cairo_surface_t* scratch, 
	     const npatternrosette& pattern);

    RosyDraw(Drawing* src);

    virtual ~RosyDraw(){}
    virtual void do_draw_path();
void draw_and_paint_scaled_ring_to_context(Cairo::RefPtr<Cairo::Context> cr,
					   unsigned char ring);
    
    //TODO: ie set appropriate colour in
    //rosette.strokefills
    void set_facet_colour(const PatternColour& source, unsigned char index,
			  int role =COLOURROLE_STROKE);
  
    void draw_and_paint_path_to_context(Cairo::RefPtr<Cairo::Context> cr);
    virtual void do_draw_path(const ncoords& rotator);
    virtual void set_line_width(double width);
    npatternrosette& get_rosette();
    const npatternrosette& get_rosette()const;
    void draw_paint_rings();
    void draw_scaled_ring(unsigned char ring);

    void draw_to_path();

    /////////
    //TODO: gui and callbacks in xndraw
    void set_ring_ratio(double val){
      rosette.set_scale_ratio(val);
    }
    void set_ringoffset_angle(double val){
      rosette.set_ringoffset_angle(val);
    }
    //////////

    NCOORDS::simple_click_history* initialise_path();
    NCOORDS::simple_click_history* initialise_path(const simple_click_history& path);

    virtual void assign_history(NCOORDS::simple_click_history* clicks);
    virtual void draw_to(simple_click_history& clixhistory){
      draw_to_path(clixhistory);
    }
    virtual void draw_to(std::pair<coords, int>& location){
      Drawing::draw_to(location);

    }

    void  draw_to_path(const simple_click_history& path);
    void draw_to_path(const ncoords& rotator);
    void cleanup_history();

  };

  struct Colourfacet{
    typedef std::deque<PatternColour>::size_type size_type;

    simple_click_history path;
    std::deque<PatternColour> fills;
   

    size_type get_variations()const{
      return fills.size();
    }
    
    Colourfacet( const simple_click_history& p, 
		 const  std::deque<Draw_colour>& f):
      path(p){
      for(std::deque<Draw_colour>::const_iterator w = f.begin();
	  w != f.end();w++){
	fills.push_back(PatternColour(*w));
      }
    }

    Colourfacet(){}


  };

  class Ratioflower: public Drawing{
  protected:
    Colourfacet facet;
    double scaleratio;
    double innerradius;
    size_t ringcount;
 
    ;
  public:
    Ratioflower(cairo_t* cr,
		cairo_surface_t* scratch, 
		const Colourfacet& face ,
		size_t iters_per_ring = 20, 
		double scal =1.23, double innerrad = 60, 
		size_t rings =6);

    Ratioflower(const std::pair<cairo_t*, cairo_surface_t*>& cr_pair,
		size_t iters_per_ring = 20, 
		double scal =1.23, 
		double innerrad = 60, 
		size_t rings =6);
    

    virtual ~Ratioflower(){}

    void set_path(const simple_click_history& newpath);

    simple_click_history& get_path();
    const  simple_click_history& get_path()const;

  };

}

#endif
