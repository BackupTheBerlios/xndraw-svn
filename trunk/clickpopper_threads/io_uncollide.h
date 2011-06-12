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


// Prevents ostream collisions.
#ifndef NCOORDS_IO_UNCOLLIDE_H
#define NCOORDS_IO_UNCOLLIDE_H
#if HAVE_CONFIG_H
#include "config.h"
#endif
#include <iostream>
#include <sstream>
#include <glibmm/object.h>
#include <glibmm/thread.h>


#define THREAD_COUT(EXP)  THREAD_OUTPUT(EXP, &(std::cout)) 
#define THREAD_CERR(EXP)  THREAD_OUTPUT(EXP, &(std::cerr)) 

#define NAMESPACE_THREAD_COUT(EXP)  NAMESPACE_THREAD_OUTPUT(EXP, &(std::cout)) 
#define NAMESPACE_THREAD_CERR(EXP)  NAMESPACE_THREAD_OUTPUT(EXP, &(std::cerr)) 

#define THREAD_OUTPUT(EXP, p_STREAM) do{ \
  No_iostream_collision& inst= No_iostream_collision::get_instance(); \
  inst << EXP;}while(0)

#define NAMESPACE_THREAD_OUTPUT(EXP, p_STREAM) do{ \
    NCOORDS::No_iostream_collision& inst= NCOORDS::No_iostream_collision::get_instance(); \
  inst << EXP;}while(0)

#define EXP_TO_STREAM(exp,p_STREAM) std::ostringstream stream;	\
  stream << #exp << " = " << exp << std::endl;		\
  No_iostream_collision::get_instance().std_out(stream, p_STREAM)

#define OBJ_TO_STREAM_WITH_ADDRESS(exp,p_STREAM) std::ostringstream stream;   \
  stream <<"OBJECT @ "<<&exp<< ": "<< #exp << " = " << exp << std::endl;     \
  No_iostream_collision::get_instance().std_out(stream, p_STREAM)

#define IO_STREAMISE(exp) EXP_TO_STREAM(exp, &(std::cout))
#define COUT(s) THREAD_COUT(s)
#define CERR(s) THREAD_CERR(s)


namespace NCOORDS{

  //base class with singleton instantiation(
  //should be template<typenam CHAR, typename TRAITS>
  class No_iostream_collision :public Glib::Object{ // Share one of these among all threads.Use Guardedstream for multiple instancse
  protected:
    static No_iostream_collision* io_guard;
    No_iostream_collision ():outputting(false), guardedstream(NULL){
      if(!Glib::thread_supported()){
	Glib::thread_init();
      }
    }
    
    No_iostream_collision (const No_iostream_collision& src);//unavailable
    No_iostream_collision& operator=(No_iostream_collision& rhs);
    Glib::Mutex mutex;
    bool outputting;
    std::ostream* guardedstream;
    //Glib::Cond cond_output;
  public:
    Glib::Mutex& get_mutex(){
      No_iostream_collision& n= get_instance();
      return n.mutex;
    }

    void lock(){
      get_mutex().lock();
    }
    void unlock(){get_mutex().unlock();}
    bool trylock(){return get_mutex().trylock();}
    bool locked(){
      //this result just an indication of the moment it was called
      //better to just call trylock and handle result
      if (!trylock()){
	return false;
      }
      unlock();
      return true;
    }

    static No_iostream_collision& get_instance(){
      if(!Glib::thread_supported()){
	Glib::thread_init();
      }
      if (io_guard == NULL){
	static No_iostream_collision the_instance;
	io_guard = &the_instance ;
      }
      return *io_guard;
    }
    void std_out(std::ostringstream& ostmp, std::ostream* os=NULL) {
      using namespace std;
      Glib::Mutex::Lock lock(get_mutex());
      if(!os){ 
	if (guardedstream){
	  os=guardedstream;
	}
	else{os= &cout;}
      }
      (*os) << ostmp.str() << flush;
    }

    void set_stream(std::ostream& os){
      guardedstream =  &os;
      if(!guardedstream){
	std::cerr<<"guardedstream set to NULL"<<std::endl;
      }
    }

    void print_string(std::string& s, std::ostream* os=NULL){
      using namespace std;
      Glib::Mutex::Lock lock(get_mutex());
      if(!os){ 
	if (guardedstream){
	  os=guardedstream;
	}
	else{os= &cout;}
      }
      (*os) << s << flush;   
    }
    std::ostream*get_guardedstream(){return guardedstream;}
  };

  template <typename T>
  No_iostream_collision& operator<<( No_iostream_collision& streamwrapper,
				     const T& printable){
    std::ostream* os=streamwrapper.get_guardedstream();
    if (!os ){os = &std::cout;}
    Glib::Mutex::Lock lock(streamwrapper.get_mutex());
    (*os)<< printable<<std::flush;
    return streamwrapper;
  }
  /**
     @class  Guardedstream
     //threadsafe stream output
     instantiable child class for multiple threads
     //share an instance, or each have separaye instances
     //wrapping the same stream
     */
  class Guardedstream:public No_iostream_collision{
  protected:
    Guardedstream(std::ostream& guardedos){
      guardedstream= &guardedos;
    }
  public:
    static  Guardedstream* create(std::ostream& guardedos){
      return new  Guardedstream(guardedos);
    }
  };

}/*namespace NCOORDS*/



namespace xNDRAW{


  typedef NCOORDS::No_iostream_collision No_iostream_collision;
}

#endif // NCOORDS_IO_UNCOLLIDE_H ///:~
