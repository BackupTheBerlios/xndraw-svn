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

#include "pathview.h"
#include "cairopath.h"


namespace xNDRAW{

std::ostream& operator<<(std::ostream& os, const Gdk::Rectangle& rect){
    os <<"Gdk::Rectangle: x, y, width, height: "<< rect.get_x()<< ", "<<rect.get_y()<< ", "
       <<rect.get_width()<< ", "<<rect.get_height();
    return os;
  }

 void PathPreview::on_size_allocate(Gtk::Allocation& allocation){
    std::cerr <<"PathPreview::on_size_allocate"<< std::endl;
cRect rect;
    path->get_extents(rect);
    double xscale = allocation.get_width() / rect.width();
    double yscale = allocation.get_height() / rect.height();
 double scaleval = std::min(xscale, yscale);
 cairo_identity_matrix(pathcontext);
 cairo_scale(pathcontext, scaleval,scaleval);
  }

PathViewWidget::PathViewWidget(const std::deque<path_type*>& paths){
  typedef std::deque<path_type*>::const_iterator const_path_iterator;
  scrolledwindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
  add(elementframe);
  elementframe.pack_start(scrolledwindow);
  scrolledwindow.add(treeview);
  treelist = Gtk::ListStore::create(modelcolumns);
  treeview.set_model(treelist);
  for(const_path_iterator w = paths.begin(); w!=paths.end();w++){
    Gtk::TreeModel::Row row = *(treelist->append());
    row[modelcolumns.ID] = (*w)->get_ID();
    std::string nm = (*w)->get_name();
    if(nm.size()){
      row[modelcolumns.pathname] = nm;
    }
    else{row[modelcolumns.pathname] = "[anonymous path]";}
    row[modelcolumns.path] = *w;
  }
  treeview.append_column("name", modelcolumns.pathname);
  pathrenderer = Gtk::manage(new PathRenderer(this, &treeview));
  setup_namecell();
  treeview.append_column("view", modelcolumns.path);
  setup_pathviewcell();
  std::cerr<< "exiting pathview ctor!\n"<<std::endl;
  show_all_children();
std::cerr<< "exiting pathview ctor!\n"<<std::endl;
}

void PathRenderer::get_size_vfunc (Gtk::Widget& widget,
				 const Gdk::Rectangle* cell_area,
				 int * x_offset,
				 int * y_offset,
				 int * width,
				 int* height) {
      Gtk::CellRenderer::get_size_vfunc (widget, cell_area, x_offset,y_offset,width,height);
      std::cerr<<"PathRenderer::get_size_vfunc(): defaults from Gtk::CellRenderer:\n"
	       << "cell_area(param 2): ";
      if(cell_area){ std::cerr<< *cell_area <<std::endl;}
      else{ std::cerr<<" NULL\n";}
      std::cerr<<"x_offset(param 3): ";
      if(x_offset){ std::cerr<< *x_offset<<std::endl;}
      else{std::cerr<<" NULL\n";}
std::cerr<<"y_offset(param 4): ";
      if(y_offset){ std::cerr<< *y_offset<<std::endl;}
      else{std::cerr<<" NULL\n";}
      std::cerr<<"width(param 4): ";
      if(width){std::cerr<< *width <<std::endl;}
      else{std::cerr<<" NULL\n";}//shan't be NULL surely
      int cellwidth(0), cellheight(0);
      XNDRAW_GET_PREF("PATHRENDERER_CELL_HEIGHT", cellwidth);
      XNDRAW_GET_PREF("PATHRENDERER_CELL_HEIGHT", cellheight);
      *width = cellwidth;
      *height = cellheight;
    }

void PathRenderer::init_prefs(){
      if(!XNDRAW_HAVE_PREF("PATHRENDERER_CELL_WIDTH")){
	XNDRAW_SET_PREF("PATHRENDERER_CELL_WIDTH", 100u);
      }
      if(!XNDRAW_HAVE_PREF("PATHRENDERER_CELL_HEIGHT")){
	XNDRAW_SET_PREF("PATHRENDERER_CELL_HEIGHT", 100u);
      }
    }

double PathRenderer::get_scale(const Gdk::Rectangle& cellarea, const cairo_rectangle_t& bufferarea){
      if(get_area(bufferarea) < 10){return 1;}
      double xscale = cellarea.get_width() / bufferarea.width;
      double yscale = cellarea.get_height() / bufferarea.height;
      return std::min(xscale, yscale);
    }

size_t PathRenderer::get_row_count(){
      size_t count(0);
      if(treeview){
	Glib::RefPtr<Gtk::TreeModel> model = treeview->get_model();
	Gtk::TreeModel::Children childrows = model->children();
	typedef Gtk::TreeModel::Children::iterator iterator;
	for(iterator i = childrows.begin(); i != childrows.end(); i++){
	  count++;
	}
      }
      return count;
    }

PathRenderer::path_type* PathRenderer::get_drawpath_data(const Gtk::TreeModel::Path&  	treepath){
      if(!treeview){std::cerr<< "treeview NULL"<<std::endl;
	return NULL;}
      Glib::RefPtr<Gtk::TreeModel> model = treeview->get_model();
      const Gtk::TreeModel::iterator iter= model->get_iter(treepath);
      if(iter){
	Gtk::TreeModel::Row row = *iter;
	Pathviewcolumns* modelcolumns = get_columns();
	path_type* data = row[modelcolumns->path];
	return data;
      }
      return NULL;
    }

void PathRenderer::render_vfunc(const Glib::RefPtr<Gdk::Drawable>& window, 
			      Gtk::Widget& widget, 
			      const Gdk::Rectangle& background_area, 
			      const Gdk::Rectangle& cell_area, 
			      const Gdk::Rectangle& expose_area, 
			      Gtk::CellRendererState flags){
      std::cerr << "PathRenderer::render(...)\nbackground area: "<<background_area<<"\ncell_area: "
		<< cell_area<< "\nexpose_area: "<< expose_area<< std::endl;
     
      Gtk::TreeModel::Path  	treepath;
      Gtk::TreeViewColumn* vcolumn = NULL;
      int cellposx(0), cellposy(0);
      if(!treeview){
	throw xn_Error("renderer's treeview NULL");
      }
      treeview->get_path_at_pos(cell_area.get_x(), cell_area.get_y(),treepath, vcolumn,cellposx, cellposy);
      path_type* pathdata = get_drawpath_data(treepath);
      if(!pathdata){throw xn_Error("can't get path to render");}
      Glib::RefPtr<Gtk::TreeModel> model = treeview->get_model();
      const Gtk::TreeModel::iterator iter= model->get_iter(treepath);
      cairo_t* buffercontext;
      if(iter){
	buffercontext = get_srccontext(iter);
	simple_click_history* realpath = *(*pathdata);
	cairo_rectangle_t rect;
	cRect mrect;
	realpath->get_extents(mrect);
	coords offset(mrect.x(), mrect.y());
	simple_click_history drawpath = *realpath - offset;
	cairo_save(buffercontext);
	cairo_set_source_rgba(buffercontext, 1,1,1,1);
	cairo_paint(buffercontext);
	
	guidraw.draw_editline(buffercontext, drawpath);
	guidraw.draw_editcoords_path(buffercontext, drawpath);
	double scaleval =get_scaleval(*realpath, cell_area);
	cairo_surface_t* surf = cairo_get_target(buffercontext);
	//TODO: scale size of backbuffer on alloc
	cairo_scale(buffercontext,scaleval, scaleval);
	Cairo::RefPtr<Cairo::Context> contxt = window->create_cairo_context();//just the cellrenderersd turf or the whole widget?"
	cairo_t* windowcontext = contxt->cobj();
	cairo_save(windowcontext);
	cairo_set_source_surface(windowcontext,surf, cell_area.get_x(), cell_area.get_y());
	cairo_move_to(windowcontext,cell_area.get_x(), cell_area.get_y());
	cairo_line_to(windowcontext,cell_area.get_x() + cell_area.get_width(),cell_area.get_y());
	cairo_line_to(windowcontext,cell_area.get_x() + cell_area.get_width(),
		      cell_area.get_y() + cell_area.get_height());
	cairo_line_to(windowcontext,cell_area.get_x(),cell_area.get_y() + cell_area.get_height());
	cairo_close_path(windowcontext);
	cairo_fill(windowcontext);
	cairo_restore(windowcontext);
	cairo_restore(buffercontext);
	std::cerr <<"render_func complete"<<std::endl;
      }
      // Gtk::CellRenderer::render_vfunc(window, widget, background_area, cell_area,  expose_area,  flags);
    }

void PathViewWidget::on_edit_pathname_cell(Gtk::CellEditable* editable, const Glib::ustring& path){
      Gtk::Entry* pEntry = dynamic_cast<Gtk::Entry*>(&nameeditor);
      if(pEntry){
	Gtk::TreePath p(path);
	Gtk::TreeModel::iterator iter = treelist->get_iter(p);
	if(iter){
	  Gtk::TreeModel::Row row = *iter;
	  std::string stdstr = row[modelcolumns.pathname];
	  Glib::ustring ustr = stdstr;
	  pEntry->set_text(ustr);
	}
      }
    }
void PathViewWidget::display_path_cell(Gtk::CellRenderer* renderer ,
				   const Gtk::TreeModel::iterator& iter){
      if(iter){
	Gtk::TreeModel::Row row = *iter;
	Glib::RefPtr<Gtk::TreeModel> model = treeview.get_model();
	Gtk::TreePath treepath = model->get_path(iter);
	path_type* path_object = row[modelcolumns.path];
	simple_click_history* path  = *(*path_object);
	Gdk::Rectangle rect;
	treeview.get_cell_area(treepath, *(treeview.get_column(2)), rect);
	cairo_rectangle_t cairorect;
	cRect rectum;
	path->get_extents(rectum);
	//double xscale, yscale, viewscale;
	double bufferscale;
	if(rectum.area() < 160000){ //400 x 400 or equiv
	  bufferscale = 1.;
	}
	//FINISH ME - PathRenderer::activate_func(..) is the place to do setup
      }
}

void PathViewWidget::display_name_cell(Gtk::CellRenderer* renderer ,
				   const Gtk::TreeModel::iterator& iter){
      if(iter){
	Gtk::TreeModel::Row row = *iter;
	std::string stdstr = row[modelcolumns.pathname];
	Glib::ustring viewable = stdstr;
#ifdef GLIBMM_PROPERTIES_ENABLED
	nameeditor.property_text() = viewable;
#else
	nameeditor.set_property("text", viewable);
#endif 
    }
}

void PathViewWidget::on_edit_pathname_cell_done(const Glib::ustring& path_string, const Glib::ustring& new_text){
      Gtk::TreePath path(path_string);
 //Get the row from the path:
      Glib::RefPtr<Gtk::TreeModel> model = treeview.get_model();
    Gtk::TreeModel::iterator iter = model->get_iter(path);
    if(iter){
      Gtk::TreeModel::Row row = *iter;
      std::string nm = new_text;
      row[modelcolumns.pathname] = nm;
      path_type* drawpath = row[modelcolumns.path];
      drawpath ->set_name(nm);
    }
    }
void PathViewWidget::setup_pathviewcell(){
  Gtk::TreeView::Column* col = Gtk::manage(new Gtk::TreeView::Column);
  col->set_title("View Path");
  col->pack_start(*pathrenderer);
  treeview.append_column(*col);
col->set_cell_data_func(*pathrenderer, 
			sigc::mem_fun(*this, &PathViewWidget::display_path_cell));
}
void PathViewWidget::setup_namecell(){ 
 Gtk::TreeView::Column* col = Gtk::manage(new Gtk::TreeView::Column);
 
      col->set_title("path name...");
      col->pack_start(nameeditor);
      treeview.append_column(*col);
      col->set_cell_data_func(nameeditor, 
			      sigc::mem_fun(*this, &PathViewWidget::display_name_cell));
#ifdef GLIBMM_PROPERTIES_ENABLED
      nameeditor.property_editable() = true;
#else
      nameeditor.set_property("editable", true);
#endif
      nameeditor.signal_editing_started().connect(sigc::mem_fun(*this, 
								&PathViewWidget::on_edit_pathname_cell));
      nameeditor.signal_edited().connect(sigc::mem_fun(*this, &PathViewWidget::on_edit_pathname_cell_done));
    }

double PathRenderer::get_scaleval(const simple_click_history& path, const Gdk::Rectangle& cellarea){
    if(path.size()< 2){ return 1.;} // avoid div by zero
    cRect rect;
    path.get_extents(rect);
    if(!(rect.width()) || !(rect.height())){ return 1.;} // its a straight line along an axis
    double xval = cellarea.get_width() / rect.width();
    double yval= cellarea.get_height() / rect.height();
    return xval > yval ? xval : yval;
  }
cairo_t*  PathRenderer::get_srccontext(Gtk::TreeModel::iterator iter){
  if(iter){
    Gtk::TreeModel::Row row = *iter;
    Pathviewcolumns* cols = get_columns();
    path_type* pathdata = row[cols->path];
    cairo_t* res =NULL;
    if(celldrawbuffers.find(pathdata) != celldrawbuffers.end()){
      std::cerr <<"PathRenderer::get_srccontextFOUND pathdata\n";
      res =  celldrawbuffers[pathdata];
    }
    cRect rect;
    simple_click_history* h = *(*pathdata);
    h->get_extents(rect);
    if(res){
      cairo_surface_t* surf = cairo_get_target(res);
      if(!(rect.width() > cairo_image_surface_get_width(surf) || 
	   rect.height() > cairo_image_surface_get_height(surf))){
	return res;
      }
    }
    cairo_t* old = res;
    cairo_surface_t* surf = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, int(rect.width() + 20), int(rect.height() + 20));
    res = cairo_create(surf);
    cairo_surface_destroy(surf);
    celldrawbuffers[pathdata] = res;
    if(old){cairo_destroy(old);}
    return res;
  }
  return NULL;
}

