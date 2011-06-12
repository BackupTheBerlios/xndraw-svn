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


#ifndef XNDRAW_NPATHFRAME_H
#define XNDRAW_NPATHFRAME_H

#include "xndrawexceptions.h"

#if HAVE_CONFIG_H
#include "config.h"
#endif


#include <iostream>
#include <gtkmm.h>
#include "clickhistory.h"
#include "guiutils.h"
#include "paintcontext.h"
#include "bufferalloc.h"
#include "cairopath.h"
#include "pathshape.h"
#include "pingbase.h"
//TODO: xn_Error(const std::exception& e, const std::string) constructor  


#define CATCH_AND_RETHROW catch(const std::exception& e){	\
    std::string s= e.what();					\
    throw xn_Error(s);						\
  }


namespace xNDRAW{

  class PathArea: public ping_base{
  public:
    typedef NCOORDS::simple_click_history path_type;
    typedef path_type::const_iterator const_iterator;
    typedef path_type::iterator iterator;
  protected:
    path_type* path;
    //NCOORDS::coords pathoffset;
    double pathscale;
    
    Cairo::RefPtr<Cairo::Context> cairo;
    ColourManager c_manager;
    PathArea(const PathArea&);
    PathArea& operator=(const PathArea&);
  public:
    virtual ~PathArea(){}
    PathArea();
    PathArea(unsigned int buffer_w, unsigned int buffer_h, 
	     cairo_format_t format,Pingbufferflags flags = PINGBUFFER_RESIZE_LARGER);

    virtual void reallocate_drawbuffer(const Gtk::Allocation& alloc);

    virtual void on_path_changed();

    void clear_path();
    virtual void set_path(path_type* pathnext);
    void set_pathscale();
    virtual bool 	on_expose_event (GdkEventExpose* event);
    virtual void on_size_allocate(Gtk::Allocation& alloc);
    double get_scaled_path_width()const;
    double get_scaled_path_height()const;
    virtual void set_source_from_buffer(Cairo::RefPtr<Cairo::Context> buffercontext=Cairo::RefPtr<Cairo::Context>(NULL));
    path_type get_drawable_path()const;
    void resize_buffer(unsigned w, unsigned h){
      buffer.resize(w, h, true);
    }
    void initialise_cairo();
void draw_and_paint_path( ColourManager& colourmanager,
			  const simple_click_history& drawpath);


    virtual void paint_buffer_to_window();
  };

  class PathWindow : public XnWindowBase{
  public:
    typedef NCOORDS::simple_click_history path_type;
  protected:
    //member data
    Gtk::VBox vbox;
    PathArea drawingarea;
    Gtk::HBox toolbox;
    //CairoBuffer backbuffer;
   path_type* path;
    path_type outputpath;
    sigc::connection init_connection;
   
    

    //unimplemented constructors/operator=
    PathWindow(const PathWindow&);
    PathWindow& operator=(const PathWindow&);

  public:
    PathWindow(Glib::ustring& argtitle, path_type* argpath);
    virtual ~PathWindow(){}

    //Cairo::RefPtr<Cairo::Context> get_gdkcairo();

    //CairoBuffer& get_backbuffer(); //?
    //const CairoBuffer& get_backbuffer()const;

    //Cairo::RefPtr<Cairo::Context> get_backbuffer_context();
    //Cairo::RefPtr<Cairo::Surface> get_backbuffer_surface();
    static void set_default_buffersize(unsigned int width,
				       unsigned int height);


    bool on_init_cairo();
    void initialise_cairo();
    virtual void on_path_changed();

    virtual void set_path(path_type* p);
    void draw_and_paint_path();

    virtual path_type get_output_path(){return *path;}
 //overloaded signal handlers for debug
    virtual void 	on_set_focus (Widget* focus){
      try{
	XnWindowBase::on_set_focus(focus);
      }
      CATCH_AND_RETHROW
	}
   
    virtual bool 	on_frame_event (GdkEvent* event){
      try{
	return XnWindowBase::on_frame_event(event);
      }
      CATCH_AND_RETHROW
	}
   
    virtual void 	on_add (Widget* widget){
      try{
	XnWindowBase::on_add ( widget);
      }
      CATCH_AND_RETHROW
	}
   
 
 
