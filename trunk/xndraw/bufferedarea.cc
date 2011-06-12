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


#include "bufferedarea.h"


extern "C"{
  GdkRectangle set_rectangle(int x, int y, int w, int h){
    GdkRectangle r;
    r={x,y,w,h};
    return r;
  }

}

namespace xNDRAW{


void BufferedArea::add_to_damage(GdkRectangle& rect){
      gdk_rectangle_union(&rect, &damagearea, &damagearea);
    }

    void BufferedArea::on_realize(){
      DrawingArea::on_realize();
      //allocate_framebuffer();
    }

BufferedArea::BufferedArea( Cairo::RefPtr<Cairo::Surface> isource):
    framebuffer(NULL),
      imagesource(isource)
{
  damagearea= set_rectangle(0,0,0,0);
	set_app_paintable ();
	initconnection =Glib::signal_idle().connect(sigc::mem_fun(*this, &BufferedArea::allocate_framebuffer));
}

  bool  BufferedArea::allocate_framebuffer(){

    Gdk::Rectangle rect=get_allocation();  
    unsigned int width(rect.get_width());
    unsigned int height(rect.get_height());
    framebuffer=new LockingBuffer(width, height);
    if(!framebuffer){throw xn_Error("buffer allocation failed");}
    initconnection.disconnect();
    return true;
  }

}
