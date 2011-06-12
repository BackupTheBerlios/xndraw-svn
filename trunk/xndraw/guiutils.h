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

#ifndef XNDRAW_GUIUTILS_H
#define XNDRAW_GUIUTILS_H
#include "exceptions.h"

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <gtkmm.h>
#include "drawingbase.h"
#include "ratioflower.h"

namespace xNDRAW{

  enum XndrawWindowType{
    XNDRAW_WINDOW_MAIN=1,
    XNDRAW_WINDOW_TOOLBOX=2,
    XNDRAW_WINDOW_COLOURS=3,
    XNDRAW_WINDOW_FMATRIX=4
  };

  struct DisplayInfo{
    typedef std::pair<unsigned int, unsigned int> uint_pair_type;
    static uint_pair_type monitor_size(int display_no=-1,// == use the default 
				       unsigned int screen_no =0,
				       unsigned int monitor_no=0);

    static uint_pair_type monitor_size_for_window(Glib::RefPtr<Gdk::Window> window_on_monitor);

  };

  template<typename E>
  class ErrorDialog: public Gtk::Window{
  public:
    typedef E error_type;
  
  protected:
    error_type the_error;
    bool rethrow;//is this sensible?
    Gtk::HBox box;
  public:
    ErrorDialog(const error_type& e,bool rethrow_):
      the_error(e),
      rethrow(rethrow_){
      add(box);
      std::ostringstream ostmp;
      ostmp<<"A fatal error occurred :\n"<<e.what();
      std::string estring= ostmp.str();
      Gtk::Label* elabel = Gtk::manage(new Gtk::Label(estring.c_str()));
      box.pack_start(*elabel);
      Gtk::Button* okbutton = Gtk::manage(new Gtk::Button("OK"));
      box.pack_start(*okbutton, Gtk::PACK_SHRINK);
      okbutton->signal_clicked().connect(sigc::mem_fun(*this, &ErrorDialog::on_dismiss));
    }
    ErrorDialog(const error_type& e, Glib::RefPtr<Gtk::Window> parent, bool rethrow_ =false):
      Gtk::Window(parent),
      the_error(e),
      rethrow(rethrow_){
      if(parent){set_transient_for(parent);}
      add(box);
      std::ostringstream ostmp;
      ostmp<<"A fatal error occurred :\n"<<e.what();
      std::string estring= ostmp.str();
      Gtk::Label* elabel = Gtk::manage(new Gtk::Label(estring.c_str()));
      box.pack_start(*elabel);
      Gtk::Button* okbutton = Gtk::manage(new Gtk::Button("OK"));
      box.pack_start(*okbutton, Gtk::PACK_SHRINK);
      okbutton->signal_clicked().connect(sigc::mem_fun(*this, ErrorDialog::on_dismiss));
    }
    virtual ~ErrorDialog(){}
    virtual void on_dismiss(){
      hide();
      if(rethrow){throw the_error;}
    }

  };

  class ButtonFrame: public Gtk::HBox{
  public:
    typedef std::map<std::string, sigc::slot<void> > slotmap_type;;
    typedef slotmap_type::const_iterator const_slotmap_iterator;
    typedef slotmap_type::iterator slotmap_iterator;
    std::map<size_t, Gtk::VButtonBox*> buttonbox_map_type;

  protected:
    const size_t num_columns;
    std::map<size_t, Gtk::VButtonBox*> buttonboxes;

  public:
    ButtonFrame(const slotmap_type& slots,
		size_t numcols =2);
    ButtonFrame(size_t numcols =2);
    void add_boxes();
    void add_button(Gtk::Button* button);
    void add_buttons(const slotmap_type& slots);
    Gtk::ButtonBox* get_emptiest();
    size_t boxsize(size_t whichbox);

  };

  //compress these classes ,no need for
  //PatternGui or ProgramGui
  //remove 'bool independent'
  //amd add the xndrawpatterntype in GuiElement instead
  class GuiElement{
  protected:
    
    Glib::RefPtr<Gtk::Widget> packwidget;
bool independent;
  public:
    GuiElement(Glib::RefPtr<Gtk::Widget> widget,
	       bool indep = true);

    virtual ~GuiElement(){}
    //is_pattern_independent()()=> whether is loaded because of a pattern
    //currently used or if pattern independent
    //(and will therefore survive change of pattern)
    void hide();

    void show();

    virtual XndrawPatternType get_patterntype()const;
    virtual bool is_pattern_independent()const;
    virtual int get_xndrawpatterntype(){return  XNDRAW_PATTERN_NONE;}//ie all
  };

  class ProgramGui:public GuiElement{
  public:
    ProgramGui(Glib::RefPtr<Gtk::Widget> widget):
      GuiElement(widget){}
    virtual ~ProgramGui(){}
    //ProgramGui(){} //auto OK
  };


  class PatternGui:public GuiElement{
    int patterntype;
  public:
    virtual ~PatternGui(){}


    PatternGui(Glib::RefPtr<Gtk::Widget> widget,
	       XndrawPatternType t_pattern);

    virtual XndrawPatternType get_patterntype()const;
  };

  class MosaicControls :public PatternGui{
  public:
    //typedef sigc::signal<void, double> controlsignal_type;
    //typedef sigc::slot<void, double> slot_type;
  protected:

    RosyDraw* draw;
    Gtk::Box* m_box;
    Gtk::Box* colourbox;
    Gtk::Adjustment ringratio_adjust;
    Gtk::Adjustment ringoffsetangle_adjust;
 Gtk::Adjustment colourcount_adjust;

    Gtk::SpinButton ringratio;
    Gtk::SpinButton ringoffsetangle;
    Gtk::SpinButton colourcount;

    //controlsignal_type ringratio_signal;
    //controlsignal_type ringoffsetangle_signal;
    //fix signature
    MosaicControls(RosyDraw* draww,
		   bool horiz =false );
    //needs callback into the patterngenerator
    //for colourcount

    //template <typename GENERATOR>
    int get_colourcount(const NCOORDS::npatternrosette& gen)const;
    virtual void on_ringratio_changed();

    virtual void on_ringoffsetangle_changed();



  };


  class XnWindowBase: public Gtk::Window{
    //base class for Windows in xndraw
  protected:
    std::deque<GuiElement*> guielements;
  public:
    virtual ~XnWindowBase(){}
    explicit XnWindowBase(Gtk::WindowType type = Gtk::WINDOW_TOPLEVEL);

    virtual void on_pattern_set(XndrawPatternType ptype);
  };

}

#endif