    virtual void 	on_remove (Widget* widget){
      try{
	XnWindowBase::on_remove ( widget);
      }
      CATCH_AND_RETHROW
	}
   
    virtual void 	on_check_resize (){
      try{
      XnWindowBase::on_check_resize ();
      }
      CATCH_AND_RETHROW
	}
   
    virtual void 	on_set_focus_child (Widget* widget){
      try{
	XnWindowBase::on_set_focus_child (widget);
      }
      CATCH_AND_RETHROW
	}

    virtual void 	on_show (){
      try{
	XnWindowBase::on_show();
      }
      CATCH_AND_RETHROW
	}


    virtual void 	on_hide (){
      try{
	XnWindowBase::on_hide() ;
      }
      CATCH_AND_RETHROW
	}
    virtual void 	on_map (){
      try{
	XnWindowBase::on_map () ;
      }
      CATCH_AND_RETHROW
	}

    virtual void 	on_unmap (){
      try{
	XnWindowBase::on_unmap () ;
      }
      CATCH_AND_RETHROW
	}

    virtual void 	on_realize (){
      try{
	Gtk::Window::on_realize () ;
      }
      CATCH_AND_RETHROW
	}

    virtual void 	on_unrealize (){
      try{
	Gtk::Window::on_unrealize () ;
      }
      CATCH_AND_RETHROW
	}

    virtual void 	on_size_request (Gtk::Requisition* requisition){
      try{
	Gtk::Window::on_size_request ( requisition) ;
      }
      CATCH_AND_RETHROW
	}

    virtual void 	on_size_allocate (Gtk::Allocation& allocation){
      try{
	/*unsigned int drawareawidth(allocation.get_width()), 
	  drawareaheight(allocation.get_height()), 
	  p_width(backbuffer.get_width()), 
	  p_height(backbuffer.get_height());
	// drawareawidth = allocation.get_width();
	//drawareaheight = allocation.get_height();
	if((drawareawidth > p_width)||(drawareaheight >  p_height)){
	  backbuffer.resize(drawareawidth, drawareaheight);
	}
      //if(path){
      */
	Gtk::Window::on_size_allocate (allocation) ;
	if(get_is_drawable()){
	  on_path_changed();
	}
      }
      CATCH_AND_RETHROW
	}

    virtual void 	on_state_changed (Gtk::StateType previous_state){
      try{
	Gtk::Window::on_state_changed ( previous_state) ;
      }
      CATCH_AND_RETHROW
	}

    virtual void 	on_parent_changed (Widget* previous_parent){
      try{
	Gtk::Window::on_parent_changed ( previous_parent) ;
      }
      CATCH_AND_RETHROW
	}

    virtual void 	on_hierarchy_changed (Widget* previous_toplevel){
      try{
	Gtk::Window::on_hierarchy_changed (previous_toplevel) ;
      }
      CATCH_AND_RETHROW
	}

    virtual void 	on_style_changed (const Glib::RefPtr< Gtk::Style >& previous_style){
      try{
	Gtk::Window::on_style_changed ( previous_style);
      }
      CATCH_AND_RETHROW
	}

    virtual void 	on_direction_changed (Gtk::TextDirection direction){
      try{
	Gtk::Window::on_direction_changed ( direction) ;
      }
      CATCH_AND_RETHROW
	}

    virtual void 	on_grab_notify (bool was_grabbed){
      try{
	Gtk::Window::on_grab_notify(was_grabbed) ;
      }
      CATCH_AND_RETHROW
	}

    virtual void 	on_child_notify (GParamSpec* pspec){
      try{
	Gtk::Window::on_child_notify( pspec) ;
      }
      CATCH_AND_RETHROW
	}

    virtual bool 	on_mnemonic_activate (bool group_cycling){
      try{
	return Gtk::Window::on_mnemonic_activate (group_cycling) ;
      }
      CATCH_AND_RETHROW
	}

    virtual void 	on_grab_focus (){
      try{
	Gtk::Window::on_grab_focus () ;
      }
      CATCH_AND_RETHROW
	}

    virtual bool 	on_focus (Gtk::DirectionType direction){
      try{
	return   Gtk::Window::on_focus (direction) ;
      }
      CATCH_AND_RETHROW
	}

    virtual bool 	on_event (GdkEvent* event){
      try{
	return   Gtk::Window::on_event(event) ;
      }
      CATCH_AND_RETHROW
	}

