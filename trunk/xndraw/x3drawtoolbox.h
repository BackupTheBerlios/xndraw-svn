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


#ifndef X3DRAW_X3DRAWTOOLBOX_H
#define  X3DRAW_X3DRAWTOOLBOX_H
#include "xndrawexceptions.h"

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <gtkmm.h>
#include <iostream>
#include "drawing.h"
#include "guiutils.h"
#include "colourchooser.h"
#include "imageview.h"

namespace xNDRAW{

  class X3drawtoolbox: public XnWindowBase{
  public:
    X3drawtoolbox(std::string window_title);
    virtual ~X3drawtoolbox();

  protected:
    
    typedef sigc::signal<void, std::string> filename_signal;
    typedef sigc::signal<void, Cairo::Operator> compositing_signal;
    typedef sigc::signal<void, Cairo::FillRule> fill_rule_signal;
    typedef sigc::signal<void, Cairo::LineJoin> line_join_signal;
    typedef sigc::signal<void, Cairo::LineCap> line_cap_signal;
    typedef sigc::signal<void, Draw_colour> rgba_signal;
    
  public:
    rgba_signal m_signal_rgba;
    rgba_signal m_signal_back_rgba;
    filename_signal signal_filename();
    compositing_signal signal_operator();
    compositing_signal signal_operator_visual(){
      return m_signal_compositing_visual;
    }
    compositing_signal signal_operator_buffer(){
      return m_signal_compositing_buffer;
    }
    fill_rule_signal signal_fill_rule();
    line_join_signal signal_line_join();
    line_cap_signal signal_line_cap();
    
    rgba_signal signal_rgba(){return m_signal_rgba;}
    rgba_signal signal_back_rgba(){return m_signal_back_rgba;}
  protected:
    filename_signal m_signal_filename;
    fill_rule_signal m_signal_fill_rule;
    compositing_signal m_signal_compositing;
    compositing_signal m_signal_compositing_buffer;
    compositing_signal m_signal_compositing_visual;
    line_join_signal m_signal_line_join;
    line_cap_signal  m_signal_line_cap;
    //Signal handlers
  public:
    void on_button_CLEAR();
    void on_button_OVER();
    void on_button_SOURCE();
    void on_button_IN();
    void on_button_OUT();
    void on_button_ATOP();
    void on_button_DEST();
    void on_button_DEST_OVER();
    void on_button_DEST_IN();
    void on_button_DEST_OUT();
    void on_button_DEST_ATOP();
    void on_button_XOR();
    void on_button_ADD();
    void on_button_SATURATE();
    //////////
    //set some more operators...
    void on_button_buffer_CLEAR();
    void on_button_buffer_OVER();
    void on_button_buffer_SOURCE();
    void on_button_buffer_IN();
    void on_button_buffer_OUT();
    void on_button_buffer_ATOP();
    void on_button_buffer_DEST();
    void on_button_buffer_DEST_OVER();
    void on_button_buffer_DEST_IN();
    void on_button_buffer_DEST_OUT();
    void on_button_buffer_DEST_ATOP();
    void on_button_buffer_XOR();
    void on_button_buffer_ADD();
    void on_button_buffer_SATURATE();
    ///////////
    void on_button_visual_CLEAR();
    void on_button_visual_OVER();
    void on_button_visual_SOURCE();
    void on_button_visual_IN();
    void on_button_visual_OUT();
    void on_button_visual_ATOP();
    void on_button_visual_DEST();
    void on_button_visual_DEST_OVER();
    void on_button_visual_DEST_IN();
    void on_button_visual_DEST_OUT();
    void on_button_visual_DEST_ATOP();
    void on_button_visual_XOR();
    void on_button_visual_ADD();
    void on_button_visual_SATURATE();


    ////////////    
    void on_button_MITER();
    void on_button_ROUND();
    void on_button_BEVEL();
    
    void on_button_EVEN_ODD();
    void on_button_WINDING();
    
    void on_button_BUTT();
    void on_button_ROUNDY();
    void on_button_SQUARE();

   
    
