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

/**
 * @file blockalloc.h
 * allocate a contiguous block
 * for a bunch of new(allocblok+ sizeof(T)*tcount) T(whatever)
 */ 
#ifndef SNARKY_APPCOMMON_BLOCKALLOC_H 
#define SNARKY_APPCOMMON_BLOCKALLOC_H

#if HAVE_CONFIG_H
#include "appcommonconfig.h"
#endif

#ifndef NAMESPACE_APPCOMMON
#define NAMESPACE_APPCOMMON namespace APPCOMMON
#define NAMESPACE APPCOMMON
#endif

#include "exceptions.h"
//#ifdef DEBUG
#include <iostream>
//#endif
#include <sys/types.h>
#include <bitset>
#include <iterator>
#include <cstring> //memset
#include <cstdlib>
#include <climits>

#define GETPAGESZ sysconf(_SC_PAGESIZE)
// a page?
//#define ALLOKBLOCK_DEFAULT GETPAGESZ
//runtime check won't work for static allocation
#define ALLOKBLOCK_DEFAULT 4096

#ifndef XN_MEMALIGN_DEFINED
#define XN_MEMALIGN_DEFINED

#define BLOK_USE_POSIX_MEMALIGN 0
#define BLOK_USE_MEMALIGN 0
#define BLOK_USE_VALLOC 0
#define BLOK_USE_MALLOC 0

#if HAVE_POSIX_MEMALIGN
#ifdef BLOK_USE_POSIX_MEMALIGN 
#undef BLOK_USE_POSIX_MEMALIGN 
#endif
#define BLOK_USE_POSIX_MEMALIGN 1

#define XN_MEMALIGN(errresult, memptr, alignsize, sizecnt)	\
  do{								\
    errresult= posix_memalign(memptr, alignsize, sizecnt);	\
  }while(0)
#else
#if  HAVE_MEMALIGN
#define BLOK_USE_MEMALIGN 1

#define XN_MEMALIGN(memptr, alignsize, sizecnt) \
  do{						\
    errresult= 0;				\
    *memptr = memalign(alignsize, sizecnt);	\
    if(!(*memptr)){errresult = 1;}		\
  }while(0)

#else
#if  HAVE_VALLOC
#define BLOK_USE_VALLOC 1

#define XN_MEMALIGN(memptr, alignsize, sizecnt) \
  do{						\
    errresult= 0;				\
    *memptr = valloc(sizecnt);			\
    if(!(*memptr)){errresult = 1;}		\
  }while(0)

#else
#if  HAVE_MALLOC
#define BLOK_USE_MALLOC 1

#define XN_MEMALIGN(memptr, alignsize, sizecnt)				\
  do{									\
    errresult= 0;							\
    *memptr = malloc(sizecnt);						\
    if(!(*memptr)){							\
      std::cerr<<"XN_MEMALIGN():malloc failed to allocate memory"	\
	       <<std::endl;						\
      errresult = 1;}							\
  }while(0)

#else
#error "no c allocator visible"
#endif //HAVE_MALLOC
#endif //HAVE_VALLOC
#endif //HAVE_MEMALIGN
#endif //HAVE_POSIX_MEMALIGN
#endif //XN_MEMALIGN_DEFINED



namespace BLOCKALLOC{

  typedef NAMESPACE::NoMemoryError NoMemoryError;
  typedef NAMESPACE::BadAllocError BadAllocError;
  enum{
    BLOCKALLOC_OK = 0,
    BLOCKALLOC_MEMBLOCK_NULL = 1,
    BLOCKALLOC_DELETE_NOTALLOCATED = BLOCKALLOC_MEMBLOCK_NULL << 1,
    BLOCKALLOC_DELETE_OUTOFRANGE = BLOCKALLOC_DELETE_NOTALLOCATED << 1,
    BLOCKALLOC_DELETE_MISALIGNED = BLOCKALLOC_DELETE_OUTOFRANGE << 1,
    BLOCKALLOC_ZERO_ISALLOCATED = BLOCKALLOC_DELETE_MISALIGNED << 1,
    BLOCKALLOC_ZERO_OUTOFRANGE = BLOCKALLOC_ZERO_ISALLOCATED << 1,
    BLOCKALLOC_ZERO_MEMSETERROR = BLOCKALLOC_ZERO_OUTOFRANGE <<  1,
    BLOCKALLOC_BADINITVAL = BLOCKALLOC_ZERO_MEMSETERROR <<  1
  };


