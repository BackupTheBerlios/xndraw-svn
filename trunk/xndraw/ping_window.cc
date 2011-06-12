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


#include "ping_window.h"
#include "hframe.h"

using namespace NCOORDS;
namespace xNDRAW{

bool ping_window::pingwindow_fix_nodes= true;

 
ping_window::ping_window(int base, double xscale, double yscale):
  scale_x(xscale), 
  scale_y(yscale),
  draw (NULL),
  pencil(NULL),
  //put buffers in Draw?
 
  pngoutputbuffer(XNDRAW_GET_PREF("IMAGEBUFFER_WIDTH", 0u),
		  XNDRAW_GET_PREF("IMAGEBUFFER_HEIGHT", 0u), 
		  CAIRO_FORMAT_ARGB32),
  savecount(0), 
  zoom_ratio(1.5),
  editinfo(this, XNDRAW_EDIT_DRAW)
{
  set_events(Gdk::ALL_EVENTS_MASK);
  init_pencil();
    sequence_recorder.set_sequence_store(&opsequence_store);
  ignition_connection = Glib::signal_idle().connect(sigc::mem_fun(*this, &ping_window::on_init),false); 
  Glib::signal_timeout().connect(sigc::mem_fun(*this, &ping_window::update_screen), 1000);//once per second 
  //setting this in hframe is what matters 
  //signal_key_press_event().connect( sigc::mem_fun(*this, &ping_window::on_key_press_event));
}

bool ping_window::on_motion_notify_event(_GdkEventMotion* m_event){
  ping_base::on_motion_notify_event(m_event);//ignore return
  static int counter = 0;
  if (counter++ > 950){
    counter = 0;
    std::cout << "#######\n  #\nping_window::on_motion_notify_event = DUMMY FUNCTION   -starting to wake up     #\n#######################" << std::endl;
  }
  //how does path transformer get the refpt from centre of path?
  //in order to drive the preview?
  // const simple_click_history& p= draw->get_history();
  //coords refpt= p.get_centre();
  return pencil->on_motion_event(m_event);
}

