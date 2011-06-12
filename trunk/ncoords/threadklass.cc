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

#include "threadklass.h"

namespace NCOORDS{

  ThreadKlass::ThreadKlass(Glib::Thread* t):kls_thread(t){
      Glib::Thread* s_elf=Glib::Thread::self();
#ifdef DEBUG_THREADKLASS
      creation_thread=s_elf;//==Glib::Thread::self()
#endif
      std::cerr<<"ThreadKlass("<<kls_thread
	       <<")\nGlib::Thread::self()(ie creation thread):"
	       <<s_elf <<std::endl;
    }

ThreadKlass::~ThreadKlass(){
      try{
	wake_all();
      }//doesn't catch Glib::Thread::Exit
      catch(const std::logic_error& eL){
	std::cerr <<"~ThreadKlass(): logic error or subclass:\n"<<eL.what()<<std::endl;
      }
      catch(const std::runtime_error& eR){
	std::cerr <<"~ThreadKlass(): runtime error or subclass:\n"<<eR.what()
		  <<std::endl;
      
      }
      catch (const std::exception& ex){
	std::cerr <<"~ThreadKlass():some exception occurred" <<std::endl;
      }
    }

 ThreadKlass::Lock::Lock(const ThreadKlass& k):
   lock_type((const_cast<ThreadKlass&>(k)).get_mutex()),
	ownerref(const_cast<ThreadKlass&>(k)){}

 ThreadKlass::Lock::Lock(const ThreadKlass& k, NotLock):
   lock_type((const_cast<ThreadKlass&>(k)).get_mutex(),Glib::NOT_LOCK),
      ownerref(const_cast<ThreadKlass&>(k)){}

 ThreadKlass::Lock::Lock(const ThreadKlass& k, TryLock):
   lock_type((const_cast<ThreadKlass&>(k)).get_mutex(),Glib::TRY_LOCK),
      ownerref(const_cast<ThreadKlass&>(k)){}
      
bool ThreadKlass::is_self_thread()const throw(xn_Error){
      if(!kls_thread){
	throw xn_Error("kls_thread not set");
      }
      return kls_thread == Glib::Thread::self();
    }

 void ThreadKlass::join(){
      Glib::Thread* tmp= kls_thread;
      if(tmp){
	kls_thread = NULL;
	tmp->join();
      }
    }

}
