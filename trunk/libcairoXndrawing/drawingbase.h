// -*- C++ -*-

/* Copyright (C)2006-2011 John O'Driscoll - biggerbumme@ yahoo.co.in
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */


#ifndef DRAWINGBASE_H
#define DRAWINGBASE_H

#include "exceptions.h"

#if HAVE_CONFIG_H
#include "config.h"
#endif
//#if HAVE_PARALLEL_PARALLEL_H
//#include <parallel/parallel.h>
//#endif


#include <cairomm/context.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <typeinfo>
#include <bitset>
#include "xndrawexceptions.h"

#include "paintcontext.h"
#include "x3draw.h"
#include "clickhistory.h"
#include "cairopath.h"
#include "typedefs.h"
#include "selection.h"
#include "nodesmap.h"
#include "boundinstruction.h"
#include "bufferalloc.h"
#include "guidraw.h"
#include <glibmm/main.h>

namespace xNDRAW{

class Drawingbase;
  class CairoConfiguration;

  //add new as more patterns added
  //dynamic system via prefs would be better
  //allow for a libload mechanism
 enum XndrawPatternType{
   XNDRAW_PATTERN_TYPEERROR =-1,
   XNDRAW_PATTERN_NONE=0,
    XNDRAW_PATTERN_DRAW=1,
    XNDRAW_PATTERN_DRAWSET=2,
   XNDRAW_PATTERN_MOSAIC=4,
   XNDRAW_PATTERN_DLL=8 //toDO
  };

std::istream& operator>>(std::istream& instream, CairoConfiguration& conf);
std::ostream& operator<<(std::ostream& instream, const CairoConfiguration& conf);

  template<typename T>
  class Constrained{
    T max;
    T min;
    bool use_max, use_min;//...
  };

  /**
   * savable loadable config for a drawing context
@todo: make me work
   *
*/
  class CairoConfiguration{
    //Finish and apply
    //complete set of values, with subsets usable?
    //(subsets:an enum with ORable values, one for each in the complete set
    //and a state integer to hold the value)
    //to be saved,loaded and applied
    // via operator<<
    // or maybe operator<< for StateSerialiser<Configuration>, or StateSerialiser<Cairo::Context>
  protected:
    ///@todo: a constraints regime for these vars
    double linewidth;//must be >=0
    double trans; //in range 0 - 1
    Cairo::Operator operatore; // save as  int
    Cairo::LineJoin linejoin; // save as int
  public:
    CairoConfiguration(bool loadprefs=true);//generate from prefs
    friend std::istream& operator>>(std::istream& instream, CairoConfiguration& conf);
friend std::ostream& operator<<(std::ostream& instream, const CairoConfiguration& conf);

    friend class Drawingbase;
    //problem is, GUI callbacks aren't called to update the interface
    //will need sigc hooks
    void apply(Drawingbase& apply_to)const;
    static CairoConfiguration create(const Drawingbase& source);
    void set_line_width(double lw);
    void set_transparency(double val);
  };

  void xn_cairo_stroke_fill(cairo_t* cr);
  void xn_cairo_fill_stroke(cairo_t* cr);

  class FileInfo{
    //no point to instantiate-just use its static functions
  protected:
    FileInfo(){}
    FileInfo(const FileInfo&){}
    FileInfo& operator=(const FileInfo&){return *this;}
  public:
    //get_extension for file creation
    //use globstring/Gtk::FileFilter to check type of existing file
    //or some check that does the same
    static std::string get_filetypename(xnFileType tp);
    static std::string get_extension(xnFileType tp);
    static std::string get_mimetype(xnFileType tp);
    static std::vector<std::string> get_globstrings(xnFileType tp);

    static std::string get_validtypes_string();  
  };


  typedef  NCOORDS::PointerMode PointerMode;
  using NCOORDS::XNDRAW_NO_CLICKDATA;
  using NCOORDS::LINE_CLICK;
  using NCOORDS::MOVE_CLICK;
  using NCOORDS::BEZIER_CLICK;
  using NCOORDS::BEZIER_HANDLE;
  using NCOORDS::CLOSE_PATH;
  using NCOORDS::ORIGIN_CLICK;
  using NCOORDS::OFFSET_CLICK;
  using NCOORDS::NULL_CLICK;
  using NCOORDS::XNDRAW_CLICK_BAD_DATA;
  using NCOORDS::CLICK_ERROR;
  
