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


#ifndef X3DRAW_HFRAME_H
#define X3DRAW_HFRAME_H
#include "xndrawexceptions.h"

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <gtkmm.h>

#include "colourchooser.h"
#include "linewidth.h"
#include "x3draw.h"
#include "pingbuffer.h"
#include "drewadit.h"
#include "imageview.h"
#include "pathview.h"
#include "guiutils.h"
#include "xnmenus.h"
#include "npathframe.h"

namespace xNDRAW{

  class h_frame;

  class CoordmapSetter:public Gtk::Frame{
  public:

    enum Indices{
      ROT = 0,
      XSCALE =1,
      YSCALE =2,
      XTRANSLATE =3,
      YTRANSLATE =4
    };
 
    typedef sigc::signal<void, coords> coords_signal;
    typedef sigc::signal<void,double> double_signal;
    typedef std::pair<Gtk::SpinButton*, Gtk::Adjustment*> entry_type;
  protected:
    Gtk::VBox boxframe;
    Gtk::HBox scale_box;
    Gtk::VBox scale_slider_box;
    Gtk::HBox transform_box;
    Gtk::VBox transform_slider_box;
    
    ///
    std::map<int, entry_type> adjusters;
    Gtk::RadioButton scale_entrain_button, upendcoords_button;

    coords_signal translation_signaller;
    coords_signal scale_signaller;
    double_signal rotation_signaller;
    coords translation;
    coords scaler;//100% size is (1.0, 1.0)
    double rotval;
    bool scale_xy_entrained;
   
  public:
    CoordmapSetter();

    entry_type create_spinner(double value, double min, 
			      double max, double step =1.);

    void create_spinners();

    void on_rotadjust_changed(){}

    void on_xscale_changed();
    void on_yscale_changed();
    
    void on_toggle_xyscale_entrained(){
      scale_xy_entrained = !scale_xy_entrained;
    }

    void on_xtranslate_changed(){}
    void on_ytranslate_changed(){}

    sigc::connection connect_upendcoords(sigc::slot<void> slot){
      return upendcoords_button.signal_toggled().connect(slot);
    }

    sigc::connection connect_rotator(sigc::slot<void, double> slot){
      return rotation_signaller.connect(slot);
    }

    sigc::connection connect_scaler(sigc::slot<void, coords> slot){
      return scale_signaller.connect(slot);
    }

    sigc::connection connect_translator(sigc::slot<void, coords> slot){
      return translation_signaller.connect(slot);
    }

    

  };
  


  struct AboutDialog: public Gtk::AboutDialog{
    
    AboutDialog();
  };
  /**TODO: replace hand packing of buttonboxes with 
   *register_button, and arrange_buttons in Ctor
   *and add call to arrange_buttons in exposeevent
   *if button_bin.size() changed
   *maybe inherit from rather than invoke deq<Button>
   *and add a bool changed to it
   */

  class PathSelector: public Gtk::Frame{
  protected:
    //2.3.7
    //added parent - call draw via the widget
    //drawtool needs reset
    h_frame* parent;
    Drawingbase* draw;
    Gtk::VBox pathbox;
    Gtk::HBox subbox;
    Gtk::Label label;
    Gtk::Button path_upper, path_lower;
    unsigned int pathindex;
  public:
    PathSelector(h_frame* parentwidget):
      parent(parentwidget), draw(NULL),
      path_upper("next"),
      path_lower("previous"){
      add_and_draw_all();
    }
    /*  PathSelector(Drawingbase* d=NULL):parent(NULL),draw(d),
	path_upper("next path"),
	path_lower("previous path"){
	add_and_draw_all();
	}*/
    void add_and_draw_all();

    void set_draw(Drawingbase* d);
    
    void initialise();
    void update_label();
    
    void  on_toggle_lower();
    void on_toggle_upper();

  }; 

  class h_frame: public Gtk::Window{ 
  public:
    typedef sigc::signal<void,std::string> string_signal;
    
  protected:
    
       
    std::deque<NCOORDS::simple_click_history> set_of_paths;
    
    Gtk::ScrolledWindow scrolledwindow;
    Gtk::VBox vframe;
    Gtk::HBox hframe; 
    Gtk::VBox v_box; 
    ping_buffered* buffd_gtk_surf; //why a pointer?
  
    
    Gtk::HBox top_hbox;
    Gtk::VBox drawtoolchooser_box;
    Gtk::VBox topbox_sub_box2;
    Gtk::VBox colourbuttonbox;
    //Gtk::Label label
    //proxy colour buttons
    //TODO: button update callbacks
    Gtk::ColorButton strokebutton;
    Gtk::ColorButton fillbutton;
    Gtk::ColorButton bgbutton;
    ////////////
    CoordmapSetter map_adjuster;
    Gtk::HBox bung_box;
    ButtonFrame buttonframe;
    Gtk::Button init_draw_butt;   
    Gtk::Button use_coordset_butt;
    PathSelector pathselector;    
    Gtk::Button png_bung;

