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

#include "npathframe.h"
#include "x3draw.h"


namespace xNDRAW{
 
    bool 	PathArea::on_expose_event (GdkEventExpose* event){
    //to PathArea or ping_base
      ping_base::on_expose_event(event);
    try{

      std::cerr<< "entering exposeevent()" << std::endl;
       bool res =ping_base::on_expose_event (event);;
       std::cerr<< "called base class ping_base::on_expose_event (): "
		<<(res? "true": "false")
		<<std::endl;
       if(!cairo){
      cairo = get_gdkcairo_ref();
       }
      if (cairo){
	cairo_rectangle_t eventrect;

	eventrect.x =event->area.x;
	eventrect.y =event->area.y;
	eventrect.width = event->area.width;
	eventrect.height = event->area.height;
	cairo->save();
	cairo->begin_new_path();

	cairo->rectangle(event->area.x, event->area.y, 
			 event->area.width, event->area.height);
	set_source_from_buffer();
	std::cerr <<"post set_source_from_buffer();"<<std::endl;
	cairo->fill();
	cairo->restore();
      }
      return    true;
    }
    
      CATCH_AND_RETHROW
	return    false;
	}
  /*
 CairoBuffer& PathWindow::get_backbuffer()
 {return drawingarea.get_buffer();}

const CairoBuffer& PathWindow::get_backbuffer()const 
{return drawingarea.get_buffer();}*/
void PathArea::on_path_changed(){
  if(!get_is_drawable()){return;}
  if(!path){
    throw xn_Error("on_path_changed(): Path NULL");
  }
  try{
    draw_and_paint_path(c_manager, *path);
  }
  catch(const std::exception& e){ 
    std::string s= e.what();				  
    throw xn_Error(s);					  
  }
  
}
  void PathWindow::on_path_changed(){
    if((!get_is_drawable()) ||(!drawingarea.get_is_drawable() )){return;}
if(!path){
      throw xn_Error("Path NULL");
    } 
try{
    outputpath = get_output_path();
    drawingarea.set_path(&outputpath);
    drawingarea.on_path_changed();
    std::cerr<<"on_path_changed"<<std::endl;
    }
    catch(const std::exception& e){ 
      std::string s= e.what();				  
      throw xn_Error(s);					  
    }
  }

void PathArea::on_size_allocate(Gtk::Allocation& alloc){
  //reset cairo on reallocation-> old one has old Gdk::Window?
  std::cerr<<"PathArea::on_size_allocate\n(x,y)=> " <<alloc.get_x()
	   <<", "<< alloc.get_y()<< "\nwidth: "<< alloc.get_width()
	   <<"\nheight: "<< alloc.get_height()<<std::endl;
  ping_base::on_size_allocate(alloc);
  //set_pathscale();
}

 double PathArea::get_scaled_path_width()const{
      NCOORDS::cRect pathextents;
      path->get_extents( pathextents);
      return pathextents.width()* pathscale;
    }
    double PathArea::get_scaled_path_height()const{
      NCOORDS::cRect pathextents;
      path->get_extents( pathextents);
      return pathextents.height()* pathscale;
    }

void PathArea::set_pathscale(){
  if(!get_is_drawable()){return;}
      Gtk::Allocation alloc = get_allocation();
      unsigned int drawareawidth(0), drawareaheight(0);
	double path_width(0), path_height(0);    
      
      drawareawidth = alloc.get_width();
      drawareaheight = alloc.get_height();
      NCOORDS::cRect pathextents;
      path->get_extents( pathextents);
      path_width= pathextents.width();
      path_height = pathextents.height();
      if(!path_width && !path_height){pathscale=1;}
      else if(!path_width){pathscale = drawareaheight / path_height;}
      else if(!path_height){pathscale = drawareawidth / path_width;}
      else{
      double wide_ratio_d=(double)drawareawidth / path_width;
      double high_ratio_d=(double)drawareaheight / path_height;
      //int w_ratio=(int)(drawareawidth /(double)p_width) +1;
      //int h_ratio=(int)(drawareaheight /(double)p_height) +1;
      pathscale = std::max(wide_ratio_d, high_ratio_d);
      //pathscale = (double)max_ratio;
      }
      std::cerr<<"pathscale : "<<pathscale  <<std::endl;
      if(pathscale >5){pathscale=5;}
      if(pathscale <0.15){pathscale=0.15;}
 std::cerr<<"pathscale after mollifying extreme values: "<<pathscale  <<std::endl;
      
    }
PathArea::PathArea():
  path(NULL), 
  pathscale(1),
  cairo(NULL){}

    PathArea::PathArea(unsigned int buffer_w, unsigned int buffer_h, 
	     cairo_format_t format,Pingbufferflags flags):
      ping_base(buffer_w, buffer_h, format, flags),
      path(NULL), 
      pathscale(1),
      cairo(NULL){
      std::cerr<<"PathArea::PathArea \nbuffer_w: "<<buffer_w
	       <<"\nbuffer_h : "<<buffer_h<<std::endl;
}

