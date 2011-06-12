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

#include "basecoords.h"
#include "blockalloc.h"
#include <iostream>
//iclude <GL/glut.h>
#ifndef __gl_h_
#define GLdouble double
#endif

namespace NCOORDS{

  template <typename SCALAR, typename UINTTYPE,  UINTTYPE SZ>
    class PointDataStaticBase: public basecoords{
    SCALAR data[SZ];
    
    //SCALAR * p_data;
  public:
    PointDataStaticBase(SCALAR D[SZ]){
      for(UINTTYPE i = 0; i < SZ; i++){
	data[i] = D[i];
      }
    }
    template<typename SCALAR2, typename UINTTYPE2,  UINTTYPE2 SZ2>
      PointDataStaticBase(const SCALAR2 srcdata[SZ2]){
      UINTTYPE minsz = std::min(SZ, (UINTTYPE)SZ2);
      for(UINTTYPE i = 0; i < minsz; i++){
	data[i] = srcdata[i];
      }
      if(SZ2 < SZ){
	for(UINTTYPE j = SZ2; j < SZ; j++){
	  data[j] = 0;
	}
      }
    }
    
    
    template<typename SCALAR2, typename UINTTYPE2,  UINTTYPE2 SZ2>
      PointDataStaticBase(const PointDataStaticBase<SCALAR2, UINTTYPE2, SZ2>& src){
      UINTTYPE minsz = std::min(SZ, (UINTTYPE)SZ2);
      for(UINTTYPE i = 0; i < minsz; i++){
	data[i] = src[i];
      }
      if(SZ2 < SZ){
	for(UINTTYPE j = SZ2; j < SZ; j++){
	  data[j] = 0;
	}
      }
    }
    
  protected:
    SCALAR* get_data()const {return data;}
  public:
    const SCALAR* at(UINTTYPE n)const{
      if(n >= SZ || n < 0){
	//TODO: compile check for unsignedness of UINTTYPE
	throw std::range_error("index out of range");
      }
      return data + n;
    }
    SCALAR* at(UINTTYPE n){
      if(n >= SZ || n < 0){
	//TODO: compile check for unsignedness of UINTTYPE
	throw std::range_error("index out of range");
      }
      return data + n;
    }
    const SCALAR* operator[](UINTTYPE n)const{
      return this->at(n);
    }
    SCALAR* operator[](UINTTYPE n){
      return this->at(n);
    }
    
    void set(const SCALAR* src, UINTTYPE startoffset = 0, UINTTYPE count = SZ){
      UINTTYPE endoffset = startoffset + count;
      if (endoffset  > SZ || startoffset < 0){
	throw std::range_error("end index out of range");
      }
      for (UINTTYPE i = startoffset; i < endoffset; i++){
	data[i] = src[i];
      }
    }
  };
  
  template <typename SCALAR, size_t SZ, unsigned char OFFSET>
    class VertexData: 
  public PointDataStaticBase<SCALAR, size_t, SZ>{
  public:
    typedef PointDataStaticBase<SCALAR, size_t, SZ> vertex_basetype;
    
    VertexData(SCALAR src[SZ]): vertex_basetype(src){}
      
      template<typename SCALAR2, typename UINTTYPE,  UINTTYPE SZ2>
	VertexData(const PointDataStaticBase<SCALAR2, UINTTYPE, SZ2>& src):
	vertex_basetype(src){}
	template<typename SCALAR2,size_t SZ2, unsigned char OFFSET2>
	  VertexData(const VertexData< SCALAR2,SZ2,OFFSET2>& src):vertex_basetype(src){}
	  
	  SCALAR* nth_array(size_t n)const{
	    return this->at(n * OFFSET);
  }
  };
  
  template<typename SCALAR> // A 'number'
    class CoordinateData{
    SCALAR* theData;
    size_t datasize;
    
