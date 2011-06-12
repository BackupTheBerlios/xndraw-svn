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


#ifndef LIBCAIROXNDRAWING_PATHPRODUCER_H
#define LIBCAIROXNDRAWING_PATHPRODUCER_H

#include "xndrawexceptions.h"

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <iostream>
#include "clickhistory.h"
#include "transpredicate.h"
#include "threadklass.h"
#include "paintcontext.h"
#include <queue>
#include <glibmm.h>
#include <glibmm/thread.h>
#include <glibmm/threadpool.h>
#include "guardedqueue.h"
#include "io_uncollide.h"
//threadsafe stream inserters


//from upstream library... -link problems?
//need a way of setting the prefs at startup,
//or move the definition of the prefs object to a lower level
#include "x3draw.h"

#define THREADCREATE_PTR(ptr,joinable)			\
  Glib::Thread::create(sigc::ptr_fun(ptr),joinable)

#define THREADCREATE_MEM(obj, member,joinable)			\
  Glib::Thread::create(sigc::mem_fun(obj, member),joinable)

#define THREADCREATE_BIND1_PTR(ptr,arg,joinable)			\
  Glib::Thread::create(sigc::bind(sigc::ptr_fun(ptr),arg),joinable)

#define THREADCREATE_BIND1_MEM(obj, member,arg,joinable)		\
  Glib::Thread::create(sigc::bind(sigc::mem_fun(obj, member),arg),joinable)

#define THREADCREATE_BIND2_PTR(ptr,arg0,arg1,joinable)			\
  Glib::Thread::create(sigc::bind(sigc::ptr_fun(ptr),arg0,arg1),joinable)

#define THREADCREATE_BIND2_MEM(obj, member,arg0,arg1,joinable)		\
  Glib::Thread::create(sigc::bind(sigc::mem_fun(obj, member), arg0,arg1),joinable)

#define THREADCREATE_BIND3_PTR(ptr,arg0,arg1,arg2, joinable)		\
  Glib::Thread::create(sigc::bind(sigc::ptr_fun(ptr),arg0,arg1,arg2),joinable)

#define THREADCREATE_BIND3_MEM(obj, member,arg0,arg1,arg2, joinable)	\
  Glib::Thread::create(sigc::bind(sigc::mem_fun(obj, member), arg0,arg1,arg2),joinable)

#define THREADCREATE_BIND4_PTR(ptr,arg0,arg1,arg2,arg3, joinable)	\
  Glib::Thread::create(sigc::bind(sigc::ptr_fun(ptr),arg0,arg1,arg2,arg3),joinable)

#define THREADCREATE_BIND4_MEM(obj, member,arg0,arg1,arg2,arg3, joinable) \
  Glib::Thread::create(sigc::bind(sigc::mem_fun(obj, member), arg0,arg1,arg2,arg3),joinable)


namespace xNDRAW{

  using namespace NCOORDS;
  using NCOORDS::ThreadKlass;
  

  //TODO: need the actual predicates and transforms(flipping etc)
  template <typename T>
    class ThreadCleanupPtr{

    T* functor;
    template <typename ARG>
      ThreadCleanupPtr(ARG aeg){
      functor = new T(aeg);
      functor();
    }
    virtual ~ThreadCleanupPtr(){delete functor;}

  };

  class PathTransformBase{
  public:
    typedef NCOORDS::simple_click_history simple_click_history;
    typedef simple_click_history path_type;
    typedef path_type::size_type size_type;
    typedef simple_click_history::iterator path_iterator;
    typedef simple_click_history::const_iterator const_path_iterator;
    typedef simple_click_history::reference reference;
    typedef simple_click_history::const_reference const_reference;
    typedef simple_click_history::value_type value_type;
    typedef std::pair<simple_click_history, SourceTransformInfo> rendersource_type;
    typedef std::deque<rendersource_type> container_type;    
    typedef GuardedQueue<container_type*> queue_type;
    typedef TransformGroup::iterator iterator;
    typedef TransformGroup::const_iterator const_iterator;
    typedef PredicateGroup::iterator predicate_iterator;
    typedef PredicateGroup::const_iterator const_predicate_iterator;

