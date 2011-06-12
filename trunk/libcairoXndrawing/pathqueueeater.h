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

#ifndef LIBCAIROXNDRAWING_PATHQUEUEEATER_H
#define LIBCAIROXNDRAWING_PATHQUEUEEATER_H
#include "xndrawexceptions.h"

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include  "pathproducer.h"
#include "cairopath.h"

namespace xNDRAW{

  using namespace NCOORDS; //move to .cc file

  class BGRenderPath;

  

  //TODO: change queue_type
  //to something like GuardedQueue<std::pair<PaintContext* paintinfo, simple_click_history> >
  //where 'PaintContext' contains -function ptr to some cairo 'paint command'
  //                              -a (suitably transformed if applicable) PatternColour/OffsetPattern
  //                              - method which calls the 'paint command' after setting the source(inside save/restore nest)

  //other requirements - locking wrapper around cairo(although it must have its own locks internally?)
  
  class PathEater: public ThreadKlass{
  public:
    typedef simple_click_history::iterator iterator;
    typedef simple_click_history::const_iterator const_iterator;
    typedef std::pair<simple_click_history,SourceTransformInfo> renderpair_type;
    typedef std::deque<renderpair_type> container_type;
    typedef std::pair<PaintContext, container_type*> output_pair_type;
typedef std::pair<PaintContext, simple_click_history> input_pair_type;
    typedef container_type::const_iterator const_container_iterator;
    typedef container_type::iterator container_iterator;
    typedef GuardedQueue<input_pair_type > input_queue_type;
  protected:

#if DEBUG_PATHQUEUE
  public:
    typedef std::map<ptrdiff_t, std::ostringstream*> log_map_type;
    typedef log_map_type::iterator log_map_iterator;
typedef log_map_type::const_iterator const_log_map_iterator;

    protected:
    static std::map<ptrdiff_t, std::ostringstream*> debug_output_streams;
#endif
    BGtransform_despatcher* pathsource;//use base type ptr
    //PathMultiplier* pathsource;
    BGRenderPath* parent;
    PaintContext renderinfo;
    bool working_;
    //not available
    PathEater(const PathEater&);
    PathEater& operator=(const PathEater&);
  public:
    PathEater(PathMultiplier* source,   const PaintContext& paint,
	      BGRenderPath* mapa=NULL);

    PathEater(BGtransform_despatcher* source,   const PaintContext& paint,
	      BGRenderPath* mapa=NULL);
    //virtual?
    virtual ~PathEater();

    void initialise();//create a logstream, if DEBUG_PATHQUEUE #DEFINED

#if DEBUG_PATHQUEUE 
    std::ostringstream& get_outstream()const;
    std::string get_log_output()const;
    void destroy_logstream();
#endif
    //TODO: delete this->thread when it dies
    bool busy()const{return working_;}
    void draw_path(const simple_click_history& path, 
		   cairo_t* cr)const;
    void draw_paths(const std::deque<simple_click_history>& pathwheel,
		    cairo_t* cr)const;

    void set_context(Cairo::RefPtr<Cairo::Context> ct){
      renderinfo.set_context(ct);
    }
    void set_paint_method(void(*meth)(cairo_t*)){
      renderinfo.set_paint_method(meth);
    }

    PaintContext& get_paintcontext(){return renderinfo;}
    const PaintContext& get_paintcontext()const{return renderinfo;}
    void set_paintcontext(const PaintContext& p){renderinfo=p;}

    bool consume_pathwheel(PaintContext paint=PaintContext());
    BGRenderPath* get_parent(){return parent;}
    cairo_t* get_cairo()const;
    void consume_queue(const PaintContext& p);

    void consume();

    std::pair<PathEater*, Glib::Thread*> 
      run_in_thread();

    void render_paths(const container_type& pathwheel,
		      PaintContext& paint)const;

    static void create(PathEater*& outptr, BGtransform_despatcher* source,
		       const PaintContext& paint,
		       BGRenderPath* rend);
    virtual void join();

    static std::pair<PathEater*, Glib::Thread*> 
      create_in_thread(BGtransform_despatcher* source, 
		       const PaintContext& paint, 
		       BGRenderPath* rend);

    //template< typename NODE>
    //static std::pair<PathEater*, Glib::Thread*> 
    //create_in_thread(PathNodeMultiplier< NODE>* source, BGRenderPath* rend);

    bool alive()const{
      return pathsource && pathsource->alive();
    }
    void kill(){pathsource->kill();}
  };

  class BGRenderPath: public ThreadKlass{
  public:
    typedef Cairo::RefPtr<Cairo::Context> context_type;
    //typedef std::pair<PaintContext, container_type*> output_pair_type;
typedef std::pair<PaintContext, simple_click_history> input_pair_type;
typedef GuardedQueue<input_pair_type > input_queue_type;

 typedef int (*file_command_type)(Cairo::RefPtr<Cairo::Context>, 
				  const std::string&, 
				  xnFileType);

  protected:
#if DEBUG_PATHQUEUE
 std::ostringstream* logoutput;
#endif


    BGtransform_despatcher* pathproducer;//make resettable
    //PathMultiplier* pathproducer;
    PathEater* renderer;
    PaintContext outputimage;
    input_queue_type pathswaiting;
   
