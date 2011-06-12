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

#include "drawingbase.h"

//UNDEFINe when done
//at least 2 bugs fixed - 1)wrong order of svg handles when importing
//                        2) handler in Drawingbase - was confused and broken
//More bugs remain: first/last point in imported path(bezier only?) doesn't transform properly
//Occasional crashes from drawtool/bezier - middle or right button handler
//some commands that reset the path - all such commands should
//call pingwindow->reset_drawtool()
//#define DEBUG_BEZIER 1


namespace xNDRAW{
using namespace NCOORDS;
///for xndrawprefs.cc

bool Drawingbase::points_selected()const{
      if(!history){
	throw xn_Error("Drawingbase::points_selected()path NULL");
      }
      return history->points_selected();
    }

  int Drawingbase::initialised=Drawingbase::initialise_path_flattener();

void CairoConfiguration::set_transparency(double val){
if(val < 0){val =0;}
 else if(val > 1){val =1;}
 trans=val;
  } 
  void CairoConfiguration::apply(Drawingbase& apply_to)const{
      //don't save/restore!
      apply_to.set_line_width(linewidth);
      apply_to.set_operator(operatore);
      apply_to.set_line_join(linejoin);
    }
void CairoConfiguration::set_line_width(double lw){
      double LINE_WIDTH_MAX;
      XNDRAW_GET_PREF("LINE_WIDTH_MAX",LINE_WIDTH_MAX);
      if(lw < 0){lw=0;}
      else if(lw > LINE_WIDTH_MAX){lw = LINE_WIDTH_MAX;}
      linewidth=lw;
    }
  void Drawingbase::initialise_prefs(){
      ColourPair* defs = &(ColourManager::get_default_colours());
      if(!defs){
	throw xn_Error(" ColourManager::get_default_colours() returned NULL");
      }
      //this is backwds - define here, create_pref, then the ColourManager
      //defaults FIXME
      double maxline(240);
      XNDRAW_SET_PREF("LINE_WIDTH_MAX", maxline);
      std::string s_name("COLOUR_STROKE");
      XNDRAW_SET_PREF(s_name, defs->stroke->get_topcolour());
      s_name = "COLOUR_FILL";
      XNDRAW_SET_PREF(s_name, defs->fill->get_topcolour());
      s_name = "COLOUR_BG";
      std::string mingrid("MINIMUM_GRID");
      std::string maxgrid("MAXIMUM_GRID");
      XNDRAW_SET_PREF(mingrid, 100u);
      XNDRAW_SET_PREF(maxgrid, 500u);
      PatternColour& bg =ColourManager::get_default_background();
      XNDRAW_SET_PREF(s_name,bg.get_topcolour());
      unsigned long bufferw(2000), bufferh(2000), svg_w(1500), svg_h(1500);
      std::string b_w("BUFFERWIDTH"), b_h("BUFFERHEIGHT"),s_wide("SVGWIDTH"),
	s_high("SVGHEIGHT");
      XNDRAW_SET_PREF(b_w, bufferw);
      XNDRAW_SET_PREF(b_h, bufferh);
      XNDRAW_SET_PREF(s_wide, svg_w);
      XNDRAW_SET_PREF(s_high, svg_h);
    }

template<>
std::string get_valueType<NCOORDS::simple_click_history>(const NCOORDS::simple_click_history& path){
  return "simple_click_history";
}

template<>
std::string get_valueType<NCOORDS::coords>(const NCOORDS::coords& pt){
  return "coords";
}
template<>
std::string get_valueType<NCOORDS::ncoords>(const NCOORDS::ncoords& pt){
  return "ncoords";
}

PaintContext Drawingbase::create_paintcontext(cairo_t* cr){
      Cairo::Context * ct = new Cairo::Context(cr);
      Cairo::RefPtr<Cairo::Context> context(ct);
      return PaintContext(colourmanager.get_patterncolour().copy(), context);
    }

    PaintContext 
    Drawingbase::create_paintcontext(Cairo::RefPtr<Cairo::Context> context){
      
      return PaintContext(colourmanager.get_patterncolour().copy(), context);
    }

  PaintContext 
  Drawingbase::create_paintcontext(cairo_t* cr, 
				   ColourRole role, 
				   void (*meth)(cairo_t*)){
      Cairo::Context * ct = new Cairo::Context(cr);
      Cairo::RefPtr<Cairo::Context> context(ct);
      return PaintContext(colourmanager.get_patterncolour(role).copy(), context, meth);
    }

 void 
    Drawingbase::pointOp_with_paintOp(void(Drawingbase::*pointOp)(const std::pair<coords, int>&), 
			 const std::pair<coords, int>& pointarg, 
			 void(Drawingbase::*paintOp)(void)){
      (this->*pointOp)(pointarg);
      (this->*paintOp)();
    }

Cairo::Path*  Drawingbase::get_iterated_path(){
      //so it can be written to svg output
      Cairo::Path* savedpath = copy_path();
      if(m_cobject != buffer){
	std::cerr<< "m_cobject != buffer!"<<::std::endl;
      }
      redraw_line(false);

      Cairo::Path* result = copy_path();
      std::cerr<< "buffer status: ";
      if(do_check_status(buffer)== CAIRO_STATUS_SUCCESS){
	std::cerr<< "good\n";
      }
    
      std::cerr << "result of get_iterated_path(): "<<result
		<< "\nnumber of data: "<< result->cobj()->num_data<<std::endl;
      begin_new_path();
      if(savedpath){
	append_path(*savedpath)  ;
      }
      return result;    
    } 
    
    void Drawingbase::pathOp_with_paintOp(void(Drawingbase::*pathOp)(const path_type&),
			     const  path_type& patharg,
			     void(Drawingbase::*paintOp)(void)){
      (this->*pathOp)(patharg);
      (this->*paintOp)();
    }

