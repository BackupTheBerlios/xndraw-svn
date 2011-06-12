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


#ifndef LIBCAIROXNDRAWING_TRANSPREDICATE_H
#define LIBCAIROXNDRAWING_TRANSPREDICATE_H
#include "xndrawexceptions.h"

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include "clickhistory.h"

namespace xNDRAW{
  enum Trinary{
    T_BAD=0,
    T_GOOD =1,
    T_FIXABLE=2
  };
  //PointPredicate::predicate_functions 
  // bool click_selected(const std::pair<coords, int>& pt);
  // bool drawing_click(const std::pair<coords, int>& pt);
  //bool handle_click(const std::pair<coords, int>& pt);
  //bool closepath_click(const std::pair<coords, int>& pt);
  //bool cairo_click(const std::pair<coords, int>& pt);
  //

  template <typename ARG, typename RV>
  struct CopyableFunctor: public std::unary_function<ARG, RV>{
    
    typedef std::unary_function<ARG, RV> functor_type;
    typedef typename functor_type::argument_type argument_type;
    typedef typename functor_type::result_type result_type;
    //typedef typename functor_type::reference reference;
    //typedef typename functor_type::const_reference reference;

    virtual ~CopyableFunctor(){}

    virtual CopyableFunctor* copy()const{
      return new CopyableFunctor(*this);
    }

  };

  class  PointPredicate:
    public CopyableFunctor<const std::pair<NCOORDS::coords, int>&, bool>{
  public:

    typedef  CopyableFunctor<const std::pair<NCOORDS::coords, int>&, bool> \
    copyable_functor_type;

    //using NCOORDS::coords;
  
    typedef NCOORDS::coords coords;
    typedef NCOORDS::simple_click_history::iterator iterator;
    typedef NCOORDS::simple_click_history::const_iterator const_iterator;
    typedef const std::pair<NCOORDS::coords, int>& const_reference;
    typedef bool(*predicate_function)(const std::pair<NCOORDS::coords, int>&) ;
    //    typedef bool(*coords_predicate_function)(const NCOORDS::coords&) ;

    typedef bool (PointPredicate::*predicate_method)
    (const std::pair<coords, int>&) ;
    //  typedef bool (PointPredicate::*coords_predicate_method)
    //(const coords&) ;
  protected:
    predicate_function fn;
    predicate_method method;
    bool use_fn;

  public:
    virtual ~ PointPredicate(){}
    PointPredicate(predicate_function fnn=NULL);
    PointPredicate(predicate_method m);


    virtual copyable_functor_type* copy()const{
      return new PointPredicate(*this);
    }

    virtual bool decide(const std::pair<coords, int>& pathpoint);
    virtual bool alt_decide(const std::pair<coords, int>& p);
    void set_method(predicate_method m);
    void set_function(predicate_function f);

    bool falsehood(const std::pair<coords, int>&){return false;}
    bool truth(const std::pair<coords, int>&){return true;}

    bool use_function()const{return use_fn;}
    bool use_method()const{return !use_fn;}

    virtual bool exec_method(const std::pair<coords, int>& point);
    virtual bool  exec_function(const std::pair<coords, int>& point);

    virtual bool  eval_iterator_range(const_iterator rangestart, 
				      const_iterator iter, 
				      const_iterator sentinel);
    static PointPredicate* create_false_predicate();
    static PointPredicate* create_true_predicate();

    template<typename INPUTITER>
    bool eval_iterator(INPUTITER iter, INPUTITER sentinel){
      if (iter == sentinel){
	throw xn_Error("past end of range");
      }
      return (*this)(*iter);
    }
    //call on a PredicateGroup to make being selected an extra criteria
    //or on a PointPredicate to make it the only one
    static PointPredicate& click_selected_predicate(PointPredicate& pred);
    predicate_function get_function()const{return  fn;}
    predicate_method  get_method()const{return method;}
    virtual bool overload(const std::pair<coords, int>& pathpoint);
    bool operator()( const std::pair<coords, int>& pathpoint);

  };

  class ClipPredicate: public PointPredicate{
  protected:
    NCOORDS::cRect cliparea;
  public:

    ClipPredicate();
    ClipPredicate(const NCOORDS::cRect& clipbox);

