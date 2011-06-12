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


#include "hframe.h"
#include <ctime>
#include <cstdlib>

//#include "nodesmap.h"
#define PRINTT(thing) std::cout << #thing << " = " << thing << std::endl;

using namespace xNDRAW;
using namespace NCOORDS;
AboutDialog::AboutDialog(){
      const std::string EMPTY;
      std::ostringstream ostmp;
      ostmp <<PACKAGE<<"-"<<PACKAGE_VERSION;
	Glib::ustring nm = ostmp.str();;
	set_name(nm);
	nm = PACKAGE_VERSION;
	set_version(nm);
	ostmp.str(EMPTY);
	ostmp <<"Copyright (C) "<< XNDRAW_YEARS <<" "<<XNDRAW_PACKAGE_AUTHORS;
	nm= ostmp.str();
	set_copyright(nm);
	ostmp.str(EMPTY);
	nm =PACKAGE;
	set_program_name(nm);
	ostmp <<PACKAGE<<"-"<<PACKAGE_VERSION<<"\nCopyright (C) "<< XNDRAW_YEARS <<" "
	      <<XNDRAW_PACKAGE_AUTHORS <<"\n"<<PACKAGE
	      <<" comes with ABSOLUTELY NO WARRANTY;\nfor details press the 'no warranty' button\nYhis is free software, and you may redistribute it under certain conditions. For details see the file 'COPYING' distributed with the software, \nor press the 'Free License' button";
	nm = ostmp.str();
	set_comments(nm);
	ostmp.str(EMPTY);
	ostmp<<"This program is free software; you can redistribute it and/or modify\n"
	     <<"it under the terms of the GNU General Public License as published by\n"
	     <<"the Free Software Foundation; either version 2 of the License, or\n"
	     <<"(at your option) any later version.\n\n"
	     <<" This program is distributed in the hope that it will be useful,\n"
	     <<"but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
	     <<"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
	     <<"GNU General Public License for more details.\n\n"
	     <<" You should have received a copy of the GNU General Public License\n"
	     <<"along with this program; if not, write to the Free Software\n"
	     <<"Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA";
	nm=ostmp.str();
	set_license(nm);
	ostmp.str(EMPTY);
    }

void CoordmapSetter::on_xscale_changed(){
  coords curscale = scaler;
  double cur_x= scaler.x;
  double v = adjusters[XSCALE].first->get_value()/100.;
  double realscale= v/cur_x;
  coords realscaler(1., 1.);
  if(scale_xy_entrained){
    realscaler.set(realscale, realscale);
  }
  else{realscaler.x = realscale;}
  
  scale_signaller.emit(realscaler);
  scaler *= realscale;
  if(scale_xy_entrained){
    adjusters[YSCALE].first->set_value(scaler.y * 100.);
  }
}

CoordmapSetter::CoordmapSetter():scale_entrain_button("constrain\nproportion"),
				 upendcoords_button("fix first\nmap point"),
				 scaler(1.0,1.0), rotval(0.), scale_xy_entrained(false){
  add(boxframe);
  create_spinners();
  show_all_children();
}

CoordmapSetter::entry_type CoordmapSetter::create_spinner(double value, double min, 
							  double max, double step ){
  Gtk::Adjustment* adjust = Gtk::manage(new Gtk::Adjustment(value, min, max, step));
  Gtk::SpinButton* spin = Gtk::manage(new Gtk::SpinButton(*adjust));
  return entry_type(spin, adjust);
}

void CoordmapSetter::create_spinners(){
  adjusters[ROT] =create_spinner(0,0,360.);
  adjusters[XSCALE]= create_spinner(100, 1, 1000);
  adjusters[XSCALE].first->signal_value_changed().connect(sigc::mem_fun(*this, &CoordmapSetter::on_xscale_changed));
  adjusters[YSCALE]= create_spinner(100, 1, 1000);
  adjusters[YSCALE].first->signal_value_changed().connect(sigc::mem_fun(*this, &CoordmapSetter::on_yscale_changed));
  adjusters[XTRANSLATE]= create_spinner(0, -500, 500, 1);
  adjusters[YTRANSLATE]= create_spinner(0, -500, 500, 1);
  boxframe.pack_start(*(adjusters[ROT].first));
  boxframe.pack_start(*(Gtk::manage(new Gtk::Label("rotation"))));
  boxframe.pack_start(scale_box);
  scale_box.pack_start(scale_slider_box);
  scale_slider_box.pack_start(*(adjusters[XSCALE].first));
  scale_slider_box.pack_start(*(Gtk::manage(new Gtk::Label("x scale"))));
  scale_slider_box.pack_start(*(adjusters[YSCALE].first));
  scale_slider_box.pack_start(*(Gtk::manage(new Gtk::Label("y scale"))));
  scale_box.pack_start(scale_entrain_button);
  scale_entrain_button.signal_toggled().connect(sigc::mem_fun(*this, &CoordmapSetter::on_toggle_xyscale_entrained));
  boxframe.pack_start(transform_box);
  transform_box.pack_start(transform_slider_box);
  transform_slider_box.pack_start(*(adjusters[XTRANSLATE].first));
  transform_slider_box.pack_start(*(Gtk::manage(new Gtk::Label("x translate"))));
  transform_slider_box.pack_start(*(adjusters[YTRANSLATE].first));
  transform_slider_box.pack_start(*(Gtk::manage(new Gtk::Label("y translate"))));
  transform_box.pack_start(upendcoords_button);
}

