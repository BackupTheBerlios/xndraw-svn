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

#include "transpredicate.h"
#include "x3draw.h" //prefs - TODO: base app class in appcommon
namespace xNDRAW{

  using namespace NCOORDS;
 PointPredicate::PointPredicate(PointPredicate::predicate_function fnn):
    fn(fnn), method(&PointPredicate::decide),
    use_fn(false){
    if(fn){
      use_fn =true;
    }
  }

 PointPredicate::PointPredicate(PointPredicate::predicate_method m):fn(NULL),
								     method(m),
								     use_fn(false)
  {}
bool PointPredicate::eval_iterator_range(const_iterator rangestart, const_iterator iter, 
					 const_iterator sentinel){ 
  /*std::cerr<< "PointPredicate::eval_iterator_range(start, iter, endd)"
	   <<"\noverload me where predicate needs previous point "
	   <<"for a LineSegment";*/
  return eval_iterator(iter, sentinel);
}

  void PointPredicate::set_method(PointPredicate::predicate_method m){
    method = m;
    use_fn = false;
  }

  bool PointPredicate::alt_decide(const std::pair<coords, int>& p){
    //overload
    return false;
  }
PointPredicate* PointPredicate::create_false_predicate(){
      return new PointPredicate(& PointPredicate::falsehood);//always returns false
    }
 PointPredicate* PointPredicate::create_true_predicate(){
      return new PointPredicate(& PointPredicate::truth);//always returns true
    }

  void PointPredicate::set_function(PointPredicate::predicate_function f){
    fn = f;
    if(fn){ use_fn = true;}
    else{use_fn = false;}
  }

  bool PointPredicate::exec_function(const std::pair<coords, int>& point){
    if(fn){
      if( use_fn){
	return  (*fn)(point);
      }
      else{throw xn_Error("exec_function() called but use_fn false");}
    }
    else {throw xn_Error("fn is NULL and exec_function() called");}
  }

  bool  PointPredicate::exec_method(const std::pair<coords, int>& point){
    return (this->*method)(point);
  }

 bool PointPredicate::operator()( const std::pair<coords, int>& pathpoint){
    //calls virtual methods - overload them
    if(use_fn && fn){
      return exec_function(pathpoint);
    }
    else{
      return exec_method(pathpoint);
    }
  }

  PointPredicate& PointPredicate::click_selected_predicate(PointPredicate& pred){
      pred.set_function(&click_selected);
      return pred;
    }

  bool PointPredicate::decide(const std::pair<coords, int>& pathpoint){
    //overload
    return true;
  }
  cRect ClipPredicate::buffer_area(){
size_t sz(0);
      double xmax =XNDRAW_GET_PREF("IMAGEBUFFER_WIDTH", sz);
double ymax =XNDRAW_GET_PREF("IMAGEBUFFER_HEIGHT", sz);
 coords small(0,0), large(xmax, ymax);
 cRect box(small, large);
 return box;
  }

bool  ClipPredicate::eval_iterator_range(const_iterator rangestart, 
				      const_iterator iter, 
					 const_iterator sentinel){
  //more sane to do whole range in one fn, with maybe a helper, with params for coords/segments already determined
  //so fn opens, and we look for the first Segment(adding the first drawing_click(, and 2nd segment, moving along a segment at a time,
  //since bezier handles will be included depending on where the point they pull from is(if a line is shortened, proportionally scale the handle(s))
  const std::pair<coords, int>* before;
  const std::pair<coords, int>* after;
  const_reference refarg  =*iter;
  const_iterator goback=iter;
  if (iter== rangestart){after= NULL;}
  else{
    while (-- goback != rangestart){
      const coords* pt=&((*goback).first);
    }
  }
  return true;
}


bool PointPredicate::overload(const std::pair<coords, int>& pathpoint){
      throw xn_Error("base claSS version: must overload to use");
    }
  ClipPredicate::ClipPredicate(): 
    PointPredicate(&PointPredicate::overload),
    cliparea(buffer_area()){}
    
