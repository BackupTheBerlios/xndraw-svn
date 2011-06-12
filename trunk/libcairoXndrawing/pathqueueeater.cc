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




#include "pathqueueeater.h"


namespace xNDRAW{
#if DEBUG_PATHQUEUE

  std::map<ptrdiff_t, std::ostringstream*> PathEater::debug_output_streams;
#endif

  void PathEater::draw_path(const simple_click_history& path, 
			    cairo_t* cr)const{
#if DEBUG_PATHQUEUE
    std::ostringstream& out=get_outstream();
    out << "draw_path() called"<<std::endl;
    std::cerr<< "draw_path() called"<<std::endl;
#endif
    cairo_draw_clickhistory(cr, path);
  
  }
  void PathEater::draw_paths(const std::deque<simple_click_history>& pathwheel,
			     cairo_t* cr)const{
    throw xn_Error("call rendewr_paths instread");
    /* for(const_container_iterator w = pathwheel.begin(); w != pathwheel.end();w++){
       draw_path(*w, cr);
       }
    */
  }
  
  void PathEater::render_paths(const container_type& pathwheel,
		     
			       PaintContext& paint)const{
#if DEBUG_PATHQUEUE
    std::ostringstream& out=get_outstream();
    out << "render_paths() called"<<std::endl;
    std::cerr<< "PathEater::render_paths() called"<<std::endl;
#endif
    for(const_container_iterator w = pathwheel.begin(); w!= pathwheel.end();w++){
      cairo_t* cr = paint.get_context()->cobj();
      cairo_save(cr);
      paint.set_source_transformation((*w).second);
      //if draw_path returned a cRect showing the extents of transformed path
      //use 
      draw_path((*w).first, cr);
      paint();
      cairo_restore(cr);
    }
  }

  bool PathEater::consume_pathwheel(PaintContext paint){
#if DEBUG_PATHQUEUE
    std::ostringstream& out=get_outstream();
    out << "consume_pathwheel() called"<<std::endl;
    std::cerr<<"PathEater::consume_pathwheel() called"<<std::endl;
#endif
    if(!paint){
      paint= this->get_paintcontext();
    }
    if(pathsource->queue_empty()){
#if DEBUG_PATHQUEUE
      out << "pathsource->queue_empty() ... returning"<<std::endl;
      std::cerr<<"pathsource->queue_empty() ... returning"<<std::endl;
  
#endif
      return false;
    }
    output_pair_type wheel = pathsource->pop();

    this->render_paths( *(wheel.second), wheel.first);
    //make conditional, if wheel can be reused?
    PathMultiplier::pathwheel_dispose(wheel.second);
    return true;
  }

  cairo_t* PathEater::get_cairo()const{
#if DEBUG_PATHQUEUE
    std::ostringstream& out=get_outstream();
    out << "get_cairo() called"<<std::endl;
    std::cerr<<"PathEater::get_cairo() called"<<std::endl;
#endif
    if(renderinfo){return renderinfo.get_context()->cobj();}
    if(parent){return parent->get_cairo();}
    return NULL;
  }

