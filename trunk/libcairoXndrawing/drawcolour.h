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


#ifndef XNDRAW_DRAWCOLOUR_H
#define XNDRAW_DRAWCOLOUR_H

#include "exceptions.h"
#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <iostream>
#include <sstream>
#include <string>
#include <cairomm/context.h>
#include "preferences.h"
#include "clickhistory.h"// why?

namespace xNDRAW{

  template<typename T>
  class Clamp{
  protected:
    T min, max;
  public:
    Clamp(T minn, T maxx):min(minn), max(maxx){
      if(max < min){
	throw xn_Error("min > max");
      }
    }

    template <typename U>
    Clamp(const Clamp<U>& src):min(src.get_min()), max(src.get_max()){}


    T get_min()const{return min;}
    T get_max()const{return max;}

    T operator()(T val){
      if(val <min){val=min;}
      else if(val >max){val=max;}
      return val;
    }
  };
  struct Draw_colour;

  
  //main template in appcommon/preferences.h
  template <>
  std::string get_valueType<Draw_colour>(const Draw_colour& i);
  
  template<typename CHARTYPE, typename TRAITS>
  std::basic_ostream<CHARTYPE, TRAITS>& 
  operator<<(std::basic_ostream<CHARTYPE, TRAITS>& os,
	     const Draw_colour& d);

template<typename CHARTYPE, typename TRAITS>
std::basic_istream<CHARTYPE, TRAITS>&
 operator>>(std::basic_istream<CHARTYPE, TRAITS>& istrim,  
	    Draw_colour& dataload);

  template<>
  std::string get_valueType<NCOORDS::simple_click_history>(const NCOORDS::simple_click_history& path);

  template<>
  std::string get_valueType<NCOORDS::coords>(const NCOORDS::coords& pt);

  template<>
  std::string get_valueType<NCOORDS::ncoords>(const NCOORDS::ncoords& pt);


  //hopefully will be useful to translate etc a push_group()'ed cairo
  // to paint a source as per the pattern
  //not just picking it up from under the destination
  //ie so a photo is not just 'chromakeyed' in by drawing,
  //but also appropriately transformed
  Cairo::Matrix create_matrix(const NCOORDS::coords& zero_pt,
			      const NCOORDS::coords& offset,
			      double rotation =0,
			      NCOORDS::coords scaling = NCOORDS::coords(1,1));


  struct Draw_colour{
    static bool randyseeded;
    double r,g,b,a;
    Draw_colour() : r(0.7), g(0.3), b(0.0), a(0.7) {}
    Draw_colour(const Draw_colour& src):r(src.r), g(src.g), b(src.b), a(src.a){}
    
    Draw_colour(double red ,double green, double blue,double  alpha = 1.0 );
    virtual  ~ Draw_colour(){}
    static bool seedrand(long seed);
    void randomise();

    Draw_colour& operator=(const Draw_colour& src);

    //'complement' operator
    Draw_colour operator~()const{
      //invert colour but keep same alpha
      Draw_colour result(1-r, 1-g, 1-b, a);
      //or convert to hsb using some imaging lib (cairo, gdk?),
      //or an algorithm from similar,
      //then shift the hue by '180 degrees'
      return result;
    }

    void set(double red ,double green, double blue,double  alpha);

    //Cairo::ColorStop as_colourstop(double offf=0.);

    static Cairo::RefPtr<Cairo::RadialGradient> 
    create_radial_gradient(const Draw_colour& start, 
			   const Draw_colour& finish,
			   const NCOORDS::coords& centre_0,
			   double r0, 
			   const NCOORDS::coords& centre_1,
			   double r1);

    static Cairo::RefPtr<Cairo::LinearGradient> 
    create_linear_gradient(const Draw_colour& start, 
			   const Draw_colour& finish,
			   const NCOORDS::coords& control1,
			   const NCOORDS::coords& control2);

    void set_rgb(const Draw_colour& dc);

    void set(const Draw_colour& ref);

    void set_red(double red){r=clamp(red);}
    void set_green(double green){g=clamp(green);}
    void set_blue(double blue){b=clamp(blue);}
    void set_alpha(double  alpha){a=clamp(alpha);}

    double get_red(){return r;}
    double get_green(){return g;}
    double get_blue(){return b;}
    double get_alpha(){return a;}
 
    static double clamp(double v, double min=0., double max=1.);
    //Gdk::Color to_gdkcolor(){}
  };

