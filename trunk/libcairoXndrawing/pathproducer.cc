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

#include "pathproducer.h"


namespace xNDRAW{
void path_transform_predicate(simple_click_history& path,  
			       PointTransformInPlace& transformation, 
			      PointPredicate* selector){
  typedef simple_click_history::iterator iterator;
  for(iterator w = path.begin(); w != path.end(); w++){
    if(selector){
      if((*selector)(*w)){
	transformation(*w);
      }
    }
    else{
	transformation(*w);
    }
  }
}

  //BGtransform_despatcher::set_maxthreads_pref(24);
  //set a necessary preference...
  //TODO: sane manner of doing same
  unsigned int dummy_pref(){
    //XNDRAW_SET_PREF("DRAWPOOL_MAXTHREADS",24);
    return 24;
  }
  //  static unsigned int threadnum= dummy_pref();
  int  BGtransform_despatcher::initcount=0;


  BGtransform_despatcher::BGtransform_despatcher(const std::pair<PaintContext,simple_click_history>& src):
    ThreadKlass(NULL),
    source_reset(false),
    source(src.second),
    output_context(src.first),
    threadpool(get_maxthreads_pref()),
    started_(false),
    kaput_(false){}

  void BGtransform_despatcher::wait_for_output(){
    queue.wait_for_output();
  }

  void BGtransform_despatcher::wait_to_input(){
    queue.wait_to_input();
  }

  void BGtransform_despatcher::wait_for_threads(){
    wait();
  }

  bool BGtransform_despatcher::idle()const{
    return (!threadpool.unprocessed()) && queue.empty();
  }

  bool BGtransform_despatcher::finished_job()const{
    //TODO:remove queue.empty()
    return started_ && (!threadpool.unprocessed()) ;
  }
bool BGtransform_despatcher::finished_jobs()const{
    //TODO:remove queue.empty()
    return finished_job()  && queue.empty();
  }

  bool BGtransform_despatcher::alive()const{
    return !kaput_;
  }

  void BGtransform_despatcher::kill(){kaput_ = true;}

  void PathMultiplier::create_threads(){
    //todo: nonblocking!
    MutexNodedeq* tolockonly=const_cast<MutexNodedeq*>(nodes);
    MutexNodedeq::Lock lock(*tolockonly, NCOORDS::LOCKINFO_READ(), Glib::TRY_LOCK); 
    if (!lock.locked()){
      // tolockonly->set_inhibit_writers();
      while(!lock.try_acquire_reader()){
	std::cerr<<"waiting for reader\n";
	tolockonly->wait_for_reader();
      }
    }
      // tolockonly->set_inhibit_writers(false);
    
    for(const_node_iterator w =nodes->begin();w!= nodes->end();w++){
      launch_thread(w);//blocks when gets to max threads,woken as a thread dies
    }
    //tolockonly->unlock_reader();
  }

  void BGtransform_despatcher::push(container_type* pathwheel){
    // Paintcontext  pcontext(this->get_paintcontext());
    // pair_type pathpair=pair_type(pathwheel, pcontext);
    queue.push(output_pair_type( output_context, pathwheel));
  }
 
  //shutdown threadpool(and wait till its done, then join the main thread)
  void  BGtransform_despatcher::join(){
    threadpool.shutdown();//thread BLOCKS UNTIL all threadpool tasks
    //including those not yet started
    wake_all();//wake all waiters
    ThreadKlass::join();
  }

  void BGtransform_despatcher::loop(){
    initialise();
    while (alive()){
      Glib::usleep(300);
      if(source_reset){
	std::cerr<<"BGtransform_despatcher::loop: source was reset"<<std::endl;
	source_reset=false;
	//started=true;//launch thread will do this
	create_threads();
      }
      //could look for the next path in a queue here
      //let parent do that
      //currently this thread just starts the threadpool,and waits
      //and creates threads(probably right)
    }
    join();
  }

  BGtransform_despatcher::output_pair_type BGtransform_despatcher::pop(){
    std::cerr<<"BGtransform_despatcher::output_pair_type BGtransform_despatcher::pop()"<<std::endl;
    output_pair_type res = queue.front();
    queue.pop();
    while(!finished_job()){   
      wait_for_threads();
    }
    return res;
  }

  PathMultiplier::PathMultiplier(const input_pair_type& pat, 
				 const MutexNodedeq* nods,
				 const ncoords* nco, 
				 const coords* data_c): 
    BGtransform_despatcher(pat),
    nodes(nods), 
    draw_ncoords(nco)    {
    if(data_c){data_centre= *data_c;}
    else{data_centre=*(*(nodes->begin()));}//should be same as (coords&)nco
    
  }