  public:
    CoordinateData(size_t  n):theData(new SCALAR[n]), datasize(n){}
      
      
  };

  template<size_t DIMENSIONS=2, unsigned char OFFSET=4>
    class XNglarray: public VertexData<GLdouble, DIMENSIONS *OFFSET, OFFSET>{ 
public:
XNglarray(GLdouble src[DIMENSIONS]):VertexData<GLdouble, DIMENSIONS*OFFSET, OFFSET>(src){}
 
  };
template<size_t DIMENSIONS>
  class XNglarray<DIMENSIONS, 0>:public VertexData<GLdouble, DIMENSIONS, 0>{
  public:
    XNglarray(GLdouble src[DIMENSIONS]):VertexData<GLdouble, DIMENSIONS, 0>(src){}
  };
}


/**
///from Eckel: thinking in c++ vol 2
//: C05:MyVector2.cpp
// Handles sums of any length with expression templates.
#include <cstddef>
#include <cstdlib>
#include <ctime>
#include <iostream>
using namespace std;
 
// A proxy class for sums of vectors
template<class, size_t, class, class> class MyVectorSum;
 
template<class T, size_t N> class MyVector {
  T data[N];
public:
  MyVector<T,N>& operator=(const MyVector<T,N>& right) {
    for(size_t i = 0; i < N; ++i)
      data[i] = right.data[i];
    return *this;
  }
  template<class Left, class Right> MyVector<T,N>&
  operator=(const MyVectorSum<T,N,Left,Right>& right);
  const T& operator[](size_t i) const {
    return data[i];
  }
  T& operator[](size_t i) {
    return data[i];
  }
};
 
// Allows mixing MyVector and MyVectorSum
template<class T, size_t N, class Left, class Right>
class MyVectorSum {
  const Left& left;
  const Right& right;
public:
  MyVectorSum(const Left& lhs, const Right& rhs)
  : left(lhs), right(rhs) {}
  T operator[](size_t i) const {
    return left[i] + right[i];
  }
};
 
template<class T, size_t N>
template<class Left, class Right>
MyVector<T,N>&
MyVector<T,N>::
operator=(const MyVectorSum<T,N,Left,Right>& right) {
  for(size_t i = 0; i < N; ++i)
    data[i] = right[i];
  return *this;
}
// operator+ just stores references
template<class T, size_t N>
inline MyVectorSum<T,N,MyVector<T,N>,MyVector<T,N> >
operator+(const MyVector<T,N>& left,
          const MyVector<T,N>& right) {
  return MyVectorSum<T,N,MyVector<T,N>,MyVector<T,N> >
      (left,right);
}
 
template<class T, size_t N, class Left, class Right>
inline MyVectorSum<T, N, MyVectorSum<T,N,Left,Right>,
            MyVector<T,N> >
operator+(const MyVectorSum<T,N,Left,Right>& left,
          const MyVector<T,N>& right) {
  return MyVectorSum<T,N,MyVectorSum<T,N,Left,Right>,
                         MyVector<T,N> >
    (left, right);
}
// Convenience functions for the test program below
template<class T, size_t N> void init(MyVector<T,N>& v) {
  for(size_t i = 0; i < N; ++i)
    v[i] = rand() % 100;
}
 
template<class T, size_t N> void print(MyVector<T,N>& v) {
  for(size_t i = 0; i < N; ++i)
    cout << v[i] << ' ';
  cout << endl;
}
 
int main() {
  srand(time(0));
  MyVector<int, 5> v1;
  init(v1);
  print(v1);
  MyVector<int, 5> v2;
  init(v2);
  print(v2);
  MyVector<int, 5> v3;
  v3 = v1 + v2;
  print(v3);
  // Now supported:
  MyVector<int, 5> v4;
  v4 = v1 + v2 + v3;
  print(v4);
  MyVector<int, 5> v5;
  v5 = v1 + v2 + v3 + v4;
  print(v5);
} ///:~



*/