    linewidth l_wideness;
    Matrix_fucker* m_fucker;
    X3drawtoolbox x3tool;
     
    
  
    Gtk::Button  cairo_clear_butt;

    Gtk::Button Draw_butt;
    Gtk::Button Freedraw_butt;
    Gtk::Button Bezier_butt;
    Gtk::Button Select_butt;

    FloatingToolbox pointertools;
    //PointerToolBox pointertools;
    //end quarantine
    Previewer  preview;//?

    Glib::RefPtr<uiManager> ref_ui_manager;
    FileDialogManager dialogmanager;
 
    std::deque<Gtk::Button*> button_bin;
    //move these - try and setup all sigs in action classes,
    //for easy access of menus;
    string_signal m_signal_bg_svgsave;
    string_signal m_signal_bg_pngsave;
    string_signal m_signal_pngwrite;
  public: 
    string_signal signal_pngwrite()const{return m_signal_pngwrite;}
    string_signal signal_bg_svgsave()const{return m_signal_bg_svgsave;}
    string_signal signal_bg_pngsave()const{return m_signal_bg_pngsave;}

    h_frame();
    virtual ~h_frame();

    static std::pair<unsigned int, unsigned int> 
    get_framesize(Glib::RefPtr<Gdk::Screen>& outscreen, 
		  Gdk::Rectangle& monitor);

    bool verbose(){
      return X3_draw::instance().get_verbose();
    }

    void on_show_about_dialog()const;

    FileDialogManager& get_dialog_manager();

    Matrix_fucker* get_fucker(){
      return m_fucker;
    }
    //

    void on_svg_to_native_convert(){
      dialogmanager.do_filedialog_slot(*this, XNDRAW_SVGPATH, "convert A svg to native file", sigc::mem_fun(*this, &h_frame::svg_to_native_get_native), Gtk::FILE_CHOOSER_ACTION_OPEN);
    }
    void svg_to_native_get_native(const std::string& svgfname){
      dialogmanager.do_filedialog_slot_with_arg(*this,XNDRAW_PATH, 
						"choose a filename for native output",
						sigc::signal<void, std::string, std::string>(),
						sigc::mem_fun(*this,
							      &h_frame::do_svg_to_native_paths), 
						svgfname, Gtk::FILE_CHOOSER_ACTION_SAVE);
    }
    void do_svg_to_native_paths(const std::string&  native,
				const std::string& svgfname);


    void on_draw_initialised();
    void on_reinit_drawset();
    void on_init_rosydraw();
    void on_pathframe_create();
    void on_svg_file_export();
    void on_import_svg_paths(const std::string& fname);
    void on_svg_fileget();
    void on_native_path_import();
    void connect_drawtool_selectors();
    void reset_drawtool();
    void on_png_file_import();
    void on_png_bung();
    void on_edit_sys_copy();
    void on_edit_sys_cut();
    void on_edit_sys_paste();

    void on_view_saved_paths();

    void register_button(Gtk::Button* butt);
    void arrange_buttons();
    void on_bg_svgsave();
    void on_bg_pngsave();

    ping_buffered& get_pingbuffer(){
      return *buffd_gtk_surf;
    }

    void on_png_save();
    void on_transform_signal(Cairo::Matrix& matr);
    void pack_drawtoolchooser();

    void create_uiManager();

    void on_action_quit();
    
    void on_nodes_save();
   
    void on_nodes_read();
    
    void on_init_preview();
    void on_init_draw();
    void on_realize();

    void x3tool_connect_signals();

    std::deque<NCOORDS::simple_click_history>& get_paths();
    //bool on_init();
    
    bool on_button_release_event(GdkEventButton* event);
    
    void add_path(NCOORDS::simple_click_history path);
    
    bool on_idle();
    
    void launch_drawaddikt();
    
    Gtk::Window* make_window_for_widget(Gtk::Widget* contents);
    
    Glib::RefPtr<Gtk::Window> create_drawedit_window(int width =400,int height =400,drawedit_widget* drew= NULL); 
  
    static XndrawMap<int, std::string> create_pointertoolinfo();
  };

 
  
}

#endif /*X3DRAW_HFRAME_H*/