Pathviewcolumns* PathRenderer::get_columns(){
  if(widget){
    return widget->get_columns();
  }
  return NULL;
}

void PathRenderer::draw_path(Gtk::TreeModel::iterator& iter){
  if(iter){
    Gtk::TreeModel::Row row = *iter;

    Pathviewcolumns* cols = get_columns();
    path_type* pathdata = row[cols->path];
    simple_click_history* history = *(*pathdata);
    cairo_t* srccontext = get_srccontext(iter);
    //cairo_surface_t* surf = cairo_get_target(srccontext);
    cairo_save(srccontext);
    cairo_set_source_rgb( srccontext, 1,1,1);
    cairo_paint(srccontext);
    //gui
    cRect rect;
    history->get_extents(rect);
    coords offset (rect.x(), rect.y());
    simple_click_history drawpath = *history - offset;
    //TODO:guidraw.draw_editline(srccontext, drawpath,coords offset, coords scale, double rot);
    //
    guidraw.draw_editline(srccontext, drawpath);
    cairo_restore(srccontext);
  }
}

void PathPreview::reallocate_context(){
int canvaswidth = this->get_width();
      int canvasheight = this->get_height();
      cRect pathrect;
path->get_extents(pathrect);
 double zoomx = (double)canvaswidth / pathrect.x();
 double zoomy = (double)canvasheight / pathrect.y();
     init_ratio = std::max(zoomx, zoomy);
      
      if(pathcontext){
	cairo_destroy(pathcontext);
      }
      cairo_rectangle_t cairorect= cRect_to_cairo_rect(pathrect);
      pathcontext = create_pathcontext(cairorect);
      }

void PathPreview::draw_path(){
cRect rect;
 cairo_surface_t* surf = cairo_get_target(pathcontext);
    path->get_extents(rect);
    if(rect.width() > cairo_image_surface_get_width(surf) || rect.height() > cairo_image_surface_get_height(surf)){
	 reallocate_context();
    }
    cairo_save(pathcontext);
    cairo_set_source_rgba(pathcontext, 1,1,1,1);
    cairo_paint(pathcontext);
    //gui
    coords offset (rect.x(), rect.y());
    simple_click_history drawpath = *path - offset;
    guidraw.draw_editline(pathcontext, drawpath);
    cairo_restore( pathcontext);
  }

cairo_t*  PathPreview::create_pathcontext(const cairo_rectangle_t& pathrect){
      int pathwidth = std::max(100, int(pathrect.width +20));
int pathheight = std::max(100, int(pathrect.height +20));

      cairo_surface_t* outsurf = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, pathwidth,pathheight);
      set_surface(outsurf);
      
      cairo_t* result = cairo_create(outsurf);
cairo_surface_destroy(outsurf);
      return result;
      }



}