  typedef NCOORDS::XNPathError XNPathError;
  typedef Cairo::RefPtr<Cairo::Pattern> PatternPtr;
  

  cairo_rectangle_t coords_create_cairo_rect(const coords& a, const coords& b);

  class BackgroundDraw: public Glib::Thread{
    //set up signals from mainloop and dispatch drawing commands
    //put the pattern drawing there to to avoid a zillion communicatons
    // for each move_to() etc
    // 
  protected:
    Glib::RefPtr<Glib::MainLoop> runloop;
    Drawingbase* draw;
    cairo_t* context;
  };


  class Cairo_c_operation: public std::unary_function<cairo_t*, void>{
  public:
    typedef void (*Cairo_op)(cairo_t*);
  protected:
    Cairo_op op;

  public:
    Cairo_c_operation(Cairo_op opp): op(opp){}
    void operator()(cairo_t* cr)const{
      (*op)(cr);
    }

  };
  using NCOORDS::Last3Clicks;
  // typedef std::pair<std::pair< NCOORDS::coords, int>, std::pair< NCOORDS::coords, int> > Last2Clicks;

  std::ostream& operator<< (std::ostream& os , const Drawingbase& draw);

  bool operator==(const Drawingbase& left, const Drawingbase& right);
  bool operator!=(const Drawingbase& left, const Drawingbase& right);
  bool operator< (const Drawingbase& left, const Drawingbase& right);
  bool operator>(const Drawingbase& left, const Drawingbase& right);
  bool operator<=(const Drawingbase& left, const Drawingbase& right);
  bool operator>=(const Drawingbase& left, const Drawingbase& right);
	
  bool xndraw_has_svgcairo();

  paintmode& operator|=(paintmode& left, const paintmode& right);
  paintmode& operator&=(paintmode& left, const paintmode& right);
  paintmode& operator^=(paintmode& left, const paintmode& right);
  void set_source_and_do_paint_cmd(cairo_t* dest, cairo_t* src, 
				   void(*fn)(cairo_t*));
  void set_source_and_paint(cairo_t* dest, cairo_t* src);

  class Drawingbase : public Cairo::Context{
  public:
    typedef std::pair<NCOORDS::coords, int> click_type;
    typedef simple_click_history::iterator iterator;
    typedef simple_click_history::const_iterator const_iterator;
    typedef simple_click_history path_type;
    typedef std::pair<simple_click_history, SourceTransformInfo> rendersource_type;
    typedef std::deque<rendersource_type> path_container_type;
    typedef path_container_type::iterator path_container_iterator;
    typedef path_container_type::const_iterator const_path_container_iterator;

    typedef size_t size_type;
  protected://TODO: more settings:
    //buffer- cairo surface from a CairoBuffer(held in ping_base)
    //visual - cairo of an onscreen Gtk::DrawingArea(ie ping_something)
    //               obtained from gdk
    //m_cobject - the actual cairo_t* member of Cairo::Context (the base class)
    //            which Drawingbase trickily switches between visual and buffer

    //CHANGE To, maybe- get rid of visual to ping* classes,
    //- which are also the source of buffer - where
    //the cairo_t* that is now visual 
    //could be obtained afresh and updated in pingthing->on_expose_event() etc,
    //but what to do with guidraw, which draws the source path, and selection
    //and transformation effects/preview direct to visual

    //          _OR_

    //put buffer/visual/m_cobject into a base class?
    //
    //override Cairo::Context methods there, 
    //so drawing/painting is seamless in this class,
    //where coords and history can be introduced
    //so , (possibly), the draw commands cumulatively keep records of their extents
    //through a pair of path iterators (class SubPath)
    //until some flushing command like stroke() or fill()
    //or(more likely), get a copy of the path b4 such cmd,
    //then only paint the updatedareas to buffer and hence visual
    //all the calculating might make it no speedup
    //compositing mode at each level
    static unsigned int serialcount;
    const unsigned int  serial;
    static int initialised;
    cairo_t* visual;
    CairoBuffer bufferdata;
    //CairoBuffer scratchdata;
    cairo_t* buffer;
    cairo_t* scratch;//

