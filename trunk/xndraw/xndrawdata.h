
//$Id: exampletreemodel.h,v 1.5 2005/02/18 15:20:01 murrayc Exp $ -*- c++ -*-

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

/*pillaged and adapted John O'Driscoll 2006*/

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


#ifndef XNDRAWDATA_H
#define XNDRAWDATA_H
#include "xndrawexceptions.h"

#if HAVE_CONFIG_H
#include "config.h"
#endif


#include <gtkmm.h>
#include "drawing.h"
#include "selection.h"

namespace xNDRAW{

  
  struct PathColumnRecord: public Gtk::TreeModelColumnRecord{
    //a list of paths
    Gtk::TreeModelColumn<Glib::ustring> pathid;
    Gtk::TreeModelColumn<size_t> numpoints;
    Gtk::TreeModelColumn<double> bbox_topL_x;
 Gtk::TreeModelColumn<double> bbox_topL_y;
 Gtk::TreeModelColumn<double> bbox_bottomR_x;
 Gtk::TreeModelColumn<double> bbox_bottomR_y;
    Gtk::TreeModelColumn<bool> current_path;

    PathColumnRecord(){
      add(pathid);
      add(numpoints);
      add(bbox_topL_x);
      add(bbox_topL_y);
      add(bbox_bottomR_x);
      add(bbox_bottomR_y);
      add(current_path);
    }


  };
 struct HistoryColumnRecord: public Gtk::TreeModelColumnRecord{
    Gtk::TreeModelColumn<size_t> ordinals;
    Gtk::TreeModelColumn<Glib::ustring> clicks;
   Gtk::TreeModelColumn<double> location_x;//x and y of a coords or
   Gtk::TreeModelColumn<double> location_y;//first point  of path etc
   Gtk::TreeModelColumn<Glib::ustring> state;

   //render an image?
    HistoryColumnRecord(){
      add(ordinals);
      
      add(clicks);
      add(location_x);
      add(location_y);
      add(state);
    }
  };
    
    
    
  class xN_draw_modelcolumns: public Gtk::TreeModelColumnRecord{
  public:
    xN_draw_modelcolumns(){
      add(rownumbers);
      add(names);
      add(drawings);
      add(simple_paths);
      add(coordmaps);
    }
    Gtk::TreeModelColumn<Glib::ustring> names;
    Gtk::TreeModelColumn<int> rownumbers;
    Gtk::TreeModelColumn<Drawing*> drawings;
    Gtk::TreeModelColumn<NCOORDS::simple_click_history> simple_paths;
    Gtk::TreeModelColumn<NCOORDS::Nodedeq> coordmaps;
  };

  
  class Path_textframe: public Gtk::Frame
  {
  protected:
    HistoryColumnRecord columninfo;
    
    const simple_click_history* path;
    int timestamp;
    size_t itercount; // ! !
    Gtk::ScrolledWindow dataport;
    Glib::RefPtr<Gtk::TreeStore> model;
    Gtk::TreeView viewer;
    
  public:
    typedef simple_click_history::const_iterator path_iterator;
    
    Path_textframe(const simple_click_history& history);
    virtual ~Path_textframe(){}
    
    static Glib::RefPtr<Path_textframe> create(const simple_click_history& history);
    void add_rows();
    void update();

    void set_path(simple_click_history* p){
      if(!p){
	throw xn_Error("passed a NULL path");
      }
      path = p;
      update();
    }
    
  };


  //don't use this dumb type: TreeModel better
  class xN_draw_data
    : public Glib::Object,
      public Gtk::TreeModel
  {
  protected:
    //Create a TreeModel with @a columns_count number of columns, each of type Glib::ustring.
    xN_draw_data(unsigned int columns_count = 10);
    virtual ~xN_draw_data();
    xN_draw_modelcolumns m_draw_columns;
  public:
    static Glib::RefPtr<xN_draw_data> create();
    
    Gtk::TreeModelColumn< Glib::ustring >& get_model_column(int column);
    
  protected:
    
    // Overrides:
    virtual Gtk::TreeModelFlags get_flags_vfunc() const;
    virtual int get_n_columns_vfunc() const;
    virtual GType get_column_type_vfunc(int index) const;
    virtual void get_value_vfunc(const TreeModel::iterator& iter, int column, Glib::ValueBase& value) const;
    
    bool iter_next_vfunc(const iterator& iter, iterator& iter_next) const;
    //TODO: Make sure that we make all of these const when we have made them all const in the TreeModel:
    virtual bool iter_children_vfunc(const iterator& parent, iterator& iter) const;
    virtual bool iter_has_child_vfunc(const iterator& iter) const;
    virtual int iter_n_children_vfunc(const iterator& iter) const;
    virtual int iter_n_root_children_vfunc() const;
    virtual bool iter_nth_child_vfunc(const iterator& parent, int n, iterator& iter) const;
    virtual bool iter_nth_root_child_vfunc(int n, iterator& iter) const;
    virtual bool iter_parent_vfunc(const iterator& child, iterator& iter) const;
    virtual Path get_path_vfunc(const iterator& iter) const;
    virtual bool get_iter_vfunc(const Path& path, iterator& iter) const;
    
    virtual bool iter_is_valid(const iterator& iter) const;
    
  private:
    typedef std::vector< Glib::ustring> rowstring_type; //X columns, all of type string.
    typedef std::vector< rowstring_type > rowstring_list_type; //Y rows.
    
    //This maps the GtkTreeIters to potential paths:
    //Each GlueItem might be stored in more than one GtkTreeIter,
    //but it will be deleted only once, because it is stored
    //only once in the GlueList.
    //GtkTreeIter::user_data might contain orphaned GlueList pointers,
    //but nobody will access them because GtkTreeIter::stamp will have the
    //wrong value, marking the user_data as invalid.
    class GlueItem
    {
    public:
      GlueItem(int row_number);
      int get_row_number() const;
      
    protected:
      int m_row_number;
    };
    
    //Allow the GlueList inner class to access the declaration of the GlueItem inner class.
    //SUN's Forte compiler complains about this.
    class GlueList;
    friend class GlueList; 
    
    class GlueList
    {
    public:
      GlueList();
      ~GlueList();
      
      //This is just a list of stuff to delete later:
      typedef std::list<GlueItem*> type_listOfGlue;
      type_listOfGlue m_list;
    };
    
   rowstring_list_type::iterator get_data_row_iter_from_tree_row_iter(const iterator& iter);
    rowstring_list_type::const_iterator get_data_row_iter_from_tree_row_iter(const iterator& iter) const;
    bool check_treeiter_validity(const iterator& iter) const;
    void remember_glue_item(GlueItem* item) const;
    
    //The data:
    rowstring_list_type m_rows;
    
    //Column information:
    ColumnRecord m_column_record;

    typedef Gtk::TreeModelColumn<NCOORDS::simple_click_history> SimpleClickHistoryModelColumn;
    typedef Gtk::TreeModelColumn<Glib::ustring> TextColumn;
    // Usually you would have different types for each column -
    // then you would want a vector of pointers to the model columns.
    typedef std::vector<TextColumn > TextColumnList;
    TextColumnList textcolumns;
    
    int m_stamp; //When the model's stamp and the TreeIter's stamp are equal, the TreeIter is valid.
    mutable GlueList* m_pGlueList;
  };
  
  
}//namespace xNDRAW







#endif /*XNDRAWDATA_H*/
