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

#ifndef XNDRAW_PATHVIEW_H
#define XNDRAW_PATHVIEW_H
#include "exceptions.h"
#if HAVE_CONFIG_H
#include "config.h"
#endif

#include "imageview.h"
#include "guidraw.h"
#include "selection.h"
#include "x3draw.h"

namespace xNDRAW{
  using NCOORDS::simple_click_history;
  using NCOORDS::GraphicElement;
  
  std::ostream& operator<<(std::ostream& os, const Gdk::Rectangle& rect);

  class PathViewWidget;
  struct Pathviewcolumns;

  class PathRenderer: public Gtk::CellRenderer{
    //fix to have just one buffer for it's whole window, same size as the treeview coordinate system for the column
  public:
    typedef GraphicElement<simple_click_history> path_type;
    typedef simple_click_history::size_type size_type;
  protected:
    //path_type* path;
    PathViewWidget* widget;
    Gtk::TreeView* treeview;
    std::map< path_type*, cairo_t*> celldrawbuffers;
    GuiDraw guidraw;
   
    //Cairo
  public:
    PathRenderer(PathViewWidget* w,Gtk::TreeView* tree ):widget(w), treeview(tree){
      //reallocate_context();
    }
    
    virtual ~PathRenderer(){}
    void reallocate_context(size_type n);
    void draw_path(Gtk::TreeModel::iterator& iter);

    Pathviewcolumns* get_columns();


    static void init_prefs();

    cairo_t* get_srccontext(Gtk::TreeModel::iterator iter);

    static double get_scale(const Gdk::Rectangle& cellarea, const cairo_rectangle_t& bufferarea);

    size_t get_row_count();

 static double get_area_d(const cairo_rectangle_t& r){
	return r.width > r.height;
      }
      static int get_area(const Gdk::Rectangle& r){
	return r.get_width() > r.get_height();
      }
static int get_area(const cairo_rectangle_t& r){
  return (int)get_area_d(r);
}
    int get_cell_count(){return get_row_count();}
    virtual void get_size_vfunc (Gtk::Widget& widget,
				 const Gdk::Rectangle* cell_area,
				 int * x_offset,
				 int * y_offset,
				 int * width,
				 int* height) ;

    virtual void render_vfunc(const Glib::RefPtr<Gdk::Drawable>& window, 
			      Gtk::Widget& widget, 
			      const Gdk::Rectangle& background_area, 
			      const Gdk::Rectangle& cell_area, 
			      const Gdk::Rectangle& expose_area, 
			      Gtk::CellRendererState flags);
    
    double get_scaleval(const simple_click_history& path, const Gdk::Rectangle& cellarea);

    path_type* get_drawpath_data(const Gtk::TreeModel::Path&  	treepath);
    
  };
  struct Pathviewcolumns: public Gtk::TreeModel::ColumnRecord{
 
    typedef GraphicElement<simple_click_history> path_type;
  
    Gtk::TreeModelColumn<size_t> ID;
    Gtk::TreeModelColumn<std::string> pathname;
    //Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf> > picture;
    Gtk::TreeModelColumn<path_type*> path;
 
    Pathviewcolumns(){
      add(ID);
      add(pathname);
      add(path);
    }
  };
  
  class PathViewWidget : public Gtk::Frame{//could inherit from some other widget
  public:
    typedef GraphicElement<simple_click_history> path_type;
  protected:
    Gtk::VBox elementframe;
    Gtk::ScrolledWindow scrolledwindow;
 
    Pathviewcolumns modelcolumns;
    Glib::RefPtr<Gtk::ListStore> treelist;
    Gtk::TreeView treeview;
    Gtk::CellRendererText nameeditor;
    PathRenderer* pathrenderer;///needs a backbuffer for each path...?
  public:

    PathViewWidget(const std::deque<path_type*>& paths);

    void setup_namecell();
    void setup_pathviewcell();

 
     

    virtual void on_edit_pathname_cell(Gtk::CellEditable* editable, const Glib::ustring& path);

    virtual void on_edit_pathname_cell_done(const Glib::ustring& path, const Glib::ustring& new_text);

    virtual void display_name_cell(Gtk::CellRenderer* renderer ,
				   const Gtk::TreeModel::iterator& iter);

    virtual void display_path_cell(Gtk::CellRenderer* renderer ,
				   const Gtk::TreeModel::iterator& iter);

    Pathviewcolumns* get_columns(){return &modelcolumns;}

    
  };
    
    
    class PathPreview:public Previewer{
    protected:
      //imagesource
      
      simple_click_history* path;
      cairo_t* pathcontext;
      GuiDraw guidraw;
    public:
      PathPreview(simple_click_history* p):
	path(p),pathcontext(NULL){
	reallocate_context();
      }
      virtual ~PathPreview(){cairo_destroy(pathcontext);}
      
      void reallocate_context();

      cairo_t* create_pathcontext(const cairo_rectangle_t& pathrect);
     

      void on_size_allocate(Gtk::Allocation& allocation);

  bool on_expose_event(GdkEventExpose* e){
    draw_path();
    return Previewer::on_expose_event(e);
  }

  void draw_path();
  };

} //namespace xNDRAW

#endif
