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

#ifndef LIBXNCAIRODRAWING_OPSEQUENCE_H
#define LIBXNCAIRODRAWING_OPSEQUENCE_H
#include "xndrawexceptions.h"

#if HAVE_CONFIG_H
#include "config.h"
#endif
#include "drawing.h"
#include "boundinstruction.h"

namespace xNDRAW{
  // typedef BoundOpBase<Drawingbase> draw_operation_type;
  //typedef std::deque<draw_operation_type> draw_operation_sequence;
  template <typename TP, typename ALLOC= std::allocator<BoundOpBase<TP>*> >
  class OpSequence{


  public:

    typedef BoundOpBase<TP> operation_type;
    typedef ALLOC allocator_type;
    typedef std::deque<operation_type*,allocator_type> container_type;
    typedef typename container_type::iterator iterator;
    typedef typename container_type::const_iterator const_iterator;
    typedef typename container_type::size_type size_type;
    typedef typename container_type::value_type value_type;
    typedef typename container_type::reference reference;
    typedef typename container_type::const_reference const_reference;
    typedef typename container_type::pointer pointer;

  protected:
    container_type operations;

  public:

    void swap(OpSequence& seq){
      operations.swap(seq.operations);
    }

    template <typename CONTAINER>
    void swap(CONTAINER& ops){
      ops.swap(operations);
    }

    void set_null(){
      set_object();
    }
    void set_object(TP* methowner=NULL){
      for(iterator w = begin();w!=end();w++){
	(*w)->set_object(methowner);//should cairo_reference it there
      }
    }

    size_type size(){
      return operations.size();
    }

    void playback(TP* methowner=NULL){
      for(iterator w = begin();w!=end();w++){
	if(methowner){
	  (*(*w))(methowner);
	}
	else{
	  (*(*w))();
	}
      }
    }

    void playback(iterator first, iterator last, TP* methowner=NULL){
      for(;first != last; first++){
	if(methowner){
	  (*(*first))(methowner);
	}
	else{
	  (*(*first))();
	}
      }
    }

    const_iterator end()const{
      return operations.end();
    }

    iterator end(){
      return operations.end();
    }

    const_iterator begin()const{
      return operations.begin();
    }

    iterator begin(){
      return operations.begin();
    }

    reference front(){
      return operations.front();
    }
    const_reference front()const{
      return operations.front();
    }
    reference back(){
      return operations.back();
    }
    const_reference back()const{
      return operations.back();
    }

    void push_back(operation_type* op){
      operations.push_back(op);
    }
    void push_front(operation_type* op){
      operations.push_back(op);
    }

  };

  class DrawRecorder{
  public:
      
    typedef BoundOpBase<Drawingbase > operation_type;
    typedef OpSequence< Drawingbase > container_type;
    typedef std::deque<container_type> repository_type;
    typedef container_type::iterator iterator;
    typedef container_type::const_iterator const_iterator;
    typedef container_type::const_reference const_reference;
    typedef container_type::reference reference;
    typedef container_type::size_type size_type;

  protected:
    Drawingbase* trackable;
    OpSequence< Drawingbase > draw_operations;
    repository_type* sequence_store;
  public:
    DrawRecorder(Drawingbase* contex=NULL,
		 repository_type* store=NULL);

    ~DrawRecorder();

    void set_sequence_store(repository_type* store=NULL);

    void new_sequence();

    void set_methodowner(Drawingbase* d);

    const_iterator end()const;
    iterator end();

    const_iterator begin()const;
    iterator begin();

    reference front();
    const_reference front()const;

    reference back();
    const_reference back()const;

    void push_back(operation_type* op);
    void push_front(operation_type* op);

    void save();
    void restore();

    void set_operator(Cairo::Operator oper);

  
    //fn ambiguous as ptr : created(as for set_matrix()) an unambiguous name(set_patternsource)
    void set_source(const Cairo::RefPtr<const Cairo::Pattern>& source);
  
    void set_source_rgb(double r, double g, double b);

    void set_tolerance(double tolerance);

    void set_antialias(Cairo::Antialias antialias);

    void set_fill_rule(Cairo::FillRule fill_rule);
    void set_line_width(double lw);
    void set_line_cap(Cairo::LineCap line_cap);
    void set_line_join(Cairo::LineJoin line_join);
 
    void unset_dash();
    void set_miter_limit(double limit);