  void PathEater::consume(){
#if DEBUG_PATHQUEUE
    std::ostringstream& out=get_outstream();
    out << "consume() called\nalive()? :"
	     <<(alive() ? "live":"dead")<<std::endl;
    std::cerr<<"PathEater::consume()\nalive()? :"
	     <<(alive() ? "live":"dead")<<std::endl;
#endif
    while(alive()|| (!pathsource->finished_jobs())){
      if(!pathsource->finished_job() && pathsource->queue_empty()){
#if DEBUG_PATHQUEUE
	std::cerr<<"while(alive()|| (!pathsource->finished_job())){\n	if(!pathsource->finished_job() && pathsource->queue_empty())... waiting..."<<std::endl;
	out << "while(alive()|| (!pathsource->finished_job())){\n	if(!pathsource->finished_job() && pathsource->queue_empty())... waiting..."<<std::endl;
#endif
	pathsource->wait_for_output();
      }
      else if (pathsource->finished_jobs()){
	//previously, blocked and waited here, now we let it return
	/*if(alive()){
	  #if DEBUG_PATHQUEUE
	  out << "alive() and pathsource->finished_job() ... waiting ...\nmaybe should die instead"<<std::endl;
	  #endif
	  pathsource->wait();
	  }
	  else{*/
	return;
	//}
      }
      consume_queue(get_paintcontext());
    }
    join();
  }
  PathEater::PathEater(PathMultiplier* source,   const PaintContext& paint,
		       BGRenderPath* mapa):
    ThreadKlass(NULL),
    pathsource(source),
    parent(mapa),
    renderinfo(paint),
    working_(false)
  {
    initialise();
  }
  void PathEater::initialise(){
#if DEBUG_PATHQUEUE
    std::cerr<< "PathEater at "<<this <<" created"<<std::endl;
    std::ostringstream* outstream=new std::ostringstream;
    ptrdiff_t address = (ptrdiff_t)(this);
    debug_output_streams[address]=outstream;
    *outstream << "PathEater at "<<this <<" created"<<std::endl;
#endif
  }


  PathEater::PathEater(BGtransform_despatcher* source,  const PaintContext& paint,
		       BGRenderPath* mapa):
    ThreadKlass(NULL),
    pathsource(source),
    parent(mapa),
    renderinfo(paint),
    working_(false)
  {
    initialise();
  }

  PathEater::~PathEater(){
#if DEBUG_PATHQUEUE 
    std::string tmp("patheater destructor called: printing log stream\n\t++++++++\n");
    get_outstream()<< "PathEater at "<<this <<" : destructor called"<<std::endl;
    tmp += get_log_output() + "\n\t+++++++++\nend of log output\n\n";
    NAMESPACE_THREAD_CERR(tmp);
    destroy_logstream();
#endif
  }

#if DEBUG_PATHQUEUE 
  std::ostringstream& PathEater::get_outstream()const{
    ptrdiff_t addr = (ptrdiff_t)this;
    log_map_iterator iter= debug_output_streams.find(addr);
    if(iter == debug_output_streams.end()){
      throw xn_Error("no logstream?");
    }
    std::ostringstream* outstream =  (*iter).second;
    return *outstream;
  }

  std::string PathEater::get_log_output()const{
    std::ostringstream& out = get_outstream();
    return out.str();

  }
  void PathEater::destroy_logstream(){
    std::ostringstream* strstr = &(get_outstream());
    log_map_iterator iter= debug_output_streams.find((ptrdiff_t)this);
    if(iter == debug_output_streams.end()){
      //Error, but we are in a destructor when this is called
      std::string e("ERROR: no logstream to be found, but can't throw inside destructor");
    }
    debug_output_streams.erase(iter);
    if(strstr){
      delete strstr;
    }
    strstr=NULL;
  }
#endif

  void BGRenderPath::set_paintcontext(const PaintContext& con){
#if DEBUG_PATHQUEUE
    if(logoutput){
      *logoutput << "BGRenderPath::set_paintcontext called" << std::endl;
    }
#endif
    //FIXME:no need for PaintContext in parent and child
    // - just something extra to go wrong.
    outputimage= con;
    if(renderer){
      renderer->set_paintcontext(outputimage);
    }
  }

  void BGRenderPath::set_paint_method(void(*cairomethod)(cairo_t*)){
#if DEBUG_PATHQUEUE
    std::cerr<<"BGRenderPath::set_paint_method called(cairomethod)" << std::endl;
    if(logoutput){
      *logoutput << "BGRenderPath::set_paint_method called(cairomethod)" << std::endl;
    }
#endif
    outputimage.set_paint_method(cairomethod);
    if(renderer){
      renderer->set_paint_method(cairomethod);
    }     
  }

