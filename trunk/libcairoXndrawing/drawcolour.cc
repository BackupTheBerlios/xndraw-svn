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

#include "drawcolour.h"

namespace xNDRAW{
  using namespace NCOORDS;

  bool Draw_colour::randyseeded= Draw_colour::seedrand(0);




  Cairo::Matrix create_matrix(const coords& zero_pt,
		  const coords& offset,
		  double rotation ,
		  coords scaling ){
#if XNDRAW_MATRIX_REVERSE
 Cairo::Matrix mat = Cairo::translation_matrix(zero_pt.x +offset.x, 
					       zero_pt.y +offset.y);
 coords control(1,1);
    if(scaling != control){
      mat.scale(scaling.x, scaling.y);
    }
    if(rotation != 0){
      mat.rotate(rotation);
    }
mat.translate(-zero_pt.x, -zero_pt.y);
#else
    Cairo::Matrix mat = Cairo::translation_matrix(-zero_pt.x, -zero_pt.y);
    if(rotation != 0){
      mat.rotate(rotation);
    }
    coords control(1,1);
    if(scaling != control){
      mat.scale(scaling.x, scaling.y);
    }
    mat.translate(zero_pt.x +offset.x, zero_pt.y +offset.y);
#endif
    return mat;
  }


  Draw_colour& operator+=(Draw_colour& left, const Draw_colour& right){
    left.a= Draw_colour::clamp(left.a + right.a);
    left.r= Draw_colour::clamp(left.r + right.r);
    left.g = Draw_colour::clamp(left.g + right.g);
    left.b = Draw_colour::clamp(left.b + right.b);
    return left;
  }

  Draw_colour operator+(const Draw_colour& left, const Draw_colour& right){
    Draw_colour res(left);
    res.a= Draw_colour::clamp(res.a + right.a);
    res.r= Draw_colour::clamp(res.r + right.r);
    res.g = Draw_colour::clamp(res.g + right.g);
    res.b = Draw_colour::clamp(res.b + right.b);
    return res;
  }

  Draw_colour& operator-=(Draw_colour& left, const Draw_colour& right){
    left.a= Draw_colour::clamp(left.a - right.a);
    left.r= Draw_colour::clamp(left.r - right.r);
    left.g = Draw_colour::clamp(left.g - right.g);
    left.b = Draw_colour::clamp(left.b - right.b);
    return left;
  }


  Draw_colour operator-(const Draw_colour& left, const Draw_colour& right){
    Draw_colour res(left);
    res.a= Draw_colour::clamp(res.a - right.a);
    res.r= Draw_colour::clamp(res.r - right.r);
    res.g = Draw_colour::clamp(res.g - right.g);
    res.b = Draw_colour::clamp(res.b - right.b);
    return res;
  }

  Draw_colour& operator*=(Draw_colour& left, const Draw_colour& right){
    left.a *= right.a;//values will be in range, so no need to call clamp()
    left.r *= right.r;
    left.g *= right.g;
    left.b *= right.b;
    return left;
  }

  Draw_colour operator*(const Draw_colour& left, const Draw_colour& right){
    Draw_colour res(left);
    res.a *= right.a;//values will be in range, so no need to call clamp()
    res.r *= right.r;
    res.g *= right.g;
    res.b *= right.b;
    return res;
  }

  Draw_colour& operator/=(Draw_colour& left, const Draw_colour& right){
    left.a= Draw_colour::clamp(left.a / right.a);
    left.r= Draw_colour::clamp(left.r / right.r);
    left.g = Draw_colour::clamp(left.g / right.g);
    left.b = Draw_colour::clamp(left.b / right.b);
    return left;
  }

  Draw_colour operator/(const Draw_colour& left, const Draw_colour& right){
    Draw_colour res(left);
    res.a= Draw_colour::clamp(res.a / right.a);
    res.r= Draw_colour::clamp(res.r / right.r);
    res.g = Draw_colour::clamp(res.g / right.g);
    res.b = Draw_colour::clamp(res.b / right.b);
    return res;
  }

  ColourPair* ColourManager::default_colours = NULL;
  PatternColour* ColourManager::default_background =NULL;

  Draw_colour::Draw_colour(double red ,double green, double blue,double  alpha)
    :
    r(clamp(red)),
    g(clamp(green)),
    b(clamp(blue)),
    a(clamp(alpha))
  {}

  Draw_colour& Draw_colour::operator=(const Draw_colour& src){
    if(&src !=this){
      r= src.r;
      g= src.g;
      b= src.b;
      a=src.a;
    }
    return *this;
  }

