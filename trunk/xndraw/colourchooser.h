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


#ifndef X3DRAW_COLOURCHOOSER_H
#define X3DRAW_COLOURCHOOSER_H
#include "xndrawexceptions.h"
#if HAVE_CONFIG_H
#include "config.h"
#endif


#include <gtkmm.h>
#include "pngsave_window.h"
#include "drawing.h"

namespace xNDRAW{

  class GradientButton: public Gtk::ColorButton{
  public:

  protected:
    Gtk::HBox hbox;
    Gtk::ColorButton bottom;
    virtual ~GradientButton(){}
  };

  class colour_chooser : public Gtk::Frame{
  public:
    typedef sigc::signal<void, float,float,float>  colour_signal;
    typedef pngsave_window::filename_signal filename_signal;
    typedef sigc::signal<void, const std::string&, ColourRole> paint_filename_signal;
    typedef sigc::signal<void,const Draw_colour&, ColourRole > draw_colour_signal;
    typedef sigc::signal<void,const PatternColour&, ColourRole> draw_pattern_signal;
    typedef sigc::slot<void,const Draw_colour&, ColourRole > rgba_slot_type;
    typedef sigc::slot<void,const PatternColour&, ColourRole> pattern_slot_type;
    typedef void (colour_chooser::*void_handler_type)(void);

public:
    colour_chooser(Gtk::Window& dialogtransient);
        virtual ~colour_chooser();
    
    //colour_signal signal_colour();
  protected:
    //Signal handlers:
    virtual void on_stroke_rgb_set();
    virtual void on_fill_rgb_set();
    virtual void on_bg_rgb_set();
  public:
    void signal_stroke_rgb_emit(){
      m_Button_stroke_rgb.clicked();
    }//click its button
void signal_fill_rgb_emit(){
  m_Button_fill_rgb.clicked();
}//click its button
void signal_bg_rgb_emit(){m_Button_bg_rgb.clicked();}
 void connect_proxy(Gtk::Button& button, void_handler_type handler){
   button.signal_clicked().connect(sigc::mem_fun(*this, handler));
 } 

  public:
void connect_proxy(Gtk::ColorButton& button, void_handler_type handler){
  //button.signal_clicked().disconnect();
   button.signal_clicked().connect(sigc::mem_fun(*this, handler));
 }

    virtual void on_stroke_png_set();
 virtual void on_fill_png_set();
 virtual void on_bg_png_set();


 virtual void on_stroke_gradient_set();
 virtual void on_fill_gradient_set();
 virtual void on_bg_gradient_set();

 void on_png_filename_role(const std::string& filename, ColourRole role);
  protected:
    Gtk::Window& dialog_transient;//not necessarily the parent window
    //Child widgets:
    Gtk::VBox m_VBox;
    //TODO: rather than pack all these buttons, just show the ones
    //currently appropriate. 
    //left click edits the current ColourSource,
    //right click opensd a generic set PatternColour dialog
    Gtk::ColorButton m_Button_stroke_rgb;
    Gtk::ColorButton m_Button_fill_rgb;
    Gtk::ColorButton m_Button_bg_rgb;
    Gtk::Button m_Button_stroke_cairopattern;
    Gtk::Button m_Button_fill_cairopattern;
Gtk::Button m_Button_bg_cairopattern;
 Gtk::Button m_Button_stroke_png;
 Gtk::Button m_Button_fill_png;
 Gtk::Button m_Button_bg_png;
    //Gtk::Label m_Label_stroke_cairopattern;
//Gtk::Label m_Label_fill_cairopattern;
    //Gtk::Label m_Label_bg_cairopattern;
    Gtk::Label m_Label_stroke_rgb;
    Gtk::Label m_Label_fill_rgb;
    Gtk::Label m_Label_bg_rgb;
    Gtk::DrawingArea m_DrawingArea; //To show the color.
    Gdk::Color m_Color_stroke_rgb;
    Gdk::Color m_Color_fill_rgb;
    Gdk::Color m_Color_bg_rgb;
  
    paint_filename_signal m_signal_pngfilename;
    draw_colour_signal m_signal_rgba;
    draw_pattern_signal m_signal_pattern;
    //colour_signal m_signal_colour;//remove
    sigc::connection rgba_connection;
    sigc::connection pattern_connection;

    std::string pngfolder;
  public:
    void open_png(filename_signal emitter);
    void open_png(paint_filename_signal emitter, ColourRole role);
    void connect_pattern_signal(pattern_slot_type slot);
    void connect_rgba_signal(rgba_slot_type slot);

   
  
 const Gdk::Color& get_stroke_rgbcolor()const {return   m_Color_stroke_rgb;}
 
const Gdk::Color& get_fill_rgbcolor()const {return  m_Color_fill_rgb; }

const Gdk::Color& get_bg_rgbcolor()const {return  m_Color_bg_rgb; }

    const std::string&    get_pngfolder()const { return pngfolder;}
std::string&    get_pngfolder(){ return pngfolder;}
  };
  Draw_colour gdkColor_to_Draw_colour(const Gdk::Color& col);
 



}//namespace xNDRAW
#endif /*X3DRAW_COLOURCHOOSER_H*/