    virtual bool 	on_button_press_event (GdkEventButton* event){
      try{
	return   Gtk::Window::on_button_press_event(event) ;
      }
      CATCH_AND_RETHROW
	}

    virtual bool 	on_button_release_event (GdkEventButton* event){
      try{
	return   Gtk::Window::on_button_release_event(event)  ;
      }
      CATCH_AND_RETHROW
	}

    virtual bool 	on_scroll_event (GdkEventScroll* event){
      try{
	return  Gtk::Window::on_scroll_event(event) ;
      }
      CATCH_AND_RETHROW
	}

    virtual bool 	on_motion_notify_event (GdkEventMotion* event){
      try{
	return   Gtk::Window::on_motion_notify_event(event) ;
      }
      CATCH_AND_RETHROW
	}

    virtual bool 	on_delete_event (GdkEventAny* event){
      try{
	return   Gtk::Window::on_delete_event(event)  ;
      }
      CATCH_AND_RETHROW
	}
    void set_source_from_buffer(Cairo::RefPtr<Cairo::Context> buffercontext = Cairo::RefPtr<Cairo::Context>(NULL));
 
    virtual bool 	on_expose_event (GdkEventExpose* event);

    virtual bool 	on_key_press_event (GdkEventKey* event){
      try{
	return    Gtk::Window::on_key_press_event(event) ;
      }
      CATCH_AND_RETHROW
	}

    virtual bool 	on_key_release_event (GdkEventKey* event){
      try{
	return   Gtk::Window::on_key_release_event(event) ;
      }
      CATCH_AND_RETHROW
	}

    virtual bool 	on_enter_notify_event (GdkEventCrossing* event){
      try{
	return    Gtk::Window::on_enter_notify_event (event);
      }
      CATCH_AND_RETHROW
	}

    virtual bool 	on_leave_notify_event (GdkEventCrossing* event){
      try{
	return   Gtk::Window::on_leave_notify_event (event);
      }
      CATCH_AND_RETHROW
	}

    virtual bool 	on_configure_event (GdkEventConfigure* event){
      try{
	return   Gtk::Window::on_configure_event(event) ;
      }
      CATCH_AND_RETHROW
	}

    virtual bool 	on_focus_in_event (GdkEventFocus* event){
      try{
	return  Gtk::Window::on_focus_in_event(event) ;
      }
      CATCH_AND_RETHROW
	}

    virtual bool 	on_focus_out_event (GdkEventFocus* event){
      try{
	return  Gtk::Window::on_focus_out_event(event) ;
      }
      CATCH_AND_RETHROW
	}

    virtual bool 	on_map_event (GdkEventAny* event){
      try{
	return     Gtk::Window::on_map_event(event) ;
      }
      CATCH_AND_RETHROW
	}

    virtual bool 	on_unmap_event (GdkEventAny* event){
      try{
	return    Gtk::Window::on_unmap_event(event) ;
      }
      CATCH_AND_RETHROW
	}

    virtual bool 	on_property_notify_event (GdkEventProperty* event){
      try{
	return     Gtk::Window::on_property_notify_event(event) ;
      }
      CATCH_AND_RETHROW
	}

    virtual bool 	on_selection_clear_event (GdkEventSelection* event){
      try{
	return      Gtk::Window::on_selection_clear_event(event) ;
      }
      CATCH_AND_RETHROW
	}

    virtual bool 	on_selection_request_event (GdkEventSelection* event){
      try{
	return Gtk::Window::on_selection_request_event(event) ;
      }
      CATCH_AND_RETHROW
	}

    virtual bool 	on_selection_notify_event (GdkEventSelection* event){
      try{
	return Gtk::Window::on_selection_notify_event (event) ;
      }
      CATCH_AND_RETHROW
	}

    virtual bool 	on_proximity_in_event (GdkEventProximity* event){
      try{
	return Gtk::Window::on_proximity_in_event(event) ;
      }
      CATCH_AND_RETHROW
	}

    virtual bool 	on_proximity_out_event (GdkEventProximity* event){
      try{
	return Gtk::Window::on_proximity_out_event (event) ;
      }
      CATCH_AND_RETHROW
	}