    // just open file, paint from buffer, and close
    //for svg paths, create a type witha cairo_t, and an OpSequence,
    //and use that to drive drawing: so we don't need a persistent svg,
    //just a reliable OpSequence recorder, that just records 'toplevel' calls(ie we don't record the methods they call), and mechanism to switch on and off

    //rewrite 'buffer'/visual  code
    //for new model with 3 buffers('buffer'/visual/m_cobject
    //basics converted, but need to look at the subtleties

    //TODO: ptr to parent CairoBuffer - to avoid a stale 'buffer' on reallocation - for now we avoid reallocating
    
    paintmode drawto_now;// using backbuffer?!?
  
    ColourManager colourmanager;
    NCOORDS::simple_click_history* history;
    //changed from unsigned
    int pathindex;
    Last3Clicks last3clicks;

    std::deque<NCOORDS::simple_click_history> saved_paths;
    GuiDraw guidraw;

    sigc::signal<void> m_signal_savedpaths_changed;
    //constructors
  public:
    Drawingbase (cairo_t* cr, cairo_surface_t* bakbuffer);
    Drawingbase (cairo_t* cr, cairo_surface_t* bakbuffer, const simple_click_history& clix);
    Drawingbase (Cairo::RefPtr<Cairo::Surface>& srf, cairo_surface_t* bakbuffer);
    virtual ~Drawingbase(){};//leave cairo destruction to gtk and CairoBuffer

    simple_click_history* initialise_path();
    simple_click_history* initialise_path(const simple_click_history& path);

    unsigned int get_pathindex()const;
    void set_path_by_index(unsigned int index);

    simple_click_history&  get_path_by_index(unsigned int index);
    const simple_click_history&  get_path_by_index(unsigned int index)const;


    cairo_status_t do_check_status(cairo_t* cr_status_of=NULL);
    
  protected:    
    Drawingbase(const Drawingbase& src);
    Drawingbase& operator=(const Drawingbase& src);

  public:
    static void initialise_prefs();
    static int initialise_path_flattener(){
      set_path_flattener();
      return 1;
    }
    virtual void scale_map(const coords& scaler){}
    virtual void scale_map(double x, double y){}
    virtual void map_upendcoords(size_t index=0){}

    PaintContext create_paintcontext(cairo_t* cr);
    PaintContext create_paintcontext(Cairo::RefPtr<Cairo::Context> context);
    PaintContext create_paintcontext(cairo_t* cr, ColourRole role, 
				     void (*meth)(cairo_t*));

    virtual void reset_nodes(const coords& startpoint,
			     const coords& second,  
			     unsigned int symmetry = 3,
			     int lim_x =4000, int lim_y =3000){}
    virtual void reset_nodesbase( unsigned int symmetry = 3){}
    void set_history_by_id(const std::string& pathid);

    sigc::signal<void> signal_savedpaths_changed(){
      return m_signal_savedpaths_changed;
    }
    void set_stroke_colour(double&,double&,double&,double&);
    void set_stroke_colour(double&,double&,double&);
    void set_stroke_colour(const Draw_colour& d);
    void set_fill_colour(const Draw_colour& back);
    void set_fill_colour(double&,double&,double&,double&);
    void set_fill_colour(double&,double&,double&);
    void set_source_to_fill();
    void set_source_to_stroke();
   

    void set_source_to_fill(const coords& patternoffset, size_type step=0){
      set_source_offset_and_step(patternoffset, step, COLOURROLE_FILL);
      set_source_to_fill();
    }
    void set_source_to_stroke(const coords& patternoffset, size_type step=0){
      set_source_offset_and_step(patternoffset, step);
      set_source_to_stroke();
    }

