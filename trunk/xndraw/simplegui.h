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

#ifndef XNDRAW_SIMPLEGUI_H
#define XNDRAW_SIMPLEGUI_H
#include "xndrawexceptions.h"

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include "drawingset.h"
#include "drawtool.h"
#include "drawbuffer.h"
#include <gtkmm.h>
#include <map>
#include <string>

#define DRAW_WIDTH(outval) std::string s_w("IMAGEBUFFER_WIDTH"); \
  outval = XNDRAW_GET_PREF(s_w, 0U)
#define DRAW_HEIGHT(outval) std::string s_h("IMAGEBUFFER_HEIGHT");	\
  outval = XNDRAW_GET_PREF(s_h, 0U)

namespace xNDRAW{
  //  static unsigned int xnDrawWIDTH;
  //  static unsigned int xnDrawHEIGHT;

  //class GuiTool : public Gtk::IconButton



class xnDrawWindow : public Gtk::Window{
  protected:
//Nodes nodemap;
Drawingbase* draw;
Gtk::DrawingArea drawingarea;
CairoBuffer* buffer;
Pencil* clickinterpreter;
 std::map<std::string,sigc::connection> connections;//define an enum and map with them later
  public:
 xnDrawWindow();
  protected:
xnDrawWindow(const xnDrawWindow&);
xnDrawWindow& operator=(const xnDrawWindow&);
  public:
   void connect_init_signals();
   bool on_init();

   void connect_signals(){

   }

  };
}



#endif
