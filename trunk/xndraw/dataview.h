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

#ifndef DATAVIEW_H
#define DATAVIEW_H

#include "xndrawexceptions.h"

#if HAVE_CONFIG_H
#include "config.h"
#endif


#include <iostream>
#include "xndrawdata.h"

namespace xNDRAW{

  //class PathModelView:public Gtk::TreeView
  /*
virtual void Gtk::TreeView::on_row_activated  	(  	const TreeModel::Path&   	 path,
		TreeViewColumn*  	column	 
	) 			[protected, virtual]
  */
class Data_view : public Gtk::Frame
{
public:
  typedef std::deque<simple_click_history*>::const_iterator const_iterator;
typedef std::deque<simple_click_history*>::iterator iterator;

protected:
  //Signal handlers:
  //virtual void on_button_quit();

  //Child widgets:
  Gtk::VBox m_VBox;

  Gtk::ScrolledWindow m_ScrolledWindow;
  Gtk::TreeView m_TreeView;

  //Our custom tree model: not for long
  //Glib::RefPtr<xN_draw_data> m_refTreeModel;
  Glib::RefPtr<Gtk::TreeStore> path_model;
  PathColumnRecord path_modelcolumns;
  Glib::RefPtr<Gtk::TreeSelection> selection;


  Gtk::HButtonBox m_ButtonBox;
  sigc::connection connection;
  //const std::deque<simple_click_history>* paths;
  Drawingbase* draw;//change to, a slot to get one, as required
  //connect a call to reset from ping_window::signal_draw_changed()
  sigc::connection init_connection;

public:
  Data_view(Drawingbase* d,
	    sigc::signal<void> paths_changed_signal);


  virtual ~Data_view(){}
protected:
  Data_view(const Data_view&);//don't do that
 Data_view& operator=(const Data_view&);
public:

void path_select_handler(const Gtk::TreeModel::Path& path, 
			 Gtk::TreeViewColumn* column);


  bool on_initialised();
  void on_paths_changed();
  void add_rows();

  void select_handler(const Gtk::TreeModel::Path& path, 
		      Gtk::TreeViewColumn* column);

  sigc::connection  connect_signal(sigc::signal<void> paths_changed_signal);

void  set_draw(Drawingbase* d, 
	       sigc::signal<void> update_slot);

  Drawingbase* get_draw();
  const Drawingbase* get_draw()const;
};



}//namespace xNDRAW

#endif //DATAVIEW_H