  void ping_window::do_rand_combine( int combineflag){
    unsigned sz = draw->pathcount();
    if(sz<2){return;}
    int ind=std::rand()%sz;
    while(ind == draw->get_pathindex()){
      ind=std::rand()%sz;
    }
    bool res=combine_with_indexed_path(ind,  combineflag);
    std::cerr<<"do_rand_combine("<<combineflag<<")\nresult: "
	     <<(res?"success" :"failure")<<std::endl;
  }

bool  ping_window::combine_with_indexed_path(unsigned int pathindex,
				   int combineflag){
      if(!draw){throw xn_Error("unimitialised");}
      if(pathindex >= draw->pathcount()){
	throw xn_Error("index out of range");
      }
      const simple_click_history& rhs = draw->get_path_by_index(pathindex);
      if(pathindex == draw->get_pathindex()){
    throw xn_Error("combine path with itself");
      }
      return draw->combine_with_path(rhs, combineflag);
    }

void ping_window::on_realize(){//wrong - window not initialised at this pt
  //init_pencil();
  //init_cairo();
  Gtk::DrawingArea::on_realize();
}

bool ping_window::on_init(){// connect to Glib::signal_idle()
  //get_window()->set_back_pixmap(Glib::RefPtr<Gdk::Pixmap>(0),false);
  
  if(XNDRAW_GET_PREF("XNDRAW_INIT_TYPE", 0) & XNDRAW_INIT_ON_INIT){
    init_cairo();
  }
  ignition_connection.disconnect();
  return true;
}

void ping_window::sigc_scale_map(coords pt){
  drawset_scale_map(pt);
}
void ping_window::drawset_scale_map(const coords& pt){
  std::cerr<<"drawset_scale_map()"<<std::endl;
  draw->scale_map(pt);
}
void ping_window::drawset_scale_map(double x){
  coords xx(x,x);
  draw->scale_map(xx);
}
void ping_window::drawset_scale_map(double x, double y){
  coords xy(x,y);
  draw->scale_map(xy);
}
   
Drawingbase* ping_window::get_draw(){
  return draw;
}
  //called in allocation setting callback
  void ping_window::reallocate_drawbuffer(Gtk::Allocation& alloc){
  //danger - if some other object has a pointer to internal data
  std::cerr <<"reallocate_drawbuffer called"<<std::endl;
  //set width/height/stride on cbuffer   
  //buffer.allocate_surface();
  // set draws buffer to NULL
  // determine draws type, get it's history, asnd whatwever
  //delete it and the surface from buffer
  
  //buffer.reallocate();

}

void ping_window::on_transform_signal(Cairo::Matrix& matr){
  if(draw){
    draw->transform(matr);
  }
}

void ping_window::set_line_join(Cairo::LineJoin j){
  draw->set_line_join(j);
}

void ping_window::on_signal_rgba_back(Draw_colour background){
  draw->set_fill_colour(background);
}
void ping_window::on_signal_rgba(Draw_colour background){
  draw->set_stroke_colour(background);
}

void ping_window::on_scalepreview_update(std::pair<coords, coords>& points){
  if(points.second.x > -99999){
      draw->do_scale_preview(points);
  }
  else if(points.second.y > -99999){
    draw->do_scale_preview_centred(points.first);
  }
    else if(points.first.x > points.first.y){
coords xx(points.first.x, points.first.x);
draw->do_scale_preview_centred(xx);
    }
    else{
coords yy(points.first.y, points.first.y);
draw->do_scale_preview_centred(yy);
    }
}

void  ping_window::on_scale_finish(std::pair<coords, coords>& points){
if(points.second.x > -99999){
  draw->do_scale_finish(points);
  }
  else if(points.second.y > -99999){
    draw->do_scale_finish_centred(points.first);
  }
    else if(points.first.x > points.first.y){
      coords xx(points.first.x, points.first.x);
draw->do_scale_finish_centred(xx);
    }
    else{
coords yy(points.first.y, points.first.y);
draw->do_scale_finish_centred(yy);
    }
}

void  ping_window::on_rotatepreview_update(std::pair<double,  coords>& rotargs){
  if(rotargs.second.x > -99999){
    draw->do_rotate_preview(rotargs);
  }
  else{draw->do_rotate_preview_centred(rotargs.first);}
}

void  ping_window::on_rotate_finish(std::pair<double,  coords>& rotargs){
 if(rotargs.second.x > -99999){
 draw->do_rotate_finish(rotargs);
  }
  else{draw->do_rotate_preview_centred(rotargs.first);}
}

void  ping_window::on_translatepreview_update(std::pair<coords, int>& transargs){
      draw->do_translate_preview(transargs);
}

 void  ping_window::on_translate_finish(std::pair<coords,int>& transargs){
      draw->do_translate_finish(transargs);
}

    void ping_window::set_pencil_transform_slots(){
      typedef TransformTool::rotate_slotpair_type rotate_slotpair_type;
      typedef TransformTool::rotate_slot_type rotate_slot_type;
      typedef TransformTool::scale_slotpair_type scale_slotpair_type;
      typedef TransformTool::scale_slot_type scale_slot_type;
      typedef TransformTool::translate_slotpair_type translate_slotpair_type;
      typedef TransformTool::translate_slot_type translate_slot_type;
      rotate_slot_type rot_real(sigc::mem_fun(*this, &ping_window::on_rotate_finish));
      rotate_slot_type rot_preview(sigc::mem_fun(*this, 
		        &ping_window::on_rotatepreview_update));  
      rotate_slotpair_type rot_slots(rot_real, rot_preview);
      scale_slot_type sc_real(sigc::mem_fun(*this, &ping_window::on_scale_finish));
      scale_slot_type sc_preview(sigc::mem_fun(*this, &ping_window::on_scalepreview_update));
      scale_slotpair_type sc_slots(sc_real, sc_preview);
      translate_slot_type tr_real(sigc::mem_fun(*this, &ping_window::on_translate_finish));
      translate_slot_type tr_preview(sigc::mem_fun(*this, &ping_window::on_translatepreview_update));
      translate_slotpair_type tr_slots(tr_real, tr_preview);
      pencil->set_transform_slots(rot_slots, sc_slots, tr_slots);
}

NCOORDS::coords ping_window::get_path_centre()const{
      return draw->get_path_centre();
    }

void ping_window::set_pencil_reference_hook(){
      pencil->set_reference_point_hook(sigc::mem_fun(*this, &ping_window::get_path_centre));
    }

