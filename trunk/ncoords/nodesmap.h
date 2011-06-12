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


#ifndef NCOORDS_NODESMAP_H
#define NCOORDS_NODESMAP_H
#include "xndrawexceptions.h"
#if HAVE_CONFIG_H
#include "config.h"
#endif

#include "ncoords.h"
#include <glibmm/thread.h>
#include "threadklass.h"

#define DEBUG_MUTELOCK 1

namespace NCOORDS{
  class Nodedeq;

  std::istream& operator>>(std::istream& ifs, Nodedeq& putinto);
   std::ostream& operator<<(std::ostream& os, const Nodedeq& src);

  class Nodedeq: public deqC{

    typedef std::deque<coords*> container_type;


  public:
    Nodedeq(){}
    Nodedeq(int n):deqC (n){}
    Nodedeq(const std::deque<NCOORDS::coords*>& src){
      typedef std::deque<NCOORDS::coords*> container_type;
      typedef container_type::const_iterator const_iterator;
      for (const_iterator walk = src.begin(); walk != src.end();walk++){
	push_back(*walk);
      }
    }
    virtual ~Nodedeq(){}
    Nodedeq(const Nodedeq& src):deqC(src){}
    Nodedeq& operator=(const Nodedeq& src);
    friend bool operator==(const Nodedeq& left, const Nodedeq& right);
  virtual  void scale(double ratio);
  virtual  void scale(const coords&  scaler);
 virtual   Nodedeq& operator*=(const coords& ratio);
 virtual   Nodedeq& operator*=(double ratio);
    //flip up/down stateflag
virtual    void upendcoords(size_t index =0);

    /*
      void swap(Nodedeq& other){
      this->swap((container_type&) (other));
      }
      void swap(deqC& other){
      this->swap((container_type&) (other));
      }
      void swap(container_type& other){
      container_type::swap(other);
      }
    */

    friend std::istream& operator>>(std::istream& ifs, Nodedeq& putinto);
    virtual void write_data( const Nodedeq& coordlist, const std::string& filename);
    void write_data( const Nodedeq& coordlist, std::ostream& mfile);
    virtual void write_text( const Nodedeq& coordlist, const std::string& filename);
    virtual Nodedeq read_data(const std::string& fname);
    coords* pass_coords(iterator index);
  };

  template <class T>
  class test_deq:public std::deque<T>{
  public:
    test_deq(){}
    virtual ~test_deq(){}

  };

  enum Mutexflags{
    READER_BLOCKING = 1,
    WRITER_BLOCKING = 1 <<1,
    INHIBIT_READERS =1 <<2,
    INHIBIT_WRITERS=1 <<3
  };

 
  struct   LOCKINFO_READ{};
  struct   LOCKINFO_WRITE{};

  

  class MutexNodedeq:public Nodedeq,
public ThreadKlass{
      //makeshift - not really threadsafe
      //also , a RWLock would be more useful in drawingset
    typedef ThreadKlass::Lock thread_lock_type;
  public:
    class Lock;
    friend class Lock;

    class Lock: public ThreadKlass::Lock{
 public:
 typedef ThreadKlass::Lock thread_lock_type;
 
 protected:
Glib::RWLock::ReaderLock readlock;
     Glib::RWLock::WriterLock writelock;
    public:
      Lock(MutexNodedeq& sequence, LOCKINFO_READ);
	//as readlock inited first will this block writelock despite  
	//writelock having Glib::NotLock? -if so, use pointers/new
      //TODO:const mutable ThreadKlass& in ThreadKlass::Lock
      //to allow passing const ThreadKlass& in constructors
      //Lock  only affects lock state and threadsignsals, not data
      Lock(MutexNodedeq& sequence, LOCKINFO_READ, Glib::TryLock );
	//as above but nonblocking
	//test if sequence.readmutex.locked()
       
      Lock(MutexNodedeq& sequence, LOCKINFO_WRITE);
      Lock(MutexNodedeq& sequence, LOCKINFO_WRITE, Glib::TryLock);

      ~Lock();

      bool locked()const;

      bool readlocked()const;

      void acquire();

      void acquire_reader();
      bool try_acquire_reader();

      bool try_acquire();

      void release();

      void release_reader();
	};// class MutexNodedeq::Lock

    protected:
    
      Glib::RWLock readmutex;
   
      Glib::Cond reader_available;
      unsigned char flags;
    public:

    void signal_reader(){
      reader_available.signal();
    }

      bool get_flag( Mutexflags flag)const{
	return flags & (unsigned char)flag;
      }
      void set_flag(Mutexflags flag, bool val=true){
	if(val){flags |= (unsigned char)flag;}
	else if (flags &  (unsigned char)flag){
	  flags ^=   (unsigned char)flag;
	}
      }
      void toggle_flag(Mutexflags flag){
	flags ^=   (unsigned char)flag;
      }
    //these flags an extra complication
    //if required abstract this and the Lock class into 
    //ReadWriteLockableBase or so
      bool reader_blocking()const{
	return get_flag(READER_BLOCKING);
      }
      bool writer_blocking()const{
	return get_flag(WRITER_BLOCKING);
      }
      bool inhibit_readers()const{
	return get_flag(INHIBIT_READERS);
      }
      bool inhibit_writers()const{
	return get_flag(INHIBIT_WRITERS);
      } 

      void set_reader_blocking(bool v =true){
	set_flag(READER_BLOCKING,v);
      }
      void set_writer_blocking(bool v =true){
	set_flag(WRITER_BLOCKING,v);
      }
      void set_inhibit_readers(bool v =true){
	set_flag(INHIBIT_READERS,v);
      }
      void set_inhibit_writers(bool v =true){
	set_flag(INHIBIT_WRITERS,v);
      }

    MutexNodedeq(unsigned char lockflags = READER_BLOCKING);

    MutexNodedeq(int n,unsigned char lockflags = READER_BLOCKING);

      MutexNodedeq(const std::deque<NCOORDS::coords*>& src,
		   unsigned char lockflags = READER_BLOCKING);
   
    virtual ~MutexNodedeq();
     
    //will block till neither writing, and this not reading
    MutexNodedeq(const MutexNodedeq& src);

    MutexNodedeq(const Nodedeq& src,unsigned char lockflags = READER_BLOCKING);

    //better to try to lock directly than call this
    bool locked()const{
 Lock lock(const_cast<MutexNodedeq&>(*this), LOCKINFO_READ(), Glib::TRY_LOCK);
      return !lock.locked();
  }

    bool readerlocked()const{
      Lock lock(const_cast<MutexNodedeq&>(*this), LOCKINFO_READ(), Glib::TRY_LOCK);
      return !lock.readlocked();
}
    void unlock();

    void unlock_reader(){//readlock.release();
	reader_available.signal();
      }
    

      MutexNodedeq& operator=(const  Nodedeq& src);

      MutexNodedeq& operator=(MutexNodedeq& src);
      void wait_for_reader(){
	reader_available.wait(mutex);
      }
      void scale(double ratio);
      void scale(const coords&  scaler);

      MutexNodedeq& operator*=(const coords& ratio);

      MutexNodedeq& operator*=(double ratio);
      //flip up/down stateflag
      void upendcoords(size_t index =0);
    //Glib::Mutex::Lock* get_lock(){return &reclock;}
    };

}

#endif