    void stroke_with_pattern(std::string pattername = std::string());
    void add_png_pattern(const std::string& fname);
    void write_svg_output_buffer(cairo_surface_t* outbuffer,
				 SvgSaveFlags flags = XNDRAW_SVG_WRITE_ALL);
    void write_svg_output_buffer(const std::string& fname,
				 int width=1000, int height=1000,
				 SvgSaveFlags flags = XNDRAW_SVG_WRITE_ALL);
    bool set_path_rel_index(int change);//what?
    virtual void redraw_line(bool paint_to_bg=true);
    void on_import_paths(const std::string& fname);
    /**
call a path combination op
on *history
and if it returns a path with any length
sets that path as the history
see NCOORDS::simple_click_history::combine_with() for more
     */
    bool combine_with_path(const simple_click_history& rhspath,
			   int combineflag=NCOORDS::PATH_COMBINE_OR){
      if(!history){throw xn_Error("path not inited");}
      simple_click_history res(*history);
      res.combine_with(rhspath, combineflag);
      std::cerr << "combine_with_path()\nlength of inputs:\n(1)-'history'- "
		<<history->size()<<"\n(2) "<< rhspath.size()
		<<"\nlength of outpath: "<<res.size()<<std::endl;
      if(res.size()>1){
	initialise_path(res);
	return true;
      }
      return false;
    }

    void list_paths();
    //////////////
    //new support apis
    //calls / where a cairo painting operation(possiblyclearing the cairo_t's internal path, boosting speed) can be called in the midst of iterating over a buncof points or paths, without having to return
    void 
    pointOp_with_paintOp(void(Drawingbase::*pointOp)(const std::pair<coords, int>&), 
			 const std::pair<coords, int>& pointarg, 
			 void(Drawingbase::*paintOp)(void) = &Drawingbase::stroke);
    
    void pathOp_with_paintOp(void(Drawingbase::*pathOp)(const path_type&),
			     const  path_type& patharg,
			     void(Drawingbase::*paintOp)(void) = &Drawingbase::stroke);


    ////////////////////
    void cleanup_history();
    void cleanup_histories();
    ColourManager& get_colour_manager(){return colourmanager;}
    const  ColourManager& get_colour_manager()const{return colourmanager;}
    //line painting methods- set source to the right colour
    void stroke();
    void stroke_preserve();
    void fill_preserve();
    void fill();
    virtual void set_source_with_transform(SourceTransformInfo info,
					   ColourRole role= COLOURROLE_STROKE);

    virtual void set_source_offset_and_step(const coords& off,
					    size_type step=0, 
					    ColourRole role= COLOURROLE_STROKE);


    //is there a selection on the path
    bool points_selected()const;
    //is there a selected point under location
    bool draggable_selection(const coords& location, double tolerance)const;
    
    //for benefit of OpSequence
    virtual void set_line_width(double d){Cairo::Context::set_line_width(d);}
    void set_operator(Cairo::Operator op){Cairo::Context::set_operator(op);}
    /// -set operator per internal cairo object
    void set_operator_scratch(Cairo::Operator op){
      cairo_set_operator(scratch,(cairo_operator_t)op);
    }
    void set_operator_buffer(Cairo::Operator op){
      cairo_set_operator(scratch,(cairo_operator_t)op);
    }
    void set_operator_visual(Cairo::Operator op){
      cairo_set_operator(buffer,(cairo_operator_t)op);
    }
    void set_source_rgb(double r, double g, double b){Cairo::Context::set_source_rgb(r,g,b);}
    void set_tolerance(double tolerance){Cairo::Context::set_tolerance(tolerance);}
    void set_antialias(Cairo::Antialias antialias){Cairo::Context::set_antialias(antialias);} 
    void set_line_cap(Cairo::LineCap line_cap){Cairo::Context::set_line_cap(line_cap);}
    void set_line_join(Cairo::LineJoin line_join){Cairo::Context::set_line_join(line_join);}
    void set_fill_rule(Cairo::FillRule fill_rule){Cairo::Context::set_fill_rule(fill_rule);}
    void unset_dash(){Cairo::Context::unset_dash();}
    void set_miter_limit(double limit){Cairo::Context::set_miter_limit(limit);}
    void translate(double tx, double ty){Cairo::Context::translate(tx, ty);}
    void scale(double sx, double sy){Cairo::Context::scale(sx, sy);}
    void rotate(double angle_radians){Cairo::Context::rotate(angle_radians);}
    void rotate_degrees(double angle_degres){Cairo::Context::rotate_degrees(angle_degres);}
    void begin_new_sub_path(){Cairo::Context::begin_new_sub_path();}
    void set_identity_matrix(){Cairo::Context::set_identity_matrix();}
    void rel_move_to(double dx, double dy){Cairo::Context::rel_move_to(dx, dy);}
    void rel_line_to(double dx, double dy){Cairo::Context::rel_line_to(dx, dy);}
    void paint_with_alpha(double alpha){Cairo::Context::paint_with_alpha(alpha);}
    void mask(const Cairo::RefPtr<const Cairo::Pattern>& pattern){
      Cairo::Context::mask(pattern);
    }
    void copy_page(){Cairo::Context::copy_page();}
    void show_page(){Cairo::Context::show_page();}
    void reset_clip(){Cairo::Context::reset_clip();}
    void clip(){Cairo::Context::clip();}
    void clip_preserve(){Cairo::Context::clip_preserve();}
    void push_group(){Cairo::Context::push_group();}
    void push_group_with_content(Cairo::Content content){
      Cairo::Context::push_group_with_content(content);
    }
    Cairo::RefPtr<Cairo::Pattern> pop_group(){return Cairo::Context::pop_group();}
    void pop_group_to_source(){Cairo::Context:: pop_group_to_source();}

