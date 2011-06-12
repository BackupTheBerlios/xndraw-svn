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

#ifndef XNDRAW_TREEPOO_H
#define XNDRAW_TREEPOO_H

struct xNturdcolumns: public Gtk::TreeModelColumnRecord
{
Gtk::TreeModelColumn<int> rownumbers;

  Gtk::TreeModelColumn<Drawingbase*> parent_drawings;
Gtk::TreeModelColumn<NCOORDS::simple_click_history*> paths;
Gtk::TreeModelColumn<NCOORDS::Nodedeq*> coordmaps;


  xNturdcolumns(){
    add(rownumbers);
    add(parent_drawings);
    add(paths);
    add(coordmaps);
  }
  virtual ~xNturdcolumns(){}



};

class Poo_datamodel: public Gtk::TreeModel{
  //put in X3_draw to make static
  //addm an row when Drawingbase added to drawdeque, or in Drawingbase CTor
  //create a singleton class Orphans:public Drawingbase to hold orphaned paths and maps? 
Glib::RefPtr<Gtk::TreeStore> tree;
  xNturdcolumns columns;
  Public:
  Poo_datamodel(){tree = Gtk::TreeStore::create(columns);}
  

};

}/*namespace  xNDRAW*/

#endif /* XNDRAW_TREEPOO_H*/



