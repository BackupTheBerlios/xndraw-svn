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



#include "nodesmap.h"

using namespace NCOORDS;
Nodedeq& Nodedeq::operator=(const Nodedeq& src){
  if(this!= &src){
    clear();
for(Nodedeq::const_iterator walk = src.begin(); walk != src.end();walk++){
  push_back(new coords(*(*(walk))));
  }
  }
 return *this;
}

  void Nodedeq::upendcoords(size_t index ){
#ifdef DEBUG
    if(!size() || index >= size()){
      throw std::runtime_error("bad index");
    }
#endif
    if(!size()){return;}
    index = index % size();
    coords& pt = *(this->at(index));
    int state= pt.get_state();
      pt.set_stateflag(COORDS_UP, !(state & COORDS_UP));
  }

void Nodedeq::scale(double ratio){
  coords scaler(ratio,ratio);
  scale(scaler);
}
void Nodedeq::scale(const coords& ratio){
  coords offset(*(*begin()));

for(Nodedeq::iterator walk = begin();walk != end();walk++){
  coords& multiplied =(*(*walk)) ;
  multiplied -= offset;
  multiplied *= ratio;
  multiplied += offset;
 }
}
Nodedeq& Nodedeq::operator*=(const coords& ratio){
#ifdef DEBUG_NODES
  std::cerr<< " Nodedeq::operator*= : scaler coords: "<< ratio<<std::endl;
  coords last, lastafter;
  coords second, secondafter;
  if(size()>=2){second = *(*(begin()+1));}
  if(size()>=3){last = *(*(end()-1));}
#endif
  scale(ratio);
#ifdef DEBUG_NODES
if(size()>=2){secondafter = *(*(begin()+1));}
 if(size()>=3){lastafter = *(*(end()-1));}
 std::cout << "size of coordmap: "<< size()<<"\nbefore and after:\nbefore: (2ndcoords)=> "
	   <<second<<"\n(last coords) => "<< last<< "\nafter: (2ndcoords)=> "
	   <<secondafter <<  "\n(last coords) => "<< lastafter << std::endl;
#endif
  return *this;
}
Nodedeq& Nodedeq::operator*=(double ratio){
  scale(ratio);
  return *this;
}
MutexNodedeq& MutexNodedeq::operator*=(double ratio){
  coords scaler(ratio, ratio);
  *this *= scaler;
  return *this;
}

void  MutexNodedeq::upendcoords(size_t index){
  Lock lock(*this,LOCKINFO_WRITE());
  
  Nodedeq::upendcoords(index);
 
}

void MutexNodedeq::unlock(){

	  reader_available.signal();
	  //writer_available.signal();
      }

MutexNodedeq& MutexNodedeq::operator=(MutexNodedeq& src){
  //bool copy_success(false);
      if(&src != this){
	Lock srcreadlock(src, LOCKINFO_READ());
	Lock writelock(*this,LOCKINFO_WRITE());
	Nodedeq::operator=(src);
      }      
      return *this;
    }

MutexNodedeq& MutexNodedeq::operator=(const  Nodedeq& src){   
      if(&src != this){
	Lock writelock(*this,LOCKINFO_WRITE());
	Nodedeq::operator=(src);
      }      
      return *this;
    }

MutexNodedeq& MutexNodedeq::operator*=(const coords& ratio){
  Lock lock(*this, LOCKINFO_WRITE(), Glib::TRY_LOCK);
	if (lock.locked()){
	  Nodedeq::operator*=(ratio);
	}
	else{std::cerr<<"scale failed: locked"<<std::endl;}
	  return *this;
      }
      void MutexNodedeq::scale(double ratio){
	Lock lock(*this, LOCKINFO_WRITE(), Glib::TRY_LOCK);
	if (lock.locked()){
	  Nodedeq::scale(ratio);
       
	}
	else{std::cerr<<"scale failed: locked"<<std::endl;}
      }

void MutexNodedeq::scale(const coords&  scaler){
  Lock lock(*this, LOCKINFO_WRITE(), Glib::TRY_LOCK);
  if (lock.locked()){
    Nodedeq::scale(scaler);
  }
  else{std::cerr<<"scale failed: locked"<<std::endl;}
}
bool operator==(const Nodedeq& left, const Nodedeq& right){
  //order as well as values _equal_
  
  Nodedeq::const_iterator walk = left.begin();
  Nodedeq::const_iterator walk2 = right.begin();
  if ((left.end()-walk) != (right.end() -walk2)){return false;}
  for (;walk != left.end();walk++, walk2++){
    if(*(*walk) != *(*walk2)){return false;}
  }
  return true;
}

coords* Nodedeq::pass_coords(iterator index){
  coords* result = *index;
  *index = NULL;
  return result;
}


void Nodedeq::write_text(const Nodedeq& coordlist, const std::string& filename){
  using namespace std;
  ofstream textfile((filename + ".Nodedeq.txt").c_str());
  textfile << filename << ".Nodedeq.txt: verbose list of nodes in coordmap" << endl;
  Nodedeq::const_iterator in = coordlist.begin();
  Nodedeq::const_iterator out = coordlist.end();
  for (;in !=out;in++){
    textfile << *(*in)<< endl;
  }
}
void Nodedeq::write_data( const Nodedeq& coordlist, std::ostream& mfile){
  using namespace std;
  const_iterator in = coordlist.begin();
  const_iterator out = coordlist.end();
  for (;in !=out;in++){
    coords* point = *in;
    mfile << point->x << " " << point->y << "\n";
  }
}
std::ostream& NCOORDS::operator<<(std::ostream& os, const Nodedeq& src){
  typedef Nodedeq::const_iterator const_iterator;
  for( const_iterator w = src.begin();w!=src.end();w++){
    coords* point = *w;
    os<<point->x<<" "<<point->y<<"\n";
  }
  return os;
}	 
void Nodedeq::write_data( const Nodedeq& coordlist, const std::string& filename){
  using namespace std;
  std::string filename_ = filename + ".Nodedeq.bin";
  ofstream bindata(filename_.c_str(), ios::binary);
  //bindata.setf(ios::unitbuf);
  bindata << coordlist;
      
}