  ClipPredicate::ClipPredicate(const cRect& clipbox): 
    PointPredicate(&PointPredicate::overload),cliparea(clipbox){}

bool ClipPredicate::overload(const std::pair<coords, int>& pathpoint){
  return in_bounds(pathpoint);
}

bool ClipPredicate::in_bounds(const std::pair<coords, int>& pathpoint){
      return cliparea.contains(pathpoint.first);
    }

  bool ClipPredicate::in_bounds(const coords& p){
return cliparea.contains(p);
    }


 coords ClipPredicate::fix (const coords& pt, const NCOORDS::Segment<coords*>& segment){
   Segment<coords> proxy(*(segment.first), *(segment.second));
      std::pair<Trinary, Trinary> states= segment_good(proxy);
      if((states.first!=T_GOOD) &&(states.second!=T_GOOD)){
	throw xn_Error("bad coords bad, can't fix");
      }
      coords res=pt;
      coords tryx, tryy;
      if(segment.first == &pt){
	if(states.first==T_GOOD){return pt;}
      }
      else if(segment.second == &pt){
	if(states.second==T_GOOD){return pt;}
      }
      res=pt;
      if(pt.x < cliparea.x()){
	tryy = coords(cliparea.x(), segment.y_for_x(cliparea.x()));
	if (in_bounds( tryy)){return  tryy;}
      }
      if(pt.y <  cliparea.y()){
	tryx= coords(segment.x_for_y(cliparea.y()), cliparea.y());
	if (in_bounds( tryx)){return  tryx;}
      }
      if(pt.x > cliparea.xmax()){
	tryy = coords(cliparea.xmax(), segment.y_for_x(cliparea.xmax()));
	if (in_bounds( tryy)){return  tryy;}
      }
      if(pt.y > cliparea.ymax()){
	tryx= coords(segment.x_for_y(cliparea.ymax()), cliparea.ymax());
	if (in_bounds( tryx)){return  tryx;}
      }
      std::ostringstream ostmp;
      ostmp<<"fix() failed: the bounds: "<<cliparea.x()<<" "<<cliparea.y()
	   <<"---"<<cliparea.xmax()<<" "<<cliparea.ymax()
	   <<"\nthe attempts: (0,0 not an attempt, but uninited-probably)\n"
	   <<tryx.x<<" "<<tryx.y<<"\n"<<tryy.x<<" "<<tryy.y<<std::endl;
      throw xn_Error(ostmp.str());
    }

    std::pair<Trinary, Trinary> ClipPredicate::segment_good(const Segment<coords>& seg){
      bool firstgood=in_bounds(seg.first);
      bool secondgood=in_bounds(seg.second);
      if(firstgood && secondgood){
	return std::pair<Trinary, Trinary>(T_GOOD,T_GOOD);
      }
      else if (!(firstgood || secondgood)){
return std::pair<Trinary, Trinary>(T_BAD,T_BAD);
      }
      else if(firstgood){
return std::pair<Trinary, Trinary>(T_GOOD,T_FIXABLE);
      }
      else{
	return std::pair<Trinary, Trinary>(T_FIXABLE,T_GOOD);
      }
    }



PredicateGroup::iterator PredicateGroup::begin(){
      return predicates.begin();
    }

    PredicateGroup::const_iterator PredicateGroup::begin()const{
      return predicates.begin();
    }


PredicateGroup::iterator PredicateGroup::end(){
      return predicates.end();
    }
    PredicateGroup::const_iterator PredicateGroup::end()const{
      return predicates.end();
    }

    PredicateGroup::size_type PredicateGroup::size() const{
      return predicates.size();
    }

