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



//TODO: GIVE EACH node its own thread, just the mouse's node with gui
//(so can keep up with mouse)
#ifndef X3DRAW_DRAWINGSET_H
#define X3DRAW_DRAWINGSET_H
#include "xndrawexceptions.h"
#if HAVE_CONFIG_H
#include "config.h"
#endif


#include <iostream>
#include "io_uncollide.h"
#include "typedefs.h"
#include "drawing.h"
#include "pathproducer.h"
#include "pathqueueeater.h"


namespace xNDRAW{

  

  class drawset_line_to_history:public std::unary_function<NCOORDS::coords*, void>{
    Drawing* draw;
  public:
    drawset_line_to_history(Drawing* dra):draw(dra){}
    ~drawset_line_to_history(){}

    void operator()(NCOORDS::coords* offset){
      draw->line_to_history(offset);
      COUT("drawset_line_to_history_unflipped::operator()(NCOORDS::coords* offset");
    }


  };

  using NCOORDS::MutexNodedeq;
  class Drawingset: public Drawing{
  public:
  
  protected:
  
    Nodes map_generator;
    MutexNodedeq coordmap; //fix this - blocks
    BGRenderPath* bg_renderer;

  public:
    //13-12-10 - changed constructor to take Nodes param
    Drawingset(cairo_t* vis, cairo_surface_t* buf,
	       const ncoords& baase, 
	       const Nodes& nodegenerator);
	       //TODO: remove coordlist
	       // const Nodedeq& coordlist);
Drawingset(cairo_t* vis, cairo_surface_t* buf,
	       const ncoords& baase, 
	   unsigned int grid_limit_x=4000, unsigned int grid_limit_y=3000);
    Drawingset(Drawing* drago, 
	       const Nodes& nodegenerator);

    virtual ~Drawingset();



    //set  the grid of 'nodes' drawing rotates about
    void reset_nodes(const coords& startpoint,
		     const coords& second,  
		     unsigned int symmetry = 3,
		     int lim_x =4000, int lim_y =3000);
   virtual void reset_nodesbase( unsigned int symmetry = 3){
      map_generator.set_baseno( symmetry);
      coordmap= map_generator.get_nodes();
    }

    Nodedeq& get_nodes();
    const Nodedeq& get_nodes()const;

    bool inverted();

    bool reversed();

    bool get_has_nodes();
    virtual void check_rotator_offset(const coords* offsett){
      if (!offsett){
	throw xn_Error("offset required in this class");
      }
    }
    void line_to_history();
    virtual void do_draw_path(const ncoords& rotator);
    virtual void do_draw_path();
    virtual void draw(){ do_draw_path();}
    void translate(deqC& list, coords* offset)const;
    //  void draw_to(click_pair_type clickpair);
    void do_draw_rosette_path(const std::deque<simple_click_history>& paths, 
			      const coords* offset);
    void line_to(float ex, float why){Cairo::Context::line_to(ex, why);}
    void line_to(coords* mousy);
    void curve_to(coords* mousy);
    void curve_to(double h1x,double h1y,double  h2x,double h2y,double  ptx,double pty);
    void curve_to(coords* h1,coords* h2, coords* mousy); 
    //hook these to a menu or button
    bool skip_even();
    bool set_skip_even();
    bool skip_odd();
    bool set_skip_odd();

    bool draw_correct();
    void set_draw_correct(bool setting = true);
    void flip_draw_correct();

    ncoords* flip_if(NCOORDS::ncoords* flippable);
 
    Nodedeq& set(NCOORDS::Nodes& generator);
    //get rid of this stuff - regenerate from our own Nodes
    //return old nodes in swapmap, call by  value from ping_buffer
    Nodedeq&  reset_nodes(Nodes& generator);

    virtual void draw_editmarks(){
      Drawingbase::draw_editmarks();
      guidraw.draw_grid(visual, coordmap);
    }
    virtual bool save_nodes(const std::string& filename){
      coordmap.write_data(coordmap,filename);
      coordmap.write_text(coordmap,filename);
      return true;
    }

    //fix 
    void bg_draw_path(const PaintContext& s);
    void bg_draw_path2(const PaintContext& s);

    void set_points();
    void   invert_nodestate(NCOORDS::Nodedeq& coordlist);
    void fill_history();
    void stroke_history();
 
    void fill_preserve_history();
    void stroke_preserve_history();
 
    std::pair<BGRenderPath*, Glib::Thread*> 
    create_bg_renderthread(const PaintContext& s);
    std::pair<BGRenderPath*, Glib::Thread*> 
    create_bg_renderthread(const PaintContext& s, ColourRole role,
			   void(*meth)(cairo_t*));


    static void create_bg_render(Drawingset* d,
				 BGRenderPath*& newrenderer, 
				 const PaintContext& s,
				 bool keepalive = true);

    void bg_paint_path(ColourRole role, void(*meth)(cairo_t*));

    //TODO
    void bg_paint_path(ColourRole role, void(*meth)(cairo_t*), 
		       Cairo::RefPtr<Cairo::Context> cairodest);
    
    virtual void bg_render_filename(const std::string& filepath,
				    xnFileType filetype,
				    void(*cairomethod)(cairo_t*),
				    ColourRole role= COLOURROLE_STROKE) ;

    void set_inverted(bool flip);

    void set_reversed(bool flip);

    void flip_reversed();

  void flip_inverted();

  virtual void scale_map(const coords& scaler);
  virtual void scale_map(double x, double y);
  virtual void map_upendcoords(size_t index=0);
  virtual void line_to_history(NCOORDS::coords* offset);
  
};

class DoubleDrawset:public Drawingset{
  //another coordmap offset between the the first - to 'split the pieces' in three
protected:
  //either :
  //too complicated - one would have to take over the role of updating visual etc
  //Drawingset doppelganger;
  //or
  //Nodedeq coordmap2
  //ncoords point_data2, next2;
  //whatever else is required
  //ensuring to push and pop around all calls in the underlying classes
public:
  DoubleDrawset(cairo_t* vis, cairo_surface_t* buf,const ncoords& baase, const Nodes& coordlist): Drawingset (vis, buf, baase, coordlist){}
  DoubleDrawset(Drawing* drago,const Nodes& coordlist):Drawingset (drago, coordlist){}

};


typedef Cairo::RefPtr<Drawingset> dSetPtr;
 
}
#endif /*X3DRAW_DRAWINGSET_H*/
