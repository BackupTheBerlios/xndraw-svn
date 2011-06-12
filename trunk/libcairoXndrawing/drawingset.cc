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



#include "drawingset.h"
#include <deque>
#include <iterator>



namespace xNDRAW{
using namespace NCOORDS;



NCOORDS::Nodedeq& Drawingset::reset_nodes(NCOORDS::Nodes& nodes){
  paint_white();
  return this->set(nodes);
}

NCOORDS::Nodedeq& Drawingset::set(NCOORDS::Nodes& generator){
  coords centre(generator.get_centre());
  coords radial(*(generator.get_radial()));
  unsigned int base = generator.get_base();

  map_generator.set_limits(centre, radial, base, generator.get_limit_x(),
			    generator.get_limit_y());
  coordmap = map_generator.get_nodes();
  return coordmap;
}
Drawingset::Drawingset(cairo_t* vis, cairo_surface_t* buf,const ncoords& pointbase,const Nodes& node_gen):
  Drawing(vis, buf, pointbase),
  map_generator(node_gen),
  coordmap(map_generator.get_nodes()),
  bg_renderer(NULL){
  set_points(); 
}
Drawingset::Drawingset(cairo_t* vis, cairo_surface_t* buf,
	       const ncoords& pointbase, 
	   unsigned int grid_limit_x, unsigned int grid_limit_y):
Drawing(vis, buf, pointbase),
map_generator(pointbase, grid_limit_x,grid_limit_y),
  coordmap(map_generator.get_nodes()),
  bg_renderer(NULL){
  set_points(); 
}

Drawingset::Drawingset( Drawing* drago,const Nodes& gen):
  Drawing(*drago),
  map_generator(gen),
    coordmap(map_generator.get_nodes()),
  bg_renderer(NULL){}