  //fix -> container holds ptrs now
void PredicateGroup::push_back(const PointPredicate& pred){
  PointPredicate* ptr =dynamic_cast<PointPredicate*>( (pred.copy()));
  if(!ptr){throw xn_Error("splurk von dynamic_cast<PointPredicate*>");}
  predicates.push_back( ptr);
    }
void PredicateGroup::push_front(const PointPredicate& pred){
  PointPredicate* ptr =dynamic_cast<PointPredicate*>( (pred.copy()));
  if(!ptr){throw xn_Error("splurk von dynamic_cast<PointPredicate*>");}
  predicates.push_front(ptr);
    }
void PredicateGroup::push_back(PointPredicate* pred){
  predicates.push_back(pred);
    }
void PredicateGroup::push_front(PointPredicate* pred){
  predicates.push_front(pred);
    }
 PredicateGroup::const_iterator PredicateGroup::find_predicate(predicate_method m)const{
      for(const_iterator w = predicates.begin(); w != predicates.end();w++){
	if((*w)->use_function()){continue;}
	else if((*w)->get_method() == m){return w;}
      }
      return end();
    }
  
  
PredicateGroup::iterator PredicateGroup::find_predicate(predicate_method m){
      for(iterator w = predicates.begin(); w != predicates.end();w++){
	if((*w)->use_function()){continue;}
	else if((*w)->get_method() == m){return w;}
      }
      return end();
    }

bool PredicateGroup::operator()(const std::pair<coords, int>& pathpoint){
      bool success = false;
      for (iterator w = begin(); w != end(); w++){
	if(!((*(*w))(pathpoint))){
	  if(!permissive){return false;}
	}
	else{success=true;}
      }
      return success;
    }

    PredicateGroup::const_iterator PredicateGroup::find_predicate(predicate_function fn)const{
      for(const_iterator w = predicates.begin(); w != predicates.end();w++){
	if((*w)->use_method()){continue;}
	else if((*w)->get_function() == fn){return w;}
      }
      return end();
    }

  PredicateGroup::iterator PredicateGroup::find_predicate(predicate_function fn){
      for(iterator w = predicates.begin(); w != predicates.end();w++){
	if((*w)->use_method()){continue;}
	else if((*w)->get_function() == fn){return w;}
      }
      return end();
    }

 void PredicateGroup::set_method(predicate_method m){
      iterator q= find_predicate(m);
      if(q == end()){
	push_back(PointPredicate(m));
      }
      else{
	predicates.erase(q);
      }
    }

    void PredicateGroup::set_function(predicate_function fn){
      iterator q= find_predicate(fn);
      if(q == end()){
	push_back(PointPredicate(fn));
      }
      else{
	predicates.erase(q);
      }
    }

 PredicateGroup::PredicateGroup(PointPredicate pred, bool all_true):
    PointPredicate(pred),permissive(!all_true){
      push_back(pred);
    }

    PredicateGroup::PredicateGroup(const std::deque<PointPredicate>& preds, 
				   bool all_true):
      permissive(!all_true){
      for (std::deque<PointPredicate>::const_iterator w= preds.begin();
	   w != preds.end(); w++){
	push_back(*w);
      }
	  if (!predicates.size()){
	    throw xn_Error("predicates empty:  PredicateGroup should be inited withat lest one predicate");
	  }
      }

 PointTransform::PointTransform(transform_function f ):
    fn(f), 
    method(&PointTransform::transform), 
    use_fn(false){
    if(f){use_fn=true;}
  }

  PointTransform::PointTransform(transform_method m):
    fn(NULL), 
    method(m), 
    use_fn(false)
  {}

 std::pair<coords, int> PointTransform::transform(const std::pair<coords, int>& p){
    //returns a copy of path unaltered
    //overload
    return p;
  }

 std::pair<coords, int> PointTransform::alt_transform(const std::pair<coords, int>& p){
    //returns a copy of path unaltered
    //overload
    return p;
  }

  void PointTransform::set_method(transform_method m){
    method = m;
    use_fn = false;
  }

  void PointTransform::set_function(transform_function f){
    fn = f;
    use_fn = true;
  }

std::pair<coords, int> PointTransform::exec_method(const std::pair<coords, int>& point){
    return (this->*method)(point);
  }
  
  std::pair<coords, int> 
  PointTransform::exec_function(const std::pair<coords, int>& point){
    if(fn){
      if( use_fn){
	return (*fn)(point);
      }
      else{throw xn_Error("exec_function() called but use_fn false");}
    }
    else {throw xn_Error("fn is NULL and exec_function() called");}
  }