  coords PathMultiplier::get_offset(MutexNodedeq::const_iterator iter)const{
    const coords& ref= *(*iter);
    return ref - data_centre;
  }

  void PathMultiplier::initialise(){
    started_=false;//make finished() return true, whether a path is 
    if(source.size()){ create_threads();}
    else{started_=true;}
  }

  std::pair<PathMultiplier*, Glib::Thread*> 
  PathMultiplier::create_in_thread( const input_pair_type& path,
				    const MutexNodedeq* nod,
				    const ncoords& fromdraw){
    PathMultiplier* res(NULL);
    PathMultiplier::create(res, path, nod, fromdraw);
    Glib::Thread* thread= Glib::Thread::create(sigc::mem_fun(*res,&PathMultiplier::loop), true);
    if(!thread ){throw xn_Error("thread NULL");}
    res->set_thread(thread);
    return std::pair<PathMultiplier*, Glib::Thread*> (res,thread);
  }

  unsigned int BGtransform_despatcher::get_maxthreads_pref(){
    if(!initcount){
      XNDRAW_SET_PREF("DRAWPOOL_MAXTHREADS",24);
      initcount++;
    }
    unsigned int res;
    return XNDRAW_GET_PREF("DRAWPOOL_MAXTHREADS",res);
  }

  bool BGtransform_despatcher::set_path(const input_pair_type& p){
    if(!p.second.size()){return false;}
    while (started_ && threadpool.unprocessed()){//job_finished() includes queue.empty, so it could never get past that?
      std::cerr <<"BGtransform_despatcher::set_path, unprocessed threads, must wait"<<std::endl;
      wait();
      std::cerr <<"BGtransform_despatcher::set_path, awoke..."<<std::endl;
    }
    source=p.second;
    output_context = p.first;
    started_=false;
    source_reset=true;//what does this do exactly
    wake();
    return true;
  }

  void PathMultiplier::launch_thread(MutexNodedeq::const_iterator iter){
    coords offs = get_offset(iter);
    threadpool.push(sigc::bind(sigc::mem_fun(*this,&PathMultiplier::thread_func), offs));
    if(!started_){started_=true;}
  }

  PathRotator::container_type* PathRotator::create_paths(){
    const coords *subpathstart=NULL;
    //predicates here
    //pseudo
    //selektor=drawingset->pointselectotpred
    //tranformer=drawingset->transform
    bool invert=false;
    bool reverse= false;
    container_type* result = new container_type();
    std::cerr <<"created path container"<<std::endl;
    size_t pathcount= node.get_n();
    for(size_t i=0 ; i< pathcount;i++){
      SourceTransformInfo infosrc(source_transform_info(i));
      typedef std::pair<simple_click_history, SourceTransformInfo> render_type;
      result->push_back(render_type(simple_click_history(), infosrc));
    }
    std::cerr <<"added paths\n";
    for(const_iterator w= path->begin();w!=path->end();w++){
      pointholder.clear();
      
      int state=  (*w).second;
      node.set_radial((*w).first + offset);
      node.fill_container(pointholder,invert,reverse);
      std::cerr <<"fill container\n";
      deqC::const_iterator startmarker= pointholder.begin();
      int cnt =0;
      for(deqC::const_iterator iter = pointholder.begin(); iter !=pointholder.end();iter++, cnt++){
	simple_click_history& dest =(*(result->begin()+cnt)).first;
	dest.push_back(std::pair<coords, int>(*(*iter),state));
       
      }
      std::cerr <<"added a joint to each"<<std::endl;
    }
    return result;
  }

  void PathRotator::do_generate(PathRotator::queue_type& q){
 container_type* result = create_paths();
    controller->push(result);
  controller->wake();//this should be last line
  }

  void ControlledPathRotator::generate(){
    container_type* result = create_paths();
    controller->push(result);
    controller->wake();//this should be last line
  }
  const simple_click_history* PathMultiplier::get_source()const{
    return &source;
  }

  const coords& PathMultiplier::get_map_centre()const{
    return data_centre;
  }

    
  const ncoords&  PathMultiplier::get_ncoords()const{
    return *draw_ncoords;
  } 
  PathRotator::PathRotator(queue_type& q,
			   const simple_click_history& path,
			   const ncoords& rot, 
			   const coords& off):
    NodeProducer<ncoords>(NULL,rot, path, off){
    node +=offset;//move to base
node.set_state((coord_state)offset.get_state());}

  ControlledPathRotator::ControlledPathRotator(PathMultiplier* pathcontrol, 
					       const coords& offsett):
    PathRotator(pathcontrol->get_queue(), *(pathcontrol->get_source()), 
		pathcontrol->get_ncoords() , offsett),
    controller(pathcontrol){} 
 

}