  Drawingset::~Drawingset(){}

void Drawingset::set_points(){
 if (coordmap.size() < 2){
  throw xn_Error("need at least 2 points in node list");
}
  ncoords fordrawing(*coordmap[0], *coordmap[1], point_data.get_base());
  point_data.set(fordrawing);
  next.set(fordrawing);
}
  
void   Drawingset::invert_nodestate(NCOORDS::Nodedeq& coordlist){
    for(Nodedeq::iterator walk = coordlist.begin();walk!=coordlist.end();walk++){
      coords* it = *walk;
      it->set_updownstate((it->get_state() +1) % 2);
    }
  }

/*
void Drawingset::draw_to(click_pair_type click){ 
  if(buffer && (drawto_now & DRAW_TO_BACKBUFFER)){
    do_draw_path();
  }
  else{
    do_draw_to(click);
  }
}
*/

NCOORDS::ncoords* Drawingset::flip_if(NCOORDS::ncoords* flippable){
   if(inverted() &&   flippable->get_state()){     
     flippable->invert();
   }
   return flippable;
 }
void Drawingset::reset_nodes(const coords& startpoint,
		     const coords& second,  
		     unsigned int symmetry ,
		 int lim_x , int lim_y ){
  map_generator.set_limits(startpoint, second,  symmetry, lim_x, lim_y);
  coordmap= map_generator.get_nodes();
}

void Drawingset::do_draw_path(const ncoords& rotator){
  // very slow - too much initialising and freeing memory
  //return to the non-matrix version - much faster - not using this code now?
  bool ifinvert = this->inverted();
  bool ifreverse = this->reversed();
  bool do_up = !(this->skip_odd());
  bool do_down = !(this->skip_even());
  if(!do_down && !do_up){
    throw XNPathError("if both draw_up and draw_down are switched off, nothing will be drawn at all");
  }
  //create group around a single node
  
     
#ifdef DEBUG
   
#endif // DEBUG
 
  }


//TODO: rewrite do_draw_path() to create a single 'roseeette', with a functor
//to translate and draw the points


//int Drawingset::translate_draw_path(std::vector<simple_click_history>& rosette, cairo_t* kyro,std::vector<coords*> nodes);
void Drawingset::do_draw_path(){
#ifdef DEBUG
  
  check_status();
  std::cout << "Drawingset::do_draw_path called\niterating...\t" << std::endl;
#endif // DEBUG
  
  coords centre =*(*coordmap.begin());
  coords offset;
  ncoords rotator = point_data;
  simple_click_history translatedpath = (*history) - centre;
  bool invert = inverted();
  bool reverse = reversed();
  bool skip_up = skip_odd();
  bool skip_down= (skip_up ? false : skip_even());
  std::deque<simple_click_history> rosette_brush;
  std::deque<simple_click_history> inverted_brush;
  std::deque<simple_click_history> reversed_brush;
  std::deque<simple_click_history> reversedinverted_brush;
  
  rotator.create_path_rosette(*history, rosette_brush);
  rotator.create_path_rosette(*history, inverted_brush, true);
  rotator.create_path_rosette(*history, reversed_brush, false,true);
  rotator.create_path_rosette(*history, reversedinverted_brush, true,true);
  int upstate(0), downstate(0);
  for (Nodedeq::iterator start = coordmap.begin(); start != coordmap.end();start++){
    offset = *(*start) - centre;
    //TODO drawbuffer member in Drawingbase?
    if(this->path_within_bounds(offset)){
      if(!(*start)->get_state()){
	upstate++;
	if(!skip_even()){
	  this->do_draw_rosette_path(rosette_brush, &offset);
	}
      }
      else{
	downstate++;
	if(!skip_odd()){
	  if(invert){
	    if(!reverse){this->do_draw_rosette_path(inverted_brush, &offset);}
	    else{this->do_draw_rosette_path(reversedinverted_brush,  &offset);}
	  }
	  else if(reverse){this->do_draw_rosette_path(reversed_brush,  &offset);}
	  else{this->do_draw_rosette_path(rosette_brush, &offset);}
	}
      }
    }
  }
  std::cerr <<"upstate: "<<upstate <<"\ndownstate: "<< downstate<< std::endl;
  if(!upstate || !downstate){
    std::string errstring = "coords in map all one state: ";
    if(!upstate){ errstring +="down!";}
    else{errstring +="up!";}
    throw XNPathError(errstring);
  }
}

void Drawingset::do_draw_rosette_path(const std::deque<simple_click_history>& paths, 
				      const coords* offset){
  typedef std::deque<simple_click_history>::const_iterator path_iterator;
  typedef simple_click_history::const_iterator point_iterator;
  const double x = offset->x;
  const double y = offset->y;
  
  for(path_iterator pathiter = paths.begin(); pathiter != paths.end();pathiter++){
    const coords* previous_handle(NULL);
    for(point_iterator pointiter = (*pathiter).begin(); pointiter != (*pathiter).end(); pointiter++){
      if((*pointiter).second & LINE_CLICK){
	cairo_line_to(m_cobject, (*pointiter).first.x + x, (*pointiter).first.y + y);
	previous_handle = &((*pointiter).first);
      }
      else if((*pointiter).second & MOVE_CLICK){
	cairo_move_to(m_cobject, (*pointiter).first.x + x, (*pointiter).first.y + y);
	previous_handle = &((*pointiter).first);
      }
      else if((*pointiter).second & CLOSE_PATH){
	cairo_close_path(m_cobject);//whereto previoushandle-leave for now
      }
      else if((*pointiter).second & BEZIER_CLICK){
	const coords& clikk= (*pointiter).first;
	if((pointiter+1) == (*pathiter).end()|| !((*(pointiter +1)).second & BEZIER_HANDLE)){
	  std::cerr<< "WARBNING:  bezclick with handles purleese"
		     <<std::endl;
	  if(previous_handle){
cairo_curve_to(m_cobject, previous_handle->x + x, previous_handle->y + y,
			   clikk.x + x, clikk.y + y,
			   clikk.x + x, clikk.y + y);
	  }
	  else{
	    cairo_curve_to(m_cobject,clikk.x + x, clikk.y + y,
			   clikk.x + x, clikk.y + y,
			   clikk.x + x, clikk.y + y);
	  }
	  previous_handle= &clikk;
	  }
	else if ((pointiter+2) == (*pathiter).end()|| !((*(pointiter +2)).second & BEZIER_HANDLE)){
	  const coords& handle= (*(pointiter +1)).first;
	  if(previous_handle){
cairo_curve_to(m_cobject,previous_handle->x + x, previous_handle->y + y,
	       handle.x +x, handle.y +y,		
	       clikk.x + x, clikk.y + y);
	  }
	  else{
	  cairo_curve_to(m_cobject,handle.x +x, handle.y +y,handle.x +x, handle.y +y,
			   clikk.x + x, clikk.y + y);
	  }
	  previous_handle= &clikk;
	  pointiter++;
	  }  
	else{//all good
	  const coords& handle1= (*(pointiter +1)).first;
	  const coords& handle2= (*(pointiter +2)).first;
	    cairo_curve_to(m_cobject, previous_handle->x + x, previous_handle->y + y,
			   handle1.x +x, handle1.y +y, 
			   clikk.x + x, clikk.y + y);
	    pointiter +=2;
	    previous_handle =&handle2;
	  }
      }
      else{
	std::cerr<<"unknown click type: "<< (*(pointiter +2)).second
		 <<"ignoring... "<<std::endl;	
      }
    }
  }
}
void context_xy_ellipse(Cairo::RefPtr<Cairo::Context> context, double x,
			double y, double width, double height){
context->save();
 context->translate(x, y);
 context->scale(width / 2.0, height / 2.0);
 //arc origin at top corner of bounding box!
 //for circle subtract radius off the centre x and  y
 context->arc(0.0, 0.0, 1.0, 0.0, 2 * M_PI);
 context->restore();
}
void Drawingset::set_inverted(bool whether){
  unsigned long patternstyl;
 XNDRAW_GET_PREF("DRAWINGSET_NODE_PATTERN", patternstyl);
 if (whether){
   patternstyl |= XNDRAW_NODEPATTERN_INVERTED;
 }
 else if(patternstyl & XNDRAW_NODEPATTERN_INVERTED){
   patternstyl ^= XNDRAW_NODEPATTERN_INVERTED;
 }
  XNDRAW_SET_PREF("DRAWINGSET_NODE_PATTERN" ,patternstyl );

}

void Drawingset::set_reversed(bool whether){
 unsigned long patternstyl;
 XNDRAW_GET_PREF("DRAWINGSET_NODE_PATTERN", patternstyl);
 if (whether){
   patternstyl |= XNDRAW_NODEPATTERN_REVERSED;
 }
 else if(patternstyl & XNDRAW_NODEPATTERN_REVERSED){
   patternstyl ^= XNDRAW_NODEPATTERN_REVERSED;
 }
  XNDRAW_SET_PREF("DRAWINGSET_NODE_PATTERN" ,patternstyl );
}

void Drawingset::flip_reversed(){
  std::string s("DRAWINGSET_NODE_PATTERN");
  unsigned long patternstyl;
  XNDRAW_GET_PREF(s, patternstyl);
  patternstyl ^=XNDRAW_NODEPATTERN_REVERSED;
  XNDRAW_SET_PREF("DRAWINGSET_NODE_PATTERN", patternstyl);
}

void Drawingset::flip_draw_correct(){
unsigned long patternstyl;
  XNDRAW_GET_PREF("DRAWINGSET_NODE_PATTERN", patternstyl);
  patternstyl ^=XNDRAW_NODEPATTERN_FAKECORRECT;
XNDRAW_SET_PREF("DRAWINGSET_NODE_PATTERN", patternstyl);
}

void Drawingset::flip_inverted(){
 std::string s("DRAWINGSET_NODE_PATTERN");
  unsigned long patternstyl;
  XNDRAW_GET_PREF(s, patternstyl);
  patternstyl ^=XNDRAW_NODEPATTERN_INVERTED;
  XNDRAW_SET_PREF("DRAWINGSET_NODE_PATTERN", patternstyl);
} 

bool Drawingset::inverted(){
 std::string s("DRAWINGSET_NODE_PATTERN");
 unsigned long patternstyl;
    XNDRAW_GET_PREF(s, patternstyl);
    return patternstyl & XNDRAW_NODEPATTERN_INVERTED;
  }