  std::pair<coords, int> PointTransform::operator()(const  std::pair<coords, int>& point){
    //calls virtual methods
    if(use_fn && fn){
      return exec_function(point);
    }
    else{
      return exec_method(point);
    }
  }

 std::pair<coords, int>  
   PointTransform::overload(const std::pair<coords, int>& pathpoint){
      throw xn_Error("base claSS version: must overload to use");
    }

  PointTransformInPlace::PointTransformInPlace(transform_function f ):
    fn(f), 
    method(&PointTransformInPlace::transform),
    use_fn(false){
    if(fn){use_fn=true;}
  }

  PointTransformInPlace::PointTransformInPlace(transform_method m):
    fn(NULL), method(m), use_fn(false){}
  void PointTransformInPlace::transform(std::pair<coords, int>& p){
    //does nothing
    //overload
  }
  void PointTransformInPlace::alt_transform(std::pair<coords, int>& p){
    //does nothing
    //overload
  }

  void PointTransformInPlace::set_method(transform_method m){
    method = m;
    use_fn = false;
  }

  void PointTransformInPlace::set_function(transform_function f){
    fn = f;
    if(fn){ use_fn = true;}
    else{use_fn = false;}
  }

  void PointTransformInPlace::exec_method(std::pair<coords, int>& point){
    (this->*method)(point);
  }

  void PointTransformInPlace::exec_function(std::pair<coords, int>& point){
    if(fn){
      if( use_fn){
	(*fn)(point);
      }
      else{throw xn_Error("exec_function() called but use_fn false");}
    }
    else {throw xn_Error("fn is NULL and exec_function() called");}
  }

  void PointTransformInPlace::operator()( std::pair<coords, int>& point){
    //calls virtual methods
    if(use_fn && fn){
      exec_function(point);
    }
    else{
      exec_method(point);
    }
  }
 
  OptPointTransform::OptPointTransform(PointTransform* t_form, 
		  PointPredicate* pred):
    predicate(pred),
      transformer(t_form),
      own_pointers(false){}

 OptPointTransform::OptPointTransform(const OptPointTransform& src):
    predicate(src.predicate),
      transformer(src.transformer),
      own_pointers(src.own_pointers){
      if(own_pointers){
	predicate = new PointPredicate(*(src.predicate));
	transformer = new PointTransform(*(src.transformer));
      }
    }

  OptPointTransform& OptPointTransform::operator=(const OptPointTransform& src){
      if(&src != this){
	own_pointers = src.own_pointers;
	if(own_pointers){
	  predicate = new PointPredicate(*(src.predicate));
	  transformer = new PointTransform(*(src.transformer));
	}
	else{
	  predicate = src.predicate;
	  transformer = src.transformer;
	}
      }
      return *this;
    }

OptPointTransform::OptPointTransform(const PointTransform& t_form, 
		      const PointPredicate& pred):
    PointTransform(& PointTransform::transform),
      predicate(new PointPredicate(pred)),
      transformer(new PointTransform(t_form)),
      own_pointers(true){}
    
 OptPointTransform::~OptPointTransform(){
      if(own_pointers){
	delete predicate;
	delete transformer;
      }
    }

  std::pair<coords, int>
  OptPointTransform::transform(const std::pair<coords, int>& arg){
      if ((*predicate)(arg)){
	return (*transformer)(arg);
      }
      return arg;
    }

  Copyfn_from_Inplacefn::Copyfn_from_Inplacefn(PointTransformInPlace* f):
    PointTransform(&PointTransform::transform),
      functor(f){}
 