void CoordmapSetter::on_yscale_changed(){coords curscale = scaler;
  double cur_y= scaler.y;
  double v = adjusters[YSCALE].first->get_value()/100.;
  double realscale= v/cur_y;
  coords realscaler(1., 1.);
  if(scale_xy_entrained){
    realscaler.set(realscale, realscale);
  }
  else{realscaler.y = realscale;}
  
  scale_signaller.emit(realscaler);
  scaler *= realscale;
  if(scale_xy_entrained){
    adjusters[XSCALE].first->set_value(scaler.x * 100.);
  }
}

void  PathSelector::set_draw(Drawingbase* d){
  draw=d;
  initialise();
}

void  PathSelector::initialise(){ 
  //update_label();
  path_lower.signal_clicked().connect(sigc::mem_fun(*this, &PathSelector::on_toggle_lower));
  path_upper.signal_clicked().connect(sigc::mem_fun(*this, &PathSelector::on_toggle_upper));
  show_all_children();
}

void PathSelector::update_label(){
  std::stringstream ostmp;
  ostmp <<"current path: "<< draw->get_pathid();
  std::string s = ostmp.str();
  label.set_text(s.c_str());
}

void PathSelector::add_and_draw_all(){
  add(pathbox);
  pathbox.pack_start(subbox,Gtk::PACK_SHRINK);
  pathbox.pack_start(label,Gtk::PACK_SHRINK);
  subbox.pack_start(path_upper,Gtk::PACK_SHRINK);
  subbox.pack_start(path_lower,Gtk::PACK_SHRINK);
  if(draw){initialise();
    show_all_children();
  }
}