  protected:
    TransformGroup m_transformer;//hold pointers to the actual predicates and functions in derived types
    PredicateGroup  m_predicate;

  public:

  PathTransformBase(): 
    m_transformer(NULL),  
      m_predicate(PointPredicate( &PointPredicate::decide) ){}

  PathTransformBase(PointTransform* trans, const PointPredicate& pred):
    m_transformer( trans),  m_predicate(pred){}

    virtual void set_step(size_type n){}//overload

    //kinda useless as is
    template <typename INPUTITER, typename OUTPUTITER>
      void reproduce(INPUTITER first, INPUTITER last, OUTPUTITER dest){
      INPUTITER anchor =first;
      for(;first != last;first++, dest++){
	if(m_predicate.eval_iterator_range(anchor,first, last)){
	  //these (eval_iterator() and do_transform())
	  //should take 3 iterators ie (rangestart,iter, rangeend)
	  //so the implementation can look back and forward
	  //to previous and next point
	  m_transformer.do_transform(first, last, dest);
	}
      }
    }

    iterator begin(){return m_transformer.begin();}
const_iterator begin()const{return m_transformer.begin();}
iterator end(){return m_transformer.end();}
const_iterator end()const{return m_transformer.end();}

 iterator erase_transform(iterator which){
   return m_transformer.erase(which);
 }

 predicate_iterator  erase_predicate( predicate_iterator  which){
   return m_predicate.erase(which);
 }

predicate_iterator preds_begin(){return m_predicate.begin();}
const_predicate_iterator preds_begin()const{return m_predicate.begin();}
predicate_iterator preds_end(){return m_predicate.end();}
const_predicate_iterator preds_end()const{return m_predicate.end();}
  };

  //the key class
  template <typename NODE>
    class PathTransform:public PathTransformBase{
    typedef PathTransformBase base_type;
    typedef ObjectPointTransform<NODE> functor_type;
    

  protected:
    ObjectPointTransform<NODE>* transformer;//m_transformer.functor
    PointPredicate* inputpredicate;//m_predicate;
    ClipPredicate clipcontrol;//to be called after the transformation to cull undrawable offbuffer lines and points(and trim those that cross -2 pass process -fisrt eliminate anypoints joiningonly to offscreen points
    PointFlip* flipper;//in m_transformer
    //InPlaceTransformGroup* posttransformation;
    const NODE* node;
    const simple_click_history* sourcepath;
    coords offset;//for sourcepath - node already offset
    bool flippath;
    bool reversepath;
   

  public:
    template <typename OUTITER>
      void iterate(const_path_iterator first, const_path_iterator last,  OUTITER dest){
      simple_click_history tmppath;
      const_path_iterator anchor = first;
      for(;first != last; first++){
	if(m_predicate.eval_iterator_range(anchor,first, last)){
	  //tmppath.push_back(m_transformer(*first));
	  *dest++ =m_transformer(*first);
	}
      }
      //clipcontrol.fix_path(tmppath, dest);
    }

  
    SourceTransformInfo source_transform_info(size_type step=0){
      return SourceTransformInfo(offset, step * node.get_step());
    }

  PathTransform(const NODE& n, 
		const NCOORDS::coords& offs, 
		const NCOORDS::simple_click_history* p=NULL,
		PointPredicate* inputpred = PointPredicate::create_true_predicate(),
		bool flip=false, bool rev=false):
    transformer(new ObjectPointTransform<NODE>(n,          reversepath )),
    node(&n), flipper(NULL),sourcepath(p),offset(offs),
    flippath(flip),reversepath(rev){
    
    if(flippath){
    const coords& pathstart = sourcepath->get_first_point(MOVE_CLICK);
    flipper = new PointFlip((const coords&)(*node), pathstart);
 m_transformer.push_back(flipper);
    }
    else{flipper=NULL;}
    m_predicate.push_back(inputpred);
  
    if(reversepath){transformer->set_reverse_predicate(true);}
      m_transformer.set_functor(transformer);
    }