  void Draw_colour::set(double red ,double green, double blue,double  alpha){
    r =clamp(red);
    g = clamp(green);
    b= clamp(blue);
    a = clamp(alpha);
  }

  void Draw_colour::set_rgb(const Draw_colour& ref){
    //leaves alpha alone
    r = ref.r;
    g = ref.g;
    b = ref.b;
  }
  void Draw_colour::set(const Draw_colour& ref){
    r = ref.r;
    g = ref.g;
    b = ref.b;
    a = ref.a;
  }

  //member specialisation - see ../appcommon/preferences.h
  template<>
  void Preferences::create_preference<Draw_colour>(const std::string& pname, Draw_colour  v){
    registry[pname] = "Draw_colour";
    m_map[pname] = new Preference<Draw_colour>(pname, v);
  }
  /*
    Cairo::ColorStop Draw_colour::as_colourstop(double offf){
    if(offf<0){
    offf=0;
    std::cerr<<"clamped colorstop offset to 0 (valid range 0-1)"<<std::endl;
    }
    else if(offf > 1){
    offf =1;
    std::cerr<<"clamped colorstop offset to 1(valid range 0-1)"<<std::endl;
    }
    return Cairo::ColorStop(offf, r,g,b,a);
    }
  */
  double Draw_colour::clamp(double v, double min, double max){
    if(v < min){v=min;}
    else if(v>max){v=max;}
    return v;
  }

  Cairo::RefPtr<Cairo::LinearGradient> 
  Draw_colour::create_linear_gradient(const Draw_colour& start, 
				      const Draw_colour& finish,
				      const coords& control1,
				      const coords& control2){
    Cairo::RefPtr<Cairo::LinearGradient> grad = 
      Cairo::LinearGradient::create(control1.x, control1.y,
				    control2.x, control2.y);
    grad->add_color_stop_rgba(0.0, start.r, start.g, start.b, start.a);
    grad->add_color_stop_rgba(1.0, finish.r, finish.g, finish.b, finish.a);
    return grad;
  }

  Cairo::RefPtr<Cairo::RadialGradient> 
  Draw_colour::create_radial_gradient(const Draw_colour& start, 
				      const Draw_colour& finish,
				      const coords& centre_0,
				      double r0, 
				      const coords& centre_1,
				      double r1){
    Cairo::RefPtr<Cairo::RadialGradient> grad = 
      Cairo::RadialGradient::create(centre_0.x, centre_0.y, r0,
				    centre_1.x, centre_1.y, r1);
    grad->add_color_stop_rgba(0.0, start.r, start.g, start.b, start.a);
    grad->add_color_stop_rgba(1.0, finish.r, finish.g, finish.b, finish.a);
    return grad;
  }
bool Draw_colour::seedrand(long seed){
if(!seed){
      seed= time(NULL);
    }
    std::srand(seed);
    return true;
}

 



  void Draw_colour::randomise(){
    
    r = std::rand() / (double)RAND_MAX;
    g = std::rand() / (double)RAND_MAX;
    b = std::rand() / (double)RAND_MAX;
    a = std::rand() / (double)RAND_MAX;
  }

  template <>
  std::string get_valueType<Draw_colour>(const Draw_colour& i){
    return std::string("Draw_colour");
  }
  LinearGradientSrc::LinearGradientSrc(const std::pair<Draw_colour , Draw_colour >& colours,
				       const NCOORDS::coords& p0,
				       const NCOORDS::coords& p1):
    ColourSource(colours.first),
    bottomcolour(colours.second),
    pt0(p0),
    pt1(p1){}

  LinearGradientSrc::LinearGradientSrc(const  ColourSource& top, const  Draw_colour& bottom,
				       const NCOORDS::coords& p0,
				       const NCOORDS::coords& p1):
    ColourSource(top),
    bottomcolour(bottom),
    pt0(p0),
    pt1(p1){}

  RadialSource::RadialSource (const std::pair<Draw_colour , Draw_colour >& colours,
			      const NCOORDS::coords& p0, const NCOORDS::coords& p1,
			      double r0, double r1):
    LinearGradientSrc(colours, p0, p1),
    radius_0(r0), radius_1(r1){}


  RadialSource::RadialSource(const  ColourSource& top, const  Draw_colour& bottom,
			     const NCOORDS::coords& p0,
			     const NCOORDS::coords& p1,
			     double r0, double r1):
    LinearGradientSrc(top, bottom, p0, p1),
    radius_0(r0), radius_1(r1){}