  void BGRenderPath::set_paint_method(ColourRole role, bool preserve){
#if DEBUG_PATHQUEUE
    std::cerr<<"BGRenderPath::set_paint_method called(role,preserve)" << std::endl;
    if(logoutput){
      *logoutput << "BGRenderPath::set_paint_method called(role,preserve)" << std::endl;
    }
#endif
    void(*cairomethod)(cairo_t*) = &cairo_stroke;
    switch(role){
    case COLOURROLE_STROKE:
      if(preserve){
	cairomethod= &cairo_stroke_preserve;
      }
      break;
    case COLOURROLE_FILL:
      if(preserve){
	cairomethod= &cairo_fill_preserve;
      }
      else{cairomethod= &cairo_fill;}
    }
    set_paint_method(cairomethod);
  }

  void   BGRenderPath::pop(){
#if DEBUG_PATHQUEUE
    std::cerr<<"BGRenderPath::pop()" << std::endl;
    if(logoutput){
      *logoutput << "BGRenderPath::pop()" << std::endl;
    }
#endif
    input_pair_type p= pathswaiting.front();
    pathswaiting.pop();
//remove  set_paintcontext(p.second); - no more required
    set_paintcontext(p.first);
    set_path(p);
  }

  void BGRenderPath::add_path(const input_pair_type& p){
#if DEBUG_PATHQUEUE
    std::cerr<<"BGRenderPath::add_path(pair)" << std::endl;
    if(logoutput){
      *logoutput << "BGRenderPath::add_path(pair)" << std::endl;
    }
#endif
    pathswaiting.push(p);
    //something blocking here... what?
    if(pathproducer){pathproducer->wake();}
    if(renderer){renderer->wake();}
    wake();
  }

  void BGRenderPath::add_path(const simple_click_history& p){
#if DEBUG_PATHQUEUE
    std::cerr<< "BGRenderPath::add_path(path)" << std::endl;
    if(logoutput){
      *logoutput << "BGRenderPath::add_path(path)" << std::endl;
    }
#endif
    input_pair_type pr( outputimage, p);
    pathswaiting.push(pr);
    wake();
    //revive the eater ...
  }
  void BGRenderPath::kill(bool immediately){
#if DEBUG_PATHQUEUE
    std::cerr<<"BGRenderPath::kill()" << std::endl;
    if(logoutput){
      *logoutput << "BGRenderPath::kill()" << std::endl;
    }
#endif
    stay_alive=false;
    if(immediately || finished_job()){
      pathproducer->kill();}
  }

  void BGRenderPath::write_png_file(const std::string& filepath){
#if DEBUG_PATHQUEUE
    std::cerr<<"BGRenderPath::write_png_file" << std::endl;
    if(logoutput){
      *logoutput << "BGRenderPath::write_png_file" << std::endl;
    }
#endif
    //outputimage->stroke();//in case
    if(!outputimage){throw xn_Error("image NULL");}
    Cairo::RefPtr<Cairo::Surface> surface = outputimage.get_context()->get_target();
    surface->write_to_png(filepath);
  }

  void PathEater::consume_queue(const PaintContext & paint){
#if DEBUG_PATHQUEUE
    std::cerr<<"PathEater::consume_queue called"<<std::endl;
    get_outstream() << "consume_queue called"<<std::endl;
#endif
    PaintContext c;
    if(!paint){
      if(!renderinfo){
	throw xn_Error("PathEater::consume_queue():no valid PaintContext given");
      }
      c=renderinfo;
    }
    else {c=paint;}
    std::cerr<<"consume_queue b4 while"<<std::endl;
    int loopcount(0);
    while(!pathsource->finished_job()){
      std::cerr<<"about to call consume_pathwheel("<<std::endl;
      bool res = consume_pathwheel(c);
      std::cerr <<"consume_queue(cr): :result: "<<(res?"good":"bad")
		<<"\n"<<loopcount++<< " iterations"<<std::endl;
      if(!res){
	throw xn_Error("consume_pathwheel failed");
      }
    }
    if(!pathsource->finished_jobs()){pathsource->wake();}
    std::cerr<<"consume_queue: falling off end"<<std::endl;
  }
  void PathEater::create(PathEater*& outptr, BGtransform_despatcher* source,
			 const PaintContext& paint,
			 BGRenderPath* rend){
    PathEater* res = new PathEater(source, paint, rend);
    if(!res){
      throw xn_Error("PathEater::create() new PathEater(source, paint, rend) returned NULL");
    }
#if DEBUG_PATHQUEUE
    std::cerr<<"PathEater::create"<<res<<std::endl;
    res->get_outstream() << "created in create()"<<std::endl;
#endif
    outptr=res;
  }