std::istream& NCOORDS::operator>>(std::istream& ifs, Nodedeq& putinto){
  coords point;
  
  std::string temp;
  while(std::getline(ifs, temp)){
    //i_str << temp;
    std::stringstream i_str(temp,std::ios::out | std::ios::in);
    i_str.setf(std::ios::skipws);
    //do I need to wind back its stream pointer?
    i_str >> point.x;
    i_str >> point.y;
    //i_str.clear();
    putinto.load(point);//loads a new coords
  }
  return ifs;
}

Nodedeq Nodedeq::read_data(const std::string& fname){
  using namespace std;
  // TODO: check for correct filename suffix(".Nodedeq.bin")-create a mimetype and Gtk::FileFilter
  ifstream bindata(fname.c_str(), ios::binary);
  N_COUT( "reading nodes from file: " << fname);
  Nodedeq result;
  coords point;
  while(bindata.read(reinterpret_cast<char*>(&point),sizeof (point))){
    N_COUT(point);
    result.push_back(new coords(point));
  }
  N_COUT("end of coordset");
  return result;
}

 MutexNodedeq::MutexNodedeq(unsigned char lockflags ):
	flags(lockflags)
      {}

      MutexNodedeq::MutexNodedeq(int n,unsigned char lockflags ):
	Nodedeq (n),
	flags(lockflags)
      {}

      MutexNodedeq::MutexNodedeq(const std::deque<NCOORDS::coords*>& src,
		   unsigned char lockflags):
	Nodedeq(src),
	flags(lockflags)
      {}

MutexNodedeq::~MutexNodedeq(){}//does ~deqC() delete coords*s?
     
    //will block till neither writing, and this not reading
      MutexNodedeq::MutexNodedeq(const MutexNodedeq& src):
	flags(src.flags)
      {*this = src;}

 MutexNodedeq::MutexNodedeq(const Nodedeq& src,unsigned char lockflags):
	flags(lockflags)
 {*this = src;}

void MutexNodedeq::Lock::acquire(){
	writelock.acquire();
	thread_lock_type::acquire();
      }

bool MutexNodedeq::Lock::try_acquire(){
  if(writelock.try_acquire()){
    if(thread_lock_type::try_acquire()){
      return true;
    }
    writelock.release();
  }
  return false;
}

 void MutexNodedeq::Lock::release_reader(){
	readlock.release();
      }

void MutexNodedeq::Lock::release(){
	writelock.release();
	thread_lock_type::release();
      }

void MutexNodedeq::Lock::acquire_reader(){
  readlock.acquire();
}

bool MutexNodedeq::Lock::try_acquire_reader(){
  return readlock.try_acquire();
}

bool MutexNodedeq::Lock::readlocked()const{
	bool res =readlock.locked();
#ifdef DEBUG_MUTELOCK
	if(res && thread_lock_type::locked()){
	  throw xn_Error("both locks locked!");
	}
#endif
	return  res;
      }

bool MutexNodedeq::Lock::locked()const{
	bool res = thread_lock_type::locked();
#ifdef DEBUG_MUTELOCK
	if(res && readlock.locked()){
	  throw xn_Error("both locks locked!");
	}
#endif
	return res;
      }

 MutexNodedeq::Lock::~Lock(){
	MutexNodedeq* owner = dynamic_cast<MutexNodedeq*>(&ownerref);
	if(!owner){
	  std::cerr<<"Error  getting owner in destructor: type mismatch, but can't throw exception: might be program termination"<<std::endl;
	}
	else{
	  owner->signal_reader();
	}
      }

 MutexNodedeq::Lock::Lock(MutexNodedeq& sequence, LOCKINFO_WRITE, Glib::TryLock):
	thread_lock_type(sequence, Glib::TRY_LOCK),
	readlock(sequence.readmutex, Glib::NOT_LOCK),
	writelock(sequence.readmutex)
	//if writelock bliocks here, something broken
	{     }

    MutexNodedeq::Lock::Lock(MutexNodedeq& sequence, LOCKINFO_WRITE):
	thread_lock_type(sequence),
	readlock(sequence.readmutex, Glib::NOT_LOCK),
	writelock(sequence.readmutex)

	{     }

MutexNodedeq::Lock::Lock(MutexNodedeq& sequence, LOCKINFO_READ, Glib::TryLock ):
	thread_lock_type(sequence, Glib::NOT_LOCK),
	readlock(sequence.readmutex, Glib::TRY_LOCK),
	writelock(sequence.readmutex, Glib::NOT_LOCK)
	//as above but nonblocking
	//test if sequence.readmutex.locked()
	{     }

MutexNodedeq::Lock::Lock(MutexNodedeq& sequence, LOCKINFO_READ):
	thread_lock_type(sequence, Glib::NOT_LOCK),
	readlock(sequence.readmutex),
	writelock(sequence.readmutex, Glib::NOT_LOCK)
	//as readlock inited first will this block writelock despite  
	//writelock having Glib::NotLock? -if so, use pointers/new
	{     }