  Draw_colour& operator+=(Draw_colour& left, const Draw_colour& right);
  Draw_colour operator+(const Draw_colour& left, const Draw_colour& right);
  Draw_colour& operator-=(Draw_colour& left, const Draw_colour& right);
  Draw_colour operator-(const Draw_colour& left, const Draw_colour& right);

Draw_colour& operator*=(Draw_colour& left, const Draw_colour& right);
  Draw_colour operator*(const Draw_colour& left, const Draw_colour& right);
  Draw_colour& operator/=(Draw_colour& left, const Draw_colour& right);
  Draw_colour operator/(const Draw_colour& left, const Draw_colour& right);



  struct Pixel:public Draw_colour{

    Pixel(const Draw_colour& colour):Draw_colour(colour){}
    Pixel(const Pixel& src):Draw_colour(src){}
    Pixel(){}
    Pixel(double r, double g, double b, double a):Draw_colour(r,g,b,a)
    {/* this->bork();*/   }

  };

template<typename CHARTYPE, typename TRAITS>
  std::basic_ostream<CHARTYPE, TRAITS>& 
  operator<<(std::basic_ostream<CHARTYPE, TRAITS>& os,
	     const Draw_colour& d){
  //four space separated double values per line: r g b a
  std::locale loc;
  CHARTYPE SPACE= CTYPE_FACET(loc).widen(' ');
  os << d.r<<  SPACE<< d.g<<  SPACE<< d.b<<  SPACE<< d.a <<std::endl;//adds newline
  return os;
}

template<typename CHARTYPE, typename TRAITS>
std::basic_istream<CHARTYPE, TRAITS>&
 operator>>(std::basic_istream<CHARTYPE, TRAITS>& istrim,  
		    Draw_colour& dataload){
  istrim.setf(std::ios_base::skipws);
  //istrim.exceptions(std::ios_base::failbit);
  istrim >> dataload.r >> dataload.g>> dataload.b>> dataload.a;
  return istrim;
}


  enum PatternSourceInfo{
    PATTERN_SOURCE_RGBA =1,
    PATTERN_SOURCE_LINEAR_GRADIENT=2,
    PATTERN_SOURCE_RADIAL_GRADIENT=3,
    PATTERN_SOURCE_PATTERN=4
  };

  enum ColourRole{
    COLOURROLE_NONE=0,
    COLOURROLE_STROKE=1,
    COLOURROLE_FILL=2,
    COLOURROLE_BG=3
};
struct SourceTransformInfo{
  //sets matrix offset to -offset, 
  //sets matrix rotation to -rotation
//sets matrix scale to 1/scale
  NCOORDS::coords origin;
  NCOORDS::coords offset;
    double rotation;
    NCOORDS::coords scale;
  //instead of invertorder, use macro #if XNDRAW_MATRIX_REVERSE
  // bool invertorder;
  //autogenerated copy ctor and operator=() OK
  SourceTransformInfo();
  SourceTransformInfo(const NCOORDS::coords& origin,
		      const  NCOORDS::coords& off,
		      double rot,
		      NCOORDS::coords sc = NCOORDS::coords(1,1));

  bool should_apply_offset()const;
  bool should_apply_rotation()const{return rotation != 0.0;}
  bool should_apply_scale()const;

 
		      void translate_origin_to_zero(Cairo::Matrix& outmatrix);
  void do_offset(Cairo::Matrix& outmatrix);
  void do_rotation(Cairo::Matrix& outmatrix);
  void do_scale(Cairo::Matrix& outmatrix);
		      void translate_zero_to_origin(Cairo::Matrix& outmatrix);
 

  void setup_matrix(Cairo::Matrix& outmatrix, bool resetfirst=true);
  void setup_matrix(Cairo::Matrix& outmatrix, 
		    const NCOORDS::coords& zeropoint,  
		    bool resetfirst=true);
  };


  struct ColourSourcebase{
    virtual ~ColourSourcebase(){}

    virtual Cairo::RefPtr<Cairo::Pattern> create_pattern();
    
    virtual double get_alpha()const;
    virtual void set_alpha(double);
    virtual std::string hello_world(){return "ColourSourcebase";}
    static ColourSourcebase* copy(const ColourSourcebase& src){
      return src.make_copy();
    }

    virtual ColourSourcebase* make_copy()const{
      throw xn_Error("overload to return derived type");
    }

  };

