//$Id: examplewindow.cc,v 1.3 2004/02/10 14:50:06 murrayc Exp $ -*- c++ -*-

/* gtkmm example Copyright (C) 2002 gtkmm development team
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

//GlueList classes etc from gtkmm example file, although may yet get rid of them!
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

#include "dataview.h"


namespace xNDRAW{
using namespace NCOORDS;
Data_view::Data_view(Drawingbase* d,
		     sigc::signal<void> paths_changed_signal):draw(NULL){ 
  draw=d; 
  //std::deque<NCOORDS::simple_click_history*> paths= draw->get_saved_paths();
  set_border_width(5); 
  add(m_VBox);
  m_VBox.pack_start(*(Gtk::manage(new Gtk::Label("Draft xN_draw datamanipulator", false))));
  m_ScrolledWindow.add(m_TreeView);
  //Only show the scrollbars when they are necessary:
  m_ScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
  m_VBox.pack_start(m_ScrolledWindow);
  m_VBox.pack_start(m_ButtonBox, Gtk::PACK_SHRINK);
  //m_ButtonBox.pack_start(m_Button_Quit, Gtk::PACK_SHRINK);
  m_ButtonBox.set_border_width(5);
  m_ButtonBox.set_layout(Gtk::BUTTONBOX_END);
  //Create the Tree model:
  //m_refTreeModel = xN_draw_data::create();
  path_model = Gtk::TreeStore::create(path_modelcolumns);
  m_TreeView.set_model(path_model);
  //segfault here? should widget be realized first?
 
  selection=m_TreeView.get_selection();
 selection->set_mode(Gtk::SELECTION_SINGLE);
  //Add the TreeView's view columns, to render some of the model's columns:
  m_TreeView.append_column( "ID", path_modelcolumns.pathid );
  m_TreeView.append_column( "number of points",path_modelcolumns.numpoints );
  m_TreeView.append_column( "bBox topleft 'x'", path_modelcolumns.bbox_topL_x );
 m_TreeView.append_column( "'y'", path_modelcolumns.bbox_topL_y );
m_TreeView.append_column( "bBox bottom right 'x'", path_modelcolumns.bbox_bottomR_x );
 m_TreeView.append_column( "'y'", path_modelcolumns.bbox_bottomR_y );
 m_TreeView.append_column( "current path?",path_modelcolumns.current_path);
 if(draw){
 add_rows();
 }
 init_connection = Glib::signal_idle().connect(sigc::bind_return(sigc::mem_fun(*this, &Data_view::on_initialised), true));
  show_all_children();
}

    bool Data_view::on_initialised(){
      
    
 m_TreeView.set_expander_column( *(m_TreeView.get_column(2)));
 //m_TreeView.set_hover_selection(true);
  //connect to select handler
 
  m_TreeView.signal_row_activated().connect(sigc::mem_fun(*this, &Data_view::path_select_handler));
 
  init_connection.disconnect();
  return true;
  }

void Data_view::select_handler(const Gtk::TreeModel::Path& path, 
			       Gtk::TreeViewColumn* column){}
  sigc::connection  Data_view::connect_signal(sigc::signal<void> paths_changed_signal){
  
    if(!connection.empty()){
      connection.disconnect();
    }
    connection =paths_changed_signal.connect(sigc::mem_fun(*this, &Data_view::on_paths_changed));
    on_paths_changed();
    return connection;
  }

 Drawingbase* Data_view::get_draw(){
   return draw;
 }

const Drawingbase* Data_view::get_draw()const{
  return draw;
}

void Data_view::on_paths_changed(){
    path_model->clear();
    add_rows();
  }

void Data_view::path_select_handler(const Gtk::TreeModel::Path& path, 
			       Gtk::TreeViewColumn* column){
  Gtk::TreeModel::iterator iter= selection->get_selected();
  Gtk::TreeRow row =*iter;
  Glib::ustring idpath= row.get_value(path_modelcolumns.pathid);
  std::string s_idpath= idpath;
  if(draw){
  draw->set_history_by_id( s_idpath);
  }
}

void  Data_view::set_draw(Drawingbase* d, 
	      sigc::signal<void> update_slot){
  draw=d;
  connect_signal(update_slot);
}

void Data_view::add_rows(){
  std::deque<NCOORDS::simple_click_history*> paths= draw->get_saved_paths();
  for(const_iterator w = paths.begin();w != paths.end();w++){
 Gtk::TreeModel::Row row = *(path_model->append());
 row[path_modelcolumns.pathid] = (*w)->get_pathid();
 row[path_modelcolumns.numpoints] =(*w)->size();
 cRect rect;
 (*w)->get_extents(rect);
 row[path_modelcolumns.bbox_topL_x] = rect.x();
 row[path_modelcolumns.bbox_topL_y] = rect.y();
 row[path_modelcolumns.bbox_bottomR_x] = rect.xmax();
 row[path_modelcolumns.bbox_bottomR_y] = rect.ymax();
  }
}


}