    void ping_window::set_pencil_slots(){
      set_pencil_reference_hook();
      set_pencil_transform_slots();
    }
void ping_window::init_pencil(int penciltype){
  std::cout << " calling init_pencil" << std::endl;
  Drawtool* oldpencil = pencil;
  if(pencil && pencil->tooltype() == penciltype){
    return;
  }
  if(oldpencil){
    oldpencil->disconnect_all();
  }
  sigc::connection conn;
  switch(penciltype){
  case(XNDRAW_DRAWING_SIMPLE_CLICK):
    pencil = new Drawtool(this);
    conn =pencil->coords_signal_draw().connect(sigc::mem_fun(*this, &ping_window::on_drawtool_signal_coords));
    pencil->set_connection(XNDRAW_CONNECT_COORDS_DRAW, conn);
    std::cout << "pencil is Drawtool" <<pencil << std::endl;
    break;
  case(XNDRAW_DRAWING_FREEHAND):
    pencil = new Freedrawtool(this);
    conn =pencil->coords_signal_draw().connect(sigc::mem_fun(*this, &ping_window::on_drawtool_signal_coords));
    pencil->set_connection(XNDRAW_CONNECT_COORDS_DRAW, conn);
    std::cout << "pencil is Freedrawtool" <<pencil << std::endl;
    break;
  case(XNDRAW_DRAWING_BEZIER):
    pencil = new Bezier_drawtool(this);
    conn =pencil->coords_signal_draw().connect(sigc::mem_fun(*this, &ping_window::on_drawtool_signal_coords));
    pencil->set_connection(XNDRAW_CONNECT_COORDS_DRAW, conn);
    std::cout << "pencil is Bezdrawtool" <<pencil << std::endl;
    break;
  case (XNDRAW_SELECT_BOX):
    Gtk::Widget* parent = pencil->get_parent();
    std::cerr<<"init_pencil():preSelectBoxTool ctor about to be called\npencil->parent currently: "
	     << parent<<std::endl;  
    pencil = new SelectBoxTool(this);
    conn = pencil->coords_signal_select().connect(sigc::mem_fun(*this, &ping_window::on_selecttool_signal_coords));
    pencil->set_connection(XNDRAW_CONNECT_COORDS_SELECT, conn);
    std::cout << "pencil is selectboxtool" <<pencil << std::endl;
    break;
  }
  set_pencil_slots();
  if (oldpencil){
    delete oldpencil;
    oldpencil = NULL;
  }
  //connect signals
 
  sigc::connection origin_connection = pencil->coords_signal_draworigin().connect(sigc::mem_fun(*this, &ping_window::on_drawtool_signal_origin));
  pencil->set_connection( XNDRAW_CONNECT_INITIALISATION, origin_connection);
} 

void ping_window::make_drawtool_free(){
  init_pencil(XNDRAW_DRAWING_FREEHAND);
}

  void ping_window::reset_drawtool(){
    if(!pencil){
      throw xn_Error("no pencil!");
    }
    //in case pencil would be in middle of bezier click collection
pencil->on_path_reset();
  }

void ping_window::make_drawtool_bezier(){
  init_pencil(XNDRAW_DRAWING_BEZIER);
}

void ping_window::make_drawtool_select(){
  init_pencil(XNDRAW_SELECT_BOX);
}

void ping_window::make_drawtool_simple_click(){
  init_pencil(XNDRAW_DRAWING_SIMPLE_CLICK);
}
void ping_window::make_selectboxtool(){
  init_pencil(XNDRAW_SELECT_BOX);
}
bool ping_window::update_screen(){
  if(draw){
    draw->paint();
    return true;
  }
  return false;
}
void ping_window::init_rosydraw(){
if(draw){
      RosyDraw* old_d = NULL;
      try{
       old_d = dynamic_cast< RosyDraw*>(draw);
      }
      catch(const std::exception& e){
	std::cerr<< "'Drawingset* old_d = dynamic_cast<Drawingset*>(draw);'"
		 <<"\nthrew an exception: "<<e.what()<<std::endl;
      }
      if(old_d){
	return;
      }

      //Drawing* olddraw= draw;//destroy in calling function
    simple_click_history oldpath = draw->get_history();
    ncoords pd = draw->get_point_data();
    std::deque<simple_click_history>& spaths=draw->get_saved_paths_ref();

    coords origine(pd.x, pd.y);;
    really_init_rosydraw(origine, oldpath);
    draw->swap_paths(spaths);
 }
  else{
    Gdk::Rectangle rect = get_allocation();
    coords origin(rect.get_width()/2, rect.get_height()/2);
    simple_click_history p;
    really_init_rosydraw(origin,p);
  }
}

void ping_window::really_init_rosydraw(const coords& origin, 
				       simple_click_history path){
  //if(!draw){
    cairo_t* visual = get_gdkcairo();
    cairo_surface_t* buffersurface = buffer.get_surface(); 
    
    //coords origin = *(*(seed.begin()));
    coords rad (origin.x +1, origin.y +1);
    ncoords pointbase(origin, rad, XNDRAW_GET_PREF("NCOORDS_BASE", 0u)*20);
	
    npatternrosette pattern(pointbase, path);
    set_draw(new  RosyDraw( visual , buffersurface, pattern));
    draw->check_status();
}