    coords fix (const NCOORDS::coords& pt, 
		const NCOORDS::Segment<coords*>& segment);

    virtual ~ClipPredicate(){}

    virtual  copyable_functor_type* copy()const{
      return new ClipPredicate(*this);
    }

    virtual bool overload(const std::pair<coords, int>& pathpoint);
    bool in_bounds(const std::pair<coords, int>& pathpoint);
    bool in_bounds(const coords& p);
    virtual bool  eval_iterator_range(const_iterator rangestart, 
				      const_iterator iter, 
				      const_iterator sentinel);

    double distance_from_boundary(const coords& pt){
      return cliparea.distance_from_boundary(pt);
    }

    void set_cliparea(const NCOORDS::cRect& r){
      cliparea = r;
    }
    
    std::pair<Trinary, Trinary>
    segment_good(const NCOORDS::Segment<coords>& seg);


    static NCOORDS::cRect buffer_area();
 
  };

  class PredicateGroup: public PointPredicate{
  public:
    typedef std::deque<PointPredicate*> container_type;
    typedef container_type::iterator iterator ;
    typedef container_type::const_iterator const_iterator ;
    typedef container_type::size_type size_type;
  protected:
    container_type predicates;
    bool permissive;
  public:
    PredicateGroup(PointPredicate pred, bool all_true=true);
    PredicateGroup(const std::deque<PointPredicate>& preds, bool all_true=true);
    virtual ~PredicateGroup(){}

    bool operator()(const std::pair<coords, int>& pathpoint);

    void set_method(predicate_method m);
    void set_function(predicate_function fn);

    void push_back(const PointPredicate& pred);
    void push_front(const PointPredicate& pred);
    void push_back(PointPredicate* pred);
    void push_front(PointPredicate* pred);
    const_iterator find_predicate(predicate_method m)const;  
    iterator find_predicate(predicate_method m);
    const_iterator find_predicate(predicate_function fn)const;
    iterator find_predicate(predicate_function fn);

    iterator erase(iterator which){
      return predicates.erase(which);
    }

    iterator begin();
    const_iterator begin()const;
    iterator end();
    const_iterator end()const;

    size_type size() const;
    virtual  copyable_functor_type* copy()const{
      return new PredicateGroup(*this);
    }
  };


  class PointTransform:
    public CopyableFunctor< const std::pair<NCOORDS::coords, int>&, std::pair<NCOORDS::coords, int> >{
  public:
    typedef CopyableFunctor< const std::pair<NCOORDS::coords, int>&, std::pair<NCOORDS::coords, int> > copyable_functor_type;
  
    typedef NCOORDS::coords coords;
    //using 
    typedef NCOORDS::Linear_function<double> line_function_type;
    typedef argument_type const_reference;
    typedef size_t size_type;
    typedef std::pair<coords, int>(*transform_function)
    (const std::pair<coords, int>&) ;

    typedef std::pair<coords, int>(PointTransform::*transform_method)
    (const std::pair<coords, int>&) ;

  protected:
    transform_function fn;
    transform_method method;
    bool use_fn;

  public:
    virtual ~PointTransform(){}
    PointTransform(transform_function f = NULL);
    PointTransform(transform_method m);

    inline transform_function get_function()const{return fn;}
    inline transform_method get_method()const{return method ;}
    virtual copyable_functor_type* copy()const{
      return new PointTransform(*this);
    }

    virtual std::pair<coords, int> transform(const std::pair<coords, int>& p);
    virtual std::pair<coords, int> 
    alt_transform(const std::pair<coords, int>& p);

    virtual std::pair<coords, int>  
    overload(const std::pair<coords, int>& pathpoint);

    template<typename INPUTITER, typename OUTPUTITER>
    void do_transform(INPUTITER iter, INPUTITER sentinel,OUTPUTITER outiter){
      if (iter == sentinel){
	throw xn_Error("past end of range");
      }
      //do we need to increment outputiter?
      //(doesn't matter for back/front inserter
      *(outiter++)= (*this)(*iter);
    }

    void set_method(transform_method m);
    void set_function(transform_function f);

    bool use_function()const{return use_fn;}
    bool use_method()const{return !use_fn;}

    virtual std::pair<coords, int> 
    exec_method(const std::pair<coords, int>& point);

    virtual std::pair<coords, int> 
    exec_function(const std::pair<coords, int>& point);

    std::pair<coords, int> operator()(const  std::pair<coords, int>& point);
      
  };

 

