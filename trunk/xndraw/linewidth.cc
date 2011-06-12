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


#include "linewidth.h"
using namespace xNDRAW;
using namespace NCOORDS;


linewidth::linewidth()
  :
  linebox(false, 5),
  // value, lower, upper, step_increment, page_increment, page_size
  // note that the page_size value only makes a difference for
  // scrollbar widgets, and the highest value you'll get is actually
  // (upper - page_size).
  lwidth_adjust(2.0, 0.0, 50.0, 0.1),
  
  w_scale(lwidth_adjust),
  alpha_adjust(1.0, 0.0, 2.0, 0.001),
  alpha_scale(alpha_adjust),
  deviceX_adjust(0.0, -501.0, 501.0, 1.0),
  deviceY_adjust(0.0, -501.0, 501.0, 1.0),
  
  r_adjust(1.0, 0.0, 2.0, .01),
 g_adjust(0.0, 0.0, 2.0, .001),
 b_adjust(0.0, 0.0, 2.0, .001),
 backr_adjust(0.0, 0.0, 2.0,0.01),
 backg_adjust(1.0, 0.0, 2.0, 0.01),
 backb_adjust(1.0, 0.0, 2.0, 0.01),
  b_alpha_adjust(1.0, 0.0, 2.0, 0.01),
  red(r_adjust), green(g_adjust), blue(b_adjust), b_red(backr_adjust), b_green(backg_adjust), b_blue(backb_adjust), alpha(alpha_adjust), back_alpha(b_alpha_adjust)
{
  std::cout << "linewidth constructor" << std::endl;
  add(linebox);
  
  w_scale.set_update_policy(Gtk::UPDATE_IF_VALID );
  w_scale.set_digits(2);
  //w_scale.set_value_pos(Gtk::POS_TOP);
  //w_scale.set_draw_value();
  //w_scale.set_size_request(140, 30);

alpha_scale.set_update_policy(Gtk::UPDATE_IF_VALID );
  alpha_scale.set_digits(2);
  //alpha_scale.set_value_pos(Gtk::POS_TOP);
  //alpha_scale.set_draw_value();
  //alpha_scale.set_size_request(140, 20);

back_alpha.set_update_policy(Gtk::UPDATE_IF_VALID );
  back_alpha.set_digits(2);
  Gtk::Box* widthandtrannybox= Gtk::manage(new Gtk::HBox());
Gtk::Box*widthbox= Gtk::manage(new Gtk::VBox());
 Gtk::Box*trannybox= Gtk::manage(new Gtk::VBox());
 linebox.pack_start(*widthandtrannybox);
widthandtrannybox->pack_start(*widthbox);
widthandtrannybox->pack_start(*trannybox);
 widthbox->pack_start(*Gtk::manage(new Gtk::Label("linewidth", 0)),  Gtk::PACK_SHRINK);

  widthbox->pack_start(w_scale, Gtk::PACK_SHRINK);
  trannybox->pack_start(*Gtk::manage(new Gtk::Label("transparency", 0)),  Gtk::PACK_SHRINK);
  trannybox->pack_start(alpha_scale,Gtk::PACK_SHRINK);
   
  //linebox.pack_start( colour_box); // urgh, and unnecessary
 colour_box.pack_start(*(Gtk::manage(new Gtk::Label("pick some colours...", false))));

 colour_box.pack_start(colours);
 colour_box.pack_start(*(Gtk::manage(new Gtk::Label("remember to connect the signals...underneath.. transparency", false))));
 colour_box.pack_start( buttonarea);
 buttonarea.pack_start(front_button);
 buttonarea.pack_start(back_button);
 buttonarea.pack_start(alpha_box);
 alpha_box.pack_start(alpha);
 alpha_box.pack_start(back_alpha);
 colours.pack_start(red_box);
colours.pack_start(green_box);
colours.pack_start(blue_box);
 colours.pack_start(previews_b4);
colours.pack_start(previews_after);
 red_box.pack_start(*(Gtk::manage(new Gtk::Label("red",false))));
 red_box.pack_start(red);
red_box.pack_start(b_red);
red_box.pack_start(*(Gtk::manage(new Gtk::Label("(ba\nck)",false))));

green_box.pack_start(*(Gtk::manage(new Gtk::Label("green",false))));
 green_box.pack_start(green);
green_box.pack_start(b_green);
green_box.pack_start(*(Gtk::manage(new Gtk::Label("(fo\nre)",false))));

//m_DrawingArea.modify_bg(Gtk::STATE_NORMAL, m_Color);

blue_box.pack_start(*(Gtk::manage(new Gtk::Label("blue",false))));
 blue_box.pack_start(blue);
blue_box.pack_start(b_blue);
blue_box.pack_start(*(Gtk::manage(new Gtk::Label("(fu\nck)",false))));

 fore.set_red(65535);
  fore.set_blue(0);
  fore.set_green(0);

  aft.set_red(0);
  aft.set_blue(65535);
  aft.set_green(65535);
fore_new.set_red(65535);
fore_new.set_blue(0);
fore_new.set_green(0);
aft_new.set_red(0);
 aft_new.set_blue(65535);
aft_new.set_green(65535);
 previews_b4.pack_start(*(Gtk::manage(new Gtk::Label("B4",false))));
  previews_b4.pack_start(front);
  previews_b4.pack_start(back);
  front.modify_bg(Gtk::STATE_NORMAL, fore);
  back.modify_bg(Gtk::STATE_NORMAL, aft);


previews_after.pack_start(*(Gtk::manage(new Gtk::Label("new",false))));
 previews_after.pack_start(new_front);
 previews_after.pack_start(new_back);
 new_front.modify_bg(Gtk::STATE_NORMAL,fore_new);
 new_back.modify_bg(Gtk::STATE_NORMAL,aft_new);

  w_scale.signal_value_changed().connect( sigc::mem_fun(*this, &linewidth::on_slide_linewidth));
alpha_scale.signal_value_changed().connect( sigc::mem_fun(*this, &linewidth::on_slide_alpha));
 

alpha.signal_value_changed().connect( sigc::mem_fun(*this, &linewidth::on_slide_alpha));

back_alpha.signal_value_changed().connect( sigc::mem_fun(*this, &linewidth::on_slide_back_alpha));


 show_all_children();
}

