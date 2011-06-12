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

#include "simplegui.h"
using namespace xNDRAW;
using namespace NCOORDS;

std::string s_w("IMAGEBUFFER_WIDTH");
std::string s_h("IMAGEBUFFER_HEIGHT");

static unsigned int xnDrawWIDTH = XNDRAW_GET_PREF(s_w, 0U);
static unsigned int xnDrawHEIGHT = XNDRAW_GET_PREF(s_h, 0U);

void xnDrawWindow::connect_init_signals(){
Glib::signal_idle().connect(sigc::mem_fun(*this, &xnDrawWindow::on_init)); 

}
 xnDrawWindow::xnDrawWindow():draw(NULL), 
			      buffer(NULL),
			      clickinterpreter(NULL){
   add(drawingarea);
   connect_signals();
   show_all();
}


bool xnDrawWindow::on_init(){
  //  cairo_t* pcairo  = get_window()->create_cairo_context()->cobj();
  //Nodes nodes; 
  //draw = create_drawing_object(pcairo);
  //draw->set_backbuffer(new Newdrawset
  return true;
}