  class PointTransformInPlace:
    public CopyableFunctor<std::pair<NCOORDS::coords, int>&,void>{
  public:
    typedef CopyableFunctor<std::pair<NCOORDS::coords, int>&,void>	\
    copyable_functor_type;

    virtual copyable_functor_type* copy()const{
      return new PointTransformInPlace(*this);
    }

    typedef NCOORDS::coords coords;
    typedef NCOORDS::Linear_function<double> line_function_type;
    typedef argument_type reference;

    typedef void (*transform_function)
    (std::pair<coords, int>&) ;

    typedef void (PointTransformInPlace::*transform_method)
    (std::pair<coords, int>&) ;

  protected:
    transform_function fn;
    transform_method method;
    bool use_fn;
  public:
    virtual ~PointTransformInPlace(){}
    PointTransformInPlace(transform_function f = NULL);
    PointTransformInPlace(transform_method m);

    virtual void transform(std::pair<coords, int>& p);
    virtual void alt_transform(std::pair<coords, int>& p);
    virtual void   overload(std::pair<coords, int>& pathpoint){
      throw xn_Error("base claSS version: must overload to use");
    }

    void set_method(transform_method m);
    void set_function(transform_function f);

    inline transform_function get_function()const{return fn;}
    inline transform_method get_method()const{return method ;}

    template<typename ITER>
    void do_transform(ITER iter, ITER sentinel){
      //ITER must deref to a std::pair<coords, int>& 
      if (iter == sentinel){
	throw xn_Error("past end of range");
      }
      //call operator()
      (*this)(*iter);
    }

    bool use_function()const{return use_fn;}
    bool use_method()const{return !use_fn;}

    virtual void exec_method(std::pair<coords, int>& point);
    virtual void exec_function(std::pair<coords, int>& point);

    void operator()( std::pair<coords, int>& point);

  };

  class OptPointTransform: public PointTransform{
  public:

    virtual copyable_functor_type* copy()const{
      return new OptPointTransform(*this);
    }

    typedef PointPredicate::predicate_function predicate_function;
    typedef PointPredicate::predicate_method predicate_method;
  
  protected:
    PointPredicate* predicate;
    PointTransform* transformer;
    bool own_pointers;
  public:

    OptPointTransform(PointTransform* t_form, 
		      PointPredicate* pred);

    OptPointTransform(const OptPointTransform& src);

    OptPointTransform& operator=(const OptPointTransform& src);

    OptPointTransform(const PointTransform& t_form, 
		      const PointPredicate& pred);
    
    virtual ~OptPointTransform();

    result_type transform(argument_type arg);

  };

  class Copyfn_from_Inplacefn: public PointTransform{
    // PointTransform create_PointTransform(  PointTransformInPlace functor);
    //std::pair<coords, int> call_fn(const  std::pair<coords, int>& p){
  protected:
    PointTransformInPlace* functor;

  public:
    Copyfn_from_Inplacefn(PointTransformInPlace* f);
    Copyfn_from_Inplacefn(const  Copyfn_from_Inplacefn& src);
    Copyfn_from_Inplacefn& operator=(const Copyfn_from_Inplacefn& src);
    virtual ~Copyfn_from_Inplacefn();

    virtual copyable_functor_type* copy()const;

    std::pair<coords, int> 
    transform(const  std::pair<coords, int>& p);

    std::pair<coords, int> 
    call_fn(const  std::pair<coords, int>& p);
  };

  class Inplacefn_from_Copyfn: public PointTransformInPlace{
    // PointTransform create_PointTransform(  PointTransformInPlace functor);
    //std::pair<coords, int> call_fn(const  std::pair<coords, int>& p){

    PointTransform* functor;

  public:
    Inplacefn_from_Copyfn(PointTransform* f);
    Inplacefn_from_Copyfn(const  Inplacefn_from_Copyfn& src);

    Inplacefn_from_Copyfn& operator=(const Inplacefn_from_Copyfn& src);

    virtual ~Inplacefn_from_Copyfn();

