


#ifndef X3DRAW_PINGWINDOW_H
#define X3DRAW_PINGWINDOW_H
#include "xndrawexceptions.h"

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <gtkmm.h>
#include "pingbase.h"
#include <gdk/gdkcairo.h>
#include <iostream>
#include <set>
#include <glibmm/date.h>
#include "x3draw.h"
#include "colourchooser.h"
#include "pngsave_window.h"
#include "nodes.h"
#include "nodesmap.h"
#include "drawingset.h"
#include "drawtool.h"
#include "drawbuffer.h"
#include "x3drawtoolbox.h"
#include "ratioflower.h"
#include "opsequence.h"

#include "fmatrix.h"
#include "typedefs.h"
/**
@file ping_window.h
declares GUI for Drawing* classes from ../libcairoXndrawing
using gtkmm and cairo(mm)
 */

namespace xNDRAW{

  
  
template <class EDITWIDGET>
struct EditInfo:public Glib::Object{


    EDITWIDGET* widget;

    int mode;
  SelectBox selector;
  bool dragging;
  //@member selection=>should be a simple_click_history or Drawimgbase attribute
  //currently does nothing  
NCOORDS::NcoordSelection<NCOORDS::coords*> selection;
 EditInfo(EDITWIDGET* w, EditMode startmode):widget(w),
  mode(startmode),
  dragging(false){  }
  };


  template<typename GTKCONTAINER, typename GTKWIDGET>
  void container_add_widget(GTKCONTAINER& container, GTKWIDGET& widget){
    container.add(widget);
  }
  
  class ping_window: public ping_base
  {
    ping_window(const ping_window&);
    ping_window& operator=(const ping_window&);
  public: 
    bool event_in_self(GdkEventButton* event);
    ping_window(int  base =3, double xscale=1.0, double yscale=1.0);
    virtual ~ping_window(){
      if(draw){
      delete draw;
      draw=NULL;
      }
}

    typedef sigc::signal<void> Draw_initialisation_signal;
    Draw_initialisation_signal drawinitsignaller;
  protected:
    static bool pingwindow_fix_nodes;
    int status;//ie drawbuffer status

    double scale_x;
    double scale_y;    
  
    //Drawingbase* would be better?  
    Drawing* draw;
    
    Drawtool* pencil;
   
    //pngoutputbuffer really should just belong to draw?
    //if required at all - can create as required?
    CairoBuffer pngoutputbuffer;    
    std::string png_sessionstring;
    Glib::ustring current_folder;
    
    int savecount;
    double zoom_ratio;
    sigc::connection ignition_connection;
    EditInfo<ping_window> editinfo;
    DrawViewport viewconverter;

    DrawRecorder::repository_type opsequence_store;
    DrawRecorder sequence_recorder;
    
    sigc::signal<void> m_path_update_signal;
sigc::signal<void> m_draw_update_signal;
  public:

    sigc::signal<void> signal_path_updated();
    sigc::signal<void> signal_draw_updated(){
      return m_draw_update_signal;
    }
    void set_draw(Drawing* d){
      draw=d;
      m_draw_update_signal.emit();
    }
    void recorder_set_draw(Drawingbase* d);

    void on_sigc_pngwrite(std::string s);

    void on_cleanup_path();
    /**
add a copy of a path to draw's list, and set it as the current path.
     */
    void set_history(const simple_click_history& path){
      if(!draw){	throw xn_Error("uninitialised");  }
      draw->initialise_path(path);      
    }

    //TEST FUNCTIONS FOR PATH COMBINING
    //remove when saner alternatives avail.
    void do_rand_AND(){
      do_rand_combine(PATH_COMBINE_AND);
    }
void do_rand_OR(){
      do_rand_combine(PATH_COMBINE_OR);
    }
void do_rand_NOTAND(){
      do_rand_combine(PATH_COMBINE_NOTAND);
    }
 void do_rand_DIFF(){
      do_rand_combine(PATH_COMBINE_DIFF);
    }
   void do_rand_combine( int combineflag);
    /**
combine draw->history with the path in draw's cache with the index pathindex,
if there is a result, set it as history
     */
    bool combine_with_indexed_path(unsigned int pathindex,
				   int combineflag);