  Copyfn_from_Inplacefn::~Copyfn_from_Inplacefn(){
      delete functor;
      functor= NULL;
}

Copyfn_from_Inplacefn::Copyfn_from_Inplacefn(const  Copyfn_from_Inplacefn& src):
    PointTransform(&PointTransform::transform),
    functor(dynamic_cast< PointTransformInPlace*>(src.functor->copy())){
      if(!functor){
	throw xn_Error("dynamic_cast<Copyfn_from_Inplacefn*> failed");
      }
    }


Copyfn_from_Inplacefn& Copyfn_from_Inplacefn::operator=(const Copyfn_from_Inplacefn& src){
     if(&src != this){
       functor=dynamic_cast< PointTransformInPlace*>(src.functor->copy());
       PointTransform::operator=(src);
     }
     return *this;
   }

Copyfn_from_Inplacefn::copyable_functor_type* Copyfn_from_Inplacefn::copy()const{
  return new Copyfn_from_Inplacefn(*this);
    }

Inplacefn_from_Copyfn::Inplacefn_from_Copyfn(const  Inplacefn_from_Copyfn& src):
    PointTransformInPlace(&PointTransformInPlace::transform),
    functor(dynamic_cast< PointTransform*>(src.functor->copy())){
      if(!functor){
	throw xn_Error("dynamic_cast<Inplacefn_from_Copyfn*> failed");
      }
    }
 
  Inplacefn_from_Copyfn::Inplacefn_from_Copyfn(PointTransform* f):
PointTransformInPlace(&PointTransformInPlace::transform),
 functor(f){}
 
  Inplacefn_from_Copyfn::~Inplacefn_from_Copyfn(){
      delete functor;
      functor= NULL;
}

Inplacefn_from_Copyfn& 
Inplacefn_from_Copyfn::operator=(const Inplacefn_from_Copyfn& src){
     if(&src != this){
       functor=dynamic_cast< PointTransform*>(src.functor->copy());
       PointTransformInPlace::operator=(src);
     }
     return *this;
   }


void Inplacefn_from_Copyfn::transform(std::pair<coords, int>& p){
call_fn(p);
}

  void Inplacefn_from_Copyfn::call_fn(std::pair<coords, int>& p){
    if( !functor){throw xn_Error("functor pointer NULL");}
    p = (*functor)(p);
  }

std::pair<coords, int> 
Copyfn_from_Inplacefn::transform(const  std::pair<coords, int>& p){
  return call_fn(p);
}

std::pair<coords, int> 
Copyfn_from_Inplacefn::call_fn(const  std::pair<coords, int>& p){
  std::pair<coords, int> res(p);
  (*functor)(res);
  return res;
}

  PointFlip::PointFlip(const coords& pt1, const coords& pt2):
    PointTransformInPlace(&PointTransformInPlace::overload),
    reflectaxis(const_cast<coords*>(&pt1), const_cast<coords*>(&pt2)){}

void PointFlip::flip(std::pair<coords, int>& arg){
      line_function_type linefunc= reflectaxis.get_function();
      arg.first=linefunc.opposite_pt(arg.first);
    }

TransformGroup::iterator 
TransformGroup::find_transform(inplace_transform_function fn){
      for(iterator w = transforms.begin(); w != transforms.end();w++){
	if((*w)->use_method()){continue;}
	else if((*w)->get_function() == fn){return w;}
      }
      return end();
    }

TransformGroup::const_iterator 
TransformGroup::find_transform(inplace_transform_function fn)const{
      for(const_iterator w = transforms.begin(); w != transforms.end();w++){
	if((*w)->use_method()){continue;}
	else if((*w)->get_function() == fn){return w;}
      }
      return end();
    }

TransformGroup::iterator 
TransformGroup::find_transform(inplace_transform_method m){
      for(iterator w = transforms.begin(); w != transforms.end();w++){
	if((*w)->use_function()){continue;}
	else if((*w)->get_method() == m){return w;}
      }
      return end();
    }

 void TransformGroup::set_functor(PointTransform* f){
      PointTransform* oldfun = functor;
      functor=f;
      if(oldfun){
	delete oldfun;
	oldfun=NULL;
      }
    }

TransformGroup::const_iterator
 TransformGroup::find_transform(TransformGroup::inplace_transform_method m)const{
      for(const_iterator w = transforms.begin(); w != transforms.end();w++){
	if((*w)->use_function()){continue;}
	else if((*w)->get_method() == m){return w;}
      }
      return end();
    }


void TransformGroup::set_method(TransformGroup::inplace_transform_method m){
      iterator q= find_transform(m);
      if(q == end()){
	push_back(new PointTransformInPlace(m));
      }
      else{
		PointTransformInPlace* ptr = *q;
	transforms.erase(q);
	delete ptr;
	ptr = NULL;
      }
    }