    void set_flipper(PointFlip* flip){
      for(TransformGroup::iterator w = begin(); w!= end();w++){
	PointFlip* oldflip= dynamic_cast<PointFlip*>(*w);
	if(oldflip){
	  w = m_transformer.erase(w);
	  if(oldflip != flipper){
	    throw xn_Error("seulement, nur, only one flipper please!?");
	  }
	  break;
	}
      }
      flipper =flip;
      if(flip){
	m_transformer.push_front(flipper);//rotator fn called b4 this
      }
    }
      void set_posttransform(PointFlip* flip){
	set_flipper(flip);
      }
      void set_step(size_type n){
	if(flipper){
	  Segment<coords> seg= node->get_nth_radial_segment(*sourcepath, offset, n);
	  flipper->set_reflectaxis(seg);
	}
      }
      template <typename FWDITER, typename OUTITER>
	void operator()(FWDITER first, FWDITER last, OUTITER outiter){
	FWDITER current=first;
	for(;current != last;current++){
	  if(m_predicate.eval_iterator_range(first, current, last)){
	    *outiter++= m_transformer(first, current, last);
	    
	  }
	}
      }
      void add_nth_path(container_type& container, size_type n, 
			const_iterator first, const_iterator last){
	SourceTransformInfo info(source_transform_info());
	container.push_back(rendersource_type(simple_click_history(), info));
	simple_click_history& target = (*(container.end()-1)).first;
	set_step(n);
	iterate(first, last,std::back_inserter(target));
      }
    };
 class BGtransform_despatcher;

    class PathMultiplier;
  template <typename NODE>
  class PathNodeMultiplier;
 
    struct NodeProducerBase{
      //base class - don't/can't use directly
    public:
      typedef simple_click_history path_type;
      typedef simple_click_history::size_type size_type;
typedef std::pair<simple_click_history, SourceTransformInfo> rendersource_type;
      typedef std::deque<rendersource_type> container_type;
      typedef simple_click_history::const_iterator const_iterator;
      typedef simple_click_history::iterator iterator;
      typedef container_type::iterator container_iterator;
      typedef container_type::const_iterator const_container_iterator;
      typedef std::pair<PaintContext, simple_click_history> input_pair_type;
      typedef std::pair<PaintContext,container_type* > output_pair_type;

      typedef GuardedQueue<output_pair_type > queue_type;

      NodeProducerBase(queue_type& q):outputqueue(q){}
      virtual ~NodeProducerBase(){}
      virtual void generate(){throw xn_Error("base type-overload");}
      virtual void produce(){generate();}

      virtual container_type* create_paths(){throw xn_Error("base type-overload");}

    protected:
      queue_type& outputqueue;
    };

    template<typename NODE>
      class NodeProducer: public NodeProducerBase{
    public:
      friend class PathNodeMultiplier<NODE>;
      typedef  BGtransform_despatcher controller_type;
      typedef NodeProducerBase base_type;
      using  base_type::outputqueue;
    protected:

      controller_type* controller;
      NODE node;
      coords offset;
      const simple_click_history* path;
      bool flippath;
      bool reversepath;
PathTransform<NODE> pathtransformer;
  
    public:
      //global settings -TO go: let predicates in ObjectPathTransform handle
      bool is_flipped()const{return flippath;}
      bool is_reversed()const{return reversepath;}
  
      
    public:
      virtual ~ NodeProducer(){}
      NodeProducer(controller_type* control,
		   const NODE& nod, 
		 const simple_click_history& p,
		 const coords& offs,
	     PointPredicate* inputpred,
	     PointPredicate*flip_pred,
		   PointPredicate* rev_pred);

      NodeProducer(controller_type* control,
		   const NODE& nod, 
		 const simple_click_history& p,
		 const coords& offs,
		 PointPredicate* inputpred= PointPredicate::create_true_predicate(),
		   bool flip=false, bool reverse=false);
      
      Segment<coords> get_nth_radial_segment(size_type n){
	return node.get_nth_radial_segment(*path, offset, n);
      }

      SourceTransformInfo source_transform_info(size_t step){
	return 	SourceTransformInfo(node.get_centre(),offset, step * node.get_step());
      }

      virtual void generate();