 void Drawingbase::really_clean_path(){
      saved_paths.push_back(simple_click_history());
      simple_click_history& dest = *(saved_paths.end()-1);
      //combine next 2 lines in one op!
      history->cleanup_path();
      history->copy_cleaned_path(dest);
      
      assign_history(&dest);
 
    }

void Drawingbase::write_pathfile_as_svg(const std::string& pathname){
      //beginnings of a projectfile format - could also use the format
      //projectfile=patternconf including any coordmap, paths from draw->saved_paths,

      //svgread.py uses directly, which might make that robuster
      Cairo::RefPtr<Cairo::SvgSurface> surface=Cairo::RefPtr<Cairo::SvgSurface>(NULL);
      Cairo::RefPtr<Cairo::Context> context=Cairo::RefPtr<Cairo::Context>(NULL);
      try{
	surface= Cairo::SvgSurface::create(pathname,1000,1000);
	context=Cairo::Context::create(surface);
	context->set_line_width(2);
	context->set_source_rgb(1,0,0);
      }
      catch(const std::exception& e){
	std::cerr<<"error attempting to save paths as svg:\n"<<e.what()
		 <<std::endl;//TODO:error windows to niotify user
	return;
      }
      for(std::deque<simple_click_history>::const_iterator w= saved_paths.begin();
	  w!= saved_paths.end(); w++){
	cairo_draw_clickhistory(context,*w);
	context->stroke();

      }
    }

void Drawingbase::set_source_offset_and_step(const coords& off,
					    size_type step, 
					    ColourRole role){
      colourmanager.pattern_set_offset(off, role);
      colourmanager.pattern_set_rotation_step(step, role);
    }

  std::string FileInfo::get_validtypes_string(){
    std::stringstream ostmp;
    ostmp <<"valid filetypes are:\nie...\nXNDRAW_PNG "
	  <<XNDRAW_PNG<<" XNDRAW_SVGRENDER = "<< XNDRAW_SVGRENDER
	  <<"\nXNDRAW_SVGPATH = "<<XNDRAW_SVGPATH
	  <<"\nXNDRAW_PATH = "<<XNDRAW_PATH
	  <<"\nXNDRAW_GRID = "<<XNDRAW_GRID
	  <<"\nXNDRAW_PROJECT = "<<XNDRAW_PROJECT;
  std::string err=ostmp.str();
  return err;
} 

std::vector<std::string> FileInfo::get_globstrings(xnFileType tp){
  std::vector<std::string> res;
  switch(tp){
  case XNDRAW_PNG:
    res.push_back("*.png");
    res.push_back("*.PNG");
    break;
  case XNDRAW_SVGPATH:
    res.push_back("*.svgxnd");//OR JUST '.svg'?
  case XNDRAW_SVGRENDER:
    res.push_back("*.svg");
    res.push_back("*.SVG");
    break;
  case XNDRAW_PATH:
    res.push_back("*.xnd");
    res.push_back("*.XND");
    break;
  case XNDRAW_GRID:
    res.push_back("*.nodes");
 res.push_back("*.NODES");
    break;
  case XNDRAW_PROJECT:
    throw xn_Error("XNDRAW_PROJECT: not done yet");
  default:
    std::stringstream ostmp;
    ostmp <<"ERROR: invalid filetype: "<<(int)tp
	  <<"\nmust be in xnFileTypes enum\n"<<get_validtypes_string();
    std::string err=ostmp.str();
    throw xn_Error(err.c_str());
  }
  return res;
}
std::string FileInfo::get_filetypename(xnFileType tp){
  std::string res, err;
  std::stringstream ostmp;//to report error
  switch(tp){
  case XNDRAW_PNG:
    res= "PNG files('*.(PNG|png)')";
    break;
  case XNDRAW_SVGRENDER:
    res= "SVG files('*.(svg|SVG)')";
    break;
  case XNDRAW_SVGPATH:
    res= "create xndraw paths from SVG files('*.(svgxnd|svg|SVG)')";
    break;
  case XNDRAW_PATH:
    res= "xndraw space separated values - a path ('*(xnd|XND)";
    break;

  case XNDRAW_GRID:
    res= "xndraw space separated values - a GRID ('*(nodes|NODES)";
    break;
case XNDRAW_PROJECT:
    res= "xndraw PROJECT FILE: not implemented probably space separated values for paths and grid, and whatever else is appropriate(ie current pattern and its params etc) in similar simple text format";
    ostmp <<"FIXME: => "<<res;
err=ostmp.str();
    throw xn_Error(err);
    break;
  default:
    
    ostmp <<"ERROR: invalid filetype: "<<(int)tp
	  <<"\nmust be in xnFileTypes enum\n"<<get_validtypes_string();
    err=ostmp.str();
    throw xn_Error(err.c_str());
  }
  return res;
}


std::string FileInfo::get_mimetype(xnFileType tp){
  std::string res, err;
  std::ostringstream ostmp;
  switch(tp){
  case XNDRAW_PNG:
    res= "image/png";
    break;
  case XNDRAW_SVGRENDER:
  case XNDRAW_SVGPATH:
    res= "image/svg+xml";
    break;
  case XNDRAW_PATH:
    res= "application/xndraw.path";
    break;
  case XNDRAW_GRID:
    res= "application/xndraw.grid";
  break;
  case XNDRAW_PROJECT:
    res= "application/xndraw.project";
  default:
    
    ostmp <<"ERROR: invalid filetype: "<<(int)tp
	  <<"\nmust be in xnFileTypes enum\n"<<get_validtypes_string();
    err=ostmp.str();
    throw xn_Error(err.c_str());
  }
  return res;
}

std::string FileInfo::get_extension(xnFileType tp){
  std::string res;
  switch(tp){
  case XNDRAW_PNG:
    res= ".png";
    break;
  case XNDRAW_SVGRENDER:
    res= ".svg";
    break;
  case XNDRAW_SVGPATH:
    res= ".svgxnd";//OR JUST '.svg'?
    break;
  case XNDRAW_PATH:
    res= ".xnd";
    break;
  case XNDRAW_GRID:
    res= ".nodes";
    break;
  case XNDRAW_PROJECT:
    res= ".xnp";
    break;
  default:
    std::stringstream ostmp;
    ostmp <<"ERROR: invalid filetype: "<<(int)tp
	  <<"\nmust be in xnFileTypes enum\n"<<get_validtypes_string();
    std::string err=ostmp.str();
    throw xn_Error(err.c_str());
  }
  return res;
}

cairo_rectangle_t coords_create_cairo_rect(const coords& a, 
						   const coords& b){
  cRect r(a, b);
  cairo_rectangle_t result;
  result.x= r.topL.x;
  result.y= r.topL.y;
  result.width= r.width();
  result.height = r.height();
  return result;
}

void Drawingbase::get_extents(cairo_rectangle_t& drawrect, 
			      const coords* offset){
  cRect tmp;
  history->get_extents(tmp);
  if(offset){
    tmp += *offset;
  }
  drawrect= cRect_to_cairo_rect(tmp);
}

void Drawingbase::get_extents_cRect(cRect& drawrect,
				    const coords* offset){
  cRect tmp;
  history->get_extents(drawrect);
  if(offset){
    drawrect += *offset;
  }
     
}
  bool Drawingbase::draggable_selection(const coords& location, double tolerance)const{
      if (!history){
	throw xn_Error("path NULL");
      }
      return history->draggable_selection(location, tolerance);
    }
//actual event might be more useful than 'maintain_selection' switch
void Drawingbase::select_pathcoords(const SelectBox& b, bool maintain_selection ){
  cRect the_box(b.first_corner, b.last_corner);
  for(iterator w = history->begin(); w != history->end(); w++){
    if((*w).first.is_in_box(the_box)){
#ifdef DEBUG
      std::cerr << "coords ("<<(*w).first<< ") is_in_box()"<<std::endl;
#endif
      if((*w).second &  CLICK_SELECTED){
#if DEBUG_SELECTION
	std::cerr <<"TODO: selection policy - SELECTION_POLICY_OR or SELECTION_POLICY_XOR(but with shift click"<<std::endl;
#endif
      }
     
      (*w).second |= CLICK_SELECTED;
    }
    else if(!maintain_selection){
      if((*w).second &  CLICK_SELECTED){
	std::cerr<<"deselecting point: "<<(*w).first<<std::endl;
	(*w).second ^= CLICK_SELECTED;
      }
    }
#ifdef DEBUG
    if((*w).second &  CLICK_SELECTED){
      std::cerr << "selected => "; 
    }
    else{std::cerr <<  "not selected=> ";} 
    std::cerr <<(*w).first.x<< " " << (*w).first.y<<std::endl;
#endif
  }
}

Drawingbase::Drawingbase (cairo_t* cr, cairo_surface_t* bakbuffer):
  Cairo::Context(cairo_create(bakbuffer), false), 
  visual(cr),
  bufferdata(XNDRAW_GET_PREF("IMAGEBUFFER_WIDTH", 0u),
	 XNDRAW_GET_PREF("IMAGEBUFFER_HEIGHT",0u), 
	     CAIRO_FORMAT_ARGB32),