    void TransformGroup::set_function(TransformGroup::inplace_transform_function fn){
      iterator q= find_transform(fn);
      if(q == end()){
	push_back(new PointTransformInPlace(fn));
      }
      else{
		PointTransformInPlace* ptr = *q;
	transforms.erase(q);
	delete ptr;
	ptr = NULL;
      }
    }

    TransformGroup::TransformGroup(const TransformGroup& src):
PointTransform(&PointTransform::transform), 
      functor(dynamic_cast<PointTransform*>(src.functor->copy())){
      if(!functor){throw xn_Error("bad dynamic cast?");}
      for(const_iterator w = src.begin(); w!= src.end();w++){
	PointTransformInPlace* fct=dynamic_cast<PointTransformInPlace*>((*w)->copy());
	if(fct){	push_back(fct);   }
	else{throw xn_Error("bad dynamic cast?");}
      }
    }

TransformGroup::result_type TransformGroup::transform(TransformGroup::const_reference arg){
  result_type res = (*functor)(arg);
      for(iterator w = begin(); w != end(); w++){
	(*(*w))(res);
      }
      return res;
    }


 TransformGroup::TransformGroup(PointTransform* src):
   PointTransform(&PointTransform::transform),
    functor(src)
{}

TransformGroup::TransformGroup(PointTransform* src,
		 TransformGroup::container_type& trans):
  PointTransform(&PointTransform::transform),
  functor(src){
transforms.swap(trans); 
}

  TransformGroup::TransformGroup(PointTransform* trans, 
		 PointTransformInPlace* inplacetrans):
    PointTransform(&PointTransform::transform),
functor(trans){
      push_back(inplacetrans);
    }

 void TransformGroup::push_back(PointTransformInPlace* tran){
      transforms.push_back(tran);
    }
    void TransformGroup::push_front(PointTransformInPlace* tran){
      transforms.push_front(tran);
    }


    TransformGroup::iterator TransformGroup::begin(){
      return transforms.begin();
    }
TransformGroup::const_iterator TransformGroup::begin()const{
      return transforms.begin();
    }

    TransformGroup::iterator TransformGroup::end(){
      return transforms.end();
    }
    TransformGroup::const_iterator TransformGroup::end()const{
      return transforms.end();
    }

    TransformGroup::size_type TransformGroup::size() const{
      return transforms.size();
    }

 TransformGroup::~TransformGroup(){
   if(functor){delete functor;}
   for( iterator w = begin(); w!= end();w++){
     if(*w){
       delete *w;
       *w=NULL;
     }
   }
 }

  TransformGroup::copyable_functor_type* TransformGroup::copy()const{
  return new TransformGroup(*this);
    }

 InPlaceTransformGroup::InPlaceTransformGroup(PointTransformInPlace* src):
    PointTransformInPlace(&PointTransformInPlace::transform),
      functor(src){}

 InPlaceTransformGroup::InPlaceTransformGroup(PointTransformInPlace* src,
		 container_type& trans):
    PointTransformInPlace(&PointTransformInPlace::transform),
  functor(src)
      {transforms.swap(trans);}

  InPlaceTransformGroup::InPlaceTransformGroup(std::pair<PointTransformInPlace* ,PointTransformInPlace*> transpair):
    PointTransformInPlace(&PointTransformInPlace::transform), 
      functor(transpair.first){
      push_back(transpair.second);
    }

InPlaceTransformGroup::~InPlaceTransformGroup(){ 
  if(functor){delete functor;}
   for( iterator w = begin(); w!= end();w++){
     if(*w){
       delete *w;
       *w=NULL;
     }
   }
 }