      void do_generate(queue_type& q){
	q.push(create_paths());
      }
      //need version with flipp and reverse as predicates(handle selection etc)
      static bool threadfunction(controller_type* control,
				 const NODE& nod, 
		 const simple_click_history& p,
				 const coords& offs,	
			   PointPredicate* inputpred,
				 PointPredicate* flippred= PointPredicate::create_false_predicate(), PointPredicate* revpred=PointPredicate::create_false_predicate());
      //create in a threadpool not infividually
      // Glib::Thread::create(sigc::bind(sigc::ptr_fun(&NodeProducer<NODE>::threadfunction),q,nod,p,offs,inputpred,flipped,reversed));

static void run_threadfunction(controller_type* control,
				 const NODE& nod, 
		 const simple_click_history& p,
				 const coords& offs,	
				 PointPredicate* inputpred = PointPredicate::create_true_predicate(),
				 bool flipped = false,
			       bool reversed=false){
Glib::Thread* thread = Glib::Thread::create(sigc::bind(sigc::ptr_fun(&NodeProducer<NODE>::threadfunction),control,nod,p,offs,inputpred,flipped,reversed));
}

      static bool threadfunction(controller_type* q,
				 const NODE& nod, 
		 const simple_click_history& p,
				 const coords& offs,	
				 PointPredicate* inputpred = PointPredicate::create_true_predicate(),
				 bool flipped = false,
				 bool reversed=false);

      virtual container_type* create_paths(){      
	//bool flipped(is_flipped());
	container_type* res = new container_type();
	if(!res){throw xn_Error("no memory");}
	for (size_type i =0; i< node.get_n(); i++){
	  SourceTransformInfo inf = this->source_transform_info(i);
	  res->push_back(rendersource_type(simple_click_history(), inf));
	  simple_click_history& target=( *(res->end()-1)).first;
	    pathtransformer.set_step(i);
	     pathtransformer.iterate(path->begin(), path->end(), std::back_inserter<simple_click_history>(target));	    
	  }
    
	return res;
      }




      /*
	if((*w).second & MOVE_CLICK){path_break=true;}
	else if(path_break && ((*w).second & CLOSE_PATH)){
	continue;//skipped_insertion==true already(or its a logic error)
	}
	if (!pathtransformer.predicate(*w)){skipped_insertion=true;continue;}//is_cairo_click etc, selected etc
	if((skipped_insertion ||path_break)&&((*w).second & BEZIER_HANDLE)){continue;}
	std::pair <coords, int> tmpresult=pathtransformer.transform(*w);
	if(path_break){
	if(tmpresult.second & LINE_CLICK){
	tmpresult.second ^= (LINE_CLICK|MOVE_CLICK);
	}
	if(tmpresult.second & BEZIER_CLICK){
	tmpresult.second ^= (BEZIER_CLICK|MOVE_CLICK);
	}
	if(!(tmpresult.second & MOVE_CLICK)){
	#ifdef DEBUG
	throw xn_Error("should be  MOVE_CLICK!");
	#else
	tmpresult.second |=MOVE_CLICK;
	#endif
	}
	}
	if(flipped){
	tmpresult = (*flipper)(tmpresult);
	}
	b4proxy =afterproxy;
	afterproxy = tmpresult;
	after=Segment<coords*>(&b4proxy, &afterproxy);
	if(clippredicate.in_bounds(tmpresult)){
	if(!path_break){
	if(skipped_insertion && !clippredicate.in_bounds(b4proxy)){
	coords tmp = clippredicate.fix(b4proxy, after);
	target.push_back(std::pair<coords, int>(tmp, b4proxy));
	}
	}
	path_break = skipped_insertion=false;
	target.push_back(tmpresult);
	}
	else if(clippredicate.in_bounds(b4proxy) && !path_break){
	tmpresult = clippredicate.fix(tmpresult, after);
	path_break = skipped_insertion=false;
	target.push_back(tmpresult);
	}
	else if(path_break && target.size()){
	simple_click_history::iterator prev =*(target.end()-1);
	Segment<coords*> 
	}
	//target.push_back(tmpresult)
	//////
	if(after.second){
	afterproxy0 =afterproxy;
	after.first = after.second;
	}
	afterproxy = tmpresult.first;
	after.second= &afterproxy;
	std::pair<Trinary, Trinary> = good_segment(after);
	}
      */

    };

