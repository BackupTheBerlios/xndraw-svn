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


#ifndef XNDRAW_DELETEPTR_H
#define XNDRAW_DELETEPTR_H

#include <functional>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
//#include "textio.h"
#include "xndrawexceptions.h"

namespace NCOORDS{
  
  template<class T>
    class realcopy_:public std::unary_function<T*, T*>{
    static unsigned int invocation_count;
  public:
    realcopy_(){}
    ~realcopy_(){}
    
    T* operator()(T* to_copy){
      T* result = NULL;
      std::cout  << "_**realcopy_<T>::operator()**_"<< "T to_copy = " <<  *to_copy << " at " << to_copy << " \n"<< *this  << count()<< std::endl;
      if(to_copy){
	result = new T(*to_copy);
      }
      else{throw xn_Error("ouch,Null T* being copied");}
      return result;
    }
    unsigned int count(){
      return ++invocation_count;
    }
    
    friend std::ostream& operator<<(std::ostream& os , const  realcopy_<T>& functor){
      os <<  "****realcopy_<T>***** \ninvocation count = "  << invocation_count << std::endl;
      return os;
    }

	};

  template <class CONTAINER, class INSERTER>//front and back_inserter
  class copyfirst_inserter {
    typedef CONTAINER container_type;
    typedef typename container_type::iterator    iterator;
typedef typename container_type::const_iterator    const_iterator;
    typedef typename container_type::value_type value_type;

  };

    template<class T>
    unsigned int realcopy_<T>::invocation_count =0;


    template<class COORDTYPE>
    class deleter_void:public std::unary_function<COORDTYPE*, void>{
    public:
      deleter_void(){}
      ~deleter_void(){}
      
    void operator()(COORDTYPE* deletable){
 if (deletable){
      delete deletable;
deletable = NULL;
 }   
    }
    
  };
  
  template<class COORDTYPE>
  class iterator_deleter_:public std::unary_function<std::pair<COORDTYPE*,int> , void>{
  public:
    iterator_deleter_(){}
    ~iterator_deleter_(){}
    
    void operator()(std::pair<COORDTYPE*,int> pair_ ){
      if (pair_.first){
      delete pair_.first;
      pair_.first = NULL;
      }
    }
  };
  
  template <typename DELETABLE>
  class delete_pointer:public std::unary_function<DELETABLE*,DELETABLE*>{
  public:
    delete_pointer(){}
    ~delete_pointer(){}
    DELETABLE* operator()(DELETABLE* deletable){
 if (deletable){
      delete deletable;
 }
      deletable = NULL;
      return deletable;
    }

  };



 
}/*namespace NCOORDS*/

#endif