    ///////// ////////////
    //transform signal handlers   
    virtual void do_scale_preview(std::pair<coords, coords>& points){
      guidraw.on_preview_scale(visual, *history, points);
    }
    virtual void do_scale_preview_centred(const coords& pt){
      guidraw.on_preview_scale_centred(visual, *history,pt);
    }
    virtual void do_scale_finish(const std::pair<coords, coords>& points){
      history->scale(points.first, &points.second);
    }
    virtual void do_scale_finish_centred(const NCOORDS::coords& pt){
      (*history) *= pt;
    }
    virtual void do_rotate_preview(std::pair<double,  coords>& rotargs){
      guidraw.on_preview_rotate(visual, *history,rotargs);
    }
    virtual void do_rotate_preview_centred(double v){
      guidraw.on_preview_rotate_centred(visual, *history,v);
    }
    coords get_path_centre()const{
      return history->get_centre();
    }
    virtual void do_rotate_finish_centred(double  v){
      history->rotate( v);
    }
    virtual void do_rotate_finish(std::pair<double,  coords>& rotargs){
      history->rotate(rotargs.second, rotargs.first);
    }
    virtual void do_translate_preview(std::pair<coords, int>& transargs){
      guidraw.on_preview_translate(visual, *history,transargs);
    }
    virtual void do_translate_finish(const std::pair<coords,int>& transargs){
      (*history) += transargs.first;//which redraw?
    }
    ////////////

    virtual void bg_render_filename(const std::string& path,
				    xnFileType filetype)   {   }
    
    void on_svgsave(const std::string fname){
      //todo: width, height from prefs

      int width(1500), height(1500);
      Cairo::RefPtr<Cairo::SvgSurface> newsvg= Cairo::SvgSurface::create(fname,width,height);
      //cairo_surface_t* newsurf =cairo_svg_surface_create(fname.c_str(),width, height);
      Cairo::RefPtr<Cairo::Context> svg_cr= Cairo::Context::create(newsvg);
      set_clip_bounds( svg_cr->cobj(), 0, 0, width, height);
      write_svg( svg_cr->cobj(),XNDRAW_SVG_WRITE_ALL );
    }

   
    simple_click_history& get_history();

    Cairo::RefPtr<Cairo::Pattern> get_pattern(const std::string& name);

    void add_pattern(const std::string& nm,
		     Cairo::RefPtr<Cairo::Pattern> pattern);

    cairo_t* get_visual()const{return visual;}
    //TODO: not expose buffer directly
    //create threadlocking wrapper class derived/composed from CairoBuffer
    //which provides Cairo::Context api(as much as required, at least)
    cairo_t* get_buffer()const{return buffer;}

    ///unambiguated function names - no extra overloads,
    // or opsequence breaks

