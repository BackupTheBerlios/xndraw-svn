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


#ifndef LIBCAIROXNDRAWING_IMAGEFILE_H
#define LIBCAIROXNDRAWING_IMAGEFILE_H

#include "exceptions.h"

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include "xndrawexceptions.h"

#include <cairomm/context.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>


namespace xNDRAW{

  enum xnFileType{
    XNDRAW_NOFILE=0,
    XNDRAW_PNG =1,
    XNDRAW_SVGRENDER=2,
    XNDRAW_SVGPATH=3,
    XNDRAW_PATH=4,
    XNDRAW_GRID=5,
XNDRAW_PROJECT=6
  };

enum SvgSaveFlags{
  XNDRAW_SVG_WRITE_PNG =1,
XNDRAW_SVG_WRITE_PATH = 2,
  XNDRAW_SVG_WRITE_ALL = XNDRAW_SVG_WRITE_PNG |XNDRAW_SVG_WRITE_PATH
};

  enum{
    COMMAND_FAILURE=-1,
    COMMAND_SUCCESS=0,
    COMMAND_IGNORED=1
  };

  void print_surface_type(Cairo::RefPtr<Cairo::Surface> surface,
			  std::ostream& = std::cout);

int writefile(Cairo::RefPtr<Cairo::Context> cairo, 
	      const std::string& filepath, 
	      xnFileType filetype);

struct FileCommandParams{

  typedef int(*file_command_type)(Cairo::RefPtr<Cairo::Context>, 
				  const std::string& , 
				  xnFileType);


  std::string path;
  xnFileType ftype;
  bool do_it;
  file_command_type command;
  unsigned short width;//no 8-bit shorts anywhere?
  unsigned short height;

  int operator()(Cairo::RefPtr<Cairo::Context> con);
  //auto generated copy ctor and op= are OK
  FileCommandParams();

  FileCommandParams(const std::string& p, 
		    xnFileType t= XNDRAW_PNG, 
		    bool reallywritefile=true);
FileCommandParams(const std::string& p,
		  file_command_type cmd,
		    xnFileType t= XNDRAW_PNG, 
		  bool reallywritefile=true,
		  unsigned short w=0, unsigned short h=0);

  bool do_pre_command()const{
    return do_it && ((ftype== XNDRAW_SVGRENDER)||(ftype== XNDRAW_SVGPATH));
  }
bool do_post_command()const{
    return do_it && (ftype== XNDRAW_PNG);
  }

  };


} //namespace xNDRAW


#endif //LIBCAIROXNDRAWING_IMAGEFILE_H