linewidth::~linewidth(){} 

void linewidth::on_slide_linewidth(){
  //export value to ping_window and Drawing
  //float l_wide = adjust.get_value();
  std::cout <<   lwidth_adjust.get_value() <<  " = adjust.get_value()" << std::endl;
  signal_linewidth().emit(lwidth_adjust.get_value());
}

void linewidth::on_slide_alpha(){
std::cout <<   alpha_adjust.get_value() <<  " = alpha value emitted" << std::endl;
 signal_alpha().emit(alpha_adjust.get_value());
}

void linewidth::on_slide_back_alpha(){
std::cout <<   alpha_adjust.get_value() <<  " = alpha value emitted" << std::endl;
 signal_back_alpha().emit(b_alpha_adjust.get_value());
}
//m_DrawingArea.modify_bg(Gtk::STATE_NORMAL, m_Color);
void linewidth::on_slide_red(){
  fore.set_red(gushort((double)r_adjust.get_value()*65536.0));
double d = alpha_adjust.get_value();
//signal_rgba().emit(fore,d);
 Draw_colour col = *(new Draw_colour(fore.get_red()/65536.0, fore.get_green()/65536.0, fore.get_blue()/65536.0, d));
signal_forecolour().emit(col);
front.modify_bg(Gtk::STATE_NORMAL,fore);
}
void linewidth::on_slide_green(){
  fore.set_green(gushort((double)g_adjust.get_value()*65536.0));
  double d = alpha_adjust.get_value();
  signal_rgba().emit(fore,d);
 Draw_colour col = *(new Draw_colour(fore.get_red()/65536.0, fore.get_green()/65536.0, fore.get_blue()/65536.0, d));
  signal_forecolour().emit(col);
front.modify_bg(Gtk::STATE_NORMAL,fore);
}

void linewidth::on_slide_blue(){
  fore.set_blue(gushort((double)b_adjust.get_value()*65536.0));
double d = alpha_adjust.get_value();
  signal_rgba().emit(fore,d);
Draw_colour col = *(new Draw_colour(fore.get_red()/65536.0, fore.get_green()/65536.0, fore.get_blue()/65536.0, d));
 signal_forecolour().emit(col);
front.modify_bg(Gtk::STATE_NORMAL,fore);
}

void linewidth::on_slide_backred(){
  aft.set_red(gushort((double)backr_adjust.get_value()*65536.0));
double d = b_alpha_adjust.get_value();
  signal_back_rgba().emit(aft,d);
Draw_colour col = *(new Draw_colour(aft.get_red()/65536.0, aft.get_green()/65536.0, aft.get_blue()/65536.0, d));
signal_backcolour().emit(col);
back.modify_bg(Gtk::STATE_NORMAL,aft);
}
void linewidth::on_slide_backgreen(){
  aft.set_green(gushort((double)backg_adjust.get_value()*65536.0));
double d = b_alpha_adjust.get_value();
 signal_back_rgba().emit(aft,d);
Draw_colour col = *(new Draw_colour(aft.get_red()/65536.0, aft.get_green()/65536.0, aft.get_blue()/65536.0, d));
signal_backcolour().emit(col);
back.modify_bg(Gtk::STATE_NORMAL,aft);
}

void linewidth::on_slide_backblue(){
  aft.set_blue(gushort((double)backb_adjust.get_value()*65536.0));
double d = b_alpha_adjust.get_value();
 signal_back_rgba().emit(aft,d);
Draw_colour col = *(new Draw_colour(aft.get_red()/65536.0, aft.get_green()/65536.0, aft.get_blue()/65536.0, d));
signal_backcolour().emit(col);
back.modify_bg(Gtk::STATE_NORMAL,aft);
}




void linewidth::on_transpose_x(){
  double dx = deviceX_adjust.get_value();
  signal_deviceX().emit(dx);
}

void linewidth::on_transpose_y(){
 double dy = deviceY_adjust.get_value();

 signal_deviceY().emit( dy);
}

linewidth::linewide_signal linewidth::signal_linewidth(){
  return m_linewide_signal;
}
linewidth::alpha_signal linewidth::signal_alpha(){
  return m_alpha_signal;
}

linewidth::deviceX_signal linewidth::signal_deviceX(){
  return m_deviceX_signal;
}

linewidth::deviceY_signal linewidth::signal_deviceY(){
  return m_deviceY_signal;
}
