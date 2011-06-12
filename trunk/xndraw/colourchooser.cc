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


#include <iostream>
//#include "ping_window.h"
#include "colourchooser.h"

//using namespace X3_DRAW;
namespace xNDRAW{
  using namespace NCOORDS;

  colour_chooser::colour_chooser(Gtk::Window& transient):
    dialog_transient(transient),
    m_Label_stroke_rgb("stroke"),
    m_Label_fill_rgb("fill"),
    m_Label_bg_rgb("background"),
    /*bbut*/m_Button_stroke_cairopattern("stroke-png"),
    m_Button_fill_cairopattern("fill-png"),
    m_Button_bg_cairopattern("background-png")
{

    add(m_VBox);
    m_VBox.pack_start(m_Button_stroke_rgb, Gtk::PACK_SHRINK);
    m_VBox.pack_start(m_Label_stroke_rgb, Gtk::PACK_SHRINK);
    m_VBox.pack_start(m_Button_fill_rgb, Gtk::PACK_SHRINK);
    m_VBox.pack_start(m_Label_fill_rgb, Gtk::PACK_SHRINK);
    m_VBox.pack_start(m_Button_bg_rgb, Gtk::PACK_SHRINK);
    m_VBox.pack_start(m_Label_bg_rgb, Gtk::PACK_SHRINK);
 m_VBox.pack_start(m_Button_stroke_png, Gtk::PACK_SHRINK);
    m_VBox.pack_start(m_Button_fill_png, Gtk::PACK_SHRINK);
    m_VBox.pack_start(m_Button_bg_png, Gtk::PACK_SHRINK);
    m_VBox.pack_start(m_Button_stroke_cairopattern, Gtk::PACK_SHRINK);
    //m_VBox.pack_start(m_Label_stroke_cairopattern, Gtk::PACK_SHRINK);
    m_VBox.pack_start(m_Button_fill_cairopattern, Gtk::PACK_SHRINK);
    //m_VBox.pack_start(m_Label_fill_cairopattern, Gtk::PACK_SHRINK);
    m_VBox.pack_start(m_Button_bg_cairopattern, Gtk::PACK_SHRINK);
    //m_VBox.pack_start(m_Label_bg_cairopattern, Gtk::PACK_SHRINK);
    m_Button_stroke_rgb.set_use_alpha(true);
    m_Button_fill_rgb.set_use_alpha(true);
    m_Button_stroke_rgb.signal_color_set().connect( sigc::mem_fun(*this, &colour_chooser::on_stroke_rgb_set) );
    m_Button_fill_rgb.signal_color_set().connect( sigc::mem_fun(*this, &colour_chooser::on_fill_rgb_set) );
    m_Button_bg_rgb.signal_color_set().connect( sigc::mem_fun(*this, &colour_chooser::on_bg_rgb_set) );
 m_Button_fill_png.signal_clicked().connect( sigc::mem_fun(*this, &colour_chooser::on_fill_png_set));
    m_Button_stroke_png.signal_clicked().connect( sigc::mem_fun(*this, &colour_chooser::on_stroke_png_set));
    m_Button_fill_png.signal_clicked().connect( sigc::mem_fun(*this, &colour_chooser::on_fill_png_set));
    m_Button_bg_png.signal_clicked().connect( sigc::mem_fun(*this, &colour_chooser::on_bg_png_set));

    m_Button_stroke_cairopattern.signal_clicked().connect( sigc::mem_fun(*this, &colour_chooser::on_stroke_gradient_set));
m_Button_fill_cairopattern.signal_clicked().connect( sigc::mem_fun(*this, &colour_chooser::on_fill_gradient_set));
m_Button_bg_cairopattern.signal_clicked().connect( sigc::mem_fun(*this, &colour_chooser::on_bg_gradient_set));

 m_signal_pngfilename.connect(sigc::mem_fun(*this, &colour_chooser::on_png_filename_role));
    
    //Set start color:
    //TODO: get start values when connecting callback,
    //a type to do this(pair<SLOT, Gdk::Color> maybe)
    m_Color_stroke_rgb.set_red(65535);
    m_Color_stroke_rgb.set_blue(0);
    m_Color_stroke_rgb.set_green(0);
    m_Button_stroke_rgb.set_color(m_Color_stroke_rgb);
    m_Color_fill_rgb.set_red(0);
    m_Color_fill_rgb.set_blue(25000);
    m_Color_fill_rgb.set_green(35000);
    m_Button_fill_rgb.set_color(m_Color_fill_rgb);
    m_Color_bg_rgb.set_red(65535);
    m_Color_bg_rgb.set_blue(65535);
    m_Color_bg_rgb.set_green(65535);
    m_Button_bg_rgb.set_color(m_Color_bg_rgb);
    m_DrawingArea.modify_bg(Gtk::STATE_NORMAL, m_Color_stroke_rgb);
  
    m_VBox.pack_start(m_DrawingArea);
  
    show_all_children();
    std::cout << "leaving colourchooser CTor" << std::endl;
  }