  Cairo::RefPtr<Cairo::Pattern> RadialSource::create_pattern(){
    return  Draw_colour::create_radial_gradient(topcolour, bottomcolour, pt0,
						radius_0, pt1, radius_1);
  }

  PatternColour::~PatternColour(){
    if(patternsource){
      delete patternsource;
    }
  }


  PatternColour::PatternColour(Cairo::RefPtr<Cairo::Surface> surf):
    info( PATTERN_SOURCE_PATTERN),
    patternsource( new ArbitrarySrc(surf)){}

  PatternColour::PatternColour(Cairo::RefPtr<Cairo::Pattern> p):
    info( PATTERN_SOURCE_PATTERN),
    patternsource( new ArbitrarySrc(p)){}

  PatternColour::PatternColour(const std::pair<Draw_colour , Draw_colour >& colours,
			       const NCOORDS::coords& p0, const NCOORDS::coords& p1,
			       double r0, double r1):
    info(PATTERN_SOURCE_RADIAL_GRADIENT),
    patternsource(new RadialSource(colours, p0, p1, r0, r1)){}

  PatternColour::PatternColour(const std::pair<Draw_colour , Draw_colour >& colours,
			       const NCOORDS::coords& p0,
			       const NCOORDS::coords& p1):
    info(PATTERN_SOURCE_LINEAR_GRADIENT),
    patternsource(new LinearGradientSrc(colours, p0, p1)){}

  PatternColour::PatternColour(const Draw_colour& d):
    info(PATTERN_SOURCE_RGBA),
    patternsource(new ColourSource(d)){}

  PatternColour::PatternColour():info(PATTERN_SOURCE_RGBA),
				 patternsource(new ColourSource()){}
    
  PatternColour::PatternColour(const PatternColour& src):info(src.info){
    copy_src(src);
  }

  PatternColour& PatternColour::operator=(const PatternColour& src){
    if(&src != this){
      ColourSourcebase* base = patternsource;
      if(!base){
	throw xn_Error("dest->patternsource is NULL, is always an error post construction");
      }
      copy_src(src);
      delete base;
    }
    return *this;
  }
  void PatternColour::set_source_with_transform(cairo_t* cr,
						const cRect& clip,
						const SourceTransformInfo& transform){
    if(!cr){throw xn_Error("cairo_t NULL");}
    coords centre=clip.get_centre();
    cairo_push_group(cr);
    //Cairo::RefPtr<Cairo::Pattern> p=patternsource->create_pattern();
    cairo_set_source(cr,patternsource->create_pattern()->cobj());
    
    cairo_rectangle(cr,clip.x(),clip.y(), clip.width(),  clip.height());
    cairo_translate(cr,-centre.x, -centre.y);
    //coords zero(0,0);
    coords one(1,1);
    if(transform.scale != one){
      cairo_scale(cr,transform.scale.x, transform.scale.y);
    }
    if(transform.rotation != 0){
      cairo_rotate(cr,transform.rotation);
    }       
    cairo_translate(cr,centre.x +transform.offset.x ,centre.y+transform.offset.y);
    cairo_fill(cr);
    cairo_pop_group_to_source(cr);
  }


  void PatternColour::set_source_with_transform(Cairo::RefPtr<Cairo::Context> cr,
						const cRect& clip,
						const SourceTransformInfo& transform){
    if(!cr){throw xn_Error("Context NULL");}
    coords centre=clip.get_centre();
    cr->push_group();
    //Cairo::RefPtr<Cairo::Pattern> p=patternsource->create_pattern();
    cr->set_source(patternsource->create_pattern());
    
    cr->rectangle(clip.x(),clip.y(), clip.width(),  clip.height());
    cr->translate(-centre.x, -centre.y);
    coords zero(0,0);
    coords one(1,1);
    if(transform.scale != one){
      cr->scale(transform.scale.x, transform.scale.y);
    }
    if(transform.rotation != 0){
      cr->rotate(transform.rotation);
    }       
    cr->translate(centre.x +transform.offset.x, centre.y+transform.offset.y);
    cr->fill();
    cr->pop_group_to_source();
  }


  void PatternColour::copy_src(const PatternColour& src){
    info = src.info;
    if(!src.patternsource){
      throw xn_Error("copy source is NULL");
    }
    patternsource= ColourSourcebase::copy(*(src.patternsource));
   
    if (!patternsource){
      throw xn_Error("something went wrongf!");
    }
  }

