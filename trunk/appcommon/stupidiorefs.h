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



#ifndef LIBSNARKOV_STUPIDIOREFS_H
#define LIBSNARKOV_STUPIDIOREFS_H

#if HAVE_CONFIG_H
#include "appcommonconfig.h"
#endif

#ifndef NAMESPACE_APPCOMMON
#define NAMESPACE_APPCOMMON namespace APPCOMMON
#define NAMESPACE APPCOMMON
#endif

#include <iostream>

NAMESPACE_APPCOMMON{

template <typename CHARTYPE>
  std::basic_ostream<CHARTYPE>& get_COUT();

template <typename CHARTYPE>
  std::basic_ostream<CHARTYPE>& get_CERR();

template <typename T>
  struct FormattedPrinter;
template <typename T>
  struct RawDataPrinter;


template <typename T>
  struct FormattedPrinter{
    const T& printdata;
    FormattedPrinter(const T& t):printdata(t){}
  };

template <typename T>
  struct RawDataPrinter{
    const T& printdata;
    RawDataPrinter(const T& t):printdata(t){}
  };
template <typename T>
  struct FormattedReader{
    T& readdata;
    FormattedReader (T& t):readdata(t){}
  };

template <typename T>
  struct RawDataReader{
    T& readdata; 
RawDataReader(T& t):readdata(t){}
  };


 template <typename CHARTYPE, typename T>
   std::basic_ostream<CHARTYPE>& operator<<(std::basic_ostream<CHARTYPE>& os,
					    const FormattedPrinter<T>& src){
   //override through specialisations
   return os << src.printdata;
 }

template <typename CHARTYPE, typename T>
   std::basic_ostream<CHARTYPE>& operator<<(std::basic_ostream<CHARTYPE>& os,
					    const RawDataPrinter<T>& src){
   //override through specialisations
   return os << src.printdata;
 }

 template <typename CHARTYPE, typename T>
   std::basic_istream<CHARTYPE>& operator>>(std::basic_istream<CHARTYPE>& os,
					    const FormattedReader<T>& src){
   //override through specialisations
   return os >> src.readdata;
 }

template <typename CHARTYPE, typename T>
  std::basic_ostream<CHARTYPE>& operator>>(std::basic_istream<CHARTYPE>& os,
					    const RawDataReader<T>& src){
   //override through specialisations
  return os >> src.readdata;
 }




}
#endif
