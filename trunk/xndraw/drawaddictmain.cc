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

#include "drewadit.h"
#include "config.h"

using namespace NCOORDS;
using namespace xNDRAW;


int main(int argc, char** argv){
  Gtk::Main main(argc,argv);
Glib::thread_init();
  Gtk::Window win;
  drawedit_widget widge;
  win.add(widge);
  widge.show();
  win.set_events(Gdk::ALL_EVENTS_MASK);
  win.signal_button_release_event().connect(sigc::mem_fun(widge, &drawedit_widget::on_button_release), false);
win.signal_button_press_event().connect(sigc::mem_fun(widge, &drawedit_widget::on_button_press), false);
  win.show();
  Glib::signal_idle().connect(sigc::mem_fun(widge, &drawedit_widget::on_init),false);
  Gtk::Main::run(win);
  return 0;
}
