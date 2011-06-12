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

#include "x3draw.h"
#include "hframe.h"
#include "config.h"


namespace xNDRAW{
  void fatal_error(){
    Glib::ustring message("a fatal error occurred - OOPS, soorrry");
    Gtk::MessageDialog dialog(message, false, Gtk::MESSAGE_ERROR);
    dialog.run();
  }
}

int main(int argc, char** argv){
  using namespace xNDRAW;
  using namespace NCOORDS;
  std::set_terminate(__gnu_cxx::__verbose_terminate_handler);
  Gtk::Main the_big_kahuna(argc, argv);
  Glib::ustring pkg(PACKAGE);
  Glib::set_application_name(pkg);
  sigc::connection e_connection = Glib::add_exception_handler(sigc::ptr_fun(&xNDRAW::fatal_error));
  if(!Glib::thread_supported()){
    Glib::thread_init();
  }
  commandline_opts* opts =new commandline_opts(argc,argv);
  static X3_draw& drawprog= X3_draw::instance(*opts);
 
  h_frame haitch;
  
  try{
    std::cout << "After h_frame haitch;" << std::endl;

    Glib::RefPtr<Gdk::Screen> screen(NULL);
    Gdk::Rectangle monitor;
    std::pair<unsigned int, unsigned int> widthheight=h_frame::get_framesize(screen, monitor);
    // = haitch.get_screen();
    //window maximised at startup - why?
    haitch.set_size_request(widthheight.first,widthheight.second );
    haitch.show_all_children();
    haitch.show();
    haitch.set_events(Gdk::ALL_EVENTS_MASK);
    //haitch. signal_button_release_event().connect(sigc::mem_fun(haitch, &h_frame::on_button_release),false);
    std::cout << "the_big_kahuna.run(haitch-and win-..);" << std::endl;
    the_big_kahuna.run(haitch);
  }
  catch(const std::exception& e){
    //doesn't work
    //undocumented Gtk::Widget_Class::button_press_event_callback catches and aborts first
    /*glibmm-ERROR **: 
      unhandled exception (type std::exception) in signal handler:
      what: out of memory

      aborting...

      Program received signal SIGABRT, Aborted.
      0x00007ffff1269a75 in *__GI_raise (sig=<value optimised out>)
      at ../nptl/sysdeps/unix/sysv/linux/raise.c:64
      64	../nptl/sysdeps/unix/sysv/linux/raise.c: No such file or directory.
      in ../nptl/sysdeps/unix/sysv/linux/raise.c
      (gdb) bt
      #0  0x00007ffff1269a75 in *__GI_raise (sig=<value optimised out>)
      at ../nptl/sysdeps/unix/sysv/linux/raise.c:64
      #1  0x00007ffff126d5c0 in *__GI_abort () at abort.c:92
      #2  0x00007ffff484633a in g_logv () from /lib/libglib-2.0.so.0
      #3  0x00007ffff48463d3 in g_log () from /lib/libglib-2.0.so.0
      #4  0x00007ffff65c2359 in Glib::exception_handlers_invoke() ()
      from /usr/lib/libglibmm-2.4.so.1
      #5  0x00007ffff7abc886 in Gtk::Widget_Class::button_press_event_callback (
      self=0x85f9f0, p0=0x2964430) at widget.cc:4658
      #6  0x00007ffff6924178 in ?? () from /usr/lib/libgtk-x11-2.0.so.0
      #7  0x00007ffff50f95de in g_closure_invoke () from /usr/lib/libgobject-2.0.so.0
      #8  0x00007ffff510d1dd in ?? () from /usr/lib/libgobject-2.0.so.0
      #9  0x00007ffff510e8b9 in g_signal_emit_valist ()
      from /usr/lib/libgobject-2.0.so.0
      #10 0x00007ffff510f033 in g_signal_emit () from /usr/lib/libgobject-2.0.so.0
      #11 0x00007ffff6a3b0bf in ?? () from /usr/lib/libgtk-x11-2.0.so.0
      #12 0x00007ffff691c643 in gtk_propagate_event ()
      from /usr/lib/libgtk-x11-2.0.so.0
      #13 0x00007ffff691d71b in gtk_main_do_event ()
      from /usr/lib/libgtk-x11-2.0.so.0
      #14 0x00007ffff633886c in ?? () from /usr/lib/libgdk-x11-2.0.so.0
      #15 0x00007ffff483b8c2 in g_main_context_dispatch () from /lib/libglib-2.0.so.0
      #16 0x00007ffff483f748 in ?? () from /lib/libglib-2.0.so.0
      ---Type <return> to continue, or q <return> to quit---
      #17 0x00007ffff483fc55 in g_main_loop_run () from /lib/libglib-2.0.so.0
      #18 0x00007ffff691dbb7 in gtk_main () from /usr/lib/libgtk-x11-2.0.so.0
      #19 0x00007ffff7a4699b in Gtk::Main::run (window=<value optimised out>)
      at main.cc:490
      #20 0x0000000000439f82 in main (argc=1, argv=0x7fffffffe278) at main.cc:50
    */
    //have to apply at the callback level
    ErrorDialog<std::exception> ewindow(e, true);
  }
  return 0;

}