  void PatternColour::set_alpha(double a){
    if(patternsource){
      patternsource->set_alpha(a);
    }
  }

  double PatternColour::get_alpha()const{
    if(patternsource){
      return patternsource->get_alpha();
    }
    throw xn_Error("colour eror:internal");
  }

  ColourPair& ColourPair::operator=(const ColourPair& src){
    if(&src != this){
      pair_type::operator=(src);
    }
    return *this;
  } 

  ColourPair::ColourPair(PatternColour* s,  PatternColour* f):
    pair_type(s,f),
    stroke(first), fill(second){}

  ColourPair::ColourPair(const  ColourPair& src):pair_type(src),
						 stroke(first), fill(second){}
  ///finish
  ////ie for OffsetPattern(think about fixing gradients to be able to use there)
  void ColourManager::pattern_set_offset(const NCOORDS::coords& off, 
					 ColourRole role){
    if(use_matrix_pattern()){
      //didn't, doesn't work as desired : '
      //now generates straight imposition of the sourcepattern,
      //rather than transmogrifying it as per the drawing pattern
      colours.set_offset(off, role);
    }
  }
  void ColourManager::pattern_set_rotation_step(size_type step, 
						ColourRole role){
    if(use_matrix_pattern()){
      colours.set_rotation_step(step,  role);
    }
  }

  void ColourManager::pattern_set_rotation(double radians, 
					   ColourRole role){
    if(use_matrix_pattern()){
      colours.set_rotation(radians,  role);
    }
  }
  ///pattern_set_anchor
  void ColourManager::set_anchor(const NCOORDS::ncoords& npt){
    colours.stroke->set_anchor(npt);
    colours.fill->set_anchor(npt);
  }

  void ColourManager::map_pattern(const std::string& nm, 
				  Cairo::RefPtr<Cairo::Pattern> pattern){
    PatternColour c(pattern);
    map_pattern(nm, c);
  }

  Cairo::RefPtr<Cairo::Pattern> 
  ColourManager::get_mapped_pattern(const std::string& nm){
    iterator iter= colourmap.find(nm);
    if(iter == colourmap.end()){return  Cairo::RefPtr<Cairo::Pattern>(NULL);}
    PatternColour& src = colourmap[nm];
    return src.create_pattern();
  }
  ///
  void ColourManager::set_colour(const Draw_colour& d, ColourRole role){
    if(role == COLOURROLE_STROKE){
      PatternColour* old =colours.first;
      PatternColour tmpcopy(d);
      colours.first = new OffsetPattern(tmpcopy);
      delete old;
    }
    else if(role == COLOURROLE_FILL){
      PatternColour* old =colours.second;
      colours.second = new PatternColour(d);
      delete old;
    }
    else {background = PatternColour(d);}
  }
  void ColourPair::set_offset(const coords& pt, ColourRole role){
    if(role ==COLOURROLE_STROKE){
      stroke->set_offset(pt);
    }
    else if (role ==COLOURROLE_FILL){
      fill->set_offset(pt);
    }
    else{/*silently fail*/}
  }
  void ColourPair::set_rotation(double radians, ColourRole role){
    if(role ==COLOURROLE_STROKE){
      stroke->set_rotation(radians);
    }
    else if (role ==COLOURROLE_FILL){
      fill->set_rotation(radians);
    }
    else{/*silently fail*/}
  }

  void ColourPair::set_rotation_step(size_type step, ColourRole role){
    if(role ==COLOURROLE_STROKE){
      stroke->set_rotation_step(step);
    }
    else if (role ==COLOURROLE_FILL){
      fill->set_rotation_step(step);    }
    else{/*silently fail*/}
  }

  void ColourPair::set_pattern(PatternColour* pattern, ColourRole role){
    if(!pattern){throw xn_Error("ColourPair::set_pattern:NULL pattern");
      if (role == COLOURROLE_STROKE){
	PatternColour* oldstroke = first;
	first= pattern;
	delete oldstroke ;
      }
      else if(role == COLOURROLE_FILL){
	PatternColour* oldfill = second;
	second= pattern;
	delete oldfill;

      }
      else if(role == COLOURROLE_BG){
	//ERROR -SILENTLY IGNORE
      }
      else {
	//?error, warn or ignore(ignore 4 now)
      }
    }
  }