h_frame::h_frame()
  :
  Gtk::Window(Gtk::WINDOW_TOPLEVEL),
  buffd_gtk_surf(NULL),
  init_draw_butt("redraw",false),
  use_coordset_butt("repaint",false),
  pathselector(this),
  png_bung("PNGBUNG",false),
  x3tool("XNDRAWTOOLBOX - make an adjustment.."),
  cairo_clear_butt("Clear" ,false),
  pointertools(create_pointertoolinfo())
{
  std::cerr << "entering hframe ctor body"<<std::endl;
  //set_events(Gdk::ALL_EVENTS_MASK);
  signal_button_release_event().connect(sigc::mem_fun(*this, &h_frame::on_button_release_event),false);
  /* moved past m_
  std::pair<unsigned int, unsigned int> width_height =DisplayInfo::monitor_size();
  unsigned int framewidth = width_height.first -20;
  unsigned int frameheight= width_height.second - 60;
  set_default_size(framewidth,frameheight);
  */
  buffd_gtk_surf= Gtk::manage( new ping_buffered(*this, 3, 2.0));
  signal_key_press_event().connect(sigc::mem_fun(buffd_gtk_surf, &ping_window::on_key_press_event));
  add(vframe);

  create_uiManager();
  Gtk::Widget& menuwidget = *(ref_ui_manager->get_widget("/MenuBar"));
  //menuwidget.property_height_request()  = 24;
  vframe.pack_start(menuwidget);

  vframe.pack_start(hframe);
  hframe.pack_start(scrolledwindow);
  scrolledwindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
  scrolledwindow.add(*buffd_gtk_surf);
  hframe.pack_start(v_box);
  
  v_box.pack_start(top_hbox, Gtk::PACK_SHRINK);
  v_box.pack_start(buttonframe, Gtk::PACK_SHRINK);
  //top_hbox.pack_start(topbox_sub_box2, Gtk::PACK_SHRINK);
  top_hbox.pack_start(map_adjuster, Gtk::PACK_SHRINK);
  top_hbox.pack_start(colourbuttonbox, Gtk::PACK_SHRINK);
  colourbuttonbox.pack_start(pathselector, Gtk::PACK_SHRINK); 
  ///proxy buttos(call colourchooser buttons maybe deodgy
  //and lack callback to update the colour (?)
  colourbuttonbox.pack_start(strokebutton, Gtk::PACK_SHRINK);
  colourbuttonbox.pack_start(fillbutton, Gtk::PACK_SHRINK);
  colourbuttonbox.pack_start(bgbutton, Gtk::PACK_SHRINK);

  buttonframe.add_button(&init_draw_butt);
  buttonframe.add_button(&use_coordset_butt);
  v_box.pack_start(bung_box, Gtk::PACK_SHRINK);
  buttonframe.add_button(&cairo_clear_butt); 
  buttonframe.add_button(&png_bung);
  //v_box.pack_start(pointertools, Gtk::PACK_SHRINK);
  pointertools.show();
  connect_drawtool_selectors();
  v_box.pack_start(l_wideness, Gtk::PACK_SHRINK);
  v_box.pack_start(preview, Gtk::PACK_SHRINK);
  preview.property_height_request() = 270;
  preview.set_surface(buffd_gtk_surf->get_buffer().get_surface());
  map_adjuster.connect_scaler(sigc::mem_fun( *buffd_gtk_surf, &ping_window::sigc_scale_map));
  map_adjuster.connect_upendcoords(sigc::mem_fun( *buffd_gtk_surf, &ping_window::sigc_map_upendcoords));
  use_coordset_butt.signal_clicked().connect(sigc::mem_fun(*buffd_gtk_surf,&ping_window::on_redraw_bg_line));
  init_draw_butt.signal_clicked().connect(sigc::mem_fun(*buffd_gtk_surf,&ping_window::on_redraw_line));
  //a long list of signal connections
  //origin_init_butt.signal_clicked().connect(sigc::mem_fun(*this, &h_frame::on_nodes_save));//glib::signal_idle now in charge
  
  png_bung.signal_clicked().connect(sigc::mem_fun(*this,&h_frame::on_png_bung));
 
  cairo_clear_butt.signal_clicked().connect(sigc::mem_fun(buffd_gtk_surf, &ping_buffered::clear_cairo));
  
  l_wideness.signal_linewidth().connect(sigc::mem_fun(buffd_gtk_surf, &ping_buffered::set_linewidth));
  l_wideness.signal_alpha().connect(sigc::mem_fun(buffd_gtk_surf, &ping_buffered::adjust_stroke_alpha));
  l_wideness.signal_deviceX().connect(sigc::mem_fun(buffd_gtk_surf, &ping_buffered::on_move_deviceX));
  l_wideness.signal_deviceY().connect(sigc::mem_fun(buffd_gtk_surf, &ping_buffered::on_move_deviceY));
  signal_bg_svgsave().connect(sigc::mem_fun(buffd_gtk_surf, &ping_buffered::on_bg_render_svg));
  signal_bg_pngsave().connect(sigc::mem_fun(buffd_gtk_surf, &ping_buffered::on_bg_render_png));
  m_signal_pngwrite.connect(sigc::mem_fun(buffd_gtk_surf, &ping_buffered::on_sigc_pngwrite));
  Cairo::Matrix mat_proxy;
  std::cout <<  " Cairo::Matrix mat_proxy; " << std::endl;
  m_fucker = new Matrix_fucker(mat_proxy);
  m_fucker->signal_transform().connect(sigc::mem_fun(buffd_gtk_surf, &ping_buffered::on_transform_signal));
  m_fucker->signal_baseno_reinit().connect(sigc::mem_fun(buffd_gtk_surf, &ping_buffered::reset_draw));
  m_fucker->signal_grid_symmetry().connect(sigc::mem_fun(buffd_gtk_surf, &ping_buffered::reset_nodesbase));
  m_fucker->connect_path_clean(sigc::mem_fun(buffd_gtk_surf, &ping_buffered::on_clean_path2));
  m_fucker->connect_paths_clean(sigc::mem_fun(buffd_gtk_surf, &ping_buffered::on_cleanup_paths));
  x3tool.set_transient_for(*this);
  x3tool_connect_signals();
  x3tool.show_all_children();
  x3tool.set_keep_above (false);
  x3tool.show();
  Glib::RefPtr<Gdk::Window> toolwindow = x3tool.get_window();
std::pair<unsigned int, unsigned int> width_height =DisplayInfo::monitor_size_for_window(toolwindow);
  unsigned int framewidth = width_height.first -20;
  unsigned int frameheight= width_height.second - 60;
  set_default_size(framewidth,frameheight);
  scrolledwindow.property_width_request() =framewidth - 300;
  //scrolledwindow.property_height_request()  = frameheight-24;
  //buffd_gtk_surf->property_width_request() =framewidth - 200;
  //buffd_gtk_surf->property_height_request()  = frameheight;
  buffd_gtk_surf->show();
  set_redraw_on_allocate(true);
  show_all_children();
  show();
  buffd_gtk_surf->drawinitsignaller.connect(sigc::mem_fun(*this, &h_frame::on_init_draw));
  //Glib::signal_idle().connect(sigc::bind_return(sigc::mem_fun(buffd_gtk_surf, &ping_buffered::on_init),false));
}