  bool Drawingset::reversed(){
std::string s("DRAWINGSET_NODE_PATTERN");
 unsigned long patternstyl;
    XNDRAW_GET_PREF(s, patternstyl);
    return patternstyl & XNDRAW_NODEPATTERN_REVERSED;
  }

bool Drawingset::skip_even(){
std::string s("DRAWINGSET_NODE_PATTERN");
 unsigned long patternstyl;
    XNDRAW_GET_PREF(s, patternstyl);
    return patternstyl & XNDRAW_NODEPATTERN_SKIP_EVEN;
}




bool Drawingset::skip_odd(){
std::string s("DRAWINGSET_NODE_PATTERN");
 unsigned long patternstyl;
    XNDRAW_GET_PREF(s, patternstyl);
    return patternstyl & XNDRAW_NODEPATTERN_SKIP_ODD;
}

bool Drawingset::set_skip_odd(){
std::string s("DRAWINGSET_NODE_PATTERN");
 unsigned long patternstyl;
    XNDRAW_GET_PREF(s, patternstyl);
    if(patternstyl & XNDRAW_NODEPATTERN_SKIP_EVEN){
      patternstyl ^= XNDRAW_NODEPATTERN_SKIP_EVEN;
    }
    if(!patternstyl & XNDRAW_NODEPATTERN_SKIP_ODD){
      patternstyl |= XNDRAW_NODEPATTERN_SKIP_ODD;
   }
 XNDRAW_SET_PREF(s, patternstyl);
 return true;
}

bool Drawingset::set_skip_even(){
std::string s("DRAWINGSET_NODE_PATTERN");
 unsigned long patternstyl;
    XNDRAW_GET_PREF(s, patternstyl);
    if(patternstyl & XNDRAW_NODEPATTERN_SKIP_ODD){
      patternstyl ^= XNDRAW_NODEPATTERN_SKIP_ODD;
    }
    if(!patternstyl & XNDRAW_NODEPATTERN_SKIP_EVEN){
      patternstyl |= XNDRAW_NODEPATTERN_SKIP_EVEN;
   }
 XNDRAW_SET_PREF(s, patternstyl);
 return true;
}


bool Drawingset::draw_correct(){
    if(drawto_now & DRAWING_DIRECT){
std::string s("DRAWINGSET_NODE_PATTERN");
 unsigned long patternstyl;
    XNDRAW_GET_PREF(s, patternstyl);
    return patternstyl & XNDRAW_NODEPATTERN_FAKECORRECT;
  }
  return true;
}
void Drawingset::set_draw_correct(bool setting ){
std::string s("DRAWINGSET_NODE_PATTERN");
 unsigned long patternstyl;
    XNDRAW_GET_PREF(s, patternstyl);
    if(setting){
      patternstyl |= XNDRAW_NODEPATTERN_FAKECORRECT;
  }
    else if(patternstyl & XNDRAW_NODEPATTERN_FAKECORRECT){
      patternstyl ^= XNDRAW_NODEPATTERN_FAKECORRECT;
  }
  XNDRAW_SET_PREF(s, patternstyl);
}

