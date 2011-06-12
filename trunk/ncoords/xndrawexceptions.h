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


// EXCEPTIONS

#ifndef NCOORDS_EXCEPTIONS_H
#define NCOORDS_EXCEPTIONS_H

#define DO_ERROR(e_Type, exp) do{		\
    std::ostringstream tempstream;		\
    tempstream << exp;				\
    throw e_Type(tempstream.str());		\
  } while(0)

#define XN_ERROR(exp) DO_ERROR(xn_Error, exp)

#include "exceptions.h"
#include <string>
//#include <cairo.h>
#include <sstream>


#include "exceptions.h"

namespace NCOORDS{
  
  using NAMESPACE_APPCOMMON;

  SNARKY_DEFINEERROR(xn_Error, ErrorBase);
  SNARKY_DEFINEERROR(XNPathError, xn_Error);
SNARKY_DEFINEERROR(XNIOError, xn_Error);
SNARKY_DEFINEERROR(XNCoordsError, xn_Error);
SNARKY_DEFINEERROR(XNSelectionError, xn_Error);
SNARKY_DEFINEERROR(XNGuiError, xn_Error);
  SNARKY_DEFINEERROR(XNGeometryError,XNCoordsError);


  
  /*
  class xn_Error: public st7
d::exception{
    
    xn_Error& operator=(const xn_Error&); //private and unimplemented
    
  public:
    xn_Error(const xn_Error& src):std::exception(src), errr(src.errr){} 
    virtual ~xn_Error() throw (){ }
    xn_Error(const char* err) throw():errr(std::string("xn_Error: ") + err) { }
 
    xn_Error(const char* errcomment, cairo_status_t status) throw():errr(std::string("xn_Error: ") + errcomment) { 
      errr+= cairo_status_to_string(status);
    }
    

    const char* what()const throw(){
      return errr.c_str();
    }
    xn_Error(const std::string& e_) throw() :errr( std::string("xn_Error: ") + e_) { }
    
  protected:
    std::string errr;
    
  };
  */
  void deprecation_error(const char* function);
  
} // namespace NCOORDS

namespace xNDRAW{

  typedef NCOORDS::xn_Error xn_Error;
}
#endif //NCOORDS_EXCEPTIONS_H