  template <typename TP, ptrdiff_t BLOCKSIZE>
  class BlockIterator;


  template <typename T_ALLOCATED, size_t SZ >
  class AllocTable: public std::bitset<sizeof(T_ALLOCATED) / SZ>{
  protected:
    const size_t hashnumber;//to act as key in hash_map

  public:
    AllocTable():hashnumber(generate_hash()){}
    AllocTable(const AllocTable&):hashnumber(generate_hash()){}

    size_t generate_hash()const{
      return (size_t)this;
    }
  };

  template <typename T, size_t ALLOCBYTES = ALLOKBLOCK_DEFAULT>
  class Blockalloc{

  public:
    typedef T value_type;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T* pointer;
    typedef const T* const_pointer;

    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
 
    char*  memblock; // pointer 
    //std::bitset<ALLOCBYTES / sizeof(T)> alloc_table;
    AllocTable<T, ALLOCBYTES > alloc_table;
  public:
    Blockalloc(bool allocateblock = false):
      // alloc_table ctor sets to zeroes
      memblock(NULL){
      if(allocateblock){allocate();}
    }
    Blockalloc(const Blockalloc& src):memblock(NULL){
      if(src.allocated()){
	allocate();
#ifdef DEBUG_META
	std::cerr << "\n**********************\n\nWARNING!: BlockAlloc(const Blockalloc&) DOESN'T COPY CONTENTS\n***********************\n\n"<<std::endl;
#endif
      }
    }

    virtual ~Blockalloc(){
      if(memblock){
	free( memblock);
	memblock = NULL;
      }
    }

    bool allocated()const{return memblock;}
    static size_type get_block_size(){return ALLOCBYTES;}
  private:

    Blockalloc& operator=(const Blockalloc&);
  public:
    static size_type blocksize(){
      return get_block_size();
    }

  protected:

    int zero(size_t index){
      if(!allocated()){allocate();}
      const size_t bytestozero(sizeof(T));
      if(index > (ALLOCBYTES / bytestozero)){
	return BLOCKALLOC_ZERO_OUTOFRANGE;
      }
      if(alloc_table[index]){
	return BLOCKALLOC_ZERO_ISALLOCATED;
      }   
      void* startbyte = memblock +( index *  bytestozero);
      void* res = memset(startbyte,0, bytestozero);
      if(res != startbyte){
	return BLOCKALLOC_ZERO_MEMSETERROR;
      }
      return BLOCKALLOC_OK;
    }

    int do_allocate(char*& outbuf){

      if(outbuf){
	std::stringstream ostmp;
	ostmp <<"ERROR!\nnon NULL pointer passed in as outbuf, perhaps already allocated and anyway, not NULL! Error!";
	std::cerr<< ostmp.str()<<std::endl;
	return BLOCKALLOC_BADINITVAL | BLOCKALLOC_MEMBLOCK_NULL;
      }
      void* res(NULL);
      int err=0;
      XN_MEMALIGN(err, &res, ALLOCBYTES,  ALLOCBYTES);
      std::cerr <<(err ?"failure" : "success")<<std::endl;
      if (err){
	errno= err;//
	return BLOCKALLOC_MEMBLOCK_NULL;
	//throw NoMemoryError("XN_MEMALIGN() failed!");
      }
      else  if(!res){
	return BLOCKALLOC_MEMBLOCK_NULL;
      }
      outbuf = reinterpret_cast<char*>(res);
      return BLOCKALLOC_OK;
    }