  buffer(cairo_create(bufferdata.get_surface())),
  scratch(m_cobject),
  serial(++serialcount),
  drawto_now(DRAW_TO_BACKBUFFER),
  pathindex(0),
  history(NULL),
  last3clicks(click_type(NCOORDS::coords(), XNDRAW_NO_CLICKDATA), 
	      click_type(NCOORDS::coords(), XNDRAW_NO_CLICKDATA), 
	      click_type(NCOORDS::coords(), XNDRAW_NO_CLICKDATA)){
  initialise_path();
 
#ifdef DEBUG
  check_status();
#endif //DEBUG
}

  Drawingbase::Drawingbase (cairo_t* cr, cairo_surface_t* bakbuffer, const simple_click_history& clix):
    Cairo::Context(cairo_create(bakbuffer), false), 
    visual(cr),
     bufferdata(XNDRAW_GET_PREF("IMAGEBUFFER_WIDTH", 0u),
	       XNDRAW_GET_PREF("IMAGEBUFFER_HEIGHT",0u), 
	       CAIRO_FORMAT_ARGB32),
    buffer(cairo_create(bufferdata.get_surface())),
    scratch(m_cobject),
    serial(++serialcount),
    drawto_now(DRAW_TO_BACKBUFFER),
    pathindex(0),
    history(NULL)
  {
    assign_history(initialise_path(clix));
    //signal_savedpaths_changed().emit();
#ifdef DEBUG
  check_status();
#endif //DEBUG
}


    
Drawingbase::Drawingbase (Cairo::RefPtr<Cairo::Surface>& srf, cairo_surface_t* bakbuffer):
  Cairo::Context(cairo_create(bakbuffer), false), 
  visual(cairo_create(srf->cobj())),
    bufferdata(XNDRAW_GET_PREF("IMAGEBUFFER_WIDTH", 0u),
	       XNDRAW_GET_PREF("IMAGEBUFFER_HEIGHT",0u), 
	       CAIRO_FORMAT_ARGB32),
    buffer(cairo_create(bufferdata.get_surface())),
    scratch(m_cobject),
  serial(++serialcount),
  drawto_now(DRAW_TO_BACKBUFFER),
  pathindex(0),
  history(NULL)
{
  initialise_path();
#ifdef DEBUG
  check_status();
#endif //DEBUG
}
//hope this doesn't get called
Drawingbase::Drawingbase(const Drawingbase& src):
  Cairo::Context(src),
  visual(src.visual),
bufferdata(XNDRAW_GET_PREF("IMAGEBUFFER_WIDTH", 0u),
	       XNDRAW_GET_PREF("IMAGEBUFFER_HEIGHT",0u), 
	       CAIRO_FORMAT_ARGB32),
    buffer(cairo_create(bufferdata.get_surface())),
    scratch(m_cobject),
  serial(++serialcount), 
  drawto_now(  src.drawto_now),
  saved_paths(src.saved_paths)
{
  if (saved_paths.size()){
    assign_history(&(*(saved_paths.end()-1)));
  }
  else{initialise_path();}
#ifdef DEBUG
  check_status();
#endif //DEBUG
}

bool xndraw_has_svgcairo(){
#if CAIRO_HAS_SVG_SURFACE
  return true;
#else
  return false;
#endif
}

 void Drawingbase::swap_paths(std::deque<simple_click_history>& swappaths){
      saved_paths.swap(swappaths);
      if(!saved_paths.size()){
	saved_paths.push_back(simple_click_history());
      }
      assign_history (&(*(saved_paths.begin())));
    }