    file_command_type file_command;
    
    std::string pngfilename;
    xnFileType ftype;
    bool writepng;
    bool stay_alive;
  public:

    template <typename DRAWCLASS, typename NODE>
      BGRenderPath(DRAWCLASS* master, const NODE& compilerrefnode,/*unused compiler hint*/
		   const PaintContext& paint, bool keepalive=false):
#if DEBUG_PATHQUEUE
    logoutput(new std::ostringstream),
#endif
    ThreadKlass(NULL),
      pathproducer(NULL),
      renderer(NULL),
      outputimage(paint),
      writepng(false),
      stay_alive(keepalive){
#if DEBUG_PATHQUEUE
	std::cerr<<"BGRenderPath(DRAWCLASS* master, const NODE& compilerrefnode,/*unused compiler hint*\nconst PaintContext& painty, bool keepalive=false"<<std::endl;
#endif
	const MutexNodedeq* p_nodes =dynamic_cast< const MutexNodedeq*>(&(master->get_nodes()));
	if(!p_nodes ||!( p_nodes->size())){throw xn_Error("error importing drawing offsets");}
	const ncoords& drawsrotator= master->get_point_data();
	const coords& data_c = *(*(p_nodes->begin()));
	const simple_click_history& path= master->get_history();
	std::pair<PaintContext, simple_click_history> pathpair(paint,path); 
	std::pair<PathNodeMultiplier<NODE>*, Glib::Thread*> prodpair=PathNodeMultiplier<NODE>::create_in_thread( pathpair, p_nodes, drawsrotator);
	pathproducer= prodpair.first;
	//std::pair<PathEater*, Glib::Thread*> eatpair
      }
    
    
    template <typename DRAWCLASS>
      BGRenderPath(DRAWCLASS* master, const PaintContext& caiiro, bool keepalive=false):
#if DEBUG_PATHQUEUE
    logoutput(new std::ostringstream),
#endif
    ThreadKlass(NULL),
      pathproducer(NULL),
      renderer(NULL),
      outputimage(caiiro), 
      writepng(false),
      stay_alive(keepalive){
#if DEBUG_PATHQUEUE
	std::cerr<<"BGRenderPath(DRAWCLASS* master, const NODE& compilerrefnode,/*unused compiler hint*\nconst PaintContext& paint, bool keepalive=false"<<std::endl;
#endif
	const MutexNodedeq* p_nodes =dynamic_cast< const MutexNodedeq*>(&(master->get_nodes()));
	if(!p_nodes ||!( p_nodes->size())){throw xn_Error("error importing drawing offsets");}
	const ncoords& drawsrotator= master->get_point_data();
	const coords& data_c = *(*(p_nodes->begin()));
	const simple_click_history& path= master->get_history();
	input_pair_type pathpair(caiiro, path);
	std::pair<PathMultiplier*, Glib::Thread*> prodpair=PathMultiplier::create_in_thread( pathpair, p_nodes, drawsrotator);
	pathproducer= prodpair.first;
      }
    virtual ~BGRenderPath();

#if DEBUG_PATHQUEUE
    std::ostream* get_log()const{
      return logoutput;
    }
    std::string get_log_output(){
      if(logoutput){
	return logoutput->str();
      }
      return "no log";
    }

    std::string get_renderer_log_output()const{
      if(renderer){
	return renderer->get_log_output();
      }
      return "no renderer";
    }
#endif
    void set_write_png(bool v = true){writepng = v;}
    bool get_write_png()const{return writepng;}
    void set_pngfilename(const std::string& fname){pngfilename = fname;}
    std::string get_pngfilename( )const{return pngfilename;}
  
    bool keep_alive()const{return stay_alive;}

    void write_png_file(const std::string& filepath);

    bool finished_job(){
      return pathproducer->finished_job()&& pathswaiting.empty();
    }
  protected:
    void set_path(const std::pair<PaintContext,simple_click_history>& p){
      pathproducer->set_path(p);//todo: always add via queue
      //in ctor too
    }
  public:
    void add_path(const input_pair_type& p);
    void add_path(const simple_click_history& p);
    bool alive(){
      return renderer->alive();
    }
  
    void   pop();

    void kill(bool immediately=false);
    
    void set_paint_method(void(*cairomethod)(cairo_t*));
    void set_paint_method(ColourRole role, bool preserve=false);
    void set_cairo(cairo_t* cr);
    void set_cairo(Cairo::RefPtr<Cairo::Context> cr);
    cairo_t* get_cairo()const{return outputimage.get_context()->cobj();}

    PaintContext& get_paintcontext(){return  outputimage;}
    const PaintContext& get_paintcontext()const {return  outputimage;}
    void set_paintcontext(const PaintContext& con);

    void update_renderer();

    void run  () ;
    void waitloop();
   
   

  };

template<typename DRAWCLASS>
  class BGRenderer: public BGRenderPath{
    //currently identical to parent class, except, stays alive by default
  public:
template<typename NODE>
  BGRenderer(DRAWCLASS* master, const  NODE& n, const PaintContext paint,
	     bool keepalive=true): BGRenderPath(master, n, paint, keepalive),
      owner(master){}

  protected:
    DRAWCLASS* owner;
  };
}
#endif

