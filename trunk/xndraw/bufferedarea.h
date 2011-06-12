// -*- c++ -*-

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


#ifndef XNDRAW_BUFFEREDAREA_H
#define XNDRAW_BUFFEREDAREA_H

#include "xndrawexceptions.h"

#if HAVE_CONFIG_H
#include "config.h"
#endif
#include "drawbuffer.h"
#include <gtkmm.h>
#include <cairomm/context.h>

extern "C"{
  GdkRectangle set_rectangle(int x, int y, int w, int h);
}



namespace xNDRAW{

class BufferedArea: public Gtk::DrawingArea{
  protected:
    GdkRectangle damagearea;//put expose rectangle here if on_expose_event()
    //can't update scree
    LockingBuffer* framebuffer;
    sigc::connection initconnection;
    Cairo::RefPtr<Cairo::Surface> imagesource;

    BufferedArea( Cairo::RefPtr<Cairo::Surface> isource= Cairo::RefPtr<Cairo::Surface>(NULL));

    void add_to_damage(GdkRectangle& rect);

    virtual void on_realize();

    bool  allocate_framebuffer();

};


}

#endif// XNDRAW_BUFFEREDAREA_H
