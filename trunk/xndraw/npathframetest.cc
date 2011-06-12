//-*- c++ -*-

/*  Copyright (C) 2006-2011 John O'Driscoll
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "npathframe.h"

int main(int c , char** v){
  using namespace xNDRAW;
Gtk::Main mainfunc(c,v);
 NCOORDS::coords pt(100, 100), pt1(101,200),pt2(201, 100),pt3(201, 100), pt4(234,432);
 NCOORDS::simple_click_history path;
  int MOVEE(2), LINEE(1);
  typedef std::pair<NCOORDS::coords, int> pair_type;
  path.push_back(pair_type(pt, MOVEE));
  pair_type p(pt, MOVEE);
  pair_type p1(pt1, LINEE);
  pair_type p2(pt2, LINEE);
  pair_type p3(pt3, LINEE);
  pair_type p4(pt4, LINEE);
  path.push_back(p);
  path.push_back(p1);
  path.push_back(p2);
  path.push_back(p3);
  path.push_back(p4);
  std::cerr<<"pathframetest : path :\n"<<path<<std::endl;
  NCOORDS::ncoords nc(pt, pt1, 3);
  Glib::ustring title("this a test");
  xNDRAW::nPathWindow window(title, &path, nc);
  
mainfunc.run(window);

  return 0;
}