    int allocate(char*& outbuf){
    
      return do_allocate(outbuf);
    }
    int allocate(){
#ifdef DEBUG
      std::cerr <<"allocating at ";
#endif
      int rv = do_allocate(memblock);

      //memblock = new char[ALLOCBYTES];
#ifdef DEBUG
      std::cerr << memblock << std::endl;
#endif
      return rv;
    }

  public:

    bool is_full()const throw(){
      size_t Tcapacity = capacity();
      for(size_t i =0; i < Tcapacity; i++){
	if(!alloc_table[i]){
	  return false;
	}
      }
      return true;
    }

    size_t capacity()const throw(){ return  ALLOCBYTES / sizeof(T);}

    bool verify()const{
      std::cerr << "Blockalloc at "<< this<< std::endl;
      return true;
    }

    char* do_new()throw(){
      size_t Tcapacity = capacity();
      if(!memblock){
	allocate();
#ifdef DEBUG_META
	std::cerr <<"allocated memblock at "<<(void*)memblock<<std::endl;
#endif
      }
      for(size_t i =0; i < Tcapacity; i++){
	if(!alloc_table[i]){
	  int zeroresult= zero(i);
	  if(zeroresult){
	    std::cerr << "error zeroing:" << zeroresult<< std::endl;
	    return NULL;
	  }
	  alloc_table[i] = true;
	  char* res = memblock + (i * sizeof(T));
#ifdef DEBUG_MEMORY
	  std::cerr << "returning char* at "<< (void*)res <<" from memblock of "
		    <<ALLOCBYTES<<" AT "<< (void*)memblock<<std::endl; 
#endif 
	  return res;
	}
      }
#ifdef DEBUG_META
      std::cerr << "blockalloc fuulll, returning NULL";
#endif
      return NULL;
    }

    bool valid_T(size_t index)const{return alloc_table[index];}

    int do_delete(char* was_a_T)throw(){
      //should call ~T() somewhere (not here, previously
      //in the container using this as storage
      ptrdiff_t offset = was_a_T - memblock;
      int result(0);
      if (offset % sizeof(T)){
	result |= BLOCKALLOC_DELETE_MISALIGNED;
      }
      if(offset < 0 || offset > ALLOCBYTES){
	result |= BLOCKALLOC_DELETE_OUTOFRANGE;
      }
      if(result == BLOCKALLOC_OK){
	size_t index= offset / sizeof(T);
	if(alloc_table[index]){
	  alloc_table[index]= false;
	}
	else{
	  result = BLOCKALLOC_DELETE_NOTALLOCATED;
	}
      }
      return result;
    }

    value_type& at_implementation(size_t index){
      if(!allocated()){throw BadAllocError("container uninitialised");}
      
      if(index < 0 || index >= capacity()){
	std::stringstream ostmp;
	ostmp << "index '"<< index <<"outside valid range: ( 0 - "<<capacity()-1<<" inclusive)";
	throw BadAllocError(ostmp.str());
      }
      else if(!valid_T(index)){
	std::stringstream ostmp2;
	ostmp2 << "ERROR: index "<<index<< " not constructed";
	throw BadAllocError(ostmp2.str());
      }
      ptrdiff_t T_offset= index * sizeof(value_type);
      char* address = memblock + T_offset;
      return reinterpret_cast<T&>(*address);
    }
    

      const value_type& at_implementation(size_t index)const{
	if(!allocated()){throw BadAllocError("container uninitialised");}
	if(index < 0 || index >= capacity()){
	  std::stringstream ostmp;
	  ostmp << "index '"<< index <<"outside valid range: ( 0 - "<<capacity()-1<<" inclusive)";
	  throw BadAllocError(ostmp.str());
	}
	  else if(!valid_T(index)){
	    std::stringstream ostmp2;
	    ostmp2 << "ERROR: index "<<index<< " not constructed";
	    throw BadAllocError(ostmp2.str());
	  }
	  ptrdiff_t T_offset= index * sizeof(value_type);
	  const char* address = memblock + T_offset;
	  return reinterpret_cast<const T&>(*address);
	}
      };