  class ColourSource : public ColourSourcebase{
  public:
  explicit   ColourSource(double alpha=1):topcolour(1,1,1,alpha){}//white, possibly translucent
 explicit   ColourSource(const Draw_colour& c):topcolour(c){}
    virtual ~ColourSource(){}
    virtual Cairo::RefPtr<Cairo::Pattern> create_pattern(){
      return Cairo::SolidPattern::create_rgba(topcolour.r, topcolour.b,
					      topcolour.g, topcolour.a);
    }
 virtual ColourSourcebase* make_copy()const{
   return new ColourSource(*this);
    }
virtual std::string hello_world(){return "ColourSource";}
    
const Draw_colour& get_topcolour()const{return topcolour;}
Draw_colour& get_topcolour(){return topcolour;}
    void set_topcolour( const Draw_colour& c){topcolour = c;}
    virtual double get_alpha()const{return topcolour.a;}
    virtual void set_alpha(double v){
      topcolour.a = Draw_colour::clamp(v);
    }

  protected:
	Draw_colour topcolour;
    };

  class LinearGradientSrc: public ColourSource{
  protected:
    Draw_colour bottomcolour;
    NCOORDS::coords pt0;
    NCOORDS::coords pt1;
  public:
    virtual ~LinearGradientSrc(){}
    LinearGradientSrc(const std::pair<Draw_colour , Draw_colour >& colours,
		      const NCOORDS::coords& p0,
		      const NCOORDS::coords& p1);

    LinearGradientSrc(const  ColourSource& top, const  Draw_colour& bottom,
		       const NCOORDS::coords& p0,
		      const NCOORDS::coords& p1);

virtual Cairo::RefPtr<Cairo::Pattern> create_pattern(){
  return Draw_colour::create_linear_gradient(topcolour, bottomcolour, pt0, pt1);
}

virtual std::string hello_world(){return "LinearGradientSrc";}
    

virtual ColourSourcebase* make_copy()const{
   return new  LinearGradientSrc(*this);
    }
    const NCOORDS::coords& get_pt0()const{return pt0;}
const NCOORDS::coords& get_pt1()const{return pt1;}
    void set_pt0(const NCOORDS::coords& pt){pt0=pt;}
void set_pt1(const NCOORDS::coords& pt){pt1=pt;}
    const Draw_colour& get_bottomcolour()const{return bottomcolour;}
    void set_bottomcolour( const Draw_colour& c){bottomcolour = c;}
  };

  class RadialSource : public LinearGradientSrc{
  protected:
    double radius_0;
    double radius_1;

  public:
    RadialSource (const std::pair<Draw_colour , Draw_colour >& colours,
		  const NCOORDS::coords& p0, const NCOORDS::coords& p1,
		  double r0, double r1);

  RadialSource(const  ColourSource& top, const  Draw_colour& bottom,
		       const NCOORDS::coords& p0,
		      const NCOORDS::coords& p1,
	       double r0, double r1);

virtual std::string hello_world(){return "RadialSource";}

virtual ColourSourcebase* make_copy()const{
   return new RadialSource(*this);
    }
    virtual ~RadialSource(){}

    virtual Cairo::RefPtr<Cairo::Pattern> create_pattern();

    double get_radius_0()const{return radius_0;}
  double get_radius_1()const{return radius_1;}

    void set_radius_0(double r){radius_0 = r;}
  void set_radius_1(double r){radius_1 = r;}
  };

  class ArbitrarySrc: public ColourSourcebase{

  protected:
    Cairo::RefPtr<Cairo::Pattern> the_source;
    double alpha;//how to use transparency with cairo_pattern_t?

  public:
    //transparency currently not used
 explicit   ArbitrarySrc(Cairo::RefPtr<Cairo::Pattern> p);
    ArbitrarySrc(Cairo::RefPtr<Cairo::Pattern> p, double transparency);
  explicit   ArbitrarySrc(Cairo::RefPtr<Cairo::Surface> surf);
    ArbitrarySrc(Cairo::RefPtr<Cairo::Surface> surf, double transparency);

    ArbitrarySrc(const ArbitrarySrc& src);
    virtual ~ArbitrarySrc(){}
virtual std::string hello_world(){return "ArbitrarySrc";}
    
 virtual Cairo::RefPtr<Cairo::Pattern> create_pattern(){
   return the_source;
  }
virtual double get_alpha()const{return alpha;}
    virtual void set_alpha(double v){
       alpha = Draw_colour::clamp(v);
    }

virtual ColourSourcebase* make_copy()const{
   return new ArbitrarySrc(*this);
    }

  };

  class PatternColour{
  public:
typedef size_t size_type;


  protected:
    PatternSourceInfo info;
    ColourSourcebase* patternsource;