  void Drawingset::line_to_history(){
    this->draw();
  }


void Drawingset::line_to(coords* mousy){
  point_data = next;
  coords data_centre =*(*coordmap.begin());
  coords offset;
 next.set_radial(*mousy);
 ncoords before , after;
 bool invert = inverted();
 bool reverse = reversed();
 bool skip_up = skip_odd();
 bool skip_down= (skip_up ? false : skip_even());
 //
 //int skip = draw_correct() ? 1 : 0;
  for(Nodedeq::iterator node_walker= coordmap.begin();node_walker != coordmap.end();node_walker++){
    offset = *(*node_walker) - data_centre;
    before =point_data  + offset;
    after = next + offset;
    before.set_updownstate((*node_walker)->get_state() & COORDS_UP);
    after.set_updownstate((*node_walker)->get_state()& COORDS_UP);
    after.fill_container(future, invert, reverse);
    before.fill_container(current, invert, reverse);
    deqC_iter move_walk = current.begin();
    for(deqC_iter line_walk = future.begin();line_walk != future.end(); line_walk++, move_walk++){
      coords* move = *move_walk;
      coords* line = *line_walk;
      move_to(move->get_x(),move->get_y());
      line_to(line->get_x(), line->get_y());
    }
    future.clear();
    current.clear();
  }
  stroke();
}

bool Drawingset::get_has_nodes(){
    return (bool)(coordmap.size());
  }

void  Drawingset::scale_map(const coords& scaler){
    coordmap *= scaler;
  }


void  Drawingset::bg_draw_path(const PaintContext& paint){
  BGRenderPath rendermgr(this, paint);
}
void xn_cairo_stroke_fill(cairo_t* cr){
    cairo_stroke_preserve(cr);
      cairo_fill(cr);
  }
void xn_cairo_fill_stroke(cairo_t* cr){
cairo_fill_preserve(cr);
 cairo_stroke(cr);
 }
void  Drawingset::bg_draw_path2(const PaintContext& paint){
  //memory leaking... - make a member and allow to restart,
  //and also allow to defer thread creation till after constructor returns
  BGRenderPath rendermgr(this, point_data, paint);
}
void Drawingset::create_bg_render(Drawingset* d,
			     BGRenderPath*& newrenderer, 
				  const PaintContext& paint, bool stayalive){
  std::cerr<<"Drawingset::create_bg_render"<<std::endl;
  BGRenderPath* res=new BGRenderPath(d, d->get_point_data(), paint,stayalive);
  newrenderer=res;
}