    virtual copyable_functor_type* copy()const{
      return new Inplacefn_from_Copyfn(*this);
    }

    void    transform(std::pair<coords, int>& p);

    void    call_fn(std::pair<coords, int>& p);
  };

  template<typename OBJECT>
  class ObjectPointTransform:public PointTransform{
    //only gives a common mrthod to set the object
  public:
    typedef OBJECT object_type;
    typedef PointTransform transform_functor_type;
    
  protected:
    OBJECT object;
    PointPredicate* reverse_predicate;

    void set_object(const OBJECT& o){object =o;}
  public:

    virtual copyable_functor_type*  copy()const{
      return new ObjectPointTransform(*this);
    }

    virtual ~ObjectPointTransform(){}

    ObjectPointTransform(const OBJECT& o, transform_function function):
      PointTransform(function), object(o),
      reverse_predicate(PointPredicate::create_false_predicate())
    {}
    //PointTransform::transform_method => 
    //overload a suitable virtual method from PointTransform <=
    ObjectPointTransform(const OBJECT& o, transform_method function):
      PointTransform(function), object(o),
      reverse_predicate(PointPredicate::create_false_predicate()){}

    //misnomer(set
    void set_reverse_predicate(bool v= true){
      if (reverse_predicate){delete reverse_predicate;}
      if(v){
	reverse_predicate = PointPredicate::create_true_predicate();
      }
      else{reverse_predicate = PointPredicate::create_false_predicate();}
    }

    void set_reverse_predicate(PointPredicate* revpred){
      if (reverse_predicate){delete reverse_predicate;}
      reverse_predicate = revpred;
    }
  };

  template<>
  class ObjectPointTransform<NCOORDS::ncoords>: public PointTransform{
    //class ncoordrotator: public ObjectPointTransform<NCOORDS::ncoords>{
    typedef ObjectPointTransform<NCOORDS::ncoords>  ncoordrotator;
    typedef NCOORDS::ncoords object_type;
    typedef PointTransform transform_functor_type;
    // typedef transform_functor_type::

  protected:
    NCOORDS::ncoords object;
    size_type current_step;
    PointPredicate* reverse_predicate;//possibly not here?
  public:

  public:
    virtual copyable_functor_type* copy()const{
      return new ObjectPointTransform(*this);
    }
    void set_object(const NCOORDS::ncoords& o){object =o;}
 
    ncoordrotator& operator++();

    void set_reverse_predicate(bool v= true);
    void set_reverse_predicate(PointPredicate* revpred);

    ncoordrotator operator++(int);

    virtual bool predicate(argument_type)const{return true;}
    void set_step(size_type st){
      current_step = st % object.get_n();
    }

    size_type get_step()const{return current_step;}

    void increment_step();

    virtual ~ ObjectPointTransform(){delete reverse_predicate;}

    ObjectPointTransform(const NCOORDS::ncoords& o,
			 bool reverseall=false):
      transform_functor_type(&PointTransform::transform),
      object(o),
      current_step(0),
      reverse_predicate(NULL){
      if(!reverseall){
	reverse_predicate=PointPredicate::create_false_predicate();
      }
      else{reverse_predicate=PointPredicate::create_true_predicate();}
    }
 
    ObjectPointTransform(const  NCOORDS::ncoords& o, transform_function function,
			 PointPredicate* reversepred):
      transform_functor_type(function),
      object(o),
      current_step(0),
      reverse_predicate(NULL){
      if(reversepred){reverse_predicate = reversepred;}
      else{
	reverse_predicate = PointPredicate::create_false_predicate();
      }
    }

    result_type transform(argument_type argpair){
      return rotate(argpair, current_step, (*reverse_predicate)(argpair));
    }

    result_type rotate(argument_type arg, size_type step, bool reverse, double reverse_angle= M_PI){
      result_type res =arg;
      double a = object.get_step()*step;
      if (reverse){a+= reverse_angle;}
      if(a!=0){
	res.first = object.rotate(&(res.first), a);
      }
      return res;
    }
  };

  //template<typename SEGMENT>
  class PointFlip:public PointTransformInPlace{
    typedef NCOORDS::Segment<NCOORDS::coords> segment_type;
  protected:
    NCOORDS::Segment<coords> reflectaxis;