    void translate(double tx, double ty);

    void scale(double sx, double sy);

    void rotate(double angle_radians);
 
    void rotate_degrees(double angle_degres);
  
    void transform(const Cairo::Matrix& matrix);

    void transform(const cairo_matrix_t& matrix);
  
    //provide non-overloaded names for these in drawingbase
    void set_matrix(const Cairo::Matrix& matrix);
    void set_matrix(const cairo_matrix_t& matrix);
 
    void set_identity_matrix();

    void move_to(double x, double y);
 
    void line_to(double x, double y);
    //too many params  
    //(use fake coords here)
    void set_source_rgba(double red, double green, double blue, double alpha);

    void rel_curve_to(double dx1, double dy1, double dx2, double dy2, double dx3, double dy3);
 
    void rectangle(double x, double y, double width, double height);
 
    void curve_to(double x1, double y1, double x2, double y2, double x3, double y3);
 
    void arc(double xc, double yc, double radius, double angle1, double angle2);
  
    void arc_negative(double xc, double yc, double radius, double angle1, double angle2);
    //need more specialisatiions! for referenced args
    void set_source(const Cairo::RefPtr<Cairo::Surface>& surface, double x, double y);

    void set_dash(std::valarray<double>& dashes, double offset);
 
    void set_dash(std::vector<double>& dashes, double offset);
   
    void begin_new_path(){
      operation_type* op=create_boundop(&Drawingbase::begin_new_sub_path,   trackable);
      draw_operations.push_back(op);
    }

    void begin_new_sub_path(){
      operation_type* op=create_boundop(&Drawingbase::begin_new_sub_path,   trackable);
      draw_operations.push_back(op);
    }
 
 
    void rel_move_to(double dx, double dy);
    void rel_line_to(double dx, double dy);

    void close_path();

    void paint();

    void paint_with_alpha(double alpha);
    
    //if ever serialising/saving  for another session
    //, this will need a mechanism to substitute some pattern for dead references,
    //or means of saving and reconstituting them
    void mask(const Cairo::RefPtr<const Cairo::Pattern>& pattern);
    /*
//requires another specialisation :  BoundOp<void,TP, T&,T1,T2>
       void mask(const Cairo::RefPtr<const Cairo::Surface>& surface,
	       double surface_x, double surface_y){
	 typedef const Cairo::RefPtr<const Cairo::Surface>& const_surface_reference;
	 typedef BoundOp<void,Drawingbase,const_surface_reference, double, double> local_op_type;
       operation_type* op=new local_op_type(&Drawingbase::mask,   trackable,
					    surface, surface_x, surface_y );
       draw_operations.push_back(op);
       }
    */
    void stroke();

    void stroke_preserve();

    void fill();
    void fill_preserve();

    void copy_page();
    void show_page();

    void reset_clip();
    void clip();
    void clip_preserve();
    
    void append_path(const Cairo::Path& path);
    
    void push_group();
    void push_group_with_content(Cairo::Content content);

    /**sigc::bind_return doesn't voidify return val
       (look at rv_handler_type class from the dumb python function binding code I wrote: but that is a complex set of templates..
       this fn not important: pop_group_to_source() is handier
    
       Cairo::RefPtr<Cairo::Pattern> pop_group(){
       operation_type* op=create_boundop(&Drawingbase::pop_group,   trackable);
       draw_operations.push_back(op);
       return Cairo::RefPtr<Cairo::Pattern>(NULL);
       }
    */
    void pop_group_to_source();
 	
    //Drawingset apis

    void 	redraw_line (bool paint_to_bg);
    void 	stroke_with_pattern (std::string pattername);
    void 	add_png_pattern (const std::string &fname);
    void 	cleanup_history ();
    void 	cleanup_histories ();
    //void 	on_import_paths (const std::string &fname)

    void 	paint_rect (const cairo_rectangle_t &r);
    void 	set_baseno (int n);

    void 	stroke_history ();
    void 	stroke_preserve_history ();
    void 	fill_preserve_history ();
    void 	fill_history ();
    void 	adjust_stroke_alpha (double a);
    void 	set_to_origin (NCOORDS::coords * pt);

    void 	add_pattern (const std::string &nm, 
			     Cairo::RefPtr< Cairo::Pattern > pattern);

    void 	clear_history ();
    void 	save_history (const simple_click_history &path);
    void 	set_history (NCOORDS::simple_click_history *clicks);
    void 	really_clean_path ();
 