  std::pair<BGRenderPath*, Glib::Thread*> 
  Drawingset::create_bg_renderthread(const PaintContext& paint, ColourRole role,
				     void(*meth)(cairo_t*)){
    //changed to stay alive by default
    BGRenderPath* res=new BGRenderPath(this, point_data, paint,true);
    Glib::Thread* thread= THREADCREATE_MEM(*res, &BGRenderPath::run,true);
#if DEBUG_PATHQUEUE
    std::cerr<<"create_bg_renderthread(): new thread at "<<thread<<std::endl;
#endif
    res->set_thread(thread);
    return std::pair<BGRenderPath*, Glib::Thread*>(res, thread);
  }



std::pair<BGRenderPath*, Glib::Thread*> 
Drawingset::create_bg_renderthread(const PaintContext& paint){
  //actually, a bunch of threads

  BGRenderPath* res=new BGRenderPath(this, point_data, paint,true);
  Glib::Thread* thread= THREADCREATE_MEM(*res, &BGRenderPath::run,true);

  std::cerr<<"create_bg_renderthread(): new thread at "<<thread<<std::endl;
  res->set_thread(thread);
   return std::pair<BGRenderPath*, Glib::Thread*>(res, thread);
}

  void  Drawingset::scale_map(double x, double y){
    coords xy(x, y);
    scale_map(xy);
  }
void Drawingset::map_upendcoords(size_t index){
  coordmap.upendcoords(index);
}
void Drawingset::translate(deqC& list, coords* offset)const{
  for(deqC_iter walker = list.begin(); walker != list.end(); walker++){
    (*walker)->translate(offset);
  }  
}
 NCOORDS::Nodedeq& Drawingset::get_nodes(){
    return coordmap;
  }
   const NCOORDS::Nodedeq& Drawingset::get_nodes()const{
    return coordmap;
  }