   void PathArea::clear_path(){set_path(NULL);}
    void PathArea::set_path(path_type* pathnext){
      path=pathnext;
      if(path){
      set_pathscale();
      }
      else {pathscale=1;}
    }

    void PathArea::reallocate_drawbuffer(const Gtk::Allocation& alloc){
      std::cerr<<"WARNING:reallocating buffer - if iamage disaster strikkes this may be it"<<std::endl;
      resize_buffer(alloc.get_width(), alloc.get_height());
      buffer.reallocate();
    }


  NCOORDS::simple_click_history  PathArea::get_drawable_path()const{
      path_type drawable(*path);
      NCOORDS::cRect pathextents;
      drawable.get_extents(pathextents);
      coords  zero_offsetter(pathextents.x(), pathextents.y());
      drawable -=zero_offsetter;
      
      double w_scale =  buffer.get_width() / pathextents.width();
      double h_scale = buffer.get_height() / pathextents.height();
      double scale = std::max(w_scale, h_scale);
      if(scale <1.1){
	drawable*=(0.95 *scale);
      }
      else if(scale > 5){
	drawable*=(0.5 *scale);
      }
      //drawable*=pathscale;
      drawable.get_extents(pathextents);
      coords offs(-pathextents.x(), -pathextents.y());
      drawable += offs;
      Gtk::Allocation alloc = get_allocation();
      unsigned int drawareawidth(0), drawareaheight(0);
	//path_width(pathextents.x()), path_height(pathextents.y());    
      
      drawareawidth = alloc.get_width();
      drawareaheight = alloc.get_height();
      if((drawareawidth < pathextents.width()) ||(drawareaheight < pathextents.height())){
	std::cerr<<"incipient ERROR: scaled path too large: "<<std::endl;
      }
      return drawable;
    }

  void PathWindow::initialise_cairo(){
    drawingarea.initialise_cairo();
  }
  void PathArea::initialise_cairo(){
    if(!get_is_drawable()){throw xn_Error("not ready to draw to screen");}
    std::cerr<<"screen drawable and PathArea::initialise_cairo() called"<<std::endl;
   
      cairo = get_gdkcairo_ref();
    
    if(!cairo){
      throw xn_Error("initialise_cairo() : get_gdkcairo_ref() failed;");
    }
  }

  bool PathWindow::on_init_cairo(){
    if(!get_is_drawable()){return false;}
    std::cerr<<"PathWindow::on_init_cairo()"<<std::endl;
    try{
      initialise_cairo();
    on_path_changed();
    std::cerr<<"post PathWindow::on_path_changed()"<<std::endl;
	
      init_connection.disconnect();
      return true;
    }
CATCH_AND_RETHROW
	
      return true;
    }

PathWindow::PathWindow(Glib::ustring& argtitle, simple_click_history* argpath):
  drawingarea(XNDRAW_GET_PREF("PATHFRAME_BUFFER_WIDTH",0u), 
	     XNDRAW_GET_PREF("PATHFRAME_BUFFER_HEIGHT",0u), 
	     CAIRO_FORMAT_ARGB32),
      path(argpath){
std::cerr<<"PathWindow::PathWindow"<<std::endl;
  try{
    std::cerr<<"in try statement"<<std::endl;
	  if(path){
	    NCOORDS::cRect pathbounds;
	    path->get_extents(pathbounds);

	  }
	  set_title(argtitle);
	  add(vbox);
	  vbox.pack_start(drawingarea);
	  vbox.pack_start(toolbox);
	  show_all_children();
	  init_connection = Glib::signal_idle().connect(sigc::mem_fun(*this, &PathWindow::on_init_cairo));
  }
  CATCH_AND_RETHROW
    std::cerr<<"exit basetype(PathWindow) ctor"<<std::endl;
	}
  
  void PathWindow::set_path(PathWindow::path_type* p){
    path=p;
    //resize if necessary
    if(get_is_drawable()){
      on_path_changed();
    }
  }

  bool 	PathWindow::on_expose_event (GdkEventExpose* event){
    return Gtk::Window::on_expose_event( event);

	}