    void path_transform_predicate(simple_click_history& path,  
				  PointTransformInPlace& transformation, 
				  PointPredicate* selector);
    class PathRotator: public NodeProducer<NCOORDS::ncoords>{
    public:
      //typedef simple_click_history path_type;
      typedef NodeProducer<NCOORDS::ncoords> generator_type;
      typedef generator_type::container_type container_type;
typedef std::pair<PaintContext,simple_click_history> input_pair_type;
      typedef std::pair<PaintContext,container_type*> output_pair_type;
      
      
      //typedef std::deque<path_type> container_type;
      typedef simple_click_history::const_iterator const_iterator;
  
      typedef GuardedQueue<std::pair<PaintContext, container_type*> > queue_type;
    protected:
  
      deqC pointholder;
   
    public:
      PathRotator(queue_type& q, const simple_click_history& pathpair, 
		  const ncoords& rot, const coords& off);
    protected:
      //not AAVailable
      PathRotator(const PathRotator&);
      PathRotator& operator=(const PathRotator&);

    public:
      container_type* create_paths();
      void do_generate(queue_type& q);
    };

    class ControlledPathRotator:public PathRotator{
    public:
    
      //typedef simple_click_history path_type;
      //typedef std::deque<path_type> container_type;
      //typedef simple_click_history::const_iterator const_iterator;
    protected:
      
      PathMultiplier* controller;
    public:
      ControlledPathRotator(PathMultiplier* pathcontroL,
			    const coords& offsett);

    protected:
      //not AAVailable
      ControlledPathRotator(const  ControlledPathRotator&);
      ControlledPathRotator& operator=(const  ControlledPathRotator&);

    public:
      void generate();
    };
  
    //ThreadKlass
    class BGtransform_despatcher: public NCOORDS::ThreadKlass{

      //TODO: output queue to hold a PaintContext mapped to each container
      //input queue to hold a PaintContext mapped to each path
      //otherwise where various rendered paths end up a matter of chance

    public:
      typedef simple_click_history path_type;
      typedef std::pair<simple_click_history, SourceTransformInfo> rendersource_type;
typedef std::deque<rendersource_type> container_type;
      typedef std::pair<PaintContext,simple_click_history> input_pair_type;
      typedef std::pair<PaintContext,container_type*> output_pair_type;
      typedef simple_click_history::const_iterator const_iterator;
      typedef GuardedQueue< output_pair_type> queue_type;
    protected:
      //coordmap has a lock ...
      //it will need a lock
      static int initcount; 
      bool source_reset;
      simple_click_history source;
      PaintContext output_context;
      //@member source: take copy from draw at start of 
      //each despatch round(if BGblah exists for multiple rounds)
      queue_type queue;//GuardedQueue
      Glib::ThreadPool threadpool;//they do the work, putting it in queue
      bool started_;
      bool kaput_; 
    public:
  
      BGtransform_despatcher(const std::pair<PaintContext, simple_click_history>& src);
      virtual ~BGtransform_despatcher(){}

    protected:
      //disallowed
      BGtransform_despatcher(const BGtransform_despatcher&);
      BGtransform_despatcher& operator=(const  BGtransform_despatcher&);
    public:

      bool alive()const;
      void kill();
 
      virtual void loop();
    
      const simple_click_history* get_source()const {return &source;}
      simple_click_history* get_source(){return &source;}

      std::pair<PaintContext, simple_click_history> get_source_pair(){
	std::pair<PaintContext, simple_click_history> res( output_context, source);
	return res;
      }
      
      void wait_for_output();
      void wait_to_input();
      void wait_for_threads();
      //    void add_to_queue(container_type* pathwheel);

      bool finished_job()const;
      bool finished_jobs()const;

      void push(container_type* pathwheel);

      
      void push(const std::pair<PaintContext, container_type*>& pairwheel);
      static void pathwheel_dispose(container_type* pathwheel){
	delete pathwheel;
      }
     
      queue_type& get_queue(){return queue;}
   const queue_type& get_queue()const{return queue;}

      output_pair_type pop();

      virtual void join();

      inline bool queue_empty(){return queue.empty();}