  public:
    //patternsource is ColourSource* (flat colour)
    PatternColour();
 explicit   PatternColour(const Draw_colour& d);

    //linear gradient  === LinearGradientSrc*
    PatternColour(const std::pair<Draw_colour , Draw_colour >& colours,
		  const NCOORDS::coords& p0,
		  const NCOORDS::coords& p1);
    //radial_gradient === RadialSource*
    PatternColour(const std::pair<Draw_colour , Draw_colour >& colours,
		  const NCOORDS::coords& p0, const NCOORDS::coords& p1,
		  double r0, double r1);
    //Cairo::Pattern  === ArbitrarySrc*
explicit    PatternColour(Cairo::RefPtr<Cairo::Pattern> p);
explicit    PatternColour(Cairo::RefPtr<Cairo::Surface> surf);

 virtual   ~PatternColour();
    PatternColour(const PatternColour& src);
    PatternColour& operator=(const PatternColour& src);
      virtual void set_transform(const SourceTransformInfo& setting){}//

    void copy_src(const PatternColour& src);
    ///finish - overload in subclass
    virtual bool uses_offset()const{
      return false;
    }

    virtual PatternColour* copy()const{
      return new PatternColour(*this);
    }
        virtual    void set_offset(const NCOORDS::coords& pt){}  
    virtual void set_rotation_step(size_type step){}
    virtual void set_rotation(double radians){}
    ///
    PatternSourceInfo get_info()const{return info;}
    void set_alpha(double a);
    double get_alpha()const;

virtual Cairo::RefPtr<Cairo::Pattern> create_pattern(){
  return patternsource->create_pattern();
}

    virtual void set_anchor(const NCOORDS::ncoords& anchorpts){}

    void set_source(Cairo::RefPtr<Cairo::Context> cr){
      cr->set_source(patternsource->create_pattern());
    }

    ColourSourcebase& get_patternsource(){return *patternsource;}
    const ColourSourcebase& get_patternsource()const{return *patternsource;}

    Draw_colour* get_topcolour();

    //change colour settings(ie check type with info
    //and use manipulate the reference after dynamic_casting to correct type
    std::pair<PatternSourceInfo, ColourSourcebase&> get_pattern_and_info(){
      return std::pair<PatternSourceInfo, ColourSourcebase&>(info, *patternsource);
    }

    void set_source(cairo_t* cr){
      Cairo::RefPtr<Cairo::Pattern> p=patternsource->create_pattern();
      cairo_set_source(cr, p->cobj());
    }

    void set_source_with_transform(Cairo::RefPtr<Cairo::Context> cr,
				   const NCOORDS::cRect& clip,
				   const SourceTransformInfo& transform);
void set_source_with_transform(cairo_t* cr,
				   const NCOORDS::cRect& clip,
				   const SourceTransformInfo& transform);

  };

  struct ColourPair: public std::pair<PatternColour*, PatternColour*>{
    typedef std::pair<PatternColour*, PatternColour*> pair_type;
    typedef PatternColour::size_type size_type;

       PatternColour*& stroke;
    PatternColour*& fill;

    PatternColour& strokecolour(){return *first;}
const PatternColour& strokecolour()const {return *first;}
    PatternColour& fillcolour(){return *second;}
const PatternColour& fillcolour()const {return *second;}

    ColourPair( PatternColour* s, PatternColour* f);

    ColourPair(const  ColourPair& src);
    ColourPair& operator=(const ColourPair& src);
          void use_stroke_colour(cairo_t* cr){stroke->set_source(cr);}
    void use_fill_colour(cairo_t* cr){fill->set_source(cr);}
    void use_stroke_colour(Cairo::RefPtr<Cairo::Context> cr){
      stroke->set_source(cr);
    }
    void use_fill_colour(Cairo::RefPtr<Cairo::Context> cr){
      fill->set_source(cr);
    }

    void set_pattern(PatternColour* pattern, ColourRole role);

    void set_offset(const NCOORDS::coords& pt, ColourRole role);
    void set_rotation_step(size_type step, ColourRole role);
    void set_rotation(double rads, ColourRole role);
    };

 
class ColourManager{
public:
  typedef std::map<std::string,  PatternColour> colour_map_type;
  typedef colour_map_type::iterator iterator;
typedef colour_map_type::const_iterator const_iterator;
  typedef colour_map_type::size_type size_type;
  protected:
  static ColourPair* default_colours;
  static  PatternColour* default_background;
    ColourPair colours;
    PatternColour background;
    std::map<std::string,  PatternColour> colourmap;//will this get used?
  bool use_matrix_pattern_;
public:

  ColourManager();

  ColourManager(const ColourPair& strokefill, const PatternColour& bg,
		colour_map_type* colourmp=NULL);

  static PatternColour& get_default_background();

  static PatternColour* create_default_bg();

  static ColourPair* create_default_strokefill();
  static ColourPair* create_default_colours(){
    return create_default_strokefill();
  }
  static ColourPair& get_default_colours();
  static Draw_colour& get_default_colour(ColourRole role);
  bool use_matrix_pattern()const{return use_matrix_pattern_;}
  static double get_default_alpha(ColourRole role);


  PatternColour& get_patterncolour(ColourRole role =COLOURROLE_STROKE);

  const PatternColour& get_patterncolour(ColourRole role=COLOURROLE_STROKE)const;

  //preserve == false much quicker for repeated use with many points
  void use_colour(cairo_t* cr, ColourRole role, bool preserve = false);

  void use_colour(Cairo::RefPtr<Cairo::Context> cr,
		  ColourRole role, bool preserve = false);
  ///
  void pattern_set_offset(const NCOORDS::coords& offset, 
			  ColourRole role);
void pattern_set_rotation_step(size_type step, 
			  ColourRole role);
  void pattern_set_rotation(double radians,ColourRole role);
  void pattern_set_transform(const SourceTransformInfo& info, ColourRole role);
 
  void set_anchor(const NCOORDS::ncoords& npt);
  ///
  void set_colour(const Draw_colour& gradientbottom,
		  const NCOORDS::coords& start, const NCOORDS::coords& finish,
		  ColourRole role);
  void set_colour(PatternColour* c,  ColourRole role);
  void set_colour(Cairo::RefPtr<Cairo::Surface> surface, xNDRAW::ColourRole role);
  void set_alpha(double a, ColourRole role);
  void set_colour(const Draw_colour& d, ColourRole role);

  ColourSourcebase& get_patternsource(ColourRole role);
  
  Cairo::RefPtr<Cairo::Pattern> get_source(ColourRole role);
  const ColourSourcebase& get_patternsource(ColourRole role)const;
  Draw_colour* get_topcolour(ColourRole role);

  //void set_as_source(Cairo::RefPtr<Cairo::Context context, ColourRole role){

  //}

  PatternSourceInfo get_info(ColourRole role);
  
  void map_pattern(const std::string& nm, const PatternColour& pattern){
    colourmap[nm] = pattern;
  }
  void map_pattern(const std::string& nm, 
		   Cairo::RefPtr<Cairo::Pattern> pattern);

  Cairo::RefPtr<Cairo::Pattern> get_mapped_pattern(const std::string& nm);
  PatternColour& operator[](const std::string& nm){
    return colourmap[nm];
  }

};



  class MatrixPattern:public PatternColour{
  protected:
       
    Cairo::Matrix matrix;

  public:
    MatrixPattern(const  PatternColour& pat);

    virtual ~MatrixPattern(){}

    Cairo::RefPtr<Cairo::Pattern> create_pattern();
  virtual PatternColour* copy()const{
      return new MatrixPattern(*this);
    }
  };
  //template <typename NODE>?
  class OffsetPattern: public MatrixPattern{
protected:
    //NCOORDS::coords offset;
    SourceTransformInfo matrixsetting;
NCOORDS::ncoords base_offset;
    //double rotation;
    bool rotate_first;//which order matrix transforms applied?
  public:
    OffsetPattern(const PatternColour& p, 
		  NCOORDS::coords off =  NCOORDS::coords(), 
		  NCOORDS::ncoords baseoff =  NCOORDS::ncoords(), 
		  double r=0.0,
		  bool rfirst=true);

    virtual  ~OffsetPattern(){}
  virtual PatternColour* copy()const{
      return new OffsetPattern(*this);
    }
    virtual void set_anchor(const NCOORDS::ncoords& npts);
    virtual bool uses_offset()const;
    virtual void set_offset(const NCOORDS::coords& off);
    virtual void set_transform(const SourceTransformInfo& setting){
      matrixsetting=setting; 
    }
    virtual void set_rotation(double radians){
      matrixsetting.rotation = radians;
    } 

virtual void set_rotation_step(size_type step){
  matrixsetting.rotation = step * base_offset.get_step();
}
    virtual void setup_matrix();

    virtual Cairo::RefPtr<Cairo::Pattern> create_pattern();
  };


  template<>
  void Preferences::create_preference<Draw_colour>(const std::string& pname, Draw_colour  v);

  
}
#endif
