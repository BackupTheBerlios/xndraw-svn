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


#define COMPILING_STUPIDAPP 1
#include "fileexists.h"
#include <iostream>

int main(int argc, char** argv){
  using NAMESPACE_APPCOMMON;
  if(argc < 2) return 0;
  std::cout << "listing " << argv[1] << "\n";
  std::deque<std::string> sss = get_dirlisting(argv[1]);
  std::cout << sss << std::endl;
  if (argc > 2){
    std::string name(argv[2]);
    std::cout << "name: " << name << std::endl;
    if (file_in_dir(argv[1], name)){
      std::cout <<"file " << argv[2] << " in " << argv[1]<< std::endl;
    }
    else{
      std::cout <<"file " << argv[2] << " not in " << argv[1]<< std::endl;
    }
  }
  return 0;
}
