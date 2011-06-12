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

#include "nodesmap.h"
#include <iostream>
#include <sstream>

using namespace NCOORDS;
using namespace std;

int main(int argc, char** argv){
  std::ostringstream os;
  Nodedeq N;
  coords pt(1,2);
  coords adder(0.543, -0.456);
  for (int i =0; i < 10; i++){
    N.load(pt);
    pt += adder;
  }
  N.write_data(N, os);
  std::cout << "input Nodedeq:\n" << N << std::endl;
  std::istringstream iffs(os.str());
  Nodedeq result;
  iffs>> result;
  std::cout << "output Nodedeq:\n" << result << std::endl;

  return 0;
}
