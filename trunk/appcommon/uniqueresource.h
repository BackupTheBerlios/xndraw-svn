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

#ifndef APPCOMMON_UNIQUERESOURCE_H
#define APPCOMMON_UNIQUERESOURCE_H

#ifdef HAVE_CONFIG_H
#include "appcommonconfig.h"
#endif

#ifndef NAMESPACE_APPCOMMON
#define NAMESPACE_APPCOMMON namespace APPCOMMON
#define NAMESPACE APPCOMMON
#endif
#include "exceptions.h"

typedef NAMESPACE::ContainerError ContainerError;

template <typename RESOURCE>
struct resbase{
  virtual ~resbase(){}

};
//how to define the static member -see below for error - maybe 
//parameterising the type of the ID is a fix...?
template <typename RESOURCE, typename UINTTYPE,  UINTTYPE SZ>
  class IDResourceBase;

template <typename RESOURCE, typename UINTTYPE>
struct ResFactory{
  UINTTYPE last;
ResFactory():last(0){}
template <UINTTYPE n>
  resbase<RESOURCE>* create();
};
template <typename RESOURCE, typename UINTTYPE,  UINTTYPE SZ>
  class IDResourceBase :public resbase<RESOURCE>{
 public:
    friend struct ResFactory<RESOURCE,UINTTYPE>;
 private:
  static RESOURCE * ptr_to_instance;

 IDResourceBase(const IDResourceBase&);
  IDResourceBase& operator=(const IDResourceBase&);
    
 protected:
  IDResourceBase() {}
  virtual ~IDResourceBase() {}
 public:

 
static void set_resource(const RESOURCE* p_res){
    if(ptr_to_instance){
      throw ContainerError("IDResource<RESOURCE, UINTTYPE, IDD> instance can only be set once");
    }
    ptr_to_instance = p_res;
  }
  
  static RESOURCE& instance() {
    if (ptr_to_instance == NULL){
      static RESOURCE theInstance;
      ptr_to_instance = &theInstance;
    }
    return *ptr_to_instance;
  }
  
  
  UINTTYPE get_ID(){return SZ;}

};

template <typename RESOURCE, typename UINTTYPE,  UINTTYPE SZ>
  RESOURCE* IDResourceBase<RESOURCE, UINTTYPE,  SZ>::ptr_to_instance =NULL;



template <typename RESOURCE, typename UINTTYPE>
template< UINTTYPE n>
  resbase<RESOURCE>* ResFactory<RESOURCE,UINTTYPE>::create(){
  //const UINTTYPE newID(++last);
    return new IDResourceBase<RESOURCE, UINTTYPE, n>;
  }

template<typename RESOURCE, typename UINTTYPE , UINTTYPE  IDD>
  class IDResource{
  //ie one per class
  static RESOURCE * ptr_to_Instance;
  
  
  IDResource(const IDResource&);
  IDResource& operator=(const IDResource&);
  
  
  
 protected:
  IDResource() {}
  virtual ~IDResource() {}
 public:
  
  static void set_resource(const RESOURCE* p_res){
    if(ptr_to_Instance){
      throw ContainerError("IDResource<RESOURCE, UINTTYPE, IDD> instance can only be set once");
    }
    ptr_to_Instance = p_res;
  }
  
  static RESOURCE& instance() {
    if (ptr_to_Instance == NULL){
      static RESOURCE theInstance;
      ptr_to_Instance = &theInstance;
    }
    return *ptr_to_Instance;
  }
  
  
  UINTTYPE get_ID(){return IDD;}
};
//why error?:
//error: RESOURCE* IDResource<RESOURCE, ID>::ptr_to_instance is not a static member of class IDResource<RESOURCE, ID> ? 

template<typename RESOURCE, typename UINTTYPE, UINTTYPE IDD> 
  RESOURCE* IDResource<RESOURCE, UINTTYPE, IDD>::ptr_to_Instance = NULL;



template<typename RESOURCE> 
class IDResource2{

      IDResource2(const IDResource2&);
      IDResource2& operator=(const IDResource2&);
 protected:

      RESOURCE* ptr_to_Instance;
      unsigned long ID;
 public:
      IDResource2(unsigned long I):ID(I) {}
      virtual ~IDResource2() {
	if(ptr_to_Instance){
	  delete ptr_to_Instance;
	}
      }
    public:

void set_resource(const RESOURCE* p_res){
if(ptr_to_Instance){
throw ContainerError("IDResource<RESOURCE, ID> instance can only be set once");
}
ptr_to_Instance = p_res;
}

      RESOURCE& instance() {
	if (ptr_to_Instance == NULL){
	  //static RESOURCE theInstance;
	  ptr_to_Instance = new RESOURCE;
	}
	return *ptr_to_Instance;
      }
      
      
      unsigned long get_ID(){return ID;}
    };





#endif