void  h_frame::on_show_about_dialog()const{
      
      AboutDialog dialog;
      //shouldn't need to handle this
      dialog.run();
    }

void h_frame::on_pathframe_create(){
  //'memory leak ' = nPathWindow* pf
  h_frame* thisthing=this;
  nPathWindow* pf(NULL);
  std::cerr << "hframe at "<< thisthing<<std::endl;
  if(buffd_gtk_surf){
    Drawingbase* d = buffd_gtk_surf->get_draw();
    std::cerr <<"Drawingbase* d at "<< d<<std::endl;
    if(d){
      Drawing* draw= dynamic_cast<Drawing*>(d);
      if(draw){
	ncoords nc= draw->get_point_data();
	simple_click_history* path= &(draw->get_history());

	Glib::ustring title("Path=frame");
	std::cerr<<"calling nPathWindow* pf= new nPathWindow(title, path, nc);"
		 <<"\nncoords nc: "<< nc<<"path at "<<path<<" :\n"<<*path
		 <<std::endl;
	try{
	  pf= new nPathWindow(title, path, nc);
	  std::cerr<<"nPathWindow* pf at "<< pf<<std::endl;
	  if(pf){
	    pf->set_transient_for(*this);
	    pf->show();
	  }
	}
	catch(const std::exception& e){
	  std::string s = e.what();
	  throw xn_Error(s);
	}
	std::cerr <<"post nPathWindow creation"<<std::endl;
      }
    }
  }
}

void h_frame::on_view_saved_paths(){
      
  //also, need some persistent holder for 'GraphicElementBase*
  std::deque<simple_click_history*> paths = buffd_gtk_surf->get_saved_paths();
  std::deque<GraphicElement<simple_click_history>*> elements;
  for(std::deque<simple_click_history*>::iterator iter = paths.begin();
      iter !=paths.end(); iter++){
    elements.push_back(new GraphicElement<simple_click_history>(*iter));
  }
  Gtk::Window* win = new Gtk::Window;
    
  PathViewWidget* pathview = new PathViewWidget(elements);
  win->add(*pathview);
  win->show();
  win->set_transient_for(*this);
}
h_frame::~h_frame(){delete m_fucker;}

void h_frame::pack_drawtoolchooser(){
 
}

std::pair<unsigned int, unsigned int> 
h_frame::get_framesize(Glib::RefPtr<Gdk::Screen>& outscreen, 
		       Gdk::Rectangle& monitor){
  //fix window crazy bug
  Glib::RefPtr<Gdk::DisplayManager> displayman= Gdk::DisplayManager::get();
  std::vector< Glib::RefPtr< Gdk::Display > > displays= displayman->list_displays ();
  if (displays.size() >1){
    std::cerr<<"multiple displays"<<std::endl;
  }
  Glib::RefPtr< Gdk::Display > maindisplay = displayman->get_default_display();
  Glib::RefPtr<Gdk::Screen> screen ( NULL);
  int nscreens =maindisplay->get_n_screens();
  if(nscreens==1){
    ///////
    screen = Gdk::Screen::get_default(); //-which goes crazy with a multihead 'display'
    //screen = maindisplay->get_screen(0);
    std::cout <<"\n\nusing default display!!!!!!!!!!!!!!!!!!!!!!!"<<std::endl;
  }
  else{
    
    std::cout <<"\n\nmultiple screens on display!!!!!!!!!!!!!!!!!!!!!!!\n Using the first"<<std::endl;
    screen = maindisplay->get_screen(0);
  }
  outscreen = screen;
  int n_monitors= screen->get_n_monitors();
  int framewidth,frameheight;
  if ( n_monitors ==1){
    framewidth = screen->get_width()-20;
    frameheight = screen->get_height() - 80;
  }
  else if( n_monitors >1){
    //which screen and monitor are we launched from,
    //will determine where we emerge,
    //so how to fuind out?, or make sure we land on monitor we check
    //(ie screen 0, monitor0
    //Gdk::Rectangle monitor;
    screen->get_monitor_geometry (0, monitor);
    //doesn't help monitor bouncing effect on li nux?
    int monitor_originx = monitor.get_x();
    int monitor_originy = monitor.get_y();
    std::cerr<<"multiple monitors on screen 0"<<std::endl;
    if(monitor_originx ||monitor_originy){
      std::cerr<<"WARNING: MONITOR Origin not at (0,0) but : => ( "
	       <<monitor_originx << ", "<<monitor_originy<<" )"<<std::endl;
    }
    framewidth=monitor.get_width()-20;
    frameheight = monitor.get_height()-60;
  }
  else{
    throw std::runtime_error("no monitor??");
  }
  return std::pair<unsigned int, unsigned int>(framewidth, frameheight);
}


