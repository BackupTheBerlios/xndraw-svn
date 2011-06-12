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

#include "fmatrix.h"
using namespace xNDRAW;
using namespace NCOORDS;

Matrix_fucker::Matrix_fucker(Cairo::Matrix in_matrix)
  :
  
  mat(in_matrix), 
  history_frame(NULL),
  datatree(NULL, sigc::signal<void>()),
  // value, lower, upper, step_increment, page_increment, page_size
  // note that the page_size value only makes a difference for
  // scrollbar widgets, and the highest value you'll get is actually
  // (upper - page_size).
  xmove_adjust(00.0, -1000.0, 1000.0, 1.0),
  ymove_adjust(00.0, -1000.0, 1000.0, 1.0),
  xx_adjust(log(in_matrix.xx), -16.0, 17.0, 0.1),
  xy_adjust(log(in_matrix.xy), -16.0, 17.0, 0.1),
  yx_adjust(log(in_matrix.yx), -16.0, 17.0, 0.1),
  yy_adjust(log(in_matrix.yy), -16.0, 17.0, 0.1),
  x_move(xmove_adjust),
  y_move(ymove_adjust),

  xx(xx_adjust),
  xy(xy_adjust),
  yx(yx_adjust),
  yy(yy_adjust),
  distort("distort"),
  interactive_distort("interactive_distort"),
  path_clean("path clean"),
  paths_clean("paths clean"),
  Base_no_adjust(3,1,17,1,1),
  grid_symmetry_adjustment(3,2,7,1,1),
  init_drawings_minadjust(3,1,17,1,1),
  init_drawings_maxadjust(8,1,17,1,1),
  Base_number(Base_no_adjust),
  grid_symmetry_button(grid_symmetry_adjustment),
  init_drawings_min(init_drawings_minadjust),
  init_drawings_max(init_drawings_maxadjust),
  Base_no_OK("Base\nnumber\nOK"),
  
  flipflop_verbose_butt("set_verbose/quiet \n flip flop")
{
  //GLib-GObject-WARNING here
  add(main_box);
//GLib-GObject-WARNING **: value "-inf" of type `gdouble' is invalid or out of range for property `value' of type `gdouble'
  main_box.add(entry_frame);

  //main_box.pack_start(preview);
  //main_box.pack_start(history_frame);
  main_box.pack_start(datatree);
  main_box.pack_start(b_box);
  entry_frame.add( m_matrix_box);
  b_box.pack_start(distort);
  b_box.pack_start(interactive_distort);
  b_box.pack_start(path_clean);
  b_box.pack_start(paths_clean);
  b_box.pack_start(flipflop_verbose_butt);
  flipflop_verbose_butt.set_events(Gdk::BUTTON_PRESS_MASK);
  b_box.pack_start(Base_no_box);
  Base_no_box.pack_start(*(Gtk::manage(new Gtk::Label("set the base number of lines to draw", false))));
  Base_no_box.pack_start(Basecontrols_box);
  Base_no_box.pack_start(*(Gtk::manage(new Gtk::Label("set grid symmetry -drawingset only"))));
  Base_no_box.pack_start(grid_symmetry_box);
  Basecontrols_box.pack_start(Base_number);
  Basecontrols_box.pack_start(Base_no_OK);
  grid_symmetry_box.pack_start(grid_symmetry_button);
  grid_symmetry_box.pack_start(grid_symmetry_OK);
  Base_no_OK.signal_clicked().connect(sigc::mem_fun(*this, &Matrix_fucker::on_base_no_OK));
  grid_symmetry_button.signal_value_changed().connect(sigc::mem_fun(*this, &Matrix_fucker::signal_grid_symmetry_changed));
  m_matrix_box.pack_start(*(Gtk::manage(new Gtk::Label("Affine transform\nx_new = xx * x + xy * y + x0;\ny_new = yx * x + yy * y + y0;",false))));
  m_matrix_box.pack_start(m_matrix_a);
  m_matrix_box.pack_start(m_matrix_b);
  m_matrix_box.pack_start(m_matrix_c);
  m_matrix_a.pack_start(xx);
  m_matrix_a.pack_start(*(Gtk::manage(new Gtk::Label("xx",false))));
  m_matrix_a.pack_start(xy);
  m_matrix_a.pack_start(*(Gtk::manage(new Gtk::Label("xy",false))));
  m_matrix_b.pack_start(yx);
  m_matrix_b.pack_start(*(Gtk::manage(new Gtk::Label("yx",false))));
  m_matrix_b.pack_start(yy);
  m_matrix_b.pack_start(*(Gtk::manage(new Gtk::Label("yy",false))));
  m_matrix_c.pack_start(x_move);
  m_matrix_c.pack_start(*(Gtk::manage(new Gtk::Label("x0",false))));
  m_matrix_c.pack_start(y_move);
  m_matrix_c.pack_start(*(Gtk::manage(new Gtk::Label("y0",false))));
  xx.signal_value_changed().connect(sigc::mem_fun(*this, &Matrix_fucker::on_adjust_xx));
  xy.signal_value_changed().connect(sigc::mem_fun(*this, &Matrix_fucker::on_adjust_xy));
  yx.signal_value_changed().connect(sigc::mem_fun(*this, &Matrix_fucker::on_adjust_yx));
  yy.signal_value_changed().connect(sigc::mem_fun(*this, &Matrix_fucker::on_adjust_yy));
  x_move.signal_value_changed().connect(sigc::mem_fun(*this, &Matrix_fucker::on_adjust_xmove));
  y_move.signal_value_changed().connect(sigc::mem_fun(*this, &Matrix_fucker::on_adjust_ymove));

  distort.signal_clicked().connect(sigc::mem_fun(*this, &Matrix_fucker::on_distort));
 
  flipflop_verbose_butt.signal_clicked().connect(sigc::mem_fun(*this, &Matrix_fucker::on_flipflop_verbose));
  // preview.set_size_request(300,300);
  show_all_children();
  show();
}