  public:
    PointFlip(const coords& pt1, const coords& pt2);
    //converts from Segment<coords*>
    virtual ~PointFlip(){}

    template <typename SEGMENT>
    void set_reflectaxis(const SEGMENT& seg){
      reflectaxis = segment_type(seg);
    }
    virtual copyable_functor_type* copy()const{
      return new PointFlip(*this);
    }

    void overload(std::pair<coords, int>& arg){ flip(arg);}
    void flip(std::pair<coords, int>& arg);
  };
  typedef ObjectPointTransform<NCOORDS::ncoords>  ncoordrotator;
    

  class TransformGroup: public PointTransform{
  public:
    typedef std::deque<PointTransformInPlace*> container_type;
    typedef container_type::iterator iterator ;
    typedef container_type::const_iterator const_iterator ;
    typedef container_type::size_type size_type;
    typedef PointTransformInPlace::transform_method inplace_transform_method;
    typedef PointTransformInPlace::transform_function inplace_transform_function;
  protected:
    PointTransform* functor;
    container_type transforms;

  public:
    TransformGroup(PointTransform* src);
    TransformGroup(PointTransform* src,
		   container_type& trans);

    TransformGroup(PointTransform* trans, 
		   PointTransformInPlace* inplacetrans);

    virtual copyable_functor_type* copy()const;

    virtual ~TransformGroup();

    TransformGroup(const TransformGroup& src);
    result_type transform(const_reference arg);
    void set_functor(PointTransform* f);

    void set_method(inplace_transform_method m);
    void set_function(inplace_transform_function f);
    void set_method(transform_method m){PointTransform::set_method(m);}
    void set_function(transform_function f){PointTransform::set_function(f);}

    iterator find_transform(inplace_transform_method m);
    const_iterator find_transform(inplace_transform_method m)const;
    iterator find_transform(inplace_transform_function fn);
    const_iterator find_transform(inplace_transform_function fn)const;

    void push_back(PointTransformInPlace* tran);
    void push_front(PointTransformInPlace* tran);
    iterator erase(iterator which){return transforms.erase(which);}

    iterator begin();
    const_iterator begin()const;
    iterator end();
    const_iterator end()const;

    size_type size() const;

  };

  class InPlaceTransformGroup: public PointTransformInPlace{
  public:
    typedef std::deque<PointTransformInPlace*> container_type;
    typedef container_type::iterator iterator ;
    typedef container_type::const_iterator const_iterator ;
    typedef container_type::size_type size_type;
    typedef PointTransformInPlace::transform_method inplace_transform_method;
    typedef PointTransformInPlace::transform_function inplace_transform_function;
  protected:
    PointTransformInPlace* functor;
    container_type transforms;

  public:
    InPlaceTransformGroup(PointTransformInPlace* src);
    InPlaceTransformGroup(PointTransformInPlace* src,
			  container_type& trans);
    InPlaceTransformGroup(std::pair<PointTransformInPlace* ,PointTransformInPlace*> transpair);

    virtual ~InPlaceTransformGroup();
    InPlaceTransformGroup(const InPlaceTransformGroup& src);

    virtual copyable_functor_type* copy()const;
    void transform(reference arg);

    void set_method(inplace_transform_method m);
    void set_function(inplace_transform_function f);

    void set_functor(PointTransformInPlace* f);
  
    iterator find_transform(transform_method m);
    const_iterator find_transform(transform_method m)const;
    iterator find_transform(transform_function fn);
    const_iterator find_transform(transform_function fn)const;

    void push_back(PointTransformInPlace* tran){
      transforms.push_back(tran);
    }
    void push_front(PointTransformInPlace* tran){
      transforms.push_back(tran);
    }
    iterator begin(){
      return transforms.begin();
    }
    const_iterator begin()const{
      return transforms.begin();
    }

    iterator end(){
      return transforms.end();
    }
    const_iterator end()const{
      return transforms.end();
    }

    size_type size() const{
      return transforms.size();
    }




  };

  //initial transform - ie rotate and offset
  //posts transforms - dependiing on a predicate



} //namespace xNDRAW{




#endif //LIBCAIROXNDRAWING_TRANSPREDICATE_H

