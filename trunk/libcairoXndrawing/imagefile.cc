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

#include "imagefile.h"
#include <cairomm/surface.h>


namespace xNDRAW{


  int writefile(Cairo::RefPtr<Cairo::Context> cairo, 
		const std::string& path,
		xnFileType ftype){
    Cairo::RefPtr<Cairo::Surface> surface = cairo->get_target();
    switch(ftype){
    case XNDRAW_PNG:
      surface->write_to_png(path);
      return COMMAND_SUCCESS;
    case XNDRAW_SVGRENDER:
    default:
      throw xn_Error("FInishME:nsee code in drawingbase etc");
    }

  }

  FileCommandParams::FileCommandParams():path(""),
					 ftype(XNDRAW_PNG),
					 do_it(false),
					 command(&writefile),width(0), height(0){}

  FileCommandParams::FileCommandParams(const std::string& p, 
				       xnFileType t, 
				       bool reallywritefile):
    path(p),
    ftype(t),
    do_it( reallywritefile),
    command(&writefile),width(0), height(0){}
  FileCommandParams::FileCommandParams(const std::string& p,
				       file_command_type cmd,
				       xnFileType t, 
				       bool reallywritefile,
				       unsigned short w, unsigned short h):
    path(p),
    ftype(t),
    do_it( reallywritefile),
    command(cmd),width(w), height(0){}
  //width, height only for svg
  int FileCommandParams::operator()(Cairo::RefPtr<Cairo::Context> con){
    if(do_it){
      try{
	int res = (*command)(con, path, ftype);
	if(!res){return COMMAND_FAILURE;}
	return COMMAND_SUCCESS;
      }
      catch(const std::exception& e){
	std::cerr <<"File command failed:\n"<<e.what()<<std::endl;
	return COMMAND_FAILURE;
      }
    }
    return COMMAND_IGNORED;
  }

  void print_surface_type(Cairo::RefPtr<Cairo::Surface> surface,
			  std::ostream& os){
    Cairo::SurfaceType typesurf= surface->get_type();
    os << "type of surface at "<<surface->cobj()<< " : ";
    switch( typesurf){
    case Cairo::SURFACE_TYPE_IMAGE:
      os << " SURFACE_TYPE_IMAGE";
      break;	
    case Cairo::SURFACE_TYPE_PDF:	
      os << "SURFACE_TYPE_PDF";	
      break;
    case Cairo::SURFACE_TYPE_PS:
      os << "SURFACE_TYPE_PS";
      break;			
    case Cairo::SURFACE_TYPE_XLIB:
      os << "SURFACE_TYPE_XLIB";
      break;	
    case Cairo::SURFACE_TYPE_XCB:
      os << "SURFACE_TYPE_XCB";
      break;	
    case Cairo::SURFACE_TYPE_GLITZ:
      os << "SURFACE_TYPE_GLITZ";
      break;	
    case Cairo::SURFACE_TYPE_QUARTZ:
      os << "SURFACE_TYPE_QUARTZ";
      break;
    case Cairo::SURFACE_TYPE_WIN32:
      os << "SURFACE_TYPE_WIN32";
      break;	
    case Cairo::SURFACE_TYPE_BEOS :
      os << "SURFACE_TYPE_BEOS";
      break;	
    case Cairo::SURFACE_TYPE_DIRECTFB:
      os << " SURFACE_TYPE_DIRECTFB";
      break;	
    case Cairo::SURFACE_TYPE_SVG:	
      os << "SURFACE_TYPE_SVG";
      break;	
    case Cairo::SURFACE_TYPE_OS2 :		
      os << "SURFACE_TYPE_OS2 ";
      break;	
    case Cairo::SURFACE_TYPE_WIN32_PRINTING:	
      os << "SURFACE_TYPE_WIN32_PRINTING";
      break;		
    case Cairo::SURFACE_TYPE_QUARTZ_IMAGE :		
      os << "SURFACE_TYPE_QUARTZ_IMAGE";
      break;	
    }
    os <<std::endl;
  }


}
