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

#include "xncairoerror.h"
#include "drawingbase.h"


namespace xNDRAW{ 

std::string CairoError::caironame_of(const cairo_t* cr){
    std::stringstream ostmp;
    ostmp << "cairo_t* at "<< cr;
    return ostmp.str();
  }

 std::string CairoError::caironame_of(const cairo_surface_t* cr){
    std::stringstream ostmp;
    ostmp << "cairo_surface_t* at "<< cr;
    return ostmp.str();
  }

  std::string CairoError::caironame_of(const Cairo::RefPtr<Cairo::Context>& cr){
    std::stringstream ostmp;
    ostmp << "Cairo::RefPtr<Cairo::Context> with underlying cairo_t at "
	  << cr->cobj();
    return ostmp.str();
  }
std::string CairoError::caironame_of(const Cairo::RefPtr<Cairo::Surface>& cr){
    std::stringstream ostmp;
    ostmp << "Cairo::RefPtr<Cairo::Surface> with underlying cairo_surface_t at "
	  << cr->cobj();
    return ostmp.str();
  }

std::string CairoError::caironame_of(const Cairo::RefPtr<Cairo::ImageSurface>& cr){
    std::stringstream ostmp;
    ostmp << "Cairo::RefPtr<Cairo::ImageSurface> with underlying cairo_surface_t at "
	  << cr->cobj();
    return ostmp.str();
  }

std::string CairoError::caironame_of(const Cairo::RefPtr<Cairo::SvgSurface>& cr){
    std::stringstream ostmp;
    ostmp << "Cairo::RefPtr<Cairo::Surface> with underlying cairo_surface_t at "
	  << cr->cobj();
    return ostmp.str();
  }

  std::string CairoError::caironame_of(const Cairo::RefPtr<Drawingbase>& dr){
 std::stringstream ostmp;
    ostmp << "Cairo::RefPtr<Drawingbase> with underlying cairo_t at "
	  << dr->cobj();
    return ostmp.str();
  }
  cairo_status_t CairoError::do_status(const cairo_t* cr, bool do_throw){
cairo_status_t status= CAIRO_STATUS_SUCCESS;
    if(cr){
      status= cairo_status(const_cast<cairo_t*>(cr));
    }
    else{ status = CAIRO_STATUS_NULL_POINTER;}
    if(status != CAIRO_STATUS_SUCCESS && do_throw){
    throw CairoError("NULL", status);
    }
    return status;
  }

  cairo_status_t CairoError::do_status(const cairo_surface_t* cr, 
				       bool do_throw){
    cairo_status_t status= CAIRO_STATUS_SUCCESS;
    if(cr){
      status = cairo_surface_status(const_cast<cairo_surface_t*>(cr));
    }
   else{ status = CAIRO_STATUS_NULL_POINTER;}
    if(status != CAIRO_STATUS_SUCCESS && do_throw){
    throw CairoError("NULL", status);
    }
    return status;
  }

  cairo_status_t CairoError::do_status(const Cairo::RefPtr<Cairo::Context>& cr, 
				  bool do_throw){
    cairo_status_t status= CAIRO_STATUS_SUCCESS;
    if(cr){
      status= cairo_status(cr->cobj());
    }
    else{ status = CAIRO_STATUS_NULL_POINTER;}
    if(status != CAIRO_STATUS_SUCCESS && do_throw){
    throw CairoError("NULL", status);
    }
    return status;
  }

  cairo_status_t CairoError::do_status(const Cairo::RefPtr<Cairo::Surface>& surface, 
				  bool do_throw){
    cairo_status_t status= CAIRO_STATUS_SUCCESS;
    if(surface){
      status= cairo_surface_status(surface->cobj());
    }
    else{ status = CAIRO_STATUS_NULL_POINTER;}
    if(status != CAIRO_STATUS_SUCCESS && do_throw){
    throw CairoError("NULL", status);
    }
    return status;
  }

  cairo_status_t CairoError::do_status(const Cairo::RefPtr<Drawingbase>& dr, 
				  bool do_throw){
    cairo_status_t status= CAIRO_STATUS_SUCCESS;
    if(dr){
      status= cairo_status( dr->cobj());
    }
    else{ status = CAIRO_STATUS_NULL_POINTER;}
    if(status != CAIRO_STATUS_SUCCESS && do_throw){
    throw CairoError("NULL", status);
    }
    return status;
  }

}