      template <typename CONTAINER, ptrdiff_t BLOCKSIZE>
	class BlockIterator : 
	public std::iterator<std::random_access_iterator_tag, 
	typename CONTAINER::value_type>{
	/*public std::iterator<std::random_access_iterator_tag, 
	  typename CONTAINER::value_type,ptrdiff_t,typename CONTAINER::value_type*,    typename CONTAINER::value_type&>*/
      public:
	typedef CONTAINER container_type; 
	typedef typename CONTAINER::value_type value_type;
	//typedef Blockalloc<value_type, BLOCKSIZE> container_type;
      protected:
	container_type* p_container;

	value_type* the_block;
	ptrdiff_t current_index;

      public:
	BlockIterator():p_container(NULL), the_block(NULL),current_index(0){}

	BlockIterator(container_type&  alloc,
		      ptrdiff_t off=0):
	  p_container(&alloc),
	  the_block(&(p_container->at_implementation(0))),
	  current_index(off){}

	BlockIterator(const BlockIterator& iter):
	  p_container(iter.p_container),
	  the_block(iter.the_block),current_index(iter.current_index){
	  if(!iter.valid()){
	    throw BadAllocError("copying from invalid iterator");
	  }
	}

	BlockIterator& operator=(const BlockIterator& iter){
	  if(!iter.valid()){
	    throw BadAllocError("copying from invalid iterator");
	  }
	  if(&iter != this){
	    p_container= iter.p_container;
	    the_block = iter.the_block;
	    current_index = iter.current_index;
	  }
	  return *this;
	} 
	template <typename TP0>
	bool eQ(const TP0& other)const{
	  if(!p_container ||!valid() || !p_container  == other.p_container){return false;}
	  return current_index = other.current_index;
	}

	value_type& operator*(){return *(the_block + (sizeof(value_type) * current_index));}
	const value_type& operator*()const{return *(the_block + (sizeof(value_type) * current_index));}

	template <typename TP0>
	bool lessthan(const TP0& other)const{
	  return current_index < other. current_index;
	}

	bool valid()const{
	  if(!p_container){return false;}
	  if (current_index <0|| current_index >= p_container->capacity()){
	    throw BadAllocError("iterator pointing outside possible range");
	  }
	  if (current_index >= p_container->size()){
	    return false;
	  }
	  return true;
	}

      };
      template <typename TP0, typename TP1, ptrdiff_t BLOCKSIZE>
	bool operator<(const BlockIterator<TP0, BLOCKSIZE>& left, 
		       const BlockIterator<TP1, BLOCKSIZE>& right){
	return left.lessthan(right);
      }

      template <typename TP0, typename TP1, ptrdiff_t BLOCKSIZE>
	bool operator>=(const BlockIterator<TP0, BLOCKSIZE>& left, 
			const BlockIterator<TP1, BLOCKSIZE>& right){
      return(!(left < right));
    }
    template <typename TP0, typename TP1, ptrdiff_t BLOCKSIZE>
    bool operator<=(const BlockIterator<TP0, BLOCKSIZE>& left, 
		    const BlockIterator<TP1, BLOCKSIZE>& right){
    return((left < right) || (left  == right));
  }