    void set_matrix__mm(const Cairo::Matrix& matrix){set_matrix(matrix);}
    void set_matrix__cobj(const cairo_matrix_t& matrix){set_matrix(matrix);}
    void transform__mm(const Cairo::Matrix& matrix){transform(matrix);}
    void transform__cobj(const cairo_matrix_t& matrix){transform(matrix);}
    void set_patternsource(const Cairo::RefPtr<const Cairo::Pattern>& source){
      set_source(source);
    }
    ///

    virtual void clear_history();
    virtual void save_history(const simple_click_history& path){
      saved_paths.push_back(path);
    }
    void export_paths(const std::string& fname){
      Cairo::RefPtr<Cairo::SvgSurface> svgsurf = Cairo::SvgSurface::create(fname, 1000, 1000);
      Cairo::RefPtr<Cairo::Context> svg = Cairo::Context::create(svgsurf);
      //iter over saved paths, drawing each of them(once!)

     
    }

    virtual   void write_pathfile_as_svg(const std::string& pathname);

    void really_clean_path();
    virtual Cairo::Path* get_iterated_path();
    //returns the previous path
    cairo_path_t*  set_clip_bounds(cairo_t* cr,
				   double x, double y, double width, double height);

    void swap_paths(std::deque<simple_click_history>& swappaths);

    virtual void write_svg(cairo_t* cr_svg, 
			   SvgSaveFlags flags);
    std::deque<simple_click_history>& get_saved_paths_ref(){return saved_paths;}
    const std::deque<simple_click_history>& get_saved_paths_ref()const{return saved_paths;}

    std::deque<simple_click_history*> get_saved_paths();//need to refcount if handing out pointers
    const simple_click_history& get_saved_path(size_t index)const;
    simple_click_history& get_saved_path(size_t index);

    template<typename SIGNALWIDGET>
    void signal_path_changed(SIGNALWIDGET& sendwidget, const simple_click_history* path){
      sendwidget.signal_path_changed().emit(path);
    }
    simple_click_history copy_history();

    virtual void get_extents(cairo_rectangle_t& drawrect,
			     const coords* offset = NULL);
    virtual void get_extents_cRect(NCOORDS::cRect& drawrect,
				   const coords* offset = NULL);

    virtual void set_history(NCOORDS::simple_click_history* clicks);
    virtual void insert_path(NCOORDS::simple_click_history path, bool relative , int base ){}
    virtual void prepend_path(NCOORDS::simple_click_history path, bool relative , int base ){}

    //bad overload-clear up name collision virtual/not virtual
    virtual void append_path(NCOORDS::simple_click_history path, bool relative , int base ){}
    void append_path(const Cairo::Path& path ){Cairo::Context::append_path(path);}
    virtual void assign_history(NCOORDS::simple_click_history *newhistory);
    /**
       get the index of p in saved_paths, or -1 if not present
     */
    int index_of_path(const NCOORDS::simple_click_history* const& p)const{
      for(int i=0; i < ((int)saved_paths.size()); i++){
	if (p == &(*(saved_paths.begin() +i))){return i;}
      }
      return -1;
    }

    unsigned int pathcount()const{
      return saved_paths.size();
    }

    void clear_cairopath();
    void  begin_new_path();  
    void paint_white(bool paint_buffer = false);
    int on_edit_visual_direct();
    //draw marks for edit interface - ie not output
    virtual void draw_editmarks();
    void draw_editable_path();

    void draw_selectbox(const NCOORDS::SelectBox& b){
      guidraw.draw_selectbox(visual, b);
    }

    void select_pathcoords(const NCOORDS::SelectBox& b, 
			   bool maintain_selection = false);

    void on_complete_visual_direct(paintmode newmode = DRAW_TO_BACKBUFFER);

    template<typename CHARTYPE>
    std::basic_istream<CHARTYPE>& read_history_from_stream(std::basic_istream<CHARTYPE>& instream,
							   bool clearfirst = true){
      //TODO: use operator>>, and let it call read_datastream
      if(clearfirst){initialise_path();}
      simple_click_history& p = get_history();
      return p.read_datastream(instream);
    }
    template<typename CHARTYPE>
    std::basic_ostream<CHARTYPE>& write_history_to_stream(std::basic_ostream<CHARTYPE>& os){
      simple_click_history& p = get_history();
      return p.write_datastream(os);
    }