    virtual bool 	on_visibility_notify_event (GdkEventVisibility* event){
      try{
	return Gtk::Window::on_visibility_notify_event (event) ;
      }
      CATCH_AND_RETHROW
	}

    virtual bool 	on_client_event (GdkEventClient* event){
      try{
	return Gtk::Window::on_client_event(event) ;
      }
      CATCH_AND_RETHROW
	}

    virtual bool 	on_no_expose_event (GdkEventAny* event){
      try{
	return Gtk::Window::on_no_expose_event(event) ;
      }
      CATCH_AND_RETHROW
	}

    virtual bool 	on_window_state_event (GdkEventWindowState* event){
      try{
	return Gtk::Window::on_window_state_event(event); ;
      }
      CATCH_AND_RETHROW
	}

    virtual void 	on_selection_get (Gtk::SelectionData& selection_data, guint info, guint time){
      try{
	Gtk::Window::on_selection_get(selection_data, info, time) ;
      }
      CATCH_AND_RETHROW
	}

    virtual void 	on_selection_received (const Gtk::SelectionData& selection_data, guint time){
      try{
	Gtk::Window::on_selection_received (selection_data, time) ;
      }
      CATCH_AND_RETHROW
	}

    virtual void 	on_drag_begin (const Glib::RefPtr< Gdk::DragContext >& context){
      try{
	Gtk::Window::on_drag_begin (context) ;
      }
      CATCH_AND_RETHROW
	}

    virtual void 	on_drag_end (const Glib::RefPtr< Gdk::DragContext >& context){
      try{
	Gtk::Window::on_drag_end (context) ;
      }
      CATCH_AND_RETHROW
	}

    virtual void 	on_drag_data_get (const Glib::RefPtr< Gdk::DragContext >& context, Gtk::SelectionData& selection_data, guint info, guint time){
      try{
	Gtk::Window::on_drag_data_get (context, selection_data, info, time) ;
      }
      CATCH_AND_RETHROW
	}

    virtual void 	on_drag_data_delete (const Glib::RefPtr< Gdk::DragContext >& context){
      try{
	Gtk::Window::on_drag_data_delete(context) ;
      }
      CATCH_AND_RETHROW
	}

    virtual void 	on_drag_leave (const Glib::RefPtr< Gdk::DragContext >& context, guint time){
      try{
	Gtk::Window::on_drag_leave (context, time) ;
      }
      CATCH_AND_RETHROW
	}

    virtual bool 	on_drag_motion (const Glib::RefPtr< Gdk::DragContext >& context, int x, int y, guint time){
      try{
	return Gtk::Window::on_drag_motion(context, x, y, time) ;
      }
      CATCH_AND_RETHROW
	}

    virtual bool 	on_drag_drop (const Glib::RefPtr< Gdk::DragContext >& context, int x, int y, guint time){
      try{
	return Gtk::Window::on_drag_drop(context, x, y, time) ;
      }
      CATCH_AND_RETHROW
	}

    virtual void 	on_drag_data_received (const Glib::RefPtr< Gdk::DragContext >& context, int x, int y, const Gtk::SelectionData& selection_data, guint info, guint time){
      try{
	Gtk::Window::on_drag_data_received (context,x,y,selection_data,info, time) ;
      }
      CATCH_AND_RETHROW
	}

    virtual Glib::RefPtr< Atk::Object > 	on_get_accessible (){
      try{
	return Gtk::Window::on_get_accessible () ;
      }
      CATCH_AND_RETHROW
	}

    virtual void 	on_screen_changed (const Glib::RefPtr< Gdk::Screen >& previous_screen){
      try{
	Gtk::Window::on_screen_changed ( previous_screen) ;
      }
      CATCH_AND_RETHROW
	}

    //end overloaded signal handlers for debug	

  };

  class nPathWindow : public PathWindow{
  public:
    typedef NCOORDS::nPathShape shape_type;
  protected:

    shape_type shape;

    //unimplemented constructors/operator=
    nPathWindow(const nPathWindow&);
    nPathWindow& operator=(const nPathWindow&);

  public:
  nPathWindow(Glib::ustring& argtitle, path_type* argpath,
	      const ncoords& nsides);

    virtual ~nPathWindow(){}

    virtual void set_path(path_type* p);
    virtual path_type get_output_path();

  };

} //namespace xNDRAW{
#endif //XNDRAW_NPATHFRAME_H