void h_frame::register_button(Gtk::Button* butt){
  butt->set_events(Gdk::BUTTON_PRESS_MASK);
  button_bin.push_back(butt);
}

void h_frame::arrange_buttons(){
  //not used currently
  //could better maybe together with something like
  //void create_control(sigc::slot<void, hframe&> operation, std::string command_descr, int control_type) and if control_type = xNDRAW::BUTTON or XNDRAW::MENUITEM, the appropriate control created and rehgistered
  int n = button_bin.size();
  int n_hbox = int(std::sqrt(float(n))) + 1;
  int n_butt = (n / n_hbox) +1;
  int insertcount = 0;
  Gtk::VBox* mainbox = Gtk::manage(new Gtk::VBox);
  for(int i = 0; i < n_hbox; i++){
    Gtk::HBox* hbox = Gtk::manage(new Gtk::HBox);
    mainbox->pack_start(*hbox);
    if (insertcount >= n){
      break;
    }
    for (int j = 0; j < n_butt;j++){
      if(insertcount < n){
	hbox->pack_start(*(button_bin[insertcount++]));
      }
      else {break;}
    }
  }
}


void h_frame::create_uiManager(){
  std::cerr <<"hframe::create_uimanager()"<<std::endl;
  ref_ui_manager = uiManager::create(this);
  std::cerr <<"ref_ui_manager = uiManager::create(this); ... done"<<std::endl;
  Glib::RefPtr<MenuActions> menu_actions = ref_ui_manager->get_menuactions();
  std::cerr <<"get_menuactions() ... done"<<std::endl;
  Glib::RefPtr<xNdrawAction> quitaction = menu_actions->get_quitaction();
  if (quitaction){
    sigc::slot<void> my_slot_i_hope(sigc::mem_fun(*this, &h_frame::on_action_quit)); 
  }
}

void h_frame::on_init_preview(){
  if (!preview.initialise()){
    throw xNDRAW::xn_Error("preview failed to initialise correctly");// or could just grumble
  }
}
XndrawMap<int, std::string> h_frame::create_pointertoolinfo(){
  XndrawMap<int, std::string> result;
  result[XNDRAW_DRAWING_SIMPLE_CLICK] = DRAW_SIMPLE_CLICK_ICON;
  result[XNDRAW_DRAWING_FREEHAND] = DRAW_FREEHAND_ICON;
  result[XNDRAW_DRAWING_BEZIER] = DRAW_BEZIER_ICON;
  result[XNDRAW_SELECT_BOX] = SELECT_BOX_ICON;
  return result;
}
void h_frame::on_init_draw(){
 
  l_wideness.signal_forecolour().connect(sigc::mem_fun(*buffd_gtk_surf,&ping_window::on_signal_rgba));
  l_wideness.signal_backcolour().connect(sigc::mem_fun(*buffd_gtk_surf,&ping_window::on_signal_rgba_back));
  on_init_preview();
  on_draw_initialised();
}
void h_frame::on_init_rosydraw(){
  Drawingbase* olddraw = buffd_gtk_surf->get_draw();
  buffd_gtk_surf->init_rosydraw();
on_draw_initialised();
 if(olddraw&& (olddraw != buffd_gtk_surf->get_draw())){
    delete olddraw;
  }
  
}

void h_frame::on_reinit_drawset(){
 Drawingbase* olddraw = buffd_gtk_surf->get_draw();
  buffd_gtk_surf->reinit_drawset_again();
on_draw_initialised();
 if(olddraw && (olddraw != buffd_gtk_surf->get_draw())){
    delete olddraw;
  }
}

//void 