  //don't need this 'seed' now
  void ping_window::init_drawset(const Nodes& seed){
    coords centre(seed.get_centre());
    coords rad(*(seed.get_radial()));
    unsigned int base = seed.get_base();
    unsigned int limx= seed.get_limit_x();
    unsigned int limy= seed.get_limit_y();
    if(draw){
      Drawingset* old_d = NULL;
      try{
       old_d = dynamic_cast<Drawingset*>(draw);
      }
      catch(const std::exception& e){
	std::cerr<< "'Drawingset* old_d = dynamic_cast<Drawingset*>(draw);'"
		 <<"\nthrew an exception: "<<e.what()<<std::endl;
      }
      if(old_d){
	
	old_d->reset_nodes(centre, rad, base, limx, limy);
	return;
      }
  }
    cairo_t* visual = get_gdkcairo();
    cairo_surface_t* buffersurface = buffer.get_surface(); 
    
    ncoords pointbase(centre, rad, base);
    Drawing* olddraw=draw;
    set_draw(new Drawingset( visual , buffersurface ,pointbase, limx, limy));
    draw->check_status();
    if(!draw->get_has_nodes()){
      throw xn_Error("drawingset creation failure");
    }
    if(olddraw){
      delete olddraw;
    }
  }//clear the area too if required

void ping_window::reinit_drawset_again(){
    
    if(draw){
      Drawingset* old_d = NULL;
      try{
       old_d = dynamic_cast<Drawingset*>(draw);
      }
      catch(const std::exception& e){
	std::cerr<< "'Drawingset* old_d = dynamic_cast<Drawingset*>(draw);'"
		 <<"\nthrew an exception: "<<e.what()<<std::endl;
      }
      if(old_d){
	return;
      }
  }
    cairo_t* visual = get_gdkcairo();
    cairo_surface_t* buffersurface = buffer.get_surface(); 
    coords c(250,250);
      coords d(450,350);

      ncoords pointbase(c,d,3);
    if(draw){pointbase=draw->get_point_data();}
    Drawing* olddraw=draw;
    set_draw(new Drawingset( visual , buffersurface ,pointbase, 4000,3000));
    draw->check_status();
    if(!draw || !draw->get_has_nodes()){
      throw xn_Error("drawingset creation failure");
    }
    if(olddraw){
      //delete olddraw;hframe does that
      std::deque<simple_click_history>& paths = olddraw->get_saved_paths_ref();
      if(paths.size()){
	draw->swap_paths(paths);
      }
    }
  }//clear the area too if required

void ping_window::flip_drawset_correct(){
  if(!draw){return; }
  Drawingset* d = dynamic_cast<Drawingset*>(draw);
  if(d){
    d->flip_draw_correct();
  }
}

void ping_window::flip_reversed(){
  if(!draw){return; }
  Drawingset* d = dynamic_cast<Drawingset*>(draw);
  if(d){
    d->flip_reversed();
  }
}

void ping_window::fill_history(){
  if(draw){
    draw->fill_history();
  }
}

void ping_window::fill_preserve_history(){
  if(draw){
    draw->fill_preserve_history();
  }
}

void ping_window::stroke_history(){
  if(draw){
    draw->stroke_history();
  }
}

void ping_window::stroke_preserve_history(){
  if(draw){
    draw->stroke_preserve_history();
  }
}

void ping_window::set_source_to_fill(){
  draw->set_source_to_fill();
}
  void ping_window::paint_buffer_to_window(){
    if(!draw){return;}
    Gtk::Allocation allok= get_allocation();
cairo_rectangle_t eventrect;
    eventrect.x =0;
    eventrect.y =0;
    eventrect.width = allok.get_width();
    eventrect.height =  allok.get_height();
    draw->paint_rect(eventrect);
  }


void ping_window::on_get_filename_to_save( std::string filename){
  draw->write_buffer_to_png(filename.c_str());
}
void ping_window::clear_current_history(){
  draw->clear_history();
}

void ping_window::reset_draw(int point_no){//cahnge name to draw_set_ncoordsbase
  draw->set_baseno(point_no);
     
}
void ping_window::adjust_stroke_alpha(double alpha){
  draw->adjust_stroke_alpha(alpha);
}

void ping_window::set_linewidth(double width){
  draw->set_line_width(width);
}