    virtual cairo_t* draw_path_to_cairo(cairo_t* cr);
    void do_cairo_function_with_source(cairo_pattern_t* source, const Cairo_c_operation& op,cairo_t* dest = NULL);
    void stroke_with_source(cairo_pattern_t* source, cairo_t* dest = NULL);
    void fill_with_source(cairo_pattern_t* source, cairo_t* dest = NULL);

    void paint_background(bool paint_buffer= true);

  public:
    void set_paintmode(paintmode sourcehint){
      drawto_now = sourcehint;
    }
    void set_last_drawclick(const std::pair<NCOORDS::coords, int>& click);
    //override in subclasses!
    ////
    //TODO: static or  maybe non virtual members of this base to unambiguously output simple_clcik_history to a cairo context, for  use of patternrosette or otherwise global function defined here or in cairopath 

    ///
    //drawing API -all to be called from draw_to(pair_type
    //TODO: fix - arguments to const
    virtual void draw_to(simple_click_history& clixhistory);
    virtual void draw(){do_draw_path();}
    virtual void draw_to(std::pair<coords, int>& location);
    virtual void do_draw_to(std::pair<coords, int>& location);
    virtual void do_draw_path();
    virtual void do_draw_path(const ncoords& rotator);
    virtual void move_to(coords* pt);
    virtual void line_to(coords* pt);
    virtual void curve_to(coords* handle1, coords* handle2, coords* pt);
    virtual void close_path();

    void line_to(double x, double y);
    void move_to(double x, double y);
    void curve_to(double x1, double y1, double x2, double y2,double x3, double y3);
    void paint();// paints buffer on visual
    void paint_rect(const cairo_rectangle_t& r);
    //identity
      
    unsigned int get_serialcount();
    unsigned int	get_serial_no()const;
   
    //operator friends
    friend bool operator==(const Drawingbase& left, const Drawingbase& right);
    friend bool operator< (const Drawingbase& left, const Drawingbase& right);
    friend bool operator>(const Drawingbase& left, const Drawingbase& right);
    friend bool operator<=(const Drawingbase& left, const Drawingbase& right);
    friend bool operator>=(const Drawingbase& left, const Drawingbase& right);
    friend std::ostream& operator<< (std::ostream& os , const Drawingbase& draw);
    
    //voodoo
    Drawingbase* dobj(){return this;}//dodgy obj
    
    virtual Nodedeq&  reset_nodes(Nodedeq& swapmap){ return swapmap; }
    
    
    //Drawing/Drawingfset api
    virtual void set_baseno(int){}

    void write_buffer_to_png(const char* fname);
    
    //path magic to fix -use <algorithm>
    
    virtual void stroke_history(){}
    virtual void stroke_preserve_history(){}
    virtual void fill_preserve_history(){}
    virtual void fill_history(){}
    std::string get_pathid()const{
      return history->get_pathid();
    }
    
    virtual void adjust_stroke_alpha(double);
    
    virtual void set_to_origin(NCOORDS::coords*){}
    virtual bool get_has_nodes()const{return false;}
    virtual bool path_within_bounds(const coords& offset){
      return true;
    }
    virtual void check_type(std::ostream& stream, std::string comment= "descendent of Drawingbase::typeinfo");
    
    
    void check_status();
    void check_status(const char* comment , cairo_t* cairo2check = NULL);
    
    void save();
    void restore();
  };/*class Drawingbase*/
  
  
  template <class HAS_SERIAL= Drawingbase>
  struct compare_serial:public std::binary_function<HAS_SERIAL*, HAS_SERIAL*,bool>{
    
    compare_serial(){}
    ~compare_serial(){}
    
    bool operator()(const HAS_SERIAL* left, const HAS_SERIAL* right){
      return left->get_serial_no() < right->get_serial_no();
    }
  };/*struct compare_serial**/
  
  
   
}// namespace xNDRAW;  


#endif /*DRAWINGBASE_H*/

