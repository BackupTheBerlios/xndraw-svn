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


#ifndef NCOORDS_THREADKLASS_H
#define NCOORDS_THREADKLASS_H
#include "exceptions.h"
#if HAVE_CONFIG_H
#include "config.h"

#endif
#include "xndrawexceptions.h"
#include <glibmm/thread.h>
#include <stack>

namespace NCOORDS{

  class ThreadKlass{


  public:
    class Lock;
    friend class Lock;

    class Lock : public Glib::Mutex::Lock{
    public:

      typedef Glib::Mutex::Lock lock_type;
      typedef Glib::NotLock NotLock;
      typedef Glib::TryLock TryLock;
     
      explicit Lock(const ThreadKlass& k);
      Lock(const ThreadKlass& k, NotLock);
      Lock(const ThreadKlass& k, TryLock);
      virtual ~Lock(){}

    protected:
      Lock(const Lock&);
      Lock& operator=(const Lock&);

      //const mutable 
      ThreadKlass& ownerref;

    };
 
  protected:
    Glib::Thread* kls_thread;
    Glib::Mutex mutex;
    Glib::Cond waiting;

#ifdef DEBUG_THREADKLASS
    Glib::Thread* creation_thread;//==Glib::Thread::self()
#endif
  protected:
    ThreadKlass(const ThreadKlass&);
    ThreadKlass& operator=(const ThreadKlass&);

  public:
    virtual ~ThreadKlass();
    ThreadKlass(Glib::Thread* t=NULL);

    virtual  void join();
    virtual void loop(){throw xn_Error("base class");}

    virtual  void wait(){waiting.wait(mutex);}
    virtual  void wake(){waiting.signal();}
    virtual  void wake_all(){waiting.broadcast();}
  
    bool is_self_thread()const throw(xn_Error);

    virtual Glib::Mutex& get_mutex(){return mutex;}

    Glib::Thread* get_thread(){return kls_thread;}
    const Glib::Thread* get_thread()const{return kls_thread;}

    void set_thread( Glib::Thread* t){kls_thread=t;}
  };

  template <typename TP, typename CONTAINER= std::deque<TP> >
  class ThreadStack: public ThreadKlass{
  public:
    
    typedef std::stack<TP, CONTAINER> stack_type;
    typedef CONTAINER container_type;
    typedef  typename stack_type::value_type value_type;
    typedef typename stack_type::reference                 reference;
    typedef typename stack_type::const_reference           const_reference;
    typedef typename stack_type::size_type                 size_type;
    typedef ThreadKlass::Lock Lock;

  protected:
    stack_type the_stack;

  public:
#ifndef __GXX_EXPERIMENTAL_CXX0X__
    explicit
    ThreadStack(const container_type& c = container_type()):
      the_stack(c){}
#else
    explicit
    ThreadStack(const container_type& c):
      the_stack(c){}

    explicit
    ThreadStack(container_type&& c = container_type()):
      the_stack(c){}
#endif

    virtual ~ThreadStack(){}

    bool empty() const { return the_stack.empty(); } 
    size_type size() const{ return the_stack.size(); } 

    reference top(){
      Lock lock(*this);
      return the_stack.top();
    }

    const_reference top() const   {
      Lock lock(*this);
      return the_stack.top();
    }

    void push(const value_type& x){
      Lock lock(*this);
      the_stack.push(x);
    }
#ifdef __GXX_EXPERIMENTAL_CXX0X__
     
    void push(value_type&& x){ 
      Lock lock(*this);
      the_stack.push(x); 
    }

    template<typename... _Args>
    void   emplace(_Args&&... args){
      Lock lock(*this);
      the_stack.emplace(args...); }
#endif
    void    pop()  {
      Lock lock(*this);
      the_stack.pop();
    }

#ifdef __GXX_EXPERIMENTAL_CXX0X__
    void    swap(stack&& s) { 
      Lock lock(*this);
      the_stack.swap(s.the_stack); 
    }
#endif
  };

}


#endif