 std::deque<NCOORDS::simple_click_history>& ping_window::get_saved_paths_ref(){
      return draw->get_saved_paths_ref();
    }
    
  void ping_window::on_nodes_file_read(const std::string& fname){
  typedef NCOORDS::Nodedeq grid_type;
  grid_type grid;
  grid= grid.read_data(fname); //better as istream operator>>
  //draw->set_nodes...
}

void ping_window::on_png_bung(FileDialogManager* app_dialog_manager){
  if (draw){ 
    //should use a windowgroup to track these things
    pngsave_window* this_is_png = new pngsave_window(cairoPtr(draw), get_current_folder(), app_dialog_manager); 
    if(!this_is_png){throw xn_Error("NULL from new");}
    this_is_png->signal_current_folder().connect(sigc::mem_fun(*this, &ping_window::set_current_folder));
    this_is_png->signal_filename().connect(sigc::mem_fun(*this, &ping_window::on_get_filename_to_save) );
    this_is_png->signal_altpngsave().connect(sigc::mem_fun(*this, &ping_window::on_altpngsave_write));
    this_is_png->signal_svgpathsopen().connect(sigc::mem_fun(*this, &ping_window::on_svgpathsread));
    this_is_png->signal_svgsave().connect(sigc::mem_fun(*this, &ping_window::on_draw_svgsave));
    this_is_png->signal_pngopen().connect(sigc::mem_fun(*this, &ping_window::draw_import_png));
    this_is_png->connect_svgpathssave(sigc::mem_fun(*this, &ping_window::draw_save_svg_projectpaths));
    this_is_png->show();

  }
}

void ping_window::on_svgpathsread(std::string fname){
  //oovrlooded
}

void ping_window::on_altpngsave_write(std::string fname){
  //draw->write_buffer_to_png(fname);
  //WARNING: fix locks on MutexNodedeq
  draw->bg_render_filename(fname, XNDRAW_PNG);
}

void  ping_window::on_colourchooser_signal_pattern(const PatternColour& col, 
						   ColourRole role){
      ColourManager& colourman = draw->get_colour_manager();
      colourman.set_colour(col.copy(), role);
    }

void ping_window::on_signal_rgb
(double r,double g,double b){
  draw->set_stroke_colour(r,g,b);
  double br = 1.0-r;
  double bg = 1.0-g;
  double bb = 1.0-b;
  draw->set_fill_colour(br, bg, bb);
}

drawPtr ping_window::get_draw_obj(){
  if(!draw){
    throw xn_Error("no draw");
  }
  Drawing* d = dynamic_cast<Drawing*>(draw);
  if(!d){
    throw xn_Error(" draw not a Drawing*...");
  }

  return drawPtr(d);
}

bool ping_window::event_in_self(GdkEventButton* event){
  Gtk::Allocation alloc = get_allocation ();
  int x= alloc.get_x();
  int y = alloc.get_y();
  int xmax=x+ alloc.get_width();
  int ymax =y+alloc.get_height();
  return event->x >= x && event->x <= xmax 
    &&  event->y >= y && event->y <= ymax ;
}

bool ping_window::on_button_release_event(GdkEventButton* event){
  if (event_in_self(event) && pencil){
    pencil->on_drawbutton_release(event);
  }
  return  ping_base::on_button_release_event(event);
}

surfPtr  ping_window::clone_cairosurface(){
  if (draw){
    surfPtr cairosurf = draw->get_target();

    surfPtr clone = Cairo::Surface::create(cairosurf, Cairo::CONTENT_COLOR_ALPHA, get_width(), get_height());
    return clone;
  }
  else {
    throw xn_Error("can't initialisae cloned buffer - template internal cairo is uninitialised");    
  }
}

bool ping_window::on_button_press_event(GdkEventButton* butt){
  try{
  //std::cout<< ":on_button_press_event(GdkEventButton* butt)"<<std::endl;
#ifdef DEBUG
  if(!pencil){
    throw xn_Error("no pencil");
  }
#endif
  if(event_in_self(butt)){
    if (draw){
      return pencil->on_drawbutton_press(butt);  
    }
    else{
      pencil->on_originbutton_press(butt);  
    }
  }
  return ping_base::on_button_press_event(butt);
  }
  catch(const std::exception& e){
    ErrorDialog<std::exception> ewindow(e, true);
    std::cerr << "Error: "<<e.what()<<std::endl;
    throw e;//rethrow, and abort()
  }
}

bool ping_window::save_nodes(const std::string& filename){
  Drawingset* drawset = dynamic_cast<Drawingset*>(draw);
  if(drawset){
    return drawset->save_nodes(filename);
  }
  return false;
}

void ping_window::replace_nodes(NCOORDS::Nodedeq& swapme){
  draw->reset_nodes(swapme);
}


Cairo::RefPtr<Cairo::ImageSurface> ping_window::get_surf(){
  return Cairo::RefPtr<Cairo::ImageSurface>(new Cairo::ImageSurface(buffer.get_surface()));
}

void ping_window::reinit_drawset(Nodes& listgen){
  coords centre(listgen.get_centre());
  coords rad(*(listgen.get_radial()));
  unsigned int base=listgen.get_base();
  unsigned int limx = listgen.get_limit_x();
  unsigned int limy = listgen.get_limit_y();
  draw->reset_nodes(centre, rad, base, limx, limy);
}

NCOORDS::Nodedeq ping_window::create_drawset(const coords& click1st, const coords& click2nd, int nodes_base){
  if (nodes_base == 0){
    nodes_base = XNDRAW_GET_PREF("NCOORDS_BASE", 3u);
  }
  NCOORDS::Nodes points(click1st, click2nd, nodes_base, 0,  0);
  //drawinset needs Nodes object for regenerating
  Nodedeq n;
  points.deliver(n);
  init_drawset(points);
  return n;
}