 cairo_path_t*  Drawingbase::set_clip_bounds(cairo_t* cr,
			 double x, double y, double width, double height){
      cairo_path_t* savedpath= cairo_copy_path(cr);
      cairo_move_to(cr, x, y);
cairo_line_to(cr, x + width, y);
cairo_line_to(cr, x + width, y + height);
 cairo_line_to(cr, x,  y + height);
 cairo_close_path(cr);
 cairo_clip(cr);
 return savedpath;
    }
   

NCOORDS::simple_click_history* Drawingbase::initialise_path(){
  saved_paths.push_back(simple_click_history());
  assign_history(&(*(saved_paths.end()-1)));
  return history;
}

NCOORDS::simple_click_history* Drawingbase::initialise_path(const simple_click_history& path){
  saved_paths.push_back(path);
  assign_history(&(*(saved_paths.end()-1)));
  return history;
}

void Drawingbase::list_paths(){
  for(int i=0;i< saved_paths.size();i++){
    std::cerr<<"path no: "<< i << " :\n"<< saved_paths[i]<<"\nsize"<< saved_paths[i].size()<<std::endl;
  }
} 

Cairo::RefPtr<Cairo::Pattern> Drawingbase::get_pattern(const std::string& name){
  return colourmanager.get_mapped_pattern(name);
}

void Drawingbase::add_pattern(const std::string& nm,
			      Cairo::RefPtr<Cairo::Pattern> pattern){
  colourmanager.map_pattern(nm, pattern);
}
void Drawingbase::set_path_by_index(unsigned int index){
  unsigned int sz = saved_paths.size();
  if(index >=sz){
    std::ostringstream ostmp;
    ostmp << "path index '"<< index<<"' out of range (0 - "<<sz-1<<")";
    throw xn_Error(ostmp.str());
  }
  assign_history(&(saved_paths[pathindex]));
  draw_to(*history);
}

void Drawingbase::assign_history(NCOORDS::simple_click_history *newhistory){
  int newindex= index_of_path(newhistory);
  if(newindex <0){
    throw xn_Error("asked to set history to a path not in saved_paths");
  }
  history= newhistory;
  pathindex=newindex;
signal_savedpaths_changed().emit();
}

bool Drawingbase::set_path_rel_index(int change){
  unsigned int sz = saved_paths.size();
  if(sz>1){
    int newindex=(pathindex + change) % sz;
    if(pathindex !=newindex){
      assign_history(&(saved_paths[newindex]));
      draw_to(*history);
      return true;
    }
    else{return false;}
  }
  else{
    //pathindex=0;
    return false;
  }
}

void Drawingbase::on_import_paths(const std::string& fname){
  std::ifstream infile(fname.c_str());
  std::deque<NCOORDS::simple_click_history> temp_saved_paths;
  typedef std::deque<NCOORDS::simple_click_history>::const_iterator const_paths_iterator;
  infile >> temp_saved_paths;

  if(! temp_saved_paths.size()){
    std::cerr<<"no paths successfully imported"<<std::endl;
  }
  else{
    unsigned int pathcount=saved_paths.size();
  for(const_paths_iterator w = temp_saved_paths.begin();w!=temp_saved_paths.end();w++){
    saved_paths.push_back(*w);
  }
  list_paths();
  //set history to first path of those imported
  set_path_by_index(pathcount);
  }
}

void Drawingbase::adjust_stroke_alpha(double alf){
  colourmanager.set_alpha(alf, COLOURROLE_STROKE);
} 

const NCOORDS::simple_click_history& Drawingbase::get_saved_path(size_t index)const{
  if(index < saved_paths.size()){
    return saved_paths[index];
  }
  throw XNPathError("path index out of range");
}

NCOORDS::simple_click_history& Drawingbase::get_saved_path(size_t index){
  if(index < saved_paths.size()){
    return saved_paths[index];
  }
  throw XNPathError("path index out of range");
}

std::deque<NCOORDS::simple_click_history*> Drawingbase::get_saved_paths(){
  typedef std::deque<simple_click_history>::iterator dek_iterator;
  std::deque<simple_click_history*> res;
  for(dek_iterator w = saved_paths.begin(); w != saved_paths.end();w++){
    res.push_back(&(*w));
  }
  return res;
}  

void Drawingbase::set_stroke_colour(double& red,double& green, double& blue, double& alpha){
  Draw_colour col(red, green, blue, alpha);
  colourmanager.set_colour(col, COLOURROLE_STROKE);
}

void Drawingbase::set_fill_colour(double& red,double& green, double& blue){
  PatternSourceInfo info= colourmanager.get_info(COLOURROLE_FILL);
  if(info == PATTERN_SOURCE_RGBA){
    Draw_colour& d = *(colourmanager.get_topcolour(COLOURROLE_FILL));
    d.set_red(red);
    d.set_green(green);
    d.set_blue(blue);
  }
  else{
    double alpha =colourmanager.get_default_alpha(COLOURROLE_FILL);
    Draw_colour newfill(red, green, blue, alpha);
    colourmanager.set_colour(newfill, COLOURROLE_FILL);
  }
}

void Drawingbase::set_fill_colour(double& red,double& green, double& blue, double& a){
  Draw_colour fill_c(red, green, blue, a);
  colourmanager.set_colour(fill_c, COLOURROLE_FILL);
}

void Drawingbase::set_stroke_colour(double& red,double& green, double& blue){
  PatternSourceInfo info= colourmanager.get_info(COLOURROLE_STROKE);
  if(info == PATTERN_SOURCE_RGBA){
    Draw_colour& d = *(colourmanager.get_topcolour(COLOURROLE_STROKE));
  
    d.set_red(red);
    d.set_green(green);
    d.set_blue(blue);
  }
  else{
    double alpha =colourmanager.get_default_alpha(COLOURROLE_STROKE);
    Draw_colour newfill(red, green, blue, alpha);
    colourmanager.set_colour(newfill, COLOURROLE_STROKE);
  }
  //  set_source_rgba(red,green,blue,stroke_colour.get_alpha());
}

void Drawingbase::set_fill_colour(const Draw_colour& back){
  colourmanager.set_colour(back, COLOURROLE_FILL);
}

void Drawingbase::set_stroke_colour(const Draw_colour& back){
  colourmanager.set_colour(back, COLOURROLE_STROKE);
  // set_source_rgba(back.r, back.g,back.b,back.a);
}

void Drawingbase::set_source_to_fill(){
  Cairo::RefPtr<Cairo::Pattern> source= colourmanager.get_source(COLOURROLE_FILL);
  set_source(source);
}

void Drawingbase::set_source_to_stroke(){
  Cairo::RefPtr<Cairo::Pattern> source= colourmanager.get_source(COLOURROLE_STROKE);
  set_source(source);
}

void Drawingbase::cleanup_history(){
  history->cleanup_path();
}
void Drawingbase::cleanup_histories(){
  typedef std::deque<simple_click_history>::iterator path_iterator;
  for(path_iterator w = saved_paths.begin();w!=  saved_paths.end();w++){
    (*w).cleanup_path();
  }
} 


std::ostream& operator<< (std::ostream& os , const Drawingbase& draw){
  os << "Drawingbase " << draw.serial <<" at " << &draw << "\n";
  os << "visual : " << draw.visual<< "\n";
  os << "buffer : "<< draw.buffer<< "\n";
  os <<"m_cobject: "<< draw.cobj()<< "\n";
  os << "scratch: " << draw.scratch << std::endl;
  return os;
}

unsigned int	Drawingbase::get_serial_no()const {
  return serial;
}
void Drawingbase::save(){
#ifdef DEBUG
  check_status();
#endif //DEBUG
  cairo_save(visual);
  cairo_save(buffer);
  cairo_save(scratch);
}

void Drawingbase::check_status(){
  XN_CHECK_STATUS(m_cobject);
  XN_CHECK_STATUS(visual);
  XN_CHECK_STATUS(buffer);
}

void Drawingbase::restore(){
  cairo_restore(visual);
  cairo_restore(buffer);
 cairo_restore(scratch);
}
void Drawingbase::redraw_line(bool paint_to_bg){
  if(paint_to_bg){
    paint_background(true);
  }
  begin_new_path();
  draw_to(*history);
}



void Drawingbase::write_svg_output_buffer(cairo_surface_t* outbuffer, 
					SvgSaveFlags flags ){
#if CAIRO_HAS_SVG_SURFACE
  std::cerr <<"creating svg surface"<<std::endl;
  typedef Cairo::RefPtr<Cairo::SvgSurface> surface_type;
  typedef Cairo::RefPtr<Cairo::Context> context_type;
  surface_type surf = surface_type(new Cairo::SvgSurface(outbuffer));
  context_type cr= Cairo::Context::create(surf);
  //no real reason to expose the c type here, except for 'buffer'
 
  if(cr){
    std::cerr<<"context created at"<<cr->cobj()<<std::endl;
    cairo_t* svgoutput = cr->cobj();
    
    try{
      write_svg(svgoutput, flags);
    }
    catch(const std::exception& e){
      std::cerr<<"svg write failed:what():\n "<<e.what()<<std::endl;
    }
  }
#else
  std::cerr <<"compiled with no svg surface available"<<std::endl;
#endif
}

void Drawingbase::stroke_with_pattern(std::string pattername ){
  save();
  Cairo::RefPtr<Cairo::Pattern> pattern = colourmanager.get_mapped_pattern( pattername );
  if(!pattern){
    std::ostringstream ostmp;
    ostmp <<"no such pattern: '"<< pattername<<"'";
    throw xn_Error(ostmp.str());
  }
  set_source(pattern);
  stroke();
  restore();
}

void Drawingbase::add_png_pattern(const std::string& fname){
  //typedef Cairo::ImageSurface surface_type;
  typedef Cairo::RefPtr<Cairo::ImageSurface>  surface_type;
  typedef Cairo::RefPtr<Cairo::SurfacePattern> pattern_type;
  surface_type surface = Cairo::ImageSurface::create_from_png(fname);
  pattern_type pattern =Cairo::SurfacePattern::create(surface);
  colourmanager.map_pattern(fname, pattern);
}
//misnomer -writepngbufferTosvg() descibes it better
void Drawingbase::write_svg_output_buffer(const std::string& fname,
					int width, int height, 
					SvgSaveFlags flags){
  /**
   @param fname => filename to write svg output to
@param width => image width
@param height =>image height
@param flags
  */
#if CAIRO_HAS_SVG_SURFACE
  //check filename and extension
  std::cerr <<"creating svg surface v2"<<std::endl;
  typedef Cairo::RefPtr<Cairo::SvgSurface> surface_type;
  typedef Cairo::RefPtr<Cairo::Context> context_type;
  surface_type surf = Cairo::SvgSurface::create(fname, width,  height);
  context_type cr= Cairo::Context::create(surf);
 
  if(cr){
    std::cerr<<"contect sxxeckfully created"<<std::endl;
    cairo_t* svgoutput = cr->cobj();
    set_clip_bounds( svgoutput, 0, 0, width, height);
    std::cerr <<"entering write_svg"<<std::endl;
    try{
      write_svg(svgoutput, flags);
    }
    catch(const std::exception& e){
      std::cerr<<"svg write failed:what():\n "<<e.what()<<std::endl;
    }
  }
#else
  std::cerr <<"no svg surface available"<<std::endl;
#endif
}


void Drawingbase::write_svg(cairo_t* svgoutput, 
			    SvgSaveFlags flags){
#if CAIRO_HAS_SVG_SURFACE
  cairo_pattern_t* pattern  =cairo_pattern_create_for_surface (cairo_get_target(buffer));
 
  if(flags & XNDRAW_SVG_WRITE_PNG){
    std::cerr<<"flags & XNDRAW_SVG_WRITE_PNG"<<std::endl;
    cairo_save(svgoutput);
    std::cerr<< "svgoutput status(pre set_source_pattern: ";
    if(do_check_status(svgoutput)== CAIRO_STATUS_SUCCESS){
      std::cerr<< "good\n";
    }
    cairo_set_source(svgoutput,pattern);
    cairo_paint(svgoutput);
      
    cairo_status_t svgstatus= do_check_status(svgoutput);
    std::cerr<< "svgoutput status(post paint png to svg): "
	     << (svgstatus==CAIRO_STATUS_SUCCESS ? "good" : "bad") 
	     <<std::endl;
    if(do_check_status(svgoutput)!= CAIRO_STATUS_SUCCESS){
    
      throw CairoError("error painting svg surface with pattern",svgstatus );
    }
    cairo_restore(svgoutput);
    std::cerr<<"wrote png data layer"<<std::endl;
  }
  if(flags & XNDRAW_SVG_WRITE_PATH){
    std::cerr<<"XNDRAW_SVG_WRITE_PATH"<<std::endl;
  
    cairo_save(svgoutput);
    Cairo::RefPtr<Cairo::Pattern> pattern = colourmanager.get_source(COLOURROLE_STROKE);
    cairo_set_source(svgoutput, pattern->cobj()); 
    double lw = get_line_width();
    if (lw > 6){lw=5.;}
    Cairo::Path* path= get_iterated_path();
    std::cerr <<"get_iterated_path() => path : "<<path
	      <<"cobj() => "<< path->cobj()<<std::endl;
    if(path && path->cobj()){
      std::cerr<< "svgoutput status(pre append path: ";
      if(do_check_status(svgoutput)== CAIRO_STATUS_SUCCESS){
	std::cerr<< "good\n";
      }
      cairo_append_path(svgoutput, path->cobj());
      std::cerr<< "svgoutput status(post cairo_append path: ";
      if(do_check_status(svgoutput)== CAIRO_STATUS_SUCCESS){
	std::cerr<< "good\n";
      }
    
      cairo_set_line_width(svgoutput,lw);
      //throw std::runtime_error("do it now: XNDRAW_SVG_WRITE_PATH)");
      cairo_stroke(svgoutput);
      std::cerr<< "svgoutput status(post stroke() ";
      if(do_check_status(svgoutput)== CAIRO_STATUS_SUCCESS){
	std::cerr<< "good\n";
      }
    }
    else{
      std::cerr <<" get_iterated_path() returned NULL"<<std::endl;
    }
    cairo_restore(svgoutput);
  }
#endif
}
void Drawingbase::paint_background(bool paint_buffer){
  save();
  Cairo::RefPtr<Cairo::Pattern> bg_pattern = colourmanager.get_source(COLOURROLE_BG);
  cairo_set_source(scratch, bg_pattern->cobj());
  cairo_paint(scratch);
  paintmode mode = drawto_now;
  if(paint_buffer){   drawto_now = (paintmode)(PAINT_VISUAL|PAINT_BUFFER);  }
  else{  drawto_now = PAINT_VISUAL; }
  paint();
  drawto_now = mode;
  restore();
}
void Drawingbase::paint_white(bool paint_buffer){
  paint_background( paint_buffer);
} 

void Drawingbase::draw_editmarks(){
  this->draw_editable_path();
  //guidraw.draw_grid();
}

void Drawingbase::draw_editable_path(){  
  cairo_save(visual);
  guidraw.draw_editcoords_path(visual,* history);
  guidraw.draw_editline(visual, *history);
  cairo_restore(visual);
}

void set_source_and_do_paint_cmd(cairo_t* dest, cairo_t* src, 
				   void(*fn)(cairo_t*)){
  /**
@fn set_source_and_do_paint_cmd(cairo_t* dest, cairo_t* src, 
				   void(*fn)(cairo_t*));
@param theG?hg,.xdj, dest =>destination
@param src => to use as src of pattern
@param fn => one of &cairo_paint,  &cairo_fill, &cairo_fill_preserve, &cairo_stroke, &cairo_stroke_preserve
   */

  //typedef void(*function_type)(cairo_t*);
 if(!dest || !src){
    throw xn_Error("either src or dest NULL");
  }
  cairo_pattern_t* pattern= cairo_pattern_create_for_surface(cairo_get_target(src));
#ifdef DEBUG
    cairo_status_t status = cairo_pattern_status(pattern);
    if(status != CAIRO_STATUS_SUCCESS){
      throw CairoError("PATTERN CREATED WITH ERROR", status);
    }
    check_status("dest after pattern set", dest);
    std::cout <<"painting dest to src" << std::endl;
#endif
  cairo_save(dest);
  cairo_set_source(dest,  pattern);
  (*fn)(dest);
cairo_pattern_destroy(pattern);
  cairo_restore(dest);
  }

void set_source_and_paint(cairo_t* dest, cairo_t* src){
  set_source_and_do_paint_cmd(dest, src, &cairo_paint);
}

void Drawingbase::paint(){
  check_status();
#ifdef DEBUG_VERBOSE
  std::cout <<"paint()  drawto_now = " ;
  if(drawto_now &PAINT_VISUAL){std::cout <<"PAINT_VISUAL,\t";}
  if(drawto_now & PAINT_BUFFER){ std::cout <<"PAINT_BUFFER,\t";}
  if(drawto_now & DRAW_TO_BACKBUFFER){ std::cout <<"DRAW_TO_BACKBUFFER,\t";}
  std::cout     << "\npaintmode = "<<(int)drawto_now << std::endl;
#endif
  paintmode oldmode= drawto_now;
  if(drawto_now &(PAINT_VISUAL|PAINT_BUFFER)){}
  else if(drawto_now& DRAW_TO_BACKBUFFER){
    drawto_now = (paintmode)(PAINT_VISUAL|PAINT_BUFFER);
  }
  else if (drawto_now& DRAW_DIRECT_TO_WINDOW){drawto_now=PAINTMODE_NONE;}
  else{drawto_now = PAINT_VISUAL;}
  if (drawto_now & PAINT_BUFFER){
    set_source_and_paint(buffer, scratch);
  }
  if(drawto_now & PAINT_VISUAL){
    set_source_and_paint(visual, buffer);
  }
  drawto_now = oldmode;
  this->draw_editmarks();
}

void Drawingbase::paint_rect(const cairo_rectangle_t& area){ //on_expose_event handler
  if(buffer){
    cairo_save(visual);
    //cairo_path_t* curpath = copy_path();
    cairo_new_path(visual);
    cairo_move_to(visual, area.x, area.y);
    cairo_line_to(visual,area.x + area.width,  area.y);
    cairo_line_to(visual,area.x + area.width,  area.y + area.height);
    cairo_line_to(visual,area.x ,  area.y + area.height);
    cairo_close_path(visual);
    set_source_and_do_paint_cmd(visual,buffer, &cairo_fill);
  }
}

void Drawingbase::write_buffer_to_png(const char* filename){
  if(buffer){
    cairo_status_t result = cairo_surface_write_to_png(cairo_get_target(buffer), filename);
    if (result != CAIRO_STATUS_SUCCESS){
      throw CairoError("error writing buffer to file", result);
    }
  }
}

int Drawingbase::on_edit_visual_direct(){
  paintmode oldmode= drawto_now;
  if ((drawto_now & DRAW_TO_BACKBUFFER) && buffer){
    drawto_now ^= DRAW_TO_BACKBUFFER;
    drawto_now |= DRAW_DIRECT_TO_WINDOW;
  }
  m_cobject = visual;
  return oldmode;
}
void Drawingbase::set_source_with_transform(SourceTransformInfo info,
					   ColourRole role){
      coords zero(0,0),sourceextent(1000,1000);
      cRect sourcearea(zero, sourceextent);
      PatternColour& pattern =colourmanager.get_patterncolour(role);
      PatternSourceInfo patterninfo =pattern.get_info();
	if (patterninfo != PATTERN_SOURCE_RGBA){
	  std::cerr <<"set_source_with_transform"<<std::endl;
	  //Cairo::RefPtr<Cairo::Context> m_context(m_cobject);
	  pattern.set_source_with_transform(m_cobject,sourcearea, info);
	  //colourmanager.pattern_set_transform(info, role);
	}
	else{
	  Cairo::RefPtr<Cairo::Pattern> source=colourmanager.get_source(role);
	  set_source(source);
	}
      }