void h_frame::on_draw_initialised(){
Drawingbase* draw=buffd_gtk_surf->get_draw();
  m_fucker->set_pathframe(draw->get_history());
m_fucker->treeview_set_draw(draw, draw->signal_savedpaths_changed());
  draw->signal_savedpaths_changed().connect(sigc::mem_fun(pathselector, &PathSelector::update_label));
  draw->signal_savedpaths_changed().connect(sigc::mem_fun(*m_fucker, &Matrix_fucker::on_reset_path));
  //connect to mfucker/dataview also
  //keep point list up to date
  buffd_gtk_surf->signal_path_updated().connect(sigc::mem_fun(*m_fucker, &Matrix_fucker::on_path_modified));
  pathselector.set_draw(buffd_gtk_surf->get_draw());
  pathselector.initialise();
  //this should be in 'load_defaults()' or so
  x3tool.on_button_ROUNDY();
}

void h_frame::reset_drawtool(){
  if(!buffd_gtk_surf){
    throw xn_Error("having a bad day!");
  }
  buffd_gtk_surf->reset_drawtool();
}
void h_frame::connect_drawtool_selectors(){
  std::map<int, sigc::slot0<void> > slotmap;
  slotmap[XNDRAW_DRAWING_SIMPLE_CLICK] = sigc::mem_fun(*buffd_gtk_surf, &ping_buffered::make_drawtool_simple_click);
  slotmap[XNDRAW_DRAWING_FREEHAND] = sigc::mem_fun(*buffd_gtk_surf, &ping_buffered::make_drawtool_free);
  slotmap[XNDRAW_DRAWING_BEZIER] = sigc::mem_fun(*buffd_gtk_surf,&ping_window::make_drawtool_bezier);
  slotmap[XNDRAW_SELECT_BOX] = sigc::mem_fun(*buffd_gtk_surf,&ping_window::make_drawtool_select);
  pointertools.set_slots(slotmap);
}

void h_frame::on_realize(){
  
  Gtk::Window::on_realize();
  //buffd_gtk_surf->on_init();
}

void h_frame::x3tool_connect_signals(){
  x3tool.signal_operator().connect(sigc::mem_fun(buffd_gtk_surf, &ping_buffered::set_operator));
  x3tool.signal_fill_rule().connect(sigc::mem_fun(buffd_gtk_surf, &ping_buffered::set_fill_rule));
  x3tool.signal_line_join().connect(sigc::mem_fun(buffd_gtk_surf, &ping_buffered::set_line_join));
  x3tool.signal_line_cap().connect(sigc::mem_fun(buffd_gtk_surf, &ping_buffered::set_line_cap));
  x3tool.get_choos().connect_pattern_signal(sigc::mem_fun(buffd_gtk_surf, &ping_window::on_colourchooser_signal_pattern));
  x3tool.get_choos().connect_rgba_signal(sigc::mem_fun(buffd_gtk_surf, &ping_window::on_colourchooser_signal_rgba));
  x3tool.get_choos().connect_proxy(strokebutton, &colour_chooser::signal_stroke_rgb_emit);
  x3tool.get_choos().connect_proxy(fillbutton, &colour_chooser::signal_fill_rgb_emit);
  x3tool.get_choos().connect_proxy(bgbutton, &colour_chooser::signal_bg_rgb_emit);
}
  


Gtk::Window* h_frame::make_window_for_widget(Gtk::Widget* contents){
  Gtk::Window* win = new Gtk::Window;
  win->add(*contents);
  win->show_all_children();
  win->show();
  return win;
}

void h_frame::launch_drawaddikt(){
  Gtk::Window* win = new Gtk::Window;
  drawedit_widget* widge = new drawedit_widget;
  widge->path_send_signal().connect(sigc::mem_fun(*this,&h_frame::add_path));
  win->add(*widge);
  win->set_events(Gdk::ALL_EVENTS_MASK);
  win->signal_button_release_event().connect(sigc::mem_fun(*widge, &drawedit_widget::on_button_release), false);
  win->signal_button_press_event().connect(sigc::mem_fun(*widge, &drawedit_widget::on_button_press), false);
  win->show_all_children();
  win->show();
  Glib::signal_idle().connect(sigc::mem_fun(*widge, &drawedit_widget::on_init),false);    
}
    
void h_frame::on_nodes_save(){
  dialogmanager.do_filedialog_slot(*this, XNDRAW_GRID,"export a grid from Drawingset - maybe", 
				   sigc::mem_fun(*buffd_gtk_surf, &ping_window::on_save_nodes),
				   Gtk::FILE_CHOOSER_ACTION_SAVE);
}