  void ColourManager::set_colour(Cairo::RefPtr<Cairo::Surface> surface, 
				 ColourRole role){
    if(!surface){
      throw xn_Error("ColourManager::set_colour: surface NULL");
    }
    if(role == COLOURROLE_STROKE){
      PatternColour* oldstroke=colours.first;
      PatternColour* oldfill =colours.second;
      colours= ColourPair(new PatternColour(surface), oldfill);
      delete  oldstroke;
    }
    else if(role == COLOURROLE_FILL){
      PatternColour* oldstroke=colours.first;
      PatternColour* oldfill =colours.second;
      colours = ColourPair(oldstroke, new PatternColour(surface));
      delete  oldfill;
    }
    else {background = PatternColour(surface);
    }
  }

  void ColourManager::use_colour(Cairo::RefPtr<Cairo::Context> cr, 
				 ColourRole role, bool preserve ){
    cr->save();
    if(role == COLOURROLE_STROKE){
      colours.stroke->set_source(cr);
      if(preserve){
	cr->stroke_preserve();
      }
      else{cr->stroke();}
    }
    else if(role == COLOURROLE_FILL){
      colours.fill->set_source(cr);
      if(preserve){
	cr->fill_preserve();
      }
      else{cr->fill();}
    }
    else{
      background.set_source(cr);
      cr->paint();
    }
    cr->restore();
  }
 

  void ColourManager::use_colour(cairo_t* cr, 
				 ColourRole role,
				 bool preserve ){
    cairo_save(cr);
    if(role == COLOURROLE_STROKE){
      colours.stroke->set_source(cr);
      if(preserve){
	cairo_stroke_preserve(cr);
      }
      else{cairo_stroke(cr);}
    }
    else if(role == COLOURROLE_FILL){
      colours.fill->set_source(cr);
      if(preserve){
	cairo_fill_preserve(cr);
      }
      else{cairo_fill(cr);}
    }
    else{
      background.set_source(cr);
      cairo_paint(cr);
    }
    cairo_restore(cr);
  }
  ColourPair& ColourManager::get_default_colours(){
    if(!default_colours){
      default_colours = create_default_colours();
    }
    return *default_colours;
  }

  void ColourManager::set_colour(PatternColour* c,  ColourRole role){
    switch(role){
    case COLOURROLE_STROKE:
    case COLOURROLE_FILL:
      colours.set_pattern(c, role);
      break;
    case COLOURROLE_BG:
      background = *c;
    default:
      throw xn_Error("Error(bad ColourRole)");
    }  
  }

  Draw_colour& ColourManager::get_default_colour(ColourRole role){
    if(!default_colours){default_colours = create_default_colours();}
    if(!default_background){default_background = create_default_bg();}
    Draw_colour* tmp(NULL);
    switch(role){
    case COLOURROLE_STROKE:
      tmp= default_colours->stroke->get_topcolour();
      break;
    case COLOURROLE_FILL:
      tmp= default_colours->fill->get_topcolour();
      break;
    case COLOURROLE_BG:
      tmp = default_background->get_topcolour();
    default:
      throw xn_Error("internal error(bad ColourRole)");
    }
    if(!tmp ){
      Draw_colour d(1,1,1,1);
      PatternColour colour(d);
      if(role == COLOURROLE_BG){(*default_background) = colour;}
      throw xn_Error("default colour for this role a pattern");
    }
    return *tmp;
  }

  ColourPair* ColourManager::create_default_strokefill(){
    Draw_colour defstroke(1.0, 0,0,0.7);
    Draw_colour deffill(0, 0.7, 1.0, 0.7);
    PatternColour* strok = new PatternColour(defstroke);
    PatternColour* fil = new PatternColour(deffill);
    return new ColourPair(strok, fil);
  }

  void ColourManager::set_colour(const Draw_colour& gradientbottom,
				 const coords& start, const coords& finish,
				 ColourRole role){
    PatternSourceInfo info = get_info(role);
    Draw_colour top;
    std::pair<Draw_colour, Draw_colour> argcolours(top,  gradientbottom);
	
    if(info == PATTERN_SOURCE_RGBA){
      //switch(role){
      if(role== COLOURROLE_STROKE){
	argcolours.first =*(colours.stroke->get_topcolour());
      }
      else if (role == COLOURROLE_FILL){
	argcolours.first =*(colours.fill->get_topcolour());
      }
      else if(role == COLOURROLE_BG){
	argcolours.first = *(background.get_topcolour());
      }
    }
    else{argcolours.first = get_default_colour(role);}
    if(role== COLOURROLE_STROKE || role == COLOURROLE_FILL){
      PatternColour* newcolour = new PatternColour(argcolours, start, finish);
     
      colours.set_pattern(newcolour, role);
    }
    else if(role == COLOURROLE_BG){
      background=PatternColour(argcolours, start, finish);
    }
  }