  void PathEater::join(){//empty queue first
#if DEBUG_PATHQUEUE
    std::cerr<<"join called"<<std::endl;
    get_outstream()<< "join called"<<std::endl;
#endif
    while(!pathsource->finished_jobs()){
#if DEBUG_PATHQUEUE 
      std::cerr<<"!pathsource->finished_job()"<<std::endl;
      get_outstream()<< "!pathsource->finished_job()"<<std::endl;
#endif
      
      consume_queue(get_paintcontext());
if(pathsource->queue_empty()){
#if DEBUG_PATHQUEUE   
	std::cerr<< "no qeueue - call pathsource->wait_for_output()..."<<std::endl;
	get_outstream()<<"queue empty = pathsource->wait_for_output()..."<<std::endl;
#endif
	pathsource->wait_for_output();
      }
      //wait_for_threads();
    }
#if DEBUG_PATHQUEUE    
    std::cerr<< "calling ThreadKlass::join() -previously recursed, must be an error "<<std::endl;
    get_outstream() << "calling ThreadKlass::join() -previously recursed, must be an error "<<std::endl;
#endif
    working_ =false;
    wake();//awake a waiting thread
    ThreadKlass::join();
  }
  void BGRenderPath::run  () {
#if DEBUG_PATHQUEUE
    std::cerr<< "BGRenderPath::run" << std::endl;
    if(logoutput){
      *logoutput << "BGRenderPath::run" << std::endl;
    }
#endif
  
    waitloop();
    join();
  }

  void BGRenderPath::update_renderer(){
#if DEBUG_PATHQUEUE    
    std::cerr<< "update_renderer() called"<<std::endl;
    if(logoutput){
    *logoutput << "update_renderer() called "<<std::endl;
    }
#endif
    if(!renderer){
      std::pair<PathEater*, Glib::Thread*> eatpair = PathEater::create_in_thread( pathproducer, this->get_paintcontext(), this);
      renderer=eatpair.first;
      if(!renderer){
	std::cerr<< "BGRenderPath::run(): renderer NULL"<<std::endl;
	throw xn_Error("run():no renderer (NULL)");
      }
#if DEBUG_PATHQUEUE    
    std::cerr<< "created a renderer"<<std::endl;
    if(logoutput){
    *logoutput  << "update_renderer() created a renderer "<<std::endl;
    }
#endif
    }
    else {
      while(renderer->busy()){ 
#if DEBUG_PATHQUEUE    
	std::cerr<<"renderer->busy() ... waiting"<<std::endl;
	if(logoutput){*logoutput  << "waiting till last job finished..."<<std::endl;}
#endif
	renderer->wait();
      }
      //if(!renderer->busy()){
      //renderer->wake();
#if DEBUG_PATHQUEUE    
      std::cerr<< "idle renderer - staring a workthread"<<std::endl;
      if(logoutput){
	*logoutput << "update_renderer() idle renderer - staring a workthread "<<std::endl;
      }
#endif	
      renderer->run_in_thread();
    }
  }