    void Drawingbase::stroke(){
      save();
      set_source_to_stroke();
      Cairo::Context::stroke();
      restore();
    }

void Drawingbase::stroke_preserve(){
  save();
      set_source_to_stroke();
      Cairo::Context::stroke_preserve();
      restore();
}

void Drawingbase::fill_preserve(){
save();
      set_source_to_fill();
      Cairo::Context::fill_preserve();
      restore();
}
    
    void Drawingbase::fill(){
save();
      set_source_to_fill();
      Cairo::Context::fill();
      restore();
}

    cairo_status_t Drawingbase::do_check_status(cairo_t* cr_status_of){
      if(!cr_status_of){
	cr_status_of = m_cobject;
      }
      cairo_status_t stats = cairo_status(cr_status_of);
      if(stats != CAIRO_STATUS_SUCCESS){	
	std::cerr <<"bad status for cairo_t at "<<cr_status_of
		  <<cairo_status_to_string(stats)<<std::endl;
      }
      return stats;
    }

void Drawingbase::set_history_by_id(const std::string& pathid){
      for(std::deque<simple_click_history>::iterator w = saved_paths.begin(); w != saved_paths.end(); w++){
	if(pathid == (*w).get_pathid()){
	  assign_history(&(*w));
	  return;
	}
	else{
	  std::cerr<<"WARNING: invalid pathid: '"<< pathid<<"'"<<std::endl;
	}
      }
    }

void Drawingbase::on_complete_visual_direct(paintmode newmode){
  if (newmode & DRAW_DIRECT_TO_WINDOW){ }
  else if(newmode & DRAW_TO_BACKBUFFER){
    drawto_now = newmode;
  }
  else{
    throw xn_Error("unknown mode");
  }
  m_cobject= scratch;
}

void Drawingbase::draw_to(simple_click_history& clixhistory){
  typedef simple_click_history::const_iterator const_iterator;
  typedef simple_click_history::iterator iterator;
  for(iterator iter = clixhistory.begin();iter != clixhistory.end();iter++){
    do_draw_to(*iter);
  }
}

void Drawingbase::draw_to(std::pair<coords, int>& location){
  if(!history->size()){
    if (location.second != MOVE_CLICK){
#ifdef DEBUG
      std::cerr <<" first click not a moveto" << location.first <<std::endl;
#endif
      location.second = MOVE_CLICK;
    }
  }
  paintmode oldmode= PAINTMODE_NONE;
  if(drawto_now & DRAWING_FROM_HISTORY){
    oldmode = DRAWING_FROM_HISTORY;
#ifdef DEBUG
    std::cerr << "DRAWING_FROM_HISTORY set while drawing direct from clicks, this may become an error" << std::endl;
#endif
    drawto_now ^= DRAWING_FROM_HISTORY;
  }
  drawto_now |= DRAWING_DIRECT;
  history->load(location);
  do_draw_to(location);
  drawto_now  ^= DRAWING_DIRECT;
  drawto_now |= oldmode;
}

void Drawingbase::do_draw_to(std::pair<coords, int>& location){
  
#ifdef DEBUG_BEZIER
  std::cout << "Drawingbase::draw_to(std::pair<coords*, int>) called" << std::endl;
  std::cerr << "point at "<<location.first.x<<", " << location.first.y
	    << "\npoint type : "<< NCOORDS::clicktype_as_string(location.second )<<std::endl;
#endif
  coords click,  handle1, handle2;
  //see if were waiting to handle a bez curve
  if(location.second & BEZIER_HANDLE){
    if(last3clicks.first.second & BEZIER_CLICK){
      //await another handle
#ifdef DEBUG_BEZIER
      std::cerr <<"first handle, return to await another"<<std::endl;
#endif
      set_last_drawclick(location);
      return;
    }
    else if(last3clicks.second.second & BEZIER_CLICK){
      //handle bezier curve here
      //this is how it should work - 2 handles, supplied after, (even though
      //they refer to 'before' the point)
#ifdef DEBUG_BEZIER
      std::cerr <<"who-hoo 'correct' behaviour for the bezier curve... maybe"<<std::endl;
#endif
      handle1 = last3clicks.first.first;
    click= last3clicks.second.first;
    curve_to(&handle1, &(location.first), &click);
    set_last_drawclick(location);
    return;
    }
    else{
      throw xn_Error("handle not preceded by click");
    }
  }
  else{//not a handle
    if (location.second & BEZIER_HANDLE){
      throw xn_Error("bezier handle - shouldn't get here");
    }
    //see if we have a pending bezier curve
    //ie if a 'BEZIER_CLICK' with either one or zero handles is sitting in cache
    //handle? or error?
    //if handle , then go on to handle next click
    if((last3clicks.first.second & BEZIER_CLICK)||(last3clicks.second.second & BEZIER_CLICK)){
      if((last3clicks.first.second & BEZIER_CLICK)){
	//no need too curve - draw a line
	click=last3clicks.first.first;
#ifdef DEBUG_BEZIER
	std::cerr<<"WARNING:bezier click without handles:THIS MAY BECOME AN ERROR"<<std::endl;
#endif
	line_to(&click);
      }
      else if(last3clicks.first.second & BEZIER_HANDLE){
	//one handle
	//assume it the incoming handle for the click
	//use the previous point as outgoing handle
	//(ie retracted into its click)
#ifdef DEBUG_BEZIER
	std::cerr<<"WARNING:bezier click with one handle\nassuming its the incoming(second) handle, not the outgoing handle of the previous point\ndon't rely on this:THIS MAY BECOME AN ERROR, or be handled differebtly in future"<<std::endl;
#endif
	click =last3clicks.second.first;
	handle1=last3clicks.third.first;
	handle2=last3clicks.first.first;
	curve_to(&handle1,&handle2, &click);
      }
      else{
	throw xn_Error("shouldn't get here");
      }
    }
    //we've handled any pending curve_to()
    //on to the current point(which isn't a BEZIER_HANDLE)
  
    if(location.second & MOVE_CLICK){
      move_to(&(location.first));
    }
    else if (location.second & LINE_CLICK){
      line_to(&(location.first));
    }
    else if (location.second & CLOSE_PATH){
      close_path();
    }
    else if (location.second & BEZIER_CLICK){
      //waiting_to_curve=true;
      //waiting for godot
      //HANDLED LATER when all the handles have arrived
    }
    
    else if(location.second & OFFSET_CLICK || location.second & ORIGIN_CLICK){
      std::cerr << "Drawing::do_draw_to(): location.second & OFFSET_CLICK || location.second & ORIGIN_CLICK\nCurrently does nothing - should it do somrthing?";
    }
    else{
      //default:
      std::ostringstream s;
      s << "Drawingbase::draw_to...Option currently unhandled : " << location.second << "\nLook it up in defines.h among the clicks and then fix...";
      throw xn_Error(s.str());
    }
    set_last_drawclick(location);
  }
}

void Drawingbase::set_last_drawclick(const std::pair<NCOORDS::coords, int>& click){
  last3clicks.third=last3clicks.second;
  last3clicks.second = last3clicks.first;
  last3clicks.first = click;
}


void  Drawingbase::begin_new_path(){clear_cairopath();}
void  Drawingbase::clear_cairopath(){
  cairo_new_path(scratch);
  cairo_new_path(visual);
  cairo_new_path(buffer);
}

void Drawingbase::line_to(double x, double y){
  if((drawto_now & DRAW_TO_BACKBUFFER) && buffer){
    cairo_line_to(m_cobject, x, y);
  }
  else if((drawto_now & DRAW_DIRECT_TO_WINDOW) || !buffer ){
    cairo_line_to(visual, x, y);
  }
  else{
    throw xn_Error("drawing not directed to a cairo_t");
  }
}

void Drawingbase::move_to(double x, double y){
  if((drawto_now & DRAW_TO_BACKBUFFER) && buffer){
    cairo_move_to(m_cobject, x, y);
  }
  else if((drawto_now & DRAW_DIRECT_TO_WINDOW) || !buffer ){
    cairo_move_to(visual, x, y);
  }
  else{
    throw xn_Error("drawing not directed to a cairo_t");
  }
}

void Drawingbase::curve_to(double x1, double y1, double x2, double y2,double x3, double y3){
  if((drawto_now & DRAW_TO_BACKBUFFER) && buffer){
    cairo_curve_to(m_cobject, x1, y1, x2, y2, x3, y3);
  }
  else if((drawto_now & DRAW_DIRECT_TO_WINDOW) || !buffer){
    cairo_curve_to(visual, x1, y1, x2, y2, x3, y3);
  }
  else{
    throw xn_Error("drawing not directed to a cairo_t");
  }
}

void Drawingbase::close_path(){
  if((drawto_now & DRAW_TO_BACKBUFFER) && buffer){
    cairo_close_path(m_cobject);
  }
  else if((drawto_now & DRAW_DIRECT_TO_WINDOW) || !buffer){
    Cairo::Context::close_path();
  }
  else{
    throw xn_Error("drawing not directed to a cairo_t");
  }
}

void Drawingbase::move_to(coords* pt){
  move_to(pt->x, pt->y);
}

void Drawingbase::line_to(coords* pt){
  line_to(pt->x, pt->y);
}

NCOORDS::simple_click_history& Drawingbase::get_history(){
  return  *history;
}
    
void Drawingbase::clear_history(){
  saved_paths.push_back(simple_click_history());
  assign_history(&(*(saved_paths.end()-1)));
}

unsigned int Drawingbase::get_pathindex()const{
      if(pathindex<0){
	throw xn_Error("pathindex must be positive");
      }
      return (unsigned int)pathindex;
    }

simple_click_history&  Drawingbase::get_path_by_index(unsigned int index){
  if(index >= saved_paths.size()){
    std::ostringstream ostmp;
    ostmp <<"index out of range: "<< index<<"\nmax :"<< saved_paths.size()-1;
    throw xn_Error(ostmp.str());
  }
  return saved_paths[index];
}
const simple_click_history&  Drawingbase::get_path_by_index(unsigned int index)const{
  if(index >= saved_paths.size()){
    std::ostringstream ostmp;
    ostmp <<"index out of range: "<< index<<"\nmax :"<< saved_paths.size()-1;
    throw xn_Error(ostmp.str());
  }
  return saved_paths[index];
}    
NCOORDS::simple_click_history Drawingbase::copy_history(){
  return *history;
}

void Drawingbase::set_history(NCOORDS::simple_click_history* clicks){
  initialise_path(*clicks); 
}
void Drawingbase::do_draw_path(const ncoords& rotator){}

cairo_t* Drawingbase::draw_path_to_cairo(cairo_t* cr){
  cairo_draw_clickhistory(cr, *history);
  return cr;
}
void Drawingbase::do_cairo_function_with_source(cairo_pattern_t* source, const Cairo_c_operation& op,cairo_t* dest){
  if(!dest){
    dest = m_cobject;
  }
  cairo_save(dest);
  cairo_set_source(dest, source);
  op(dest);
  cairo_restore(dest);
}

void Drawingbase::stroke_with_source(cairo_pattern_t* source, cairo_t* dest){
  Cairo_c_operation op(&cairo_stroke);
  do_cairo_function_with_source(source, op, dest);
}

void Drawingbase::fill_with_source(cairo_pattern_t* source, cairo_t* dest){
  Cairo_c_operation op(&cairo_fill);
  do_cairo_function_with_source(source, op, dest);
}

void Drawingbase::do_draw_path(){
  cairo_path_t* base_path = XndrawPath::cairo_from_history(*history);
  cairo_draw_clickhistory(m_cobject,*history);
}

void Drawingbase::check_status(const char* comment, cairo_t* obj ){
  if(!obj){
    obj = m_cobject;
  }
  cairo_status_t status = cairo_status(obj);
  if (status != CAIRO_STATUS_SUCCESS){
    throw CairoError(comment, status);
  }
}

unsigned int Drawingbase::get_serialcount(){
  return serialcount;
}
void Drawingbase::curve_to(coords* handle1, coords* handle2, coords* pt){
  curve_to(handle1->x, handle1->y, handle2->x, handle2->y, pt->x, pt->y);
}

void  Drawingbase::check_type(std::ostream& stream, std::string comment){
  stream << comment << ": draw_object type = " << typeid(*this).name() << std::endl;
}

bool operator==(const Drawingbase& left, const Drawingbase& right){
  return (left.visual==right.visual && left.buffer == right.buffer);
}
bool operator< (const Drawingbase& left, const Drawingbase& right){
  if(left.visual!=right.visual){
    return (left.visual<right.visual);
  }
  return left.buffer< right.buffer;

}

bool operator>(const Drawingbase& left, const Drawingbase& right){
  if(left.visual!=right.visual){
    return (left.visual>right.visual);
  }
  return left.buffer> right.buffer;
}

bool operator!=(const Drawingbase& left, const Drawingbase& right){
return !(left == right);
}
  
bool operator<=(const Drawingbase& left, const Drawingbase& right){
  return !(left > right);
}
bool operator>=(const Drawingbase& left, const Drawingbase& right){
  return !(left < right);
}
 paintmode& operator|=(paintmode& left, const paintmode& right){
    int tmp(left | right);
    left = (paintmode)tmp;
    return left;
  }
 paintmode& operator&=(paintmode& left, const paintmode& right){
    int tmp(left & right);
    left = (paintmode)tmp;
    return left;
  }
paintmode& operator^=(paintmode& left, const paintmode& right){
    int tmp(left ^ right);
    left = (paintmode)tmp;
    return left;
}

}