  void Drawingset::curve_to(double h1x,double h1y,double  h2x,double h2y,double  ptx,double pty){
    Cairo::Context::curve_to(h1x,  h1y, h2x, h2y,  ptx, pty);
  }

void Drawingset::curve_to(NCOORDS::coords* pt){
  line_to(pt);
}
//TODO: - determine if stroke or fill are not flat colours,
//if not add a method to PatternColour to
//set offset and step in patterns matrix as we iterate(same for line_to()...
void Drawingset::curve_to(NCOORDS::coords* h1, NCOORDS::coords* h2, NCOORDS::coords* pt){
  ncoords handle1(point_data);
ncoords handle2(point_data);
 ncoords handle1_rel, handle2_rel;
 handle1.set_radial(*h1);
handle2.set_radial(*h2);
  Nodedeq handle_can1, handle_can2;
  coords offset;
  point_data = next;
  coords data_centre = *(*coordmap.begin());
 next.set_radial(*pt);
 ncoords before , after;
 bool invert = inverted();
 bool reverse = reversed();
 //int skip = draw_correct() ? 1 : 0;
  for(Nodedeq::iterator node_walker= coordmap.begin();node_walker != coordmap.end();node_walker++){
    offset = *(*node_walker) - data_centre;
    before =point_data + offset;
    after = next + offset;
    handle1_rel =handle1 + offset;
handle2_rel =handle2 + offset;
    before.set_updownstate((*node_walker)->get_state()& COORDS_UP);
    after.set_updownstate((*node_walker)->get_state()& COORDS_UP);
handle1_rel.set_updownstate((*node_walker)->get_state()& COORDS_UP);
handle2_rel.set_updownstate((*node_walker)->get_state()& COORDS_UP);
    after.fill_container(future, invert, reverse);
    before.fill_container(current, invert, reverse);
    handle1_rel.fill_container(handle_can1, invert, reverse);
handle2_rel.fill_container(handle_can2, invert, reverse);
    deqC_iter move_walk = current.begin();
    deqC_iter h1_walk = handle_can1.begin();
    deqC_iter h2_walk = handle_can2.begin();
    for(deqC_iter line_walk = future.begin();line_walk != future.end(); line_walk++, move_walk++,h1_walk++, h2_walk++ ){
      coords* move = *move_walk;
      coords* line = *line_walk;
      coords* H1 =   *h1_walk;
      coords* H2 =   *h2_walk;
      move_to(move->get_x(),move->get_y());
      curve_to(H1->x, H1->y, H2->x, H2->y, line->get_x(), line->get_y());
    }
    future.clear();
    current.clear();
  }
 stroke();
}
  //todo: make bg render functions respect flip etc settings
  //and version to render selection(or using some 'selection' PointPredicate )
void Drawingset::bg_render_filename(const std::string& filepath,
				    xnFileType filetype,
				    void(*cairomethod)(cairo_t*),
				    ColourRole role)   {  

      Cairo::RefPtr<Cairo::Surface> s(NULL);
      Cairo::RefPtr<Cairo::Context> cr(NULL);
      //TODO: width/height pref and user chooser
      int width(1500), height(1500);
      switch(filetype){

      case XNDRAW_PNG:
	s= Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32 , width, height);
	cr=Cairo::Context::create(s);
	break;
      case XNDRAW_SVGRENDER:
	s= Cairo::SvgSurface::create(filepath, width, height);
	cr =Cairo::Context::create(s);
	break;
      default:
	throw xn_Error("svg and png only supported");
      }
      /*if(bg_renderer){
	while(!bg_renderer->finished_job()){
	  bg_renderer->wait();
	}
	if(!bg_renderer->keep_alive()){
	  delete bg_renderer;
	  bg_renderer = NULL;
	}
	}*/
      PaintContext p = create_paintcontext(cr->cobj(), role, cairomethod);
      if(filetype== XNDRAW_PNG){
	FileCommandParams parms(filepath, XNDRAW_PNG);
	p.set_post_command(parms);
      }
      if(!bg_renderer){
	std::pair<BGRenderPath*, Glib::Thread*> pr= create_bg_renderthread(p);
	bg_renderer=pr.first;
	//bg_renderer->set_thread(pr.second);//move to create_bg_renderthread()
      }
      else{
#if DEBUG_PATHQUEUE
	//std::string renderlog = bg_renderer->get_renderer_log_output();
	//std::cerr<<"renderer log:\n"<<renderlog<<std::endl;
#endif
	std::pair< PaintContext, simple_click_history> pathpr(p, *history);
bg_renderer->add_path(pathpr);
      }
    }
void Drawingset::fill_history(){
bg_paint_path(COLOURROLE_FILL, &cairo_fill);
}
void Drawingset::line_to_history(coords* offset){
  Drawing::line_to_history(offset);
}
void Drawingset::fill_preserve_history(){
 bg_paint_path(COLOURROLE_FILL, &cairo_fill_preserve);
}

void Drawingset::stroke_history(){
 bg_paint_path(COLOURROLE_STROKE, &cairo_stroke);
}

  void Drawingset::stroke_preserve_history(){
    bg_paint_path(COLOURROLE_STROKE, &cairo_stroke_preserve);
}

  void Drawingset::bg_paint_path(ColourRole role, void(*meth)(cairo_t*)){
    save();//first line of method
    PaintContext p(colourmanager.get_patterncolour(role).copy(), m_cobject, meth);
    if(!bg_renderer){
      std::pair<BGRenderPath*, Glib::Thread*> result=create_bg_renderthread(p);
      bg_renderer=result.first;
      if(!bg_renderer){throw xn_Error("creation function broken");}
    }
    else{
      if(!bg_renderer->finished_job()){
#if DEBUG_PATHQUEUE
      std::ostream* olog=bg_renderer->get_log() ;
      if(olog){
	*olog  <<"!bg_renderer->finished_job()... "<<std::endl;
      }
      std::string s = bg_renderer->get_log_output() + bg_renderer->get_renderer_log_output();
      NAMESPACE_THREAD_CERR("printing renderer log:\n"<<s<<"\n\t------=====\n");
#endif
      //      bg_renderer->wait();
      }
      std::pair<PaintContext, simple_click_history > pathpr(p, *history); 
      bg_renderer->add_path(pathpr);
    }
    paint();
    restore();//last line of method
  }


}//namespace xNDRAW