  PatternSourceInfo ColourManager::get_info(ColourRole role){
    switch(role){
    case COLOURROLE_STROKE:
      return colours.stroke->get_info();
    case COLOURROLE_FILL:
      return colours.fill->get_info();
    case COLOURROLE_BG:
      return background.get_info();
    default:
      throw xn_Error("internal error(bad ColourRole)");
    }
  }

  Draw_colour* ColourManager::get_topcolour(ColourRole role){
    switch(role){
    case COLOURROLE_STROKE:
      return colours.stroke->get_topcolour();
    case COLOURROLE_FILL:
      return colours.fill->get_topcolour();
    case COLOURROLE_BG:
      return background.get_topcolour();
    default:
      throw xn_Error("internal error(bad ColourRole)");
    }
  }


  ColourSourcebase& ColourManager::get_patternsource(ColourRole role){
    switch(role){
    case COLOURROLE_STROKE:
      return colours.stroke->get_patternsource();
    case COLOURROLE_FILL:
      return colours.fill->get_patternsource();
    case COLOURROLE_BG:
      return background.get_patternsource();
    default:
      throw xn_Error("internal error(bad ColourRole)");
    }
  }

  const ColourSourcebase& ColourManager::get_patternsource(ColourRole role)const{ 
    switch(role){
    case COLOURROLE_STROKE:
      return colours.stroke->get_patternsource();
    case COLOURROLE_FILL:
      return colours.fill->get_patternsource();
    case COLOURROLE_BG:
      return background.get_patternsource();
    default:
      throw xn_Error("internal error(bad ColourRole)");
    }
  }


  Cairo::RefPtr<Cairo::Pattern> ColourManager::get_source(ColourRole role){
    switch(role){
    case COLOURROLE_STROKE:
      return colours.stroke->create_pattern();
    case COLOURROLE_FILL:
      return colours.fill->create_pattern();
    case COLOURROLE_BG:
      return background.create_pattern();
    default:
      throw xn_Error("internal error(bad ColourRole)");
    }
  }

  PatternColour* ColourManager::create_default_bg(){
    //TODO: pref lookup
    Draw_colour defbg(1.0, 1.0, 1.0, 1.0);
    return new PatternColour(defbg);
  }

  void ColourManager::set_alpha(double a, ColourRole role){
    switch(role){
    case  COLOURROLE_STROKE:
      colours.stroke->set_alpha(a);
      return;
    case COLOURROLE_FILL:
      colours.fill->set_alpha(a);
      return;
    case COLOURROLE_BG:
      background.set_alpha(a);
      return;
    case COLOURROLE_NONE:
    default:
      std::ostringstream ostmp;
      ostmp<<"NO ROLE, OR UNKNOWn role: "<<(int)role;
      std::string s_err =ostmp.str();
      throw xn_Error(s_err.c_str());
    }
  }

  double ColourManager::get_default_alpha(ColourRole role){
    //lazy init
    if(!default_colours){default_colours = create_default_colours();}
    if(!default_background){default_background = create_default_bg();}
    switch(role){
    case COLOURROLE_STROKE:
      return default_colours->stroke->get_alpha();
    case COLOURROLE_FILL:
      return default_colours->fill->get_alpha();
    case COLOURROLE_BG:
      return default_background->get_alpha();
    default:
      throw xn_Error("bad role or no role");
    }
  }
  PatternColour& ColourManager::get_default_background(){
    if(!default_background){
      default_background = create_default_bg();
    }
    return *default_background;
  }
  ColourManager::ColourManager(const ColourPair& strokefill, 
			       const PatternColour& bg,
			       colour_map_type* colourmp):
    colours(strokefill),
    background(bg){
    if(colourmp){
      std::swap(*colourmp, colourmap);
    }
  }

  PatternColour& ColourManager::get_patterncolour(ColourRole role){
    switch(role){
    case COLOURROLE_STROKE:
      return *(colours.stroke);
    case COLOURROLE_FILL:
      return *(colours.fill);
    case COLOURROLE_BG:
      return background;
    }
  }

  const PatternColour& ColourManager::get_patterncolour(ColourRole role)const {
    switch(role){
    case COLOURROLE_STROKE:
      return *(colours.stroke);
    case COLOURROLE_FILL:
      return *(colours.fill);
    case COLOURROLE_BG:
      return background;
    }
  }

