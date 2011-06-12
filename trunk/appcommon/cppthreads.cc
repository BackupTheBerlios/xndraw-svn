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



#define COMPILING_STUPIDAPP 1

   #include "cppthreads.h"

NAMESPACE_APPCOMMON{
  Condition::Condition(){c_cond=init_pthreadcond();}

  Condition::Condition( pthread_condattr_t* condattr){
    pthread_cond_init(&c_cond, condattr);
  }

  Condition::~Condition(){pthread_cond_destroy(&c_cond);}

  int Condition::signal(){
    int res = pthread_cond_signal(&c_cond);
    if(res){throw Error("signal failed");}
    return res;
  }

  int Condition::broadcast(){
    int res = pthread_cond_broadcast(&c_cond);
    if(res){throw Error("broadcast failed");}
    return res;
  }

  int  Condition::wait(Mutex& m){
    int res = pthread_cond_wait(&c_cond, &(m.get_c_mutex()));
    if(res){throw Error("wait failed");}
    return res;
  }

  int Condition::wait(pthread_mutex_t* m){
    int res = pthread_cond_wait(&c_cond, m);
    if(res){throw Error("wait failed");}
    return res;
  }

  int Condition::timedwait(Mutex& m, const timespec* tm){
    int res = pthread_cond_timedwait(&c_cond, &(m.get_c_mutex()), tm);
    if(res){throw Error("timedwait failed");}
    return res;
  }

  int Condition::timedwait(pthread_mutex_t* m, const timespec* tm){
    int res = pthread_cond_timedwait(&c_cond, m, tm);
    if(res){throw Error("timedwait failed");}
    return res;
  }

  Mutex::Mutex(const pthread_mutexattr_t* attrs){
    int res=pthread_mutex_init(&c_mutex, attrs);
    if(res <0){throw Error("MUtex: bad initialiser");}
  }



  Mutex::Mutex(bool recursive){
    if(recursive){
      c_mutex = init_recmutex();
    }
    else{
#ifndef DEBUG_THREADS
      c_mutex =init_fastmutex();
#else
      c_mutex =init_checkmutex();
#endif
    }
  }

  std::ostream& operator<<(std::ostream& os, const timespec& spec){
    int64_t us_billion = std::pow(10,9);
    int64_t nanos = spec.tv_nsec + (std::pow(10,9) * us_billion);
    if(nanos < 1000){
      os << nanos <<" nanosec";
    }
    else if(nanos < 1000000){
      os << nanos/1000.0<<" microsec";
    }
    else if (nanos < us_billion){
      os << nanos/1000000.0  <<" millisec";
    }
    else{
      os << nanos/(double)us_billion<< " seconds";
    }
    return os;
  }

  Mutex::~Mutex(){
    int res = pthread_mutex_destroy(&c_mutex);
    while(res < 0){
      std::cerr<< "pthread_mutex_destroy failed"<<std::endl;
      std::cerr<<strerror(errno);
      if (res==EBUSY){
	//errno=0;
	for(int i =0; i < 9; i++){
	  timespec tspec;
	  timespec remainder;
	  remainder.tv_sec=0;
	  remainder.tv_nsec=0;
	  tspec.tv_sec=0;
	  //first wait about 1 microsec, last is about 0.05 sec
	  //increase by factor of 4
	  tspec.tv_nsec= 1 <<((2*i)+10);
	  nanosleep(&tspec, &remainder);
	}
	int unlockres=pthread_mutex_unlock(&c_mutex);
	if (unlockres){
	  std::cerr<< "pthread_mutex_destroy: failed to unlock thread:must belong to another thread\n"
		   <<strerror(errno);
	  break;
	}
      }
      else{break;}
    }
  }
  int  Mutex::kind()const{
    return c_mutex.__data.__kind;
  }
  int Mutex::lock(){
    int res = pthread_mutex_lock(&c_mutex);
    if  (res) {throw Error("lock failed");}
    return res;
  }

  int Mutex::trylock(){
    int res = pthread_mutex_trylock(&c_mutex);
    if  (res) {throw Error("try lock failed");}
    return res;
  }
  //all these fns should return void,since they don't need to return error value
  int Mutex::unlock(){
    int res = pthread_mutex_unlock(&c_mutex);
    if  (res) {throw Error("unlock failed");}
    return res;
  }

  bool operator==(const PThread& left, const PThread& right){
    return pthread_equal(left.c_thread, right.c_thread);
  }

  void* PThread::run(void* self_arg){
      PThread* self =reinterpret_cast<PThread*>(self_arg);
      procedure_type runfunc= self->run_function;
      pthread_cleanup_push(self->cleanup_function,self->cleanarg);
      (*runfunc)(self->argument);
      pthread_cleanup_pop(1);
      return  self_arg;
  }


}