      virtual void create_threads(){}
      virtual void initialise(){throw xn_Error ("base class");}
      virtual      bool set_path(const input_pair_type& p);
      bool idle()const;
      bool ready()const{
	return ! threadpool.unprocessed();
      }

      static unsigned int get_maxthreads_pref();

      static void set_maxthreads_pref(unsigned int v){
	XNDRAW_SET_PREF("DRAWPOOL_MAXTHREADS",v);
      }

 
    };

  template<typename NODE>
  class PathNodeMultiplier: public BGtransform_despatcher{
    public:
    typedef MutexNodedeq::const_iterator const_node_iterator ;
    typedef NodeProducer<NODE> generator_type;
    typedef  BGtransform_despatcher base_type;
    using base_type::get_queue;

    protected:

const MutexNodedeq* nodes;
      coords data_centre;
   
      const NODE* draw_ncoords;
 
    public:
    PathNodeMultiplier(const std::pair<PaintContext,simple_click_history>& pat, 
		     const MutexNodedeq* nods,
		     const NODE* nco, 
			 const coords* data_c=NULL):
	BGtransform_despatcher(pat),
	nodes(nods),
	draw_ncoords(nco){
	if(data_c){data_centre=*data_c;}
 else{data_centre=*(*(nodes->begin()));}//should be same as (coords&)nco
      }

      virtual   ~PathNodeMultiplier(){}
    protected:
      //nonono
      PathNodeMultiplier(const PathNodeMultiplier&);
      PathNodeMultiplier& operator=(const PathNodeMultiplier&);
    public:

    coords get_offset(const_node_iterator iter)const{
    return *(*iter) - data_centre;
    }
      static void create(PathNodeMultiplier*& out_producer,
			 const simple_click_history& path,
			 const MutexNodedeq* nod,
			 const ncoords& fromdraw){
	PathNodeMultiplier* pm = new PathNodeMultiplier(path,nod, &fromdraw);
	out_producer= pm;
      }
      void initialise(){
    started_=false;//make finished() return true, whether a path is 
    if(source.size()){ create_threads();}
    else{started_=true;}
  }

      static std::pair<PathNodeMultiplier*, Glib::Thread*> 
      create_in_thread( const std::pair<PaintContext, simple_click_history>& path,
			const MutexNodedeq* nod,
			const NODE& fromdraw){
			    //PathNodeMultiplier* res(NULL);
    PathNodeMultiplier* res = new PathNodeMultiplier(path, nod, &fromdraw);
    Glib::Thread* thread= Glib::Thread::create(sigc::mem_fun(*res,&PathNodeMultiplier::loop), true);
    if(!thread ){throw xn_Error("thread NULL");}
    res->set_thread(thread);
    return std::pair<PathNodeMultiplier*, Glib::Thread*> (res,thread);
  }

    //void create_thread(const coords * offset);

      virtual void create_threads(){
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

      void launch_thread(const_node_iterator iter){
    coords offs = get_offset(iter);
    threadpool.push(sigc::bind(sigc::mem_fun(*this,&PathNodeMultiplier::simple_thread_func), offs));
    if(!started_){started_=true;}
  }//better as template?
    void thread_func(const coords& offset, PointPredicate* inputpred,
		     PointPredicate* flippred,  PointPredicate* reversepred){
      bool res=generator_type::threadfunction(this, *draw_ncoords, source,
					      offset-data_centre,inputpred, 
					      flippred,  reversepred);
      wake();
    }
      void simple_thread_func(const coords& offset){
	bool res=generator_type::threadfunction(this, 
						*draw_ncoords, source,
						offset-data_centre/*,
						PointPredicate::create_true_predicate(), false, false*/); //defaults
	wake();
      }

  
    //ControlledPathRotator rot(this, offset);
    //	rot.generate();
    //}
    const coords& get_map_centre()const{return data_centre;}
    const simple_click_history* get_source()const{return source;}

    const NODE&  get_ncoords()const{//get_node() better
      return *draw_ncoords;
    }
  };
  template <typename NODE>
  void NodeProducer<NODE>::generate(){
    
    controller->push(create_paths());
    controller->wake();//this should be last line
  }