  void PathWindow::set_source_from_buffer(Cairo::RefPtr<Cairo::Context> buffercontext){
    drawingarea.set_source_from_buffer( buffercontext);
    std::cerr <<"PathWindow::set_source_from_buffer()-called drawingarea to do the job"<<std::endl;
  }
  void PathArea::set_source_from_buffer(Cairo::RefPtr<Cairo::Context> buffercontext){
    if(!get_is_drawable()){return;}
    Cairo::RefPtr<Cairo::Surface> surf;
    if(buffercontext){
      surf=buffercontext->get_target();
	  
      if(!surf){throw xn_Error("buffercontext.get_target() => NULL");}
      //Cairo::RefPtr<Cairo::Pattern> pattern = Cairo::SurfacePattern::create(surf);
    }
    else{
      surf =get_buffer_surface_ref();
      if(!surf){throw xn_Error("surf =get_buffer_surface_ref(); => NULL");}
    }
    PatternColour buffer_output(surf);
    Cairo::RefPtr<Cairo::Pattern> patternsource=  buffer_output.create_pattern();

    if(!patternsource){throw xn_Error("buffer pattern NULL");}
    cairo_status_t status = cairo_pattern_status(patternsource->cobj());
    if(status != CAIRO_STATUS_SUCCESS){
      throw xn_Error("pattern status bad");
    }
    if(!cairo){cairo = get_gdkcairo_ref();}

    if(!cairo){throw xn_Error("cairo NULL");}
    status= cairo_status(cairo->cobj());
if(status != CAIRO_STATUS_SUCCESS){
      throw xn_Error("gdkcairo status bad");
    }
    
    cairo->set_source(patternsource);
  }

  void PathArea::draw_and_paint_path( ColourManager& colourmanager,
				      const simple_click_history& drawpathignored){
    std::cerr <<"PathArea::draw_and_paint_path()"<<std::endl;
      if( get_is_drawable()){

	clear_buffer_to_bg();
	Cairo::RefPtr<Cairo::Context> buffercontext= get_buffer_context();
	if(!buffercontext){
	  throw xn_Error("buffercontext NULL");
	}
	buffercontext->save();
	
	  path_type output=get_drawable_path();
	cairo_draw_clickhistory(buffercontext, output);
	colourmanager.use_colour(buffercontext, COLOURROLE_STROKE, true);
	colourmanager.use_colour(buffercontext, COLOURROLE_FILL);
	Cairo::RefPtr<Cairo::Surface> surf = buffercontext->get_target();
	if(!surf){throw xn_Error("buffercontext.get_target() => NULL");}
	//Cairo::RefPtr<Cairo::Pattern> pattern = Cairo::SurfacePattern::create(surf);
	PatternColour buffer_output(surf);
	Cairo::RefPtr<Cairo::Pattern> patternsource=  buffer_output.create_pattern();
	if(!patternsource){
	  throw xn_Error("patternsource from buffer_output.create_pattern();-> NULL");
	}
	if(!cairo){initialise_cairo();}
	if(!cairo){throw xn_Error("cairo initialisation failed");}
	cairo->save();
	cairo->set_source(patternsource);
	//Cairo::Matrix matrix = Cairo::scaling_matrix(1/pathscale, 1/pathscale);
	//cairo->set_matrix(matrix);
	cairo->paint();
	cairo->restore();
	buffercontext->restore();
      }
  }

void PathArea::paint_buffer_to_window(){
  if(!cairo){return;}
  Cairo::RefPtr<Cairo::Surface> surf = get_buffer_surface_ref();
if(!surf){throw xn_Error("get_buffer_surface_ref() => NULL");}
	
	PatternColour buffer_output(surf);
	Cairo::RefPtr<Cairo::Pattern> patternsource=  buffer_output.create_pattern();
	if(!patternsource){
	  throw xn_Error("patternsource from buffer_output.create_pattern();-> NULL");
	}
	cairo->save();
	cairo->set_source(patternsource);
	Cairo::Matrix matrix = Cairo::scaling_matrix(1/pathscale, 1/pathscale);
	cairo->set_matrix(matrix);
	cairo->paint();
	cairo->restore();
}

  void PathWindow::draw_and_paint_path(){

      std::cerr <<"WARNING: PathWindow::draw_and_paint_path()"<<std::endl;
      outputpath= get_output_path();
      drawingarea.set_path(&outputpath);
      drawingarea.on_path_changed();
    }



  nPathWindow::nPathWindow(Glib::ustring& argtitle, 
			   simple_click_history* argpath,
			   const ncoords& nsides):
    PathWindow( argtitle,argpath),
      shape(nsides,*argpath){std::cerr <<"exit nPathWindow ctor"<<std::endl;}

void nPathWindow::set_path(simple_click_history* p){
      path =p;
      shape.set_basepath(p);
      if(get_is_drawable()){
	on_path_changed();
      }
    }

  simple_click_history nPathWindow::get_output_path(){
      path_type p= shape.as_path();
      cRect box;
      p.get_extents(box);
      std::cerr<<"nPathWindiow::get_output_path(): p extents: \nx :"<<box.x()
	       <<"\ty : "<<box.y() <<"\nwidth : "<<box.width()
	       <<"\nheight : "<< box.height()<<std::endl;
      return p;
    }


} //namespace xNDRAW