    //Child widgets:
    Gtk::HBox biggerbox;
    Gtk::VBox bigbox;
    colour_chooser choos;
    Gtk::RadioButton CLEAR, SOURCE, OVER, IN, OUT, ATOP, DEST, DEST_OVER, DEST_IN, DEST_OUT, DEST_ATOP, XOR, ADD, SATURATE;
    Gtk::RadioButton CLEAR_visual, SOURCE_visual, OVER_visual, IN_visual, OUT_visual, ATOP_visual, DEST_visual, DEST_OVER_visual, DEST_IN_visual, DEST_OUT_visual, DEST_ATOP_visual, XOR_visual, ADD_visual, SATURATE_visual;
    Gtk::RadioButton CLEAR_buffer, SOURCE_buffer, OVER_buffer, IN_buffer, OUT_buffer, ATOP_buffer, DEST_buffer, DEST_OVER_buffer, DEST_IN_buffer, DEST_OUT_buffer, DEST_ATOP_buffer, XOR_buffer, ADD_buffer, SATURATE_buffer;
    Gtk::VBox join_box;
    Gtk::RadioButton MITER, ROUND,BEVEL; 
    Gtk::VBox fill_box;
    Gtk::RadioButton WINDING, EVEN_ODD;
    Gtk::VBox cap_box;
    Gtk::RadioButton BUTT,ROUNDY,SQUARE; 
    
    Gtk::VButtonBox m_ButtonBox, op_ButtonBox, op_ButtonBox_visual,
      op_ButtonBox_buffer;
    //Gtk::Entry filename_entry;

    void connect_internal_signals();
    colour_chooser& get_choos(){
      return choos;
    }
  };
  
  class PointerToolBox:public Gtk::Frame{
  public:
    typedef std::deque<IconButton*> button_container;
    typedef std::deque<IconButton*>::iterator button_iterator;
    typedef std::deque<IconButton*>::const_iterator const_button_iterator;
    typedef std::pair<std::string, sigc::slot0<void> > button_info_type;
    
  protected:
    std::deque<IconButton*> buttons;
    Gtk::ButtonBox* bbox;
  public:
    PointerToolBox(bool hbox=true);

    PointerToolBox(const XndrawMap<int, std::string>& buttoninfo, 
		   bool hbox=true);
   
 PointerToolBox(const XndrawMap<int,  button_info_type >& buttoninfo, 
		bool hbox=true);

    void create_bbox(bool hbox=true);

    IconButton* get_button(int id)const;

    template <typename MAP_TYPE>
    void set_slots(const MAP_TYPE& intslotmap){
      typedef typename MAP_TYPE::const_iterator const_map_iterator;
      for(const_map_iterator iter = intslotmap.begin();
	  iter !=intslotmap.end();iter++){
    
	IconButton* button = get_button((*iter).first);
	if(button){
	  button->set_slot((*iter).second);
	}
	else{
	  std::cerr << "slot not set - no button with ID = "<< (*iter).first
		    << "\nbutton's click signal may be inactive"<< std::endl;
#ifdef DEBUG
	  throw XNGuiError("slot set for nonexistrent button");
#endif
      
	}
      }
    }
  
    virtual ~PointerToolBox(){
      //buttons deleted by gtk
    }
  };

  class FloatingToolbox: public XnWindowBase{
  public:
    typedef PointerToolBox::button_info_type button_info_type;
  protected:
    PointerToolBox tools;
public:
    //no slots connected in this ctor
    FloatingToolbox(const XndrawMap<int, std::string>& buttoninfo, 
		    bool horizontal=false);
    //buttons with slots setup
 FloatingToolbox(const XndrawMap<int,  button_info_type >& buttoninfo, 
		 bool horizontal=false);
  protected:
    void setup();
  public:
    template <typename MAP_TYPE>
    void set_slots(const MAP_TYPE& intslotmap){
      tools.set_slots(intslotmap);
    }

  };

}//namespace xNDRAW
#endif /* X3DRAW_X3DRAWTOOLBOX_H*/
