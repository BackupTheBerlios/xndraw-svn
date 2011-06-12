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

#ifndef X3DRAW_LINEWIDTH_H
#define  X3DRAW_LINEWIDTH_H
#include "xndrawexceptions.h"

#if HAVE_CONFIG_H
#include "config.h"
#endif


#include <gtkmm.h>
#include <iostream>

#include <gtkmm/scale.h>

#include "x3draw.h"
#include "xndrawdata.h"
#include "drawcolour.h"

namespace xNDRAW{

  class linewidth: public Gtk::Frame{
  public:

    //Gtk::Label line_width_label;
 
    Gtk::VBox linebox;
    Gtk::Adjustment lwidth_adjust;
    //Gtk::HScale w_scale;//space hog
    Gtk::SpinButton w_scale;
    Gtk::Adjustment alpha_adjust;
    //Gtk::HScale alpha_scale
    Gtk::SpinButton alpha_scale;
    Gtk::Adjustment deviceX_adjust;
    Gtk::Adjustment deviceY_adjust;
    
    Gtk::VBox colour_box;
    Gtk::HBox buttonarea, colours, alpha_box;
    Gtk::Button front_button, back_button;
    Gtk::VBox red_box, green_box, blue_box, previews_b4, previews_after;;
    Gtk::DrawingArea front, back, new_front, new_back;
    Gtk::Adjustment r_adjust, g_adjust, b_adjust;
    Gtk::Adjustment backr_adjust, backg_adjust, backb_adjust, b_alpha_adjust;
    Gtk::SpinButton red, green, blue, b_red, b_green, b_blue, alpha, back_alpha;
    Gdk::Color fore, aft, fore_new, aft_new;
    typedef sigc::signal<void, Gdk::Color&> colour_signal;
    colour_signal m_signal_colour;
    colour_signal signal_colour(){return m_signal_colour;}
    //typedef sigc::signal<void, Gdk::Color> rgb_signal;
    //rgb_signal m_signal_rgb;

    //rgb_signal signal_rgb(){return m_signal_rgb;}
    typedef sigc::signal<void, Gdk::Color, float> rgba_signal;
    rgba_signal m_signal_rgba;
    rgba_signal signal_rgba(){return m_signal_rgba;}

  
    rgba_signal m_signal_back_rgba;
    rgba_signal signal_back_rgba(){return m_signal_back_rgba;}

    typedef sigc::signal<void, Draw_colour> drawcolour_signal;
    drawcolour_signal m_signal_forecolour;
drawcolour_signal m_signal_backcolour;
 drawcolour_signal signal_forecolour(){
return  m_signal_forecolour;
 }
   drawcolour_signal signal_backcolour(){
return  m_signal_backcolour;
   }

    void on_slide_red();
    void on_slide_green();
    void on_slide_blue();
    void on_slide_backred();
    void on_slide_backgreen();
    void on_slide_backblue();
    void on_slide_back_alpha();
  public:
    linewidth();
    virtual ~linewidth();

    void on_slide_linewidth();
    void on_slide_alpha();
    void on_transpose_x();
    void on_transpose_y();
  protected:
    typedef sigc::signal<void, float> linewide_signal;
    linewide_signal m_linewide_signal;
    typedef sigc::signal<void, float> alpha_signal;
    alpha_signal m_alpha_signal;
    typedef sigc::signal<void, float> back_alpha_signal;
    back_alpha_signal m_signal_back_alpha;
    typedef sigc::signal<int, double> deviceX_signal;
    deviceX_signal m_deviceX_signal;
    typedef sigc::signal<int, double> deviceY_signal;
    deviceY_signal m_deviceY_signal;
    
  public: 

    


    linewide_signal signal_linewidth();
    alpha_signal signal_alpha();
    back_alpha_signal signal_back_alpha(){
      return m_signal_back_alpha;
    }
    deviceX_signal signal_deviceX();
    deviceY_signal signal_deviceY();
 
};
}

#endif /*X3DRAW_LINEWIDTH_H*/