    void 	clear_cairopath ();

    void 	paint_white (bool paint_buffer=false);
    void 	do_draw_to (std::pair< coords, int > &location);
    /**
       void 	select_pathcoords (const SelectBox &b, bool maintain_selection=false)

       void 	stroke_with_source (cairo_pattern_t *source, cairo_t *dest=NULL)
       void 	fill_with_source (cairo_pattern_t *source, cairo_t *dest=NULL)
       //do_draw_to might be enough
       virtual void 	draw_to (simple_click_history &clixhistory)
       virtual void 	draw ()
       virtual void 	draw_to (std::pair< coords, int > &location)


       virtual void 	do_draw_path ()
       virtual void 	do_draw_path (const ncoords &rotator)


       //need fns with unique names do_draw_to() might be enough, otherwise state setters(colour/source, operator etc
       virtual void 	move_to (coords *pt)
       virtual void 	line_to (coords *pt) 
       virtual void 	curve_to (coords *handle1, coords *handle2, coords *pt)


       //+++++++++++++
       //virtual void 	get_extents (cairo_rectangle_t &drawrect, const coords *offset=NULL)

       virtual void 	insert_path (NCOORDS::simple_click_history path, bool relative, int base)
       virtual void 	prepend_path (NCOORDS::simple_click_history path, bool relative, int base)
       virtual void 	append_path (NCOORDS::simple_click_history path, bool relative, int base)
       void 	append_path (const Cairo::Path &path)
       //=====v
       virtual void 	scale_map (const coords &scaler)
       virtual void 	scale_map (double x, double y)
       virtual void 	map_upendcoords (size_t index=0)

       //resolve overloads
       void 	set_stroke_colour (double &, double &, double &, double &)
       void 	set_stroke_colour (double &, double &, double &)
       void 	set_stroke_colour (const Draw_colour &d)
       void 	set_fill_colour (const Draw_colour &back)
       void 	set_fill_colour (double &, double &, double &, double &)
       void 	set_fill_colour (double &, double &, double &)

       //==================

       void 	set_source_to_fill ()
       void 	set_source_to_stroke ()

       void 	set_path_rel_index (int change)
       void 	on_complete_visual_direct (int newmode=DRAW_TO_BACKBUFFER)

       void 	do_cairo_function_with_source (cairo_pattern_t *source, const Cairo_c_operation &op, cairo_t *dest=NULL)
       void 	set_paintmode(int sourcehint)
       void 	set_last_drawclick (const std::pair< NCOORDS::coords, int > &click)*/

    //void 	write_svg_output_buffer (cairo_surface_t *outbuffer, SvgSaveFlags flags=XNDRAW_SVG_WRITE_ALL)
    //void 	write_svg_output_buffer (const std::string &fname, int width=1000, int height=1000, SvgSaveFlags flags=XNDRAW_SVG_WRITE_ALL)
    //virtual void 	bg_render_filename (const std::string &path, xnFileType filetype)
    //void 	on_svgsave (const std::string fname)

    //simple_click_history & 	get_history ()
    //Cairo::RefPtr< Cairo::Pattern > 	get_pattern (const std::string &name)
    //cairo_t * 	get_visual () const
    //void 	export_paths (const std::string &fname)
    //virtual void 	write_pathfile_as_svg (const std::string &pathname)

    //virtual Cairo::Path * 	get_iterated_path ()

    //cairo_path_t * 	set_clip_bounds (cairo_t *cr, double x, double y, double width, double height)
    //virtual void 	write_svg (cairo_t *cr_svg, SvgSaveFlags flags)
    //std::deque< simple_click_history * > 	get_saved_paths ()
    //const simple_click_history & 	get_saved_path (size_t index) const
    //simple_click_history & 	get_saved_path (size_t index)
    //template<typename SIGNALWIDGET >
    //void 	signal_path_changed (SIGNALWIDGET &sendwidget, const simple_click_history *path)
    //simple_click_history 	copy_history ()
    //int 	on_edit_visual_direct ()
    //virtual void 	draw_editmarks ()
    //void 	draw_editable_path ()
    //void 	draw_selectbox (const SelectBox &b)
    //template<typename CHARTYPE >
    //std::basic_istream< CHARTYPE > & 	read_history_from_stream (std::basic_istream< CHARTYPE > &instream, bool clearfirst=true)
    //template<typename CHARTYPE >
    //std::basic_ostream< CHARTYPE > & 	write_history_to_stream (std::basic_ostream< CHARTYPE > &os)
    //virtual cairo_t * 	draw_path_to_cairo (cairo_t *cr)
    //unsigned int 	get_serialcount ()
    //unsigned int 	get_serial_no () const
    //Drawingbase * 	dobj ()
    //virtual Nodedeq & 	reset_nodes (Nodedeq &swapmap)
    //void 	write_buffer_to_png (const char *fname)
    //std::string 	get_pathid () const
    //virtual bool 	get_has_nodes () const
    //virtual bool 	path_within_bounds (const coords &offset)
    //virtual void 	check_type (std::ostream &stream, std::string comment="descendent of Drawingbase::typeinfo")