  InPlaceTransformGroup::copyable_functor_type* InPlaceTransformGroup::copy()const{
  return new InPlaceTransformGroup(*this);
    }

 void InPlaceTransformGroup::set_functor(PointTransformInPlace* f){
      PointTransformInPlace* oldfun = functor;
      functor=f;
      if(oldfun){
	delete oldfun;
	oldfun=NULL;
      }
    }

    void InPlaceTransformGroup::transform(InPlaceTransformGroup::reference arg){
      (*functor)(arg);
      for(iterator w = begin(); w != end(); w++){
	(*(*w))(arg);
      }
    }

InPlaceTransformGroup::InPlaceTransformGroup(const InPlaceTransformGroup& src):
  PointTransformInPlace(&PointTransformInPlace::transform){
  functor = dynamic_cast<PointTransformInPlace*>(src.functor->copy());
  if(!functor){
    throw xn_Error("bad cast");
  }
  for(const_iterator w = src.begin(); w!= src.end();w++){
    PointTransformInPlace* tmp = dynamic_cast<PointTransformInPlace*>((*w)->copy());
    if (!tmp){throw xn_Error("bad cast");}
    push_back(tmp);
    }
}

void InPlaceTransformGroup::set_method(InPlaceTransformGroup::transform_method m){
      iterator q= find_transform(m);
      if(q == end()){
	push_back(new PointTransformInPlace(fn));
      }
      else{
	PointTransformInPlace* ptr = *q;
	transforms.erase(q);
	delete ptr;
	ptr = NULL;
      }
    }
  void InPlaceTransformGroup::set_function(TransformGroup::inplace_transform_function fn){
      iterator q= find_transform(fn);
      if(q == end()){
	push_back(new PointTransformInPlace(fn));
      }
      else{
		PointTransformInPlace* ptr = *q;
	transforms.erase(q);
	delete ptr;
	ptr = NULL;
      }
    }


InPlaceTransformGroup::iterator 
InPlaceTransformGroup::find_transform(transform_function fn){
      for(iterator w = transforms.begin(); w != transforms.end();w++){
	if((*w)->use_method()){continue;}
	else if((*w)->get_function() == fn){return w;}
      }
      return end();
    }

InPlaceTransformGroup::const_iterator 
InPlaceTransformGroup::find_transform(transform_function fn)const{
      for(const_iterator w = transforms.begin(); w != transforms.end();w++){
	if((*w)->use_method()){continue;}
	else if((*w)->get_function() == fn){return w;}
      }
      return end();
    }

InPlaceTransformGroup::iterator 
InPlaceTransformGroup::find_transform(transform_method m){
      for(iterator w = transforms.begin(); w != transforms.end();w++){
	if((*w)->use_function()){continue;}
	else if((*w)->get_method() == m){return w;}
      }
      return end();
}

  //template<>
void ObjectPointTransform<NCOORDS::ncoords>::set_reverse_predicate(PointPredicate* revpred){
if (reverse_predicate){delete reverse_predicate;}
 reverse_predicate = revpred;
  }

  //template<>
ObjectPointTransform<NCOORDS::ncoords>& 
ObjectPointTransform<NCOORDS::ncoords>::operator++(){
increment_step();
return *this;
}

  //template<>
ObjectPointTransform<NCOORDS::ncoords>  
ObjectPointTransform<NCOORDS::ncoords>::operator++(int){
  ObjectPointTransform<NCOORDS::ncoords> res(*this);
increment_step();
return res;
}

  //template<>
void ObjectPointTransform<NCOORDS::ncoords>::increment_step(){
      current_step++;
      if(current_step >= object.get_n()){
	current_step =0;
      }
    }

  //template<>
void ObjectPointTransform<NCOORDS::ncoords>::set_reverse_predicate(bool v){
    if (reverse_predicate){delete reverse_predicate;}
    if(v){
      reverse_predicate = PointPredicate::create_true_predicate();
    }
    else{reverse_predicate = PointPredicate::create_false_predicate();}
  }

} //namespace xNDRAW
