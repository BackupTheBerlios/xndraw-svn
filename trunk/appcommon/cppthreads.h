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



#ifndef APPCOMMON_CPPTHREADS_H
#define APPCOMMON_CPPTHREADS_H

#if HAVE_CONFIG_H
#include "appcommonconfig.h"
#endif

#include "fileexists.h"
#include <pthread.h>
#include <ctime>
#include <cmath>
#include "c_initialiser.h"
#include <errno.h>

NAMESPACE_APPCOMMON{

  std::ostream& operator<<(std::ostream& os, const timespec& spec);

  //no quick fix here: need threadpool
  class Mutex;

  class Condition{
  protected:
    pthread_cond_t c_cond;

  public:
    Condition();
    Condition( pthread_condattr_t* condattr= NULL);
    ~Condition();

  protected:
    Condition(const Condition&);
    Condition& operator=(const Condition&);

  public:
    int signal();
    int broadcast();
    int wait(Mutex& m);
    int wait(pthread_mutex_t* m);
    int timedwait(pthread_mutex_t* m, const timespec* tm);
    int timedwait(Mutex& m, const timespec* tm);

  };

  class Mutex{
  protected:
    pthread_mutex_t c_mutex;
  public:
    //'fastmutex'

    Mutex(bool recursive=false);
    Mutex(const pthread_mutexattr_t* attrs);
    ~Mutex();

  protected:
    Mutex& operator=(const Mutex&);
    Mutex(const Mutex&);
  public:
    const pthread_mutex_t& get_c_mutex()const{
      return c_mutex;
    }
pthread_mutex_t& get_c_mutex(){
      return c_mutex;
    }
    int kind()const;

    int lock();
    int trylock();
    int unlock();

  };

  class PThread;

  bool operator==(const PThread& left, const PThread& right);

  class PThread{

  public:
    typedef void*(*procedure_type)(void*);
typedef void(*cleanup_procedure_type)(void*);

    static void default_cleanup(void* a){}

  protected:
    pthread_t c_thread;
    pthread_attr_t* creation_attributes;
    procedure_type run_function;
    cleanup_procedure_type cleanup_function;
    void* argument;
    void* cleanarg;
  public:
  PThread(pthread_attr_t* attrs, 
	  procedure_type runfunc,
	  void* runarg=NULL,
	  cleanup_procedure_type cleanfunc= default_cleanup,
	  void* clean_arg=NULL):
    creation_attributes(attrs), 
      run_function(runfunc),
      cleanup_function(cleanfunc),
      argument(runarg),
      cleanarg(NULL){
      cleanarg = clean_arg;
      /*if (!cleanarg){cleanarg = argument;}*/
    }
    int start(){
      int res= pthread_create(&c_thread, creation_attributes, 
			      &PThread::run,reinterpret_cast<void*>(this));
      return res;
    }


    static void* run(void* self_arg);

    friend bool
      operator==(const PThread& left, const PThread& right);

    bool is_calling_thread()const{
      pthread_t self= pthread_self();
      return pthread_equal(self , c_thread);
    }

    static pthread_t calling_thread(){return pthread_self();}
    int detach(){
      return pthread_detach(c_thread);
    }

    //exitarg gets value passed to pthread_exit
    int join(void ** p_exitarg=NULL){
      return pthread_join(c_thread, p_exitarg);
    }
  };

}
#endif