    void on_cleanup_paths();
    void on_redraw_bg_line();
    void on_redraw_line();
    void on_draw_import_paths(const std::string& fname);
    void on_bg_render_svg(std::string f);
    void on_bg_render_png(std::string f);

    void draw_import_png(std::string fname);
    virtual void on_svgpathsread(std::string fname);
    void on_draw_svgsave(std::string fname);

    void reset_nodesbase(unsigned int i);

    void on_altpngsave_write(std::string fname);
    void draw_save_svg_projectpaths(std::string fname);

    void sigc_scale_map(coords pt);
    void drawset_scale_map(const coords& pt);
    void drawset_scale_map(double x);
    void drawset_scale_map(double x, double y);
    void sigc_map_upendcoords(){drawset_map_upendcoords();}
    void drawset_map_upendcoords(size_t index=0);
    virtual void on_transform_signal(Cairo::Matrix& matr);
    Drawingbase* get_draw();
bool update_screen();
    void on_clean_path2();
    ////////////
    //transform signal handlers   
    virtual void on_scalepreview_update(std::pair<coords, coords>& points);
    virtual void  on_scale_finish(std::pair<coords, coords>& points);
    virtual void  on_rotatepreview_update(std::pair<double,  coords>& rotargs);
    virtual void  on_rotate_finish(std::pair<double,  coords>& rotargs);
    virtual void  on_translatepreview_update(std::pair<coords, int>& transargs);
    virtual void  on_translate_finish(std::pair<coords,int>& transargs);

    void set_pencil_transform_slots();
    coords get_path_centre()const;
    void set_pencil_reference_hook();

    void set_pencil_slots();

    ////////////
    virtual bool on_button_press_event(GdkEventButton* butt);
    virtual bool on_button_release_event(GdkEventButton* event);
    virtual bool on_key_press_event(GdkEventKey* event);
    void on_drawtool_signal_coords(xNDRAW::click_pair_type);
    void on_drawtool_signal_origin(xNDRAW::click_pair_type);
void on_selecttool_signal_coords(xNDRAW::click_pair_type);

    void flip_skip_even();
void flip_skip_odd();

    std::deque<simple_click_history*> get_saved_paths(){
      return draw->get_saved_paths();
    }

 void init_pencil(int penciltype = XNDRAW_DRAWING_SIMPLE_CLICK);
    void make_drawtool_bezier();
    void make_drawtool_select();
    void make_drawtool_free();
    void make_drawtool_simple_click();
    void make_selectboxtool();
    void reset_drawtool();

    void init_drawset(const Nodes& seed);
    void replace_nodes(NCOORDS::Nodedeq& swapme);
    void reinit_drawset(Nodes& list);
    void reinit_drawset_again();
    Nodedeq create_drawset(const NCOORDS::coords&, const NCOORDS::coords&, int);
    bool save_nodes(const std::string& filename);
    void on_save_nodes(const std::string& filename){save_nodes(filename);}
    Glib::ustring get_current_folder(){
      //need a map of current_folders(export, import, clickfiles(ie protoproject) - see dialogmanager
      //Maybe a simple class wrapping this as well as generic dialog handler
      return current_folder;
    }
    void set_current_folder(const Glib::ustring& foldername){
      current_folder = foldername;
    }

    std::deque<NCOORDS::simple_click_history>& get_saved_paths_ref();
    void on_draw_export_path(const std::string& fname){
      //path, or paths: exmine the code for draw
      std::ofstream outfile(fname.c_str(), std::ios_base::out);
      draw->write_history_to_stream(outfile);
    }

    void on_nodes_file_read(const std::string& fname);
    virtual void on_background_import(const std::string& filename){
      throw xn_Error("do something with this");
      //either bg = Background::create_from_file(filename);
      //need a layer abstraction, and method to composite them on buffer
    }
    surfPtr  clone_cairosurface();
    virtual void init_cairo();
 
    void on_png_bung(FileDialogManager* d_man);
  

    bool get_has_cairo(){
      return (bool)draw;
    }
    std::string get_pngstring();
 void fill_preserve(cairoPtr& cairo){
      
cairo_fill_preserve(cairo->cobj());
}
    
    
    void zoom_cairo(double scale);