    class PathMultiplier: public BGtransform_despatcher{
    public:
      typedef MutexNodedeq::const_iterator const_node_iterator ;
    protected:

      friend class PathRotator;
      const MutexNodedeq* nodes;
      coords data_centre;
   
      const ncoords* draw_ncoords;
 
    public:
      PathMultiplier(const std::pair<PaintContext, simple_click_history>& pat, 
		     const MutexNodedeq* nods,
		     const ncoords* nco, 
		     const coords* data_c=NULL);

      virtual   ~PathMultiplier(){}
    protected:
      //nonono
      PathMultiplier(const PathMultiplier&);
      PathMultiplier& operator=(const PathMultiplier&);
    public:

      coords get_offset(const_node_iterator iter)const;

      static void create(PathMultiplier*& out_producer,
			 const input_pair_type& path,
			 const MutexNodedeq* nod,
			 const ncoords& fromdraw){
	PathMultiplier* pm = new PathMultiplier(path,nod, &fromdraw);
	out_producer= pm;
      }
      void initialise();

      static std::pair<PathMultiplier*, Glib::Thread*> 
	create_in_thread( const input_pair_type& path,
			  const MutexNodedeq* nod,
			  const ncoords& fromdraw);

      void create_thread(const coords * offset);

      virtual void create_threads();
      void launch_thread(const_node_iterator);//better as template?
     
      void thread_func(const coords& offset){
	ControlledPathRotator rot(this, offset);
	rot.generate();
      }
      const coords& get_map_centre()const;
      const simple_click_history* get_source()const;

      const ncoords&  get_ncoords()const;
    };


    class BGtransform:
    public std::unary_function<void,void>{

    public:
      typedef std::deque<simple_click_history> result_type;
      typedef std::queue<result_type> queue_type;
    protected:
      const simple_click_history& source;
      queue_type* queue;
      Glib::Mutex* queue_mutex;
    public:
    BGtransform(const simple_click_history& src, queue_type* q, Glib::Mutex* m):
      source(src),queue(q),queue_mutex(m){}

    };


  template <typename NODE>
  NodeProducer<NODE>::NodeProducer(controller_type* control,
		   const NODE& nod, 
		 const simple_click_history& p,
		 const coords& offs,
	     PointPredicate* inputpred,
	     PointPredicate*flip_pred,
	     PointPredicate* rev_pred):
	NodeProducerBase(control->get_queue()),
	controller(control),
      node(nod),offset(offs),
      path(&p	   ),
flippath(false),reversepath (false),
pathtransformer(node,offset, path, inputpred, flip_pred,rev_pred)
{

}

 template <typename NODE>
  NodeProducer<NODE>::NodeProducer(typename  NodeProducer<NODE>::controller_type* control,
				   const NODE& nod, 
				   const simple_click_history& p,
				   const coords& offs,
				   PointPredicate* inputpred,
				   bool flip, bool reverse): 
    NodeProducerBase(control->get_queue()),//queue member not needed,confusing
    controller(control),
    node(nod),offset(offs),
    path(&p	   ),
    flippath(flip),reversepath (reverse),
    pathtransformer(node,offset, path, inputpred){
   //clippredicate.set_cliparea(clippredicate.buffer_area());
 }

template <typename NODE>
bool NodeProducer<NODE>::threadfunction(controller_type* control,
				 const NODE& nod, 
		 const simple_click_history& p,
				 const coords& offs,	
			   PointPredicate* inputpred,
			   PointPredicate* flippred, PointPredicate* revpred){
NodeProducer prod(control, nod, p, offs, inputpred ,flippred, revpred);

 control->push(rendersource_type(prod.create_paths(), source_transform_info()));
       
 control->wake();
	return true;
      
}

template <typename NODE>
bool NodeProducer<NODE>::threadfunction(controller_type* control,
				 const NODE& nod, 
		 const simple_click_history& p,
				 const coords& offs,	
				 PointPredicate* inputpred,
				 bool flipped ,
				 bool reversed){
	NodeProducer prod(control, nod, p, offs, inputpred ,flipped, reversed);
	//	PaintContext paint= control->get_sourcetransform(offs);
	control->push(prod.create_paths());
 control->wake();
	return true;
	
      }



  }



#endif
