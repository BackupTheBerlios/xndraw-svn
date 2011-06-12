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


#ifndef LIBCAIROXNDRAWING_XNCAIROERROR_H
#define LIBCAIROXNDRAWING_XNCAIROERROR_H
#include "exceptions.h"

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <string>
#include <sstream>
#include "xndrawexceptions.h"
#include <cairomm/context.h>

#define XN_CHECK_STATUS(cr) CairoError::status(cr)


namespace xNDRAW{

  class Drawingbase;

  class CairoError: public xn_Error{

    CairoError& operator=(const CairoError&);
  protected:
    cairo_status_t errorstatus;
  public:
    CairoError(const char* errcomment, cairo_status_t status) throw():xn_Error(errcomment), 
      errorstatus(status){}
    virtual std::string what_string()const throw(){
      std::stringstream ostmp;
      ostmp <<   errorname() << " : " << cairo_status_to_string(errorstatus);
      if (errr.size()){
	ostmp << "\n" << errr;
      }
      return ostmp.str();
    }

    static std::string caironame_of(const cairo_t* cr);
    static std::string caironame_of(const cairo_surface_t* cr);
    static std::string caironame_of(const Cairo::RefPtr<Cairo::Context>& cr);
    static std::string caironame_of(const Cairo::RefPtr<Cairo::Surface>& cr);
    static std::string caironame_of(const Cairo::RefPtr<Cairo::ImageSurface>& cr);
    static std::string caironame_of(const Cairo::RefPtr<Cairo::SvgSurface>& cr);
    static std::string caironame_of(const Cairo::RefPtr<Drawingbase>& dr);
    static cairo_status_t do_status(const cairo_t* cr, bool do_throw=true);
    static cairo_status_t do_status(const cairo_surface_t* cr, bool do_throw=true);
    static cairo_status_t do_status(const Cairo::RefPtr<Cairo::Context>& cr,
				    bool do_throw=true);
    static cairo_status_t do_status(const Cairo::RefPtr<Cairo::Surface>& surface,
				    bool do_throw=true);
    static cairo_status_t do_status(const Cairo::RefPtr<Drawingbase>& dr,
				    bool do_throw=true);

    template <typename CAIROTYPE>
      static void status(CAIROTYPE cr){
      cairo_status_t stat = do_status(cr);
      if(stat != CAIRO_STATUS_SUCCESS){
	std::stringstream s_errtmp ;
	s_errtmp << "cairo object with bad status:" << caironame_of(cr);
	std::string s_err = s_errtmp.str();
	throw CairoError(s_err.c_str(), stat);
      } 
    }

  };

}

#endif