    template <typename TP0, typename TP1, ptrdiff_t BLOCKSIZE>
    bool operator>(const BlockIterator<TP0, BLOCKSIZE>& left, 
		   const BlockIterator<TP1, BLOCKSIZE>& right){
  return(!(left ==right) &&!(left < right));
}

template <typename TP0, typename TP1, ptrdiff_t BLOCKSIZE>
bool operator==(const BlockIterator<TP0, BLOCKSIZE>& left, 
		const BlockIterator<TP1, BLOCKSIZE>& right){
  return left.eQ(right);
}
template <typename TP0, typename TP1, ptrdiff_t BLOCKSIZE>
bool operator!=(const BlockIterator<TP0, BLOCKSIZE>& left, 
		const BlockIterator<TP1, BLOCKSIZE>& right){
  return !(left == right);
}

template <typename TP0, ptrdiff_t BLOCKSIZE>
BlockIterator<TP0, BLOCKSIZE> operator+(BlockIterator<TP0, BLOCKSIZE>& iter, 
					int inc){
  BlockIterator<TP0, BLOCKSIZE> cpy = iter;
  cpy.current_index += inc;
  if(!cpy.valid()){throw BadAllocError("iter advanced outside range");}

}

template <typename TP, ptrdiff_t BLOCKSIZE>
class BlockContainer{
public:
  typedef  Blockalloc<TP, BLOCKSIZE> container_type;
  typedef typename container_type::size_type size_type;
  typedef typename container_type::value_type value_type;
  typedef typename container_type::reference reference;
  typedef typename container_type::const_reference const_reference;


  typedef BlockIterator<container_type, BLOCKSIZE> iterator;
  typedef BlockIterator<const container_type, BLOCKSIZE> const_iterator;

protected:
  container_type m_container;
public:

  iterator begin(){
    return iterator(m_container,0);
  }

  const_iterator begin()const{
    return iterator(m_container,0);
  }
  iterator end(){
    return iterator(m_container,size());
  }

  const_iterator end()const{
    return iterator(m_container,size());
  }

  static size_type blocksize(){return container_type::blocksize();}

  size_type size()const{
    return m_container.size();
      }
};


template <typename T, size_t ALLOCBYTES = ALLOKBLOCK_DEFAULT>
class MultiBlock: public Blockalloc<T,  ALLOCBYTES>{
public:
  typedef AllocTable<T, ALLOCBYTES> alloc_table_type;
protected:
  SnarkyMap<alloc_table_type,char*> blocks;

};

} //namespace BLOCKALLOC


  /*

    protected:
    #if HAVE_POSIX_MEMALIGN

    class AllocationProxy{
    friend class  Blockalloc;
    char me_bytes[ALLOCBYTES];

    void* operator new(std::size_t bytes)throw(){
    std::cerr << "AllocationProxy::operator new("<<bytes<<")"<<std::endl;
    void* res(NULL);
    int err=0;
    XN_MEMALIGN(err, &res, ALLOCBYTES,  bytes);
    std::cerr <<(err ?"failure" : "success")<<std::endl;
    if (! err){return res;}
    else{
    errno= err;//
    return NULL;
    }
    }

    void* operator new[](size_t bytes)throw(){
    std::cerr << "AllocationProxy::operator new[]("<<bytes<<")"<<std::endl;
    void* res(NULL);
    int err=0;
    XN_MEMALIGN(err,&res, ALLOCBYTES,  bytes);
    std::cerr <<(err ?"failure" : "success")<<std::endl;
    if (! err){return res;}
    else{
    errno= err;//
    return NULL;
    }
    }
    void operator delete(void* address){
    p    free(address);
    }

    };
    public:

    void* operator new(std::size_t bytes)throw(){
    std::cerr<<"Blockalloc::operator new("<<bytes<<")"<<std::endl;
    void* res(NULL);
    int err=0;
    XN_MEMALIGN(err, &res, ALLOCBYTES,  bytes);
    std::cerr <<(err ?"failure" : "success")<<std::endl;
    if (! err){return res;}
    else{
    errno= err;//
    return NULL;
    }
    }

    void* operator new[](size_t bytes)throw(){
    std::cerr<<"Blockalloc::operator new[]("<<bytes<<")"<<std::endl;
    void* res(NULL);
    int err=0;
    XN_MEMALIGN(err, &res, ALLOCBYTES,  bytes);
    std::cerr <<(err ?"failure" : "success")<<std::endl;
    if (! err){return res;}
    else{
    errno= err;//
    return NULL;
    }
    }
    void operator delete(void* address)throw(){
    free(address);
    }



    #endif
  */

#endif