 sigc::signal<void> ping_window::signal_path_updated(){
return m_path_update_signal;
    }

void  ping_window::recorder_set_draw(Drawingbase* d){
      sequence_recorder.set_methodowner(d);
    }

void ping_window::on_sigc_pngwrite(std::string s){
      draw->write_buffer_to_png(s.c_str());
    }

void ping_window::on_cleanup_path(){
      draw->cleanup_history();
signal_path_updated().emit();
    }

 void ping_window::on_clean_path2(){
      draw->really_clean_path();
signal_path_updated().emit();
    }

void ping_window::drawset_map_upendcoords(size_t index){
  draw->map_upendcoords(index);
}

 void ping_window::on_redraw_bg_line(){
      draw->redraw_line();
    }
void ping_window::on_redraw_line(){
      draw->redraw_line(false);
    }

void ping_window::reset_nodesbase(unsigned int i){
      draw->reset_nodesbase(i);
    }

    void ping_window::on_cleanup_paths(){
draw->cleanup_histories();
signal_path_updated().emit();
    }

 void ping_window::draw_save_svg_projectpaths(std::string fname){
      draw-> write_pathfile_as_svg(fname);
    }

void ping_window::on_draw_import_paths(const std::string& fname){
	draw->on_import_paths(fname);
      }

void  ping_window::on_bg_render_svg(std::string f){
      draw->bg_render_filename(f, XNDRAW_SVGRENDER);
    }
void  ping_window::on_bg_render_png(std::string f){
      draw->bg_render_filename(f, XNDRAW_PNG);
    }

void ping_window::draw_import_png(std::string fname){
      draw->add_png_pattern(fname);
    }

void ping_window::on_draw_svgsave(std::string fname){
      draw->on_svgsave(fname);
    }

void ping_window::on_drawtool_signal_coords(click_pair_type pointpair){
  //int drawflag = pointpair.second
  std::pair<coords, int> stacked(*(pointpair.first), pointpair.second);
  cairo_t* viewcairo = draw->get_visual();
  viewconverter.to_device_space(viewcairo, stacked.first);
  draw->draw_to( stacked);
  signal_path_updated().emit();
  draw->stroke_preserve();
  draw->paint();
}
void ping_window::on_selecttool_signal_coords(click_pair_type pointpair){
  coords tmp(*(pointpair.first));
  cairo_t* viewcairo = draw->get_visual();
  viewconverter.to_device_space(viewcairo, tmp);
  switch(pointpair.second){
  case XNDRAW_POINTER_SELECTBOX_BUTTONDOWN:
    editinfo.selector.active = true;
    editinfo.selector.last_corner =  editinfo.selector.first_corner = tmp;
    //check for a selected points in the path
    //if(draw->points_selected()){
      //if a selected pointunder mouse, start to drag selection
      //otherwise deselect points and start to draw a box
    if(draw->draggable_selection(tmp, editinfo.selector.get_draggable_tolerance())){
      editinfo.dragging = true;
    }
    break;
  case XNDRAW_POINTER_SELECTBOX_DRAGMOTION:
    editinfo.selector.last_corner = tmp;
    if(!editinfo.dragging){
    draw->draw_selectbox(editinfo.selector);
    }
    //if dragging call motion preview for the existing translate
    break;
  case XNDRAW_POINTER_SELECTBOX_BUTTONUP:
    editinfo.selector.last_corner = tmp;
   
    draw->select_pathcoords(editinfo.selector);
    editinfo.selector.active = false;
    editinfo.dragging=false;
  }
signal_path_updated().emit();
}

void ping_window::on_drawtool_signal_origin(click_pair_type point){
  coords draw_units(*(point.first));
  cairo_t* viewcairo = draw->get_visual();
  viewconverter.to_device_space(viewcairo, draw_units);
  draw->set_to_origin(&draw_units);
}

bool ping_window::on_key_press_event(GdkEventKey* event){
  ping_base::on_key_press_event(event); //ignore return
  std::cerr <<"ping_window::on_key_press_event!!!\n\n;";
  switch(event->keyval){
  case 'a':
    std::cerr <<"char value 'a' ok in quaint switch here"<<std::endl;
  default:
    std::cerr <<"doing nothing - maybe pencil will do something:\ncalling pencil->on_key_press_event(event)"<<std::endl;
  }
  return pencil->on_key_press_event(event);;
}


bool ping_window::on_expose_event(GdkEventExpose* event){
  //call base class version first! ignoring return
  ping_base::on_expose_event(event);
  std::cout<< "entering exposeevent()" << std::endl;
  if (draw){
    cairo_rectangle_t eventrect;
    eventrect.x =event->area.x;
    eventrect.y =event->area.y;
    eventrect.width = event->area.width;
    eventrect.height = event->area.height;
#ifdef DEBUG

    std::cout << "calling paint_rect(eventrect)" << std::endl;
#endif
    draw->paint_rect(eventrect);
    //queue_draw_area(event->area.x, event->area.y,event->area.width, event->area.height);
  }
  return true;
}

void ping_window::clear_cairo(){
  if (draw) {
    draw->clear_history();
    draw->begin_new_path();
    draw->paint_white(true);
    reset_drawtool();
  }  
}

// gets a pointer to underlying gdk::Window to create cairo_t 
// and hands that to a refptr to Cairo::Context)
// Store Cairo::Contex in a container(now std::set 


NCOORDS::coords ping_window::get_default_coords(){
  double width = get_width();
  double height = get_height();
  return coords(width /2, height / 2);
}
NCOORDS::coords ping_window::get_default_offset(){
  NCOORDS::coords centre = get_default_coords();
  double compilerdummy(0.0);
  double offsetx = XNDRAW_GET_PREF("DEFAULT_OFFSET_X", compilerdummy);
  double offsety = XNDRAW_GET_PREF("DEFAULT_OFFSET_Y", compilerdummy);
  centre.x += offsetx;
  centre.y += offsety;
  return centre;
}

void ping_window::init_cairo(){
  int init_as = XNDRAW_GET_PREF("XNDRAW_INIT_TYPE", 0);
  if (draw || (init_as & XNDRAW_INIT_WITH_CLICK)){
    //set up pencil origin signal
    return;
  }
  cairo_surface_t* surface = buffer.get_surface();
  cairo_t* cr = get_gdkcairo();
  coords defaultcentre  = get_default_coords();
  coords defaultoffset= get_default_offset();
  unsigned int baseno = XNDRAW_GET_PREF("NCOORDS_BASE", 0u);
  std::cout <<"ping_window::init_cairo()\nbaseno:" << baseno<< std::endl;
  ncoords draworigin(defaultcentre, defaultcentre, baseno);
  if (init_as & XNDRAW_INIT_AS_DRAWING){
    std::cout <<"XNDRAW_INIT_AS_DRAWING"<<std::endl;
    set_draw(new Drawing(cr, surface,draworigin));
  }
  else if(init_as & XNDRAW_INIT_AS_DRAWINGSET){ 
    std::cout <<"XNDRAW_INIT_AS_DRAWINGSET"<<std::endl;
    //unsigned int nodesbaseno = XNDRAW_GET_PREF("NODES_BASE", 0u);
    //unsigned int gridsymmetrybase=XNDRAW_GET_PREF("GRID_SYMMETRY_BASE", 0U);
    set_draw(new Drawingset(cr, surface,draworigin));
  }
  draw->paint_white(true);// paint_to_background
  print_cairo_status(draw, "init_cairo()..status...");
  draw->check_status();
  drawinitsignaller.emit();
}

void ping_window::flip_skip_odd(){
  if(draw){
    Drawingset* d = dynamic_cast<Drawingset*>(draw);
    if(d){
      d->set_skip_odd();
    }
    else{
#ifdef ERROR_STRICT
      throw xn_Error("ping_window::flip_skip_odd() failed ... not a Drawingset");
#else // ! ifdef ERROR_STRICT
      std::cerr << "ping_window::flip_skip_odd() ..failed, not a Drawingset" << std::endl;
#endif // ERROR_STRICT
    }
  }


}
void ping_window::flip_skip_even(){
  if(draw){
    Drawingset* d = dynamic_cast<Drawingset*>(draw);
    if(d){
      d->set_skip_even();
    }
    else{
#ifdef ERROR_STRICT
      throw xn_Error("ping_window::flip_skip_even() failed ... not a Drawingset");
#else // ! ifdef ERROR_STRICT
      std::cerr << "ping_window::flip_skip_even() ..failed, not a Drawingset" << std::endl;
#endif // ERROR_STRICT
    }
  }

}
void ping_window::flip_inverted(){
  if(draw){
    Drawingset* d = dynamic_cast<Drawingset*>(draw);
    if(d){
      d->flip_inverted();
    }
    else{
#ifdef ERROR_STRICT
      throw xn_Error("ping_window::flip_inverted() failed ... not a Drawingset");
#else // ! ifdef ERROR_STRICT
      std::cerr << "ping_window::flip_inverted() ..failed, not a Drawingset" << std::endl;
#endif // ERROR_STRICT
    }
  }
}
void ping_window::zoom_IN(){
  zoom_cairo(get_zoom_ratio());
}
void ping_window::zoom_OUT(){
  zoom_cairo(1.0 / get_zoom_ratio());
}


void ping_window::set_zoom_ratio(double ratio){
  zoom_ratio = ratio;
}

double ping_window::get_zoom_ratio(){
  return zoom_ratio;
}

std::string ping_window::get_pngstring(){
  return png_sessionstring;
}

void ping_window::print_cairo_status(Cairo::Context* cr, std::string context_string){
  cairo_status_t cairo_stats = cairo_status(cr->cobj());
  if (cairo_stats == CAIRO_STATUS_SUCCESS){
    std::cout << context_string << ": cairo_status = " << cairo_status_to_string (cairo_stats) << std::endl;
  }
  else{
    throw xn_Error(cairo_status_to_string (cairo_stats));
  }
}

void ping_window::print_cairo_status (Drawing* cr, std::string context_string){
  cairo_status_t cairo_stats = cairo_status(cr->cobj());
  if (cairo_stats == CAIRO_STATUS_SUCCESS){
    std::cout << context_string << ": cairo_status = " << cairo_status_to_string (cairo_stats) << std::endl;
  }
  else{
    throw xn_Error(cairo_status_to_string (cairo_stats));
  }
}



void ping_window::zoom_cairo(double scale){
  if(draw){
    draw->scale(scale,scale);
  }
}

int ping_window::on_move_deviceX(double dx){ 
  if(draw){
    draw->translate(dx, 0.0);  
  }
  return 1;
}

int ping_window::on_move_deviceY(double dy){
  if(draw){
    draw->translate(0.0, dy);  
  }
  return 1;
}


}
  