  void BGRenderPath::set_cairo(Cairo::RefPtr<Cairo::Context> cr){
#if DEBUG_PATHQUEUE
    std::cerr<< "BGRenderPath::set_cairo called" << std::endl;
    if(logoutput){
      *logoutput << "BGRenderPath::set_cairo called" << std::endl;
    }
#endif
    outputimage.set_context(cr);
if(renderer){
      renderer->set_context(cr);
    }
  }

  void BGRenderPath::set_cairo(cairo_t* cr){
#if DEBUG_PATHQUEUE
    std::cerr<<"BGRenderPath::set_cairo_context(cairo_t)\ncr at " << cr<<std::endl;
    if(logoutput){
      *logoutput << "BGRenderPath::set_cairo_context(cairo_t)\ncr at " << cr<<std::endl;
    }
#endif
    Cairo::Context* crrr= new Cairo::Context(cr);
    Cairo::RefPtr<Cairo::Context> pcontext(crrr);
    set_cairo(pcontext);
  }

  
  BGRenderPath::~BGRenderPath(){
#if DEBUG_PATHQUEUE
    std::cerr<<"BGRenderPath::~destructor called"<<std::endl;
    if(logoutput){
      *logoutput <<"destructor called"<<std::endl;
      if(renderer){
	*logoutput << "adding renderer.get_log_output();\n"<< renderer->get_log_output()
		   <<std::endl;
      }
      std::string s= logoutput->str();
      NAMESPACE_THREAD_CERR(s);
      delete logoutput;
      logoutput=NULL;
    }
#endif
  }
  void BGRenderPath::waitloop(){
#if DEBUG_PATHQUEUE
    std::cerr<<"BGRenderPath::waitloop() called" << std::endl;
    if(logoutput){
      *logoutput << "BGRenderPath::waitloop() called" << std::endl;
    }
#endif
    bool firstloop=true;
    update_renderer();
    while(alive()){
      while(!pathproducer->finished_job()){
	if(!firstloop){
	  while(renderer->busy()){
	    renderer->wake();
	    renderer->wait();
	  }
	  update_renderer();
	}
	else{firstloop=false;}
#if DEBUG_PATHQUEUE
	std::cerr<<"while(alive()){while(!pathproducer->finished_job()){\npathproducer->wait_for_threads()\nwaiting..."<<std::endl;
	if(logoutput){
	  *logoutput << "while(alive()){while(!pathproducer->finished_job()){\npathproducer->wait_for_threads()\nwaiting..."<<std::endl;
	}
#endif
	pathproducer->wait_for_threads();
#if DEBUG_PATHQUEUE
	std::cerr<<"waitloop(): pathproducer woke up"<<std::endl;
#endif
      }	
      if(pathswaiting.size()){pop();}
      else if (!stay_alive){
#if DEBUG_PATHQUEUE
	std::cerr<< "about to call this->kill()"<<std::endl;
	if(logoutput){
	  *logoutput << "about to call this->kill()"<<std::endl;
	}
#endif
	this->kill();
      }
      if(writepng && pngfilename.size()){write_png_file(pngfilename);}
    }
    wake_all();
  }
 

  std::pair<PathEater*, Glib::Thread*> 
  PathEater::create_in_thread(BGtransform_despatcher* source, 
			      const PaintContext& paint,
			      BGRenderPath* renderer){
    PathEater* res=NULL;
    PathEater::create(res,source, paint,renderer);
    if(!res){
      throw xn_Error("PathEater::create() failed: NULL");
    }
    return res->run_in_thread();
  }


  std::pair<PathEater*, Glib::Thread*> 
  PathEater::run_in_thread(){
    if(working_){throw xn_Error("already patheater->busy(), wait till not ..., b4 staring another thread");}
    working_=true;
    Glib::Thread* thread = THREADCREATE_MEM(*this, &PathEater::consume,   true);
    if(!thread){
      throw xn_Error("thread creation failed");
    }
    set_thread(thread);
    return std::pair<PathEater*, Glib::Thread*> (this,thread);
  }


}