void Matrix_fucker::on_base_no_OK(){
  //drawprog.set_baseno((int)Base_no_adjust.get_value());
  signal_baseno_reinit().emit((int)Base_no_adjust.get_value());
}

Matrix_fucker::baseno_reinit_signal Matrix_fucker::signal_baseno_reinit(){
  return m_signal_baseno_reinit;
}
Matrix_fucker::init_drawings_signal Matrix_fucker::signal_init_drawings(){
  return m_signal_init_drawings;
}

void Matrix_fucker::set_pathframe(const NCOORDS::simple_click_history& p){
  Path_textframe* oldframe = history_frame;
  history_frame = new Path_textframe(p);
  if(oldframe) {
    main_box.remove(*oldframe);
    delete oldframe;
  }
  main_box.pack_start(*history_frame);
  Glib::signal_timeout().connect(sigc::mem_fun(*this, &Matrix_fucker::update_pathframe), 2000);
  show_all_children();
}
void Matrix_fucker::on_reset_path(){
 Drawingbase* d = datatree.get_draw();
  if(!d){throw xn_Error("datatree has NULL drawingbase");}
  simple_click_history* p= &(d->get_history());
  if(!history_frame){throw xn_Error("path viewer NULL");}
  history_frame->set_path(p);
  history_frame->update();
}

void Matrix_fucker::on_path_modified(){
  //  Drawingbase* d = datatree.get_draw();
  // if(!d){throw xn_Error("datatree has NULL drawingbase");}
  // simple_click_history* p= d->get_history();
  if(!history_frame){throw xn_Error("path viewer NULL");}
  // history_frame->set_path(p);
  history_frame->update();
}
void Matrix_fucker::signal_grid_symmetry_changed(){
  unsigned int value= grid_symmetry_adjustment.get_value();
  if(value ==2 ||value==3 ||value ==4|| value == 6){
    m_grid_symmetry_signal.emit(value);
  }
  //otherwise, does nothing
}

Path_textframe* Matrix_fucker::get_history_frame(){
  return history_frame;
}
const Path_textframe* Matrix_fucker::get_history_frame()const {
  return history_frame;
}

Data_view& Matrix_fucker::get_datatree(){
  return datatree;
}

const Data_view& Matrix_fucker::get_datatree()const{
  return datatree;
}

sigc::connection Matrix_fucker::connect_path_clean(sigc::slot<void> slot){
  return path_clean.signal_clicked().connect(slot);
}

sigc::connection Matrix_fucker::connect_paths_clean(sigc::slot<void> slot){
  return paths_clean.signal_clicked().connect(slot);
}

Drawingbase* Matrix_fucker::get_draw(){
  return datatree.get_draw();
}

void Matrix_fucker::treeview_set_draw(Drawingbase* d, 
				      sigc::signal<void> signal){
  datatree.set_draw(d, signal);
  if(d){
    simple_click_history* p=&(d->get_history());
    if(p && history_frame){
      history_frame->set_path(p);
      history_frame->update();
    }
  }
}

bool Matrix_fucker::update_pathframe(){
  if(history_frame){
    history_frame->update();
  }
  return true;
}

void Matrix_fucker::on_flipflop_verbose(){
  X3_draw* X = &X3_draw::instance();
  if(X->get_verbose()){
    X->set_verbose(false);
  }
  else{
    X->set_verbose(true);
  }
}

void Matrix_fucker::on_adjust_xx(){
  mat.xx = pow(2.0, xx_adjust.get_value());
}

void Matrix_fucker::on_adjust_xy(){
  mat.xy = pow(2.0, xy_adjust.get_value());
}

void Matrix_fucker::on_adjust_yx(){
  mat.yx = pow(2.0,  yx_adjust.get_value());
}

void Matrix_fucker::on_adjust_yy(){
  mat.yy = pow(2.0,  yy_adjust.get_value());
}


void Matrix_fucker::on_adjust_xmove(){
  mat.x0 =  xmove_adjust.get_value();
}

void Matrix_fucker::on_adjust_ymove(){
  mat.y0 = ymove_adjust.get_value();
}
/*****
      mat.xx = pow((double)2.0, (double)xx_adjust.get_value());
      mat.xy = pow((double)2.0, (double)xy_adjust.get_value());
      mat.yx = pow((double)2.0, (double)yx_adjust.get_value());
      mat.yy = pow((double)2.0, (double)yy_adjust.get_value());
      mat.x_move = pow((double)2.0, (double)xmove_adjust.get_value());
      mat.y_move = pow((double)2.0, (double)ymove_adjust.get_value());
******/
Matrix_fucker::~ Matrix_fucker(){}

/***
    Affine transform
    x_new = xx * x + xy * y + x0;
    y_new = yx * x + yy * y + y0;

****/ 
void Matrix_fucker::adjust(double xx_, double xy_, double yx_, double yy_, double xmov ,double ymov){
  cairo_matrix_init (&mat,xx_, xy_, yx_, yy_, xmov ,  ymov);
}

