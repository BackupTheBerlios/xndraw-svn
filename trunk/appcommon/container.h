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
 * @file container.h
 * functions etc pertaining to stl style container
 */ 
#ifndef SNARKY_APPCOMMON_CONTAINER_H 
#define SNARKY_APPCOMMON_CONTAINER_H

#if HAVE_CONFIG_H
#include "appcommonconfig.h"
#endif

#ifndef NAMESPACE_APPCOMMON
#define NAMESPACE_APPCOMMON namespace APPCOMMON
#define NAMESPACE APPCOMMON
#endif

#include "exceptions.h"

NAMESPACE_APPCOMMON{

  template <typename FWDITER, typename OBJ>
    bool in_container(FWDITER first, FWDITER last, const OBJ& obj){
    for(;first !=last;first++){
      if(*first==obj){return true;}
    }
    return false;
  }

template <typename FWDITER, typename OBJ>
    bool pair_first_in_container(FWDITER first, FWDITER last, const OBJ& obj){
    for(;first !=last;first++){
      if((*first).first == obj){return true;}
    }
    return false;
  }

  template <typename FWDITER>
    bool contains_dupes(FWDITER first, FWDITER last){
    FWDITER second(first), starter(first);
    for(;first !=last;first++){
      for(;second !=last;second++){
	if(second==first){continue;}
	else if(*second==*first){
#if CONTAINER_H_VERBOSE
	  std::cerr <<"contains_dupes(): item "<<first-starter<<"\n("<<*first
		    <<")\nis the same as item "<<second-starter<<std::endl;
#endif
	  return true;
	}
      }
    }
    return false;
  }

  template <typename FWDITER>
    bool pair_first_contains_dupes(FWDITER first, FWDITER last){
    FWDITER second(first), starter(first);
    for(;first !=last;first++){
      for(;second !=last;second++){
	if(second==first){continue;}
	else if((*second).first==(*first).first){
#if CONTAINER_H_VERBOSE
	  std::cerr <<"pair_first_contains_dupes(): item "<<first-starter<<"\n("<<(*first).first
		    <<")\nis the same as item "<<second-starter<<std::endl;
#endif
	  return true;
	}
      }
    }
    return false;
  }

  template <typename FWDITER>
    std::pair<FWDITER, FWDITER> get_next_pair_first_dupe(FWDITER first, FWDITER last){
    FWDITER second(first), starter(first);
    for(;first !=last;first++){
      for(;second !=last;second++){
	if(second==first){continue;}
	else if((*second).first==(*first).first){
	  return std::pair<FWDITER, FWDITER>(first, second);
	}
      }
    }
    return std::pair<FWDITER, FWDITER>(last, last);
  }

  template <typename FWDITER>
    std::pair<FWDITER, FWDITER> get_next_dupe(FWDITER first, FWDITER last){
    FWDITER second(first), starter(first);
    for(;first !=last;first++){
      for(;second !=last;second++){
	if(second==first){continue;}
	else if(*second==*first){
	  return std::pair<FWDITER, FWDITER>(first, second);
	}
      }
    }
    return std::pair<FWDITER, FWDITER>(last, last);
  }

 
}//namespace ... something
#endif