  Cairo::RefPtr<Cairo::Pattern> ColourSourcebase::create_pattern(){
    throw xn_Error("don't use base class object:\noverride  create_pattern() in erived type");
  }

  double ColourSourcebase::get_alpha()const{
    throw xn_Error("don't use base class object: \noverride  get_alpha() in a derived type");
  }

  void ColourSourcebase::set_alpha(double){
    throw xn_Error("don't use base class object: \noverride  set_alpha(double) in a derived type");
  }
  SourceTransformInfo::SourceTransformInfo()://'identity transformation' - do nothing
    //origin and offset -> default ctorOK
    rotation(0), scale(1,1){}

  SourceTransformInfo::SourceTransformInfo(const  coords& oRIG,
					   const  coords& off,
					   double rot,
					   coords sc):
    origin( oRIG),
    offset(off), rotation(rot),  scale(sc){}

  bool SourceTransformInfo::should_apply_offset()const{
    coords nomove(0,0);
    return nomove != offset;
  }
  void SourceTransformInfo::translate_origin_to_zero(Cairo::Matrix& outmatrix){
    //since this called first,
    //could simply assign
    //outmatrix=movematrix;
    coords control(0,0);
    if(origin != control){
      Cairo::Matrix movematrix=Cairo::translation_matrix(-origin.x, -origin.y);
#if XNDRAW_MATRIX_REVERSE
      outmatrix.multiply( movematrix, outmatrix);
#else
      outmatrix.multiply( outmatrix,  movematrix);
#endif
    }
  }

  void SourceTransformInfo::translate_zero_to_origin(Cairo::Matrix& outmatrix){
 
 coords control(0,0);
    if(origin != control){
      Cairo::Matrix movematrix=Cairo::translation_matrix(origin.x, origin.y);
#if XNDRAW_MATRIX_REVERSE
      outmatrix.multiply( movematrix, outmatrix);
#else
      outmatrix.multiply( outmatrix,  movematrix);
#endif
    }
  }

 void SourceTransformInfo::do_scale(Cairo::Matrix& outmatrix){
    if(should_apply_scale()){
      Cairo::Matrix scalematrix= Cairo::scaling_matrix(1. / scale.x, 1. / scale.y);
#if XNDRAW_MATRIX_REVERSE
      outmatrix.multiply(scalematrix, outmatrix);
#else
      outmatrix.multiply(outmatrix, scalematrix);
#endif
    }
  }

  void SourceTransformInfo::do_rotation(Cairo::Matrix& outmatrix){
    if(should_apply_rotation()){
      Cairo::Matrix rotmatrix= Cairo::rotation_matrix(rotation);
#if XNDRAW_MATRIX_REVERSE
      outmatrix.multiply(rotmatrix, outmatrix);
#else
      outmatrix.multiply(outmatrix, rotmatrix);
#endif
      //outmatrix.rotate(-rotation);
    }
  }

  bool SourceTransformInfo::should_apply_scale()const{
    NCOORDS::coords scaleunchanged(1,1);
    return scale !=scaleunchanged;
  }

  void SourceTransformInfo::do_offset(Cairo::Matrix& outmatrix){
    if(should_apply_offset()){
      Cairo::Matrix tmatrix= Cairo::translation_matrix(offset.x, offset.y);
#if XNDRAW_MATRIX_REVERSE
      outmatrix.multiply(tmatrix, outmatrix);
#else
      outmatrix.multiply(outmatrix, tmatrix);
#endif
      //outmatrix.translate(-offset.x, -offset.y);
    }
  } 

  //real order:
  //translate to zero, saving offset
  //scale
  //rotate
  //translate
  //translate by -offset

  void SourceTransformInfo::setup_matrix(Cairo::Matrix& outmatrix, 
					 bool resetfirst){
    coords zeropt(origin);
    this->setup_matrix(outmatrix, zeropt,resetfirst);
  }
  void SourceTransformInfo::setup_matrix(Cairo::Matrix& outmatrix, 
					 const coords& zeropt,
					 bool resetfirst){

    if( resetfirst){
	
      outmatrix = create_matrix(zeropt,offset, rotation, scale);
    }
    else{
      // application order handled in the individual calls
      //# if XNDRAW_MATRIX_REVERSE
      translate_origin_to_zero(outmatrix);
      do_scale(outmatrix);
      do_rotation(outmatrix);
      do_offset(outmatrix);
      translate_zero_to_origin(outmatrix);
  
      /*    # else
      translate_zero_to_origin(outmatrix);
      do_offset(outmatrix);
      do_rotation(outmatrix);
      do_scale(outmatrix);
      translate_origin_to_zero(outmatrix);
      //#endif*/
    }
  }

