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

#include "pingbuffer.h"
#include "hframe.h"


using namespace xNDRAW;
using namespace NCOORDS;
ping_buffered::~ping_buffered() {}


  void ping_buffered::on_draw_import_paths(std::string fname){
    if(!draw){
      throw xn_Error("not initialised");
    }
    unsigned int ind = draw->get_pathindex();
      draw->on_import_paths(fname);
      if(ind != draw->get_pathindex()){
	reset_drawtool();
	//start_new_opsequence();
      }
    }
void ping_buffered::on_svgpathsread(std::string fname){
  container_widget->on_import_svg_paths(fname);
}
ping_buffered::ping_buffered(h_frame& parent, int base_no, double min) :
  ping_window(base_no),
  container_widget(&parent),
  minimum(min)
{
  set_app_paintable(true);
  signal_button_release_event().connect(sigc::mem_fun(*this, &ping_buffered::on_button_release_event), false);
  signal_motion_notify_event().connect(sigc::mem_fun(*this, &ping_buffered::on_motion_what));
}

 void ping_buffered::init_drawbuffer(){
   container_widget->on_init_draw();
   }





void ping_buffered::on_clip_test(){
  if(draw){
      draw->save();
      Cairo::Path* p_path= draw->copy_path();
    //draw->set_source_rgba(1.0-transcolour.get_red,1.0-transcolour.get_green(),1.0-transcolour.get_blue(),0.5);
    draw->rotate_degrees(60.0);
    draw->append_path(*p_path);
    draw->clip_preserve();
    draw->fill();
    draw->restore();   
  }
}
void ping_buffered::read_history_from_datastream(){
  if(!container_widget){
    throw xn_Error("parent Window NULL");
  }
  FileDialogManager& d_man = container_widget->get_dialog_manager();
  d_man.do_filedialog_slot(*container_widget, XNDRAW_PATH, "LOAD PATHS FROM FILE",
			   sigc::mem_fun(*this, &ping_buffered::on_draw_import_paths),
			    Gtk::FILE_CHOOSER_ACTION_OPEN);
}

void ping_buffered::save_history_as_datastream(){
  if(!draw){ throw xn_Error("attempt to write data but draw context is NULL");}
  if(!container_widget){
    throw xn_Error("parent Window NULL");
  }
  FileDialogManager& d_man = container_widget->get_dialog_manager();
  d_man.do_filedialog_slot(*container_widget, XNDRAW_PATH, "save path (or maybe paths) to file",
			   sigc::mem_fun(*this, &ping_buffered::on_draw_export_path),
			   Gtk::FILE_CHOOSER_ACTION_SAVE);
}

 void ping_buffered::on_coordsdata_read(){
if(!draw){ throw xn_Error("attempt to read in data but draw context is NULL");}
  if(!container_widget){
    throw xn_Error("parent Window NULL");
  }
  FileDialogManager& d_man = container_widget->get_dialog_manager();
  d_man.do_filedialog_slot(*container_widget, XNDRAW_GRID, "import a grid", 
			   sigc::mem_fun(*this, &ping_buffered::on_nodes_file_read),
			   Gtk::FILE_CHOOSER_ACTION_OPEN);
}

 bool ping_buffered::load_background(){
   //Background* bg = NULL;
   if(!container_widget){throw xn_Error("no parent, can't get dialog manager");}
   FileDialogManager& d_man = container_widget->get_dialog_manager();
   d_man.do_filedialog_slot(*container_widget, XNDRAW_PNG, "LOAD A background", 
			    sigc::mem_fun(*this, &ping_buffered::on_background_import),
			    Gtk::FILE_CHOOSER_ACTION_OPEN);
   return true;
 }
 