void  PathSelector::on_toggle_lower(){
  //this should work via pingblah
  bool pathchanged= draw->set_path_rel_index(-1);
  if(pathchanged){
    parent->reset_drawtool();
    //update_label();
  }
}
void  PathSelector::on_toggle_upper(){
  bool pathchanged= draw->set_path_rel_index(1);
  if(pathchanged){
    parent->reset_drawtool();
    //called in callback
    //	update_label();
  }
}
void h_frame::on_svg_file_export(){
#if CAIRO_HAS_SVG_SURFACE
  dialogmanager.do_filedialog_slot(*this, XNDRAW_SVGRENDER, "EXPORT TO A svg file", sigc::mem_fun(*buffd_gtk_surf, &ping_window::draw_output_svg), Gtk::FILE_CHOOSER_ACTION_SAVE);
#else
  std::cerr<<"no svg CAIRO_HAS_SVG_SURFACE ==0 or undefined"<<std::endl;
  //return; // -switch on soon
#endif
 
}

void h_frame::on_png_file_import(){
  dialogmanager.do_filedialog_slot(*this, XNDRAW_PNG, 
				   "open png background/pattern",
				   sigc::mem_fun(*buffd_gtk_surf, 
						 &ping_window::draw_import_png),
				   Gtk::FILE_CHOOSER_ACTION_OPEN) ;
}  

void h_frame::on_native_path_import(){
  dialogmanager.do_filedialog_slot(*this, XNDRAW_PATH, "open a path file",
				   sigc::mem_fun(*buffd_gtk_surf, &ping_window::on_draw_import_paths),
				   Gtk::FILE_CHOOSER_ACTION_OPEN);
}


void h_frame::on_svg_fileget(){
  dialogmanager.do_filedialog_slot(*this, XNDRAW_SVGPATH,"open svg PATHS",
				   sigc::mem_fun(*this, &h_frame::on_import_svg_paths),Gtk::FILE_CHOOSER_ACTION_OPEN);
 
}

    void h_frame::on_png_bung(){
  if(buffd_gtk_surf){
  buffd_gtk_surf->on_png_bung(&dialogmanager);
  }
}

 void  h_frame::on_edit_sys_copy(){
      std::cerr<< "FIXME: TODO: 'Edit->Copy'"<<std::endl;
      throw xn_Error("Not implemented : so do it! : FIXME: TODO: 'Edit->Copy'");
    }

void  h_frame::on_edit_sys_cut(){
      std::cerr<< "FIXME: TODO: 'Edit->Cut'"<<std::endl;
      throw xn_Error("Not implemented : so do it! : FIXME: TODO: 'Edit->Cut'");
    }

void h_frame::on_edit_sys_paste(){
      std::cerr<< "FIXME: TODO: 'Edit->paste'"<<std::endl;
      throw xn_Error("Not implemented : so do it! : FIXME: TODO: 'Edit->Paste'");
    }

void h_frame::on_action_quit(){
      COUT("hframe::on_action_quit");
      //TODO: query if save paths etc
      hide();    
}

bool h_frame::on_button_release_event(GdkEventButton* event){
      std::cout<< "h_frame::on button_release_event" << std::endl;
      buffd_gtk_surf->on_button_release_event(event);
      return Gtk::Window::on_button_release_event(event);
    }
    
void h_frame::add_path(NCOORDS::simple_click_history path){
      std::deque<NCOORDS::simple_click_history>& p= get_paths();
      std::cout << "set_of_paths.push_back(path);FIXME - call a function of buffd_gtk_surf->draw" << std::endl;
      p.push_back(path);
    }

bool h_frame::on_idle(){
      std::cout <<"\nh_frame::on_idle...\nnon_idle...\non_idle...\nnon_idle...\non_idle...\n" << std::endl;
      return true;
    }

std::deque<NCOORDS::simple_click_history>&  h_frame::get_paths(){
      if(buffd_gtk_surf){
      return buffd_gtk_surf->get_saved_paths_ref();
      }
      else{
	throw xn_Error("pingwindow NULL");
      }
    }
    

void h_frame::on_png_save(){
std::string outputdir;
XNDRAW_GET_PREF("XNDRAW_PICTURE_OUTPUT_DIR", outputdir);
      if(outputdir.size()){
	dialogmanager.set_folder( XNDRAW_PNG, Gtk::FILE_CHOOSER_ACTION_SAVE, outputdir);
      }
      outputdir= dialogmanager.do_filedialog(*this, XNDRAW_PNG, "save a png file", m_signal_pngwrite, Gtk::FILE_CHOOSER_ACTION_SAVE);
      if(outputdir.size()){
      XNDRAW_SET_PREF("XNDRAW_PICTURE_OUTPUT_DIR", outputdir);
      }
    }