  colour_chooser::~colour_chooser()
  {
  }


  Draw_colour gdkColor_to_Draw_colour(const Gdk::Color& col, double alpha){
    double r=col.get_red()/65535.0;
    double g = col.get_green()/65535.0;
    double b = col.get_blue()/65535.0;
    return Draw_colour(r, g,b, alpha);
  }
  //Cairo::RefPtr<Cairo::Surface> 

  void colour_chooser::open_png(colour_chooser::filename_signal emitter){
    std::string dialoglabel ("choose a png file to create pattern");
    pngsave_window::do_filedialog(dialog_transient, dialoglabel,emitter,  
				  XNDRAW_PNG,
				  pngfolder,Gtk::FILE_CHOOSER_ACTION_OPEN);
  }
  void colour_chooser::open_png(colour_chooser::paint_filename_signal emitter,
				ColourRole role){
    std::string dialoglabel ("choose a png file to create pattern");
    pngsave_window::do_filedialog_with_arg(dialog_transient, dialoglabel,
					   emitter, role,
				  XNDRAW_PNG,
				  pngfolder,Gtk::FILE_CHOOSER_ACTION_OPEN);
  }
  void colour_chooser::connect_pattern_signal(colour_chooser::pattern_slot_type slot){
    if(!pattern_connection.empty()){
      pattern_connection.disconnect();
    }
    pattern_connection = m_signal_pattern.connect(slot);
  }

  void colour_chooser::connect_rgba_signal(colour_chooser::rgba_slot_type slot){
    if(!rgba_connection.empty()){
      rgba_connection.disconnect();
    }
    rgba_connection = m_signal_rgba.connect(slot);
  }
  void colour_chooser::on_stroke_gradient_set(){
    //FIXME
    //depending on control keys(or not), 
    //start creating linear or radial gradient-
    //open one or 2 colourchoosers in turn, 
    //ang get the necessary points(show a tooltip on the procedure)
    //then creat a patternColour and emit it and role
}
  void colour_chooser::on_fill_gradient_set(){
 //FIXME
    //depending on control keys(or not), 
    //start creating linear or radial gradient-
    //open one or 2 colourchoosers in turn, 
    //ang get the necessary points(show a tooltip on the procedure)
    //then creat a patternColour and emit it and role

}
  void colour_chooser::on_bg_gradient_set(){//FIXME
    //depending on control keys(or not), 
    //start creating linear or radial gradient-
    //open one or 2 colourchoosers in turn, 
    //ang get the necessary points(show a tooltip on the procedure)
    //then creat a patternColour and emit it and role
}
  void colour_chooser::on_stroke_rgb_set()
  {  
    //Store the chosen color, and show it:
    m_Color_stroke_rgb = m_Button_stroke_rgb.get_color();
    double alpha = m_Button_stroke_rgb.get_alpha();
    Draw_colour c= gdkColor_to_Draw_colour(m_Color_stroke_rgb, alpha);
    m_DrawingArea.modify_bg(Gtk::STATE_NORMAL, m_Color_stroke_rgb);
    m_signal_rgba.emit(c, COLOURROLE_STROKE);
  }

  void colour_chooser::on_stroke_png_set(){
    open_png(m_signal_pngfilename, COLOURROLE_STROKE);
  }
 void colour_chooser::on_fill_png_set(){
    open_png(m_signal_pngfilename, COLOURROLE_FILL);
  }
  void colour_chooser::on_bg_png_set(){
    open_png(m_signal_pngfilename, COLOURROLE_BG);
  }
  void colour_chooser::on_png_filename_role(const std::string& filename, ColourRole role){
    Cairo::RefPtr<Cairo::Surface> surface = Cairo::ImageSurface::create_from_png(filename);
    //TODO: set pattern repeat in ArbitrarySrc
    //also - alpha with pattern - how?
    PatternColour pngpattern(surface);

      m_signal_pattern.emit(pngpattern, role);
  }
 
  void colour_chooser::on_fill_rgb_set(){  
    m_Color_fill_rgb = m_Button_fill_rgb.get_color();
    double alpha = m_Button_fill_rgb.get_alpha();
    Draw_colour c= gdkColor_to_Draw_colour(m_Color_fill_rgb, alpha);
    m_signal_rgba.emit(c, COLOURROLE_FILL);
    //signal_colour_fill_rgb().emit(r,g,b);
  }
  void colour_chooser::on_bg_rgb_set(){  
    m_Color_bg_rgb = m_Button_fill_rgb.get_color();
    Draw_colour c= gdkColor_to_Draw_colour(m_Color_fill_rgb, 1.0);
    m_signal_rgba.emit(c, COLOURROLE_BG);
    //signal_colour_bg_rgb().emit(r,g,b);
  }
 


}//namespace xNDRAW