  MatrixPattern::MatrixPattern(const  PatternColour& pat):
    PatternColour(pat),
    matrix(Cairo::identity_matrix()){}

  Cairo::RefPtr<Cairo::Pattern> MatrixPattern::create_pattern(){
    Cairo::RefPtr<Cairo::Pattern> res = PatternColour::create_pattern();
    res->set_matrix(matrix);
    return res;
  }

  OffsetPattern::OffsetPattern(const PatternColour& p, 
			       NCOORDS::coords off , 
			       NCOORDS::ncoords baseoff , 
			       double r,
			       bool rfirst):
    MatrixPattern(p),
    matrixsetting(baseoff, off-(const NCOORDS::coords&)baseoff, r),
    //offset(off -(const  NCOORDS::coords&) baseoff),
    base_offset(baseoff),
    //rotation(r),
    //patternsrc(p),
    rotate_first(rfirst){}

  void OffsetPattern::set_anchor(const NCOORDS::ncoords& npts){
    coords old_o = base_offset ;
    base_offset = npts;
    matrixsetting.origin= base_offset;
    coords control;
    if(old_o != control){
      coords diff= base_offset-old_o;
      //add the old origin, and subtract the new one ...
      matrixsetting.offset = matrixsetting.offset -diff;
    }
  }

  Cairo::RefPtr<Cairo::Pattern> OffsetPattern::create_pattern(){
    if(uses_offset()){
      setup_matrix();
    }
    return MatrixPattern::create_pattern();
  }

  void OffsetPattern::setup_matrix(){
    matrixsetting.setup_matrix(matrix);
      
  }
  void ColourManager::pattern_set_transform(const SourceTransformInfo& info, 
					    ColourRole role){
    switch(role){
    case COLOURROLE_STROKE:
      colours.stroke->set_transform(info);
      return;
    case COLOURROLE_FILL:
      colours.stroke->set_transform(info);
      return;
    default:
      return;
    }
  }
 
  bool  OffsetPattern::uses_offset()const{return true;}

  void OffsetPattern::set_offset(const NCOORDS::coords& off){
    matrixsetting.offset = off - (const NCOORDS::coords& )base_offset;
  }

  ArbitrarySrc::ArbitrarySrc(const ArbitrarySrc& src):
    the_source(src.the_source),
    alpha(src.alpha){
    cairo_pattern_reference(the_source->cobj());
  }

  ArbitrarySrc::ArbitrarySrc(Cairo::RefPtr<Cairo::Pattern> p):
    the_source(p),
    alpha(1)
  {  cairo_pattern_reference(the_source->cobj());}

  ArbitrarySrc::ArbitrarySrc(Cairo::RefPtr<Cairo::Pattern> p, double transparency):
    the_source(p),
    alpha(Draw_colour::clamp(transparency))
  {  cairo_pattern_reference(the_source->cobj());}

  ArbitrarySrc::ArbitrarySrc(Cairo::RefPtr<Cairo::Surface> surf):
    the_source(Cairo::SurfacePattern::create(surf)),
    alpha(1){
    //create() should do this?
    //cairo_pattern_reference(the_source->cobj());
  }

  ArbitrarySrc::ArbitrarySrc(Cairo::RefPtr<Cairo::Surface> surf, 
			     double transparency):
    the_source(Cairo::SurfacePattern::create(surf)),
    alpha(Draw_colour::clamp(transparency))
  {
    //create() should do this?
    //cairo_pattern_reference(the_source->cobj());
  }

  Draw_colour* PatternColour::get_topcolour(){
    if((info == PATTERN_SOURCE_RGBA) ||
       (info == PATTERN_SOURCE_LINEAR_GRADIENT) ||
       (info == PATTERN_SOURCE_RADIAL_GRADIENT)){
      ColourSource* src = dynamic_cast< ColourSource*>(patternsource);
      if(!src){
	throw xn_Error("dynamic_cast< ColourSource*>(patternsource) failed, but info is PATTERN_SOURCE_RGBA or PATTERN_SOURCE_LINEAR_GRADIENT or PATTERN_SOURCE_RADIAL_GRADIENT: internal error");
      }
      return &(src->get_topcolour());
    }
    return NULL;//cairo pattern object, with no Draw_colour 
  }

  ColourManager::ColourManager():colours(get_default_colours()), 
				 background(get_default_background()),
				 use_matrix_pattern_(true){}



}