void h_frame::do_svg_to_native_paths(const std::string& native,
				     const std::string& svgfname  ){
  std::cerr <<"h_frame::do_svg_to_native_paths("<<  svgfname<<", "
	    <<native<<")"<<std::endl;
std::ostringstream ostmp;
 ostmp <<"svgread.py "<<svgfname << " "<< native;

  std::string fstr=ostmp.str();
   int sysres= system(fstr.c_str());
  if(sysres){//or throw an errot
    std::cerr<<"om_import_svg_paths() failed:svgread.py exit status"<<
      sysres ;
    if(errno){
      std::cerr<<strerror(errno)<<std::endl;
    }
    return;
  }
}

void h_frame::on_import_svg_paths(const std::string& fname){
  std::ostringstream ostmp;
  //use proper $TMP
  std::string tmpfile(get_application_home());
  //move to get_application_home() in x3draw.cc
  if(!direxists( tmpfile.c_str())){
    int res =snarky_createdir(tmpfile.c_str());
    if(res){
      throw xn_Error("can't create directory in home: permissions problewms?");
    }
  }
  //std::string tmpfile(getenv("HOME"));//fix -put in /tmp or ~/.xndraw
#ifndef WINDOWS
  if(tmpfile.size() && *(tmpfile.end()-1) != '/'){tmpfile +="/";}
#else
  if(tmpfile.size() && *(tmpfile.end()-1) != '\\'){tmpfile +="\\";}
#endif
  //std::string filestem("bork");
  //insecure
  errno=0;// in case...
  char* s_tmp= tempnam(tmpfile.c_str(), "bork");
  //char[20] templat="svg_values.out";
      
  //char* ptemplate=templat;
  //int fd =mkstemp(templat);
  if(s_tmp){
    tmpfile=s_tmp;
    free(s_tmp);
  }
  else if(errno){
    throw xn_Error("error in tempnam() time to replace");
  }
  else{
    tmpfile +="svgout.values.txt";
  }
  //convert paths in an svg(xml) file
  // to a simple text format, easier to parse! from C
  //ostmp <<"svgread.py "<<fname << " "<< fd;

  //if sometime a python interface to
  //simple_click_history is created
  //can use it here and use python directly
  //though it would still rely on having
  //svgread.py present and working
  //

  //maybe should use a pipe in preference to some tempfile
  //
  ostmp <<"svgread.py "<<fname << " "<< tmpfile;

  std::string fstr=ostmp.str();
  std::cerr<<"tempfile from tempnam(): "<<tmpfile
	   <<"string to pass to system:\n'"<<fstr<<"'"<<std::endl;
  int sysres= system(fstr.c_str());
  if(sysres){
    std::cerr<<"om_import_svg_paths() failed:svgread.py exit status"<<
      sysres ;
    if(errno){
      std::cerr<<strerror(errno)<<std::endl;
    }
    return;
  }
  buffd_gtk_surf->on_draw_import_paths(tmpfile);
  //  if(0){
  std::string s_erase="rm -rf ";
  s_erase+=tmpfile;
  sysres=system(s_erase.c_str());
  if(sysres){
    throw xn_Error("failed to remove temp file");
  }
  return;
}
// }
//remove file - anything portable in stupidapp?


void h_frame::on_bg_pngsave(){
  dialogmanager.do_filedialog(*this, XNDRAW_PNG, "save image to PNG in background : caution, broken", signal_bg_pngsave(),  Gtk::FILE_CHOOSER_ACTION_SAVE);
}
//signal_bg_svgsave().connect(sigc::mem_fun(*this, &h_frame::on_bg_svgsave));
//signal_bg_pngsave().connect(sigc::mem_fun(*this, &h_frame::on_bg_pngsave));

void h_frame::on_bg_svgsave(){
  dialogmanager.do_filedialog(*this, XNDRAW_SVGRENDER,"save rendered paths to svg-in bg thread :experimental - ie broken", signal_bg_svgsave(), Gtk::FILE_CHOOSER_ACTION_SAVE);
}
void h_frame::on_nodes_read(){
  dialogmanager.do_filedialog_slot(*this, XNDRAW_GRID,"import a grid for Drawingset-currently not operative , and maybe broken too!", sigc::mem_fun(*buffd_gtk_surf, &ping_window::on_nodes_file_read),Gtk::FILE_CHOOSER_ACTION_OPEN);
}
FileDialogManager& h_frame::get_dialog_manager(){
  return dialogmanager;
}
