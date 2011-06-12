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




#ifndef APPCOMMON_WANTHASH_H
#define APPCOMMON_WANTHASH_H

#ifdef HAVE_CONFIG_H
#include "appcommonconfig.h"
#endif

#ifndef NAMESPACE_APPCOMMON
#define NAMESPACE_APPCOMMON namespace APPCOMMON
#define NAMESPACE APPCOMMON
#endif
#include <string>
#ifndef SnarkyMap 
#define SnarkyMap XndrawMap
#endif
#ifndef SnarkySet
#define SnarkySet XndrawSet
#endif

#if HAVE_HASHMAP

#if  HAVE_UNORDERED_MAP
#define BEGIN_HASH_NAMESPACE namespace  std{
#define END_HASH_NAMESPACE }
#define HASH_NAMESPACE std
#include <unordered_map>
#else
#if  HAVE_TR1_UNORDERED_MAP
#define BEGIN_HASH_NAMESPACE namespace  std{ \
  namespace  tr1{
#define END_HASH_NAMESPACE } }
#define HASH_NAMESPACE std::tr1
#include <tr1/unordered_map>
#else
#define BEGIN_HASH_NAMESPACE namespace __gnu_cxx{
#define END_HASH_NAMESPACE }
#define HASH_NAMESPACE __gnu_cxx
#include <ext/hash_map>
#endif
#endif
#endif

#if HAVE_HASHSET
#if HAVE_UNORDERED_SET
#include <unordered_set>
#else
#if HAVE_TR1_UNORDERED_SET
#include <tr1/unordered_set>

#else
#if HAVE_EXT_HASH_SET
#include <ext/hash_set>
#else
#error "bad hash set config"
#endif // HAVE_EXT_HASH_SET
#endif // HAVE_TR1_UNORDERED_SET

#endif // HAVE_UNORDERED_SET
#else
#include <set>
#endif //HAVE_HASHSET

NAMESPACE_APPCOMMON{

template <class CHARTYPE>
  size_t hash_stringT(const CHARTYPE* s){
  unsigned long h = 0;
    for ( ; *s; ++s)
      h = 5 * h + *s;
    return size_t(h);
 }

} // NAMESPACE_APPCOMMON


BEGIN_HASH_NAMESPACE

  //specialisations: see  ext/hash_fun.h
#if USE_EXT_HASH_MAP

template<>
  struct hash<std::string>{
    size_t operator()(const std::string& s)const{
      const char* c_string = s.c_str();
      return __stl_hash_string(c_string);
    }
  };
template<>
  struct hash<wchar_t>{
    size_t operator()(wchar_t x)const{
      return x;
    }
  };

 template<>
   struct hash<std::basic_string<wchar_t> >{
     
     size_t operator()(const std::basic_string<wchar_t>& s)const{
       const wchar_t* c_string = s.c_str();
       return NAMESPACE::hash_stringT(c_string);
     }
     
   };


#endif

  template <class CHARTYPE, class TRAITS, class ALLOC>
  struct hash<std::pair<std::basic_string<CHARTYPE, TRAITS, ALLOC> , size_t>*>{
    typedef  std::basic_string<CHARTYPE, TRAITS, ALLOC> std_type;
    typedef std::pair<std_type, size_t> value_type;

    size_t operator()(value_type* const& pr)const{
      const std_type& s = pr->first;
      const CHARTYPE* c_string = s.c_str();
      return NAMESPACE::hash_stringT(c_string);
    }

    };



END_HASH_NAMESPACE // namespace __gnu_cxx / std / std::tr1

#endif // APPCOMMON_WANTHASH_H
