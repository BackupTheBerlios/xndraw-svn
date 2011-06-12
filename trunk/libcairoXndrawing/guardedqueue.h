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

#ifndef LIBCAIROXNDRAWING_GUARDEDQUEUE_H
#define LIBCAIROXNDRAWING_GUARDEDQUEUE_H

#if HAVE_CONFIG_H
#include "config.h"
#endif

//#if HAVE_PARALLEL_PARALLEL_H
//#include <parallel/queue>
//#else

#include <queue>
//#endif
#include <deque>
#include <glibmm.h>
#include <glibmm/thread.h>

namespace xNDRAW{

  using namespace NCOORDS;


  template < typename T,typename SEQ= std::deque<T> >
    class GuardedQueue: public std::queue<T,SEQ>{
 
  public:
  typedef std::queue<T,SEQ> queue_type;
  typedef typename queue_type::value_type                value_type;
  typedef typename queue_type::reference                 reference;
  typedef typename queue_type::const_reference           const_reference;
  typedef typename queue_type::size_type                 size_type;
  typedef          SEQ                           container_type;
  using queue_type::empty;
  using queue_type::size;
 
  protected:
  unsigned int queue_max;
  Glib::Mutex mutex;
  Glib::Cond cond_not_empty;
  Glib::Cond cond_not_full;

 
  bool throwerr;//instead of block
  public:

#ifndef __GXX_EXPERIMENTAL_CXX0X__
  explicit
  GuardedQueue(const SEQ& __c = SEQ(), 
	       unsigned int qmax=256,
	       bool err_not_block=false):
  queue_type(__c) , 
  queue_max(qmax),
  throwerr( err_not_block){ }

#else

  explicit
  GuardedQueue(const SEQ& __c, 
	       unsigned int qmax=256,
	       bool throw_err_not_block=false):
  queue_type(__c) , 
  queue_max(qmax),
  throwerr(throw_err_not_block){ }

  explicit
  GuardedQueue(SEQ&& __c = SEQ(), 
	       unsigned int qmax=256,
	       bool throw_err_not_block=false) :
  queue_type(std::move(__c))  ,
  queue_max(qmax),
  throwerr(throw_err_not_block{ }

  //what if qmax exceeded here?
  GuardedQueue(queue_type&& __q, 
	       unsigned int qmax=256,
	       bool throw_err_not_block=false)  : 
  queue_type(std::move(__q.c)) , 
  queue_max(qmax),
  throwerr(throw_err_not_block{ }

  GuardedQueue(GuardedQueue&& q):
  queue_type(std::move(q.c)) , 
  queue_max(q.queue_max),
  throwerr(q.throwerr){}

  GuardedQueue&  operator=(GuardedQueue&& __q){
    c = std::move(__q.c);
    queue_max= __q.queue_max;
    throwerr = q.throwerr;
    return *this;
  }

#endif

  void wait_for_output(){
    cond_not_empty.wait(mutex);
  }

  bool full ()const{
    return size() > queue_max;
  }

  void wait_to_input(){
    cond_not_full.wait(mutex);
  }

  reference front(){
    Glib::Mutex::Lock lock(mutex);
    while(empty()){
if(throwerr){
	throw xn_Error("front(): queue set to throw not block when empty");
      }
      cond_not_empty.wait(mutex);
    }
    return queue_type::front();
  }

  value_type pop_front(){//pop and front inside single Lock
    Glib::Mutex::Lock lock(mutex);
    while(empty()){//TODO: version that throws error rather tyan block
      if(throwerr){
	throw xn_Error("pop_front(): queue set to throw not block when empty");
      }
      cond_not_empty.wait(mutex);
    }
    value_type res=queue_type::front();
    queue_type::pop();
    return res;
  }

  const_reference  front() const{
    Glib::Mutex& m = const_cast<Glib::Mutex&>(mutex);
    Glib::Mutex::Lock lock(m);
    while(empty()){
      if(throwerr){
	throw xn_Error("front(): queue set to throw not block when empty");
      }
      cond_not_empty.wait(m);
    }	
    return queue_type::front();
  }

  reference  back()  {
    Glib::Mutex::Lock lock(mutex);
    while(empty()){
if(throwerr){
	throw xn_Error("back(): queue set to throw not block when empty");
      }
      cond_not_empty.wait(mutex);
    }
    return queue_type::back();
  }

void set_throwerr(bool v = true){throwerr = v;}
  const_reference  back() const {
    Glib::Mutex& m = const_cast<Glib::Mutex&>(mutex);
    Glib::Mutex::Lock lock(m);
    while(empty()){
if(throwerr){
	throw xn_Error("pop_front(): queue set to throw not block when empty");
      }
      cond_not_empty.wait(m);
    }
    return queue_type::back();
  }

  void push(const value_type& __x) { 
    Glib::Mutex::Lock lock(mutex);
    while(queue_max < size()){
      cond_not_full.wait(mutex);
    }
    queue_type::push(__x); 
    cond_not_empty.signal();
  }

#ifdef __GXX_EXPERIMENTAL_CXX0X__

  void  push(value_type&& __x) { 
    Glib::Mutex::Lock lock(mutex);
    while(queue_max < size()){
      cond_not_full.wait(mutex);
    }
    queue_type::push(__x); 
    cond_not_empty.signal();
  }

  template<typename... _Args>
  void emplace(_Args&&... __args){ //disregards queue_max...
    Glib::Mutex::Lock lock(mutex);
    while(queue_max < size()){
      cond_not_full.wait(mutex);
    }
    queue_type::emplace(__args);
    cond_not_empty.signal();
  }
#endif

  void pop(){
    Glib::Mutex::Lock lock(mutex);
    while(empty()){
if(throwerr){
	throw xn_Error("pop(): queue set to throw not block when empty");
      }
      cond_not_empty.wait(mutex);
    }
    queue_type::pop();
    if(queue_max >= size()){
      cond_not_full.signal();
    }
  }
#ifdef __GXX_EXPERIMENTAL_CXX0X__
  void swap(queue&& __q) { 
    Glib::Mutex::Lock lock(mutex);
    Glib::Mutex::Lock olock(__q.mutex);
    c.swap(__q.c); 
  }
#endif



  };

}



#endif