    void zoom_IN();

    void zoom_OUT();

    void set_zoom_ratio(double ratio);

    void flip_inverted();

    void draw_output_svg(const std::string& filename){
      int widehigh(2000);
	draw->write_svg_output_buffer(filename, widehigh, widehigh);
    }
    double get_zoom_ratio();
    void on_get_filename_to_save(std::string filename);
    void on_signal_rgba(Draw_colour background);
    int on_move_deviceX(double dx);
    int on_move_deviceY( double dy);
    void fill();
    void print_cairo_status(Cairo::Context* cr, std::string context_string);
    void print_cairo_status (Drawing* cr, std::string context_string);
    virtual   bool get_verbose(){return X3_draw::instance().get_verbose();}

  void on_coordsdata_save();//drawbuffer business
    void on_coordsdata_read();
 
  
    bool on_init();
    //cairo_t* get_gdkcairo();



    void on_colour_signal(float red, float green, float blue);

    int get_baseno() {
      return draw->get_baseno();
    }

    virtual void on_realize();
   

    void flip_reversed();
    void flip_drawset_correct();
  void on_signal_rgba_back(Draw_colour background);

    
    typedef sigc::signal<void, bool> mousedown_signal;
 protected:
    mousedown_signal m_signal_mousedown;
  public:

 inline void set_fill_rule(Cairo::FillRule rule){
   draw->set_fill_rule(rule);
 }
inline void set_line_cap(Cairo::LineCap cap){
  draw->set_line_cap(cap);
}

 inline void set_operator(Cairo::Operator op){
   draw-> set_operator(op);
 }
    mousedown_signal signal_mousedown(){ return m_signal_mousedown;}
    
  
    bool on_drawing_timeout();
 
    void on_filename_sent(std::string f_name);
    virtual void paint_white(Cairo::RefPtr<Cairo::Context>& cairo){
      cairo->save();
      cairo->set_source_rgb(1,1,1);
      cairo->paint();
      cairo->restore();
    }
 virtual void paint_white(Drawingbase* cairo){
      cairo->paint_white();
    }
    void really_init_rosydraw(const coords& origin, 
		       simple_click_history path =simple_click_history());

    virtual   void init_rosydraw();
    virtual   void clear_cairo();

    void clear_to_background(Drawing* dra);
    //new colour/pattern setting 
    //signal handlers(calling methods of draw->colourmanager)
    void  on_colourchooser_signal_rgba(const Draw_colour& col, ColourRole role){
      ColourManager& colourman = draw->get_colour_manager();
      std::cerr<<"on_colourchooser_signal_rgba\ncolour channels: "<<col
	       <<"\nrole: "<< (role == COLOURROLE_STROKE ? "STROKE" : 
			       (role == COLOURROLE_FILL ? "fill" :
				(role == COLOURROLE_BG ? "background" : 
				 "none"))) << std::endl;
      colourman.set_colour(col, role);
    }
    void on_colourchooser_signal_pattern(const PatternColour& col, 
					 ColourRole role);
    ///

    void  save_pngfile(std::string folder);
    void  save_pngfile();
 virtual   bool on_expose_event(GdkEventExpose* event);
    void fill_history();
    void fill_preserve_history();

    void stroke_history();

    void stroke_preserve_history();

    void set_source_to_fill();

    virtual void paint_buffer_to_window();

    coords get_default_coords();

    coords get_default_offset();

    void clear_current_history();

    void reset_draw(int point_no);
    void adjust_stroke_alpha(double alpha);

    void set_linewidth(double lw);
    void set_line_join(Cairo::LineJoin);
void reallocate_drawbuffer(Gtk::Allocation&);
    
    //void on_size_allocate(Gtk::Allocation&);
    
    void on_signal_rgb(double r,double g,double b);

    

    drawPtr get_draw_obj();

    Cairo::RefPtr<Cairo::ImageSurface> get_surf();

 virtual bool on_motion_notify_event(GdkEventMotion* event);


  };
}/*namespace xNDRAW*/


#endif /*X3DRAW_PINGWINDOW_H*/
