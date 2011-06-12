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



#ifndef SNARKY_APPCOMMON_FIXWIDECHAR_H
#define  SNARKY_APPCOMMON_FIXWIDECHAR_H

#ifdef HAVE_CONFIG_H
#include "appcommonconfig.h"
#endif

#include <iostream>
#include <locale>
#include "fileexists.h"


#define WIDEN_STRING(src, dest, loc)    widen_string(src, dest, loc)

#ifndef NAMESPACE_APPCOMMON
#define NAMESPACE_APPCOMMON namespace APPCOMMON
#define NAMESPACE APPCOMMON
#endif

#define FIXSHADOWS(c) ShadowFixer<CHARTYPE>::fixShadows(c)

NAMESPACE_APPCOMMON{
  //can't rely on std::isgraph, and friends(results often incorrect), or test equality against 
  // character in 'real' ascii range
  //with shadow ascii characters(the lower 128 values in every 256)
  //ie 256-383,512-639, 768-895, ....
  wchar_t& fix_asciiShadows(wchar_t& c);

  // char needs no fixing
template <class CHARTYPE>
  CHARTYPE& fixShadows(CHARTYPE& c){
  return c;
 }

template<>
  wchar_t& fixShadows(wchar_t& c);

template <typename CHARTYPE>
struct ShadowFixer{

  static CHARTYPE& fixShadows(CHARTYPE& c){ return c; }

};

template <>
struct ShadowFixer<wchar_t>{
  
  static wchar_t& fixShadows(wchar_t& c){ 
    wchar_t tmp = c % 256;
    if (tmp < 128){ c = tmp; }
    return c;
  }

};
//at least some libstdc++ versions(gcc3.4<->gcc4.1 at least)
//the char* version of widen doesn't work ...
//at least for me(maybe there's an incantation that helps?)
 template <typename CHARTYPE, typename TRAITS, typename ALLOC,
	    typename CHARTYPE2, typename TRAITS2, typename ALLOC2>
  std::basic_string<CHARTYPE, TRAITS, ALLOC>& widen_string(const std::basic_string<CHARTYPE2, TRAITS2, ALLOC2>& src,
							  std::basic_string<CHARTYPE, TRAITS, ALLOC>& dest, 
							  std::locale loc = std::locale()){
    std::basic_stringstream<CHARTYPE, TRAITS, ALLOC> tmpdest;
    typedef std::basic_string<CHARTYPE2, TRAITS2, ALLOC2> src_type;
    typedef typename src_type::const_iterator src_iterator;
    for(src_iterator w = src.begin(); w!= src.end(); w++){
      tmpdest << CTYPE_FACET(loc).widen(*w);
    }
    dest= tmpdest.str();
    return dest;
  }

  // char OK - needs no specialisation
}

#endif /// SNARKY_APPCOMMON_FIXWIDECHAR_H