    //void 	check_status ()
    //void 	check_status (const char *comment, cairo_t *cairo2check=NULL)
    //simple_click_history * 	initialise_path ()
    //simple_click_history * 	initialise_path (const simple_click_history &path)
    //cairo_status_t 	do_check_status (cairo_t *cr_status_of=NULL)


    //unwrapped  Cairo::Context API     

    //bool in_stroke(double x, double y) const;
    //bool in_fill(double x, double y) const;
    //void get_stroke_extents(double& x1, double& y1, double& x2, double& y2) const;
    //void get_fill_extents(double& x1, double& y1, double& x2, double& y2) const;

    //  void get_clip_extents(double& x1, double& y1, double& x2, double& y2) const;
    //void copy_clip_rectangle_list(std::vector<Rectangle>& rectangles) const;
    //  void get_font_matrix(Matrix& matrix) const;
    //void get_font_matrix(cairo_matrix_t& matrix) const;
    //  void get_font_options(FontOptions& options) const;
    //  RefPtr<ScaledFont> get_scaled_font();
    //RefPtr<FontFace> get_font_face();
    // RefPtr<const FontFace> get_font_face() const;
    //  void get_font_extents(FontExtents& extents) const;
    //  void get_text_extents(const std::string& utf8, TextExtents& extents) const;
    // void get_glyph_extents(const std::vector<Glyph>& glyphs, TextExtents& extents) const;
    /////////
    //void select_font_face(const std::string& family, FontSlant slant, FontWeight weight);
    //void set_font_size(double size);
    // void set_font_matrix(const Matrix& matrix);
    //void set_font_options(const FontOptions& options);
    //void set_font_matrix(const cairo_matrix_t& matrix);
    //void set_scaled_font(const RefPtr<const ScaledFont>& scaled_font);
    //void show_text(const std::string& utf8);
    //void show_glyphs(const std::vector<Glyph>& glyphs);
    //void show_text_glyphs(const std::string& utf8,
    //const std::vector<Glyph>& glyphs,
    //const std::vector<TextCluster>& clusters,
    //TextClusterFlags cluster_flags);
    //void text_path(const std::string& utf8);
    //void glyph_path(const std::vector<Glyph>& glyphs);
    //void set_font_face(const RefPtr<const FontFace>& font_face);
    ///////////
    //  Operator get_operator() const;
    //  RefPtr<Pattern> get_source();
    //RefPtr<const Pattern> get_source() const;
    //  double get_tolerance() const;
    //Antialias get_antialias() const;
    //void get_current_point (double& x, double& y) const;
    //bool has_current_point() const;
    //FillRule get_fill_rule() const;
    //double get_line_width() const;
    //LineCap get_line_cap() const;
    //LineJoin get_line_join() const;
    //double get_miter_limit() const;
    //void get_dash(std::vector<double>& dashes, double& offset) const;
    //void get_matrix(Matrix& matrix);
    //void get_matrix(cairo_matrix_t& matrix); 
    //Matrix get_matrix() const;
    //Cairo::RefPtr<Cairo::Surface> get_target();
    //Cairo::RefPtr<const Cairo::Surface> get_target() const;
    //  Path* copy_path() const;
    // void get_path_extents(double& x1, double& y1, double& x2, double& y2) const;
    //Path* copy_path_flat() const;
    //,,,,,
    //void user_to_device(double& x, double& y);
    //  void user_to_device_distance(double& dx, double& dy); 
    //void device_to_user(double& x, double& y);
    // void device_to_user_distance(double& dx, double& dy);
      
  };
}
#endif
