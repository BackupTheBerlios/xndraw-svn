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

#include "guiutils.h"
#include "xndrawexceptions.h"

namespace xNDRAW{
  using namespace NCOORDS;

  std::pair<unsigned int, unsigned int> 
  DisplayInfo::monitor_size_for_window(Glib::RefPtr<Gdk::Window> window_on_monitor){
    Glib::RefPtr<Gdk::DisplayManager> displayman= Gdk::DisplayManager::get();
    if(!displayman){throw xn_Error("no display");}
    Glib::RefPtr< Gdk::Display > maindisplay(displayman->get_default_display());
    if(!maindisplay){throw xn_Error("no display");}
    //assume default display and  screen
    Glib::RefPtr<Gdk::Screen> screen =maindisplay->get_default_screen();
    if(!screen){throw xn_Error("no screen");}
    //unsigned int n_monitors = screen->get_n_monitors();
    int monitor_no= screen->get_monitor_at_window(window_on_monitor);
  
    Gdk::Rectangle monitor;
    screen->get_monitor_geometry (monitor_no, monitor);
    
    unsigned int framewidth=monitor.get_width();
    unsigned int frameheight = monitor.get_height();
    return uint_pair_type(framewidth, frameheight);
  }

  std::pair<unsigned int, unsigned int>
  DisplayInfo::monitor_size(int display_no, 
			    unsigned int screen_no ,
			    unsigned int monitor_no){
    Glib::RefPtr<Gdk::DisplayManager> displayman= Gdk::DisplayManager::get();
    Glib::RefPtr< Gdk::Display > maindisplay(NULL);
    if(display_no<0){
      std::cerr<<"using default display"<<std::endl;
      maindisplay = displayman->get_default_display();
    }
    else{
      std::vector< Glib::RefPtr< Gdk::Display > > displays= displayman->list_displays ();
      std::cerr<<"display count: "<< displays.size()<<std::endl;
      unsigned int displaycnt=displays.size();
      if (displaycnt >1){
	std::cerr<<"multiple displays"<<std::endl;
      }
      if ((unsigned int)display_no < displaycnt){
	maindisplay = displays[display_no];
      }
      else{
	std::ostringstream ostmp;
	ostmp <<"request for display number " << display_no<<" out of range:\n"
	      <<displays.size() << "displays only(count from zero)";
	throw xn_Error(ostmp.str());
      }
    }
    Glib::RefPtr<Gdk::Screen> screen ( NULL);
    unsigned int nscreens =maindisplay->get_n_screens();
    std::cerr<<"maindisplay->get_n_screens() : "<<nscreens<<std::endl;
    if(screen_no <nscreens){
      ///////
      //screen = Gdk::Screen::get_default(); //-which goes crazy with a multihead 'display'
      screen = maindisplay->get_screen(screen_no);
      std::cerr<<"got screen "<<screen_no<<std::endl;
#if DEBUG_DISPLAY
      std::cerr <<"\n\nusing screen "<< screen_no  <<" on display "<< display_no
		<<"!!!!!!!!!!!!!!!!!!!!!!!"<<std::endl;
#endif
    }
    else{
      std::ostringstream ostmp;
      ostmp <<"request for screen number " << screen_no<<" out of range:\n"
	    <<nscreens << " screens only opn display "<<display_no
	    <<" (count from zero)";
      throw xn_Error(ostmp.str());
  
    }
    unsigned int n_monitors= screen->get_n_monitors();
    std::cerr<<"n_monitors : "<< n_monitors<<std::endl;
    int framewidth,frameheight;
    if(monitor_no >=  n_monitors){
      throw xn_Error("out of range");
    }
    if ( n_monitors ==1){
      framewidth = screen->get_width();
      frameheight = screen->get_height();
    }
    else if( n_monitors >1){
      Gdk::Rectangle monitor;
      screen->get_monitor_geometry (monitor_no, monitor);
      //doesn't help monitor bouncing effect on li nux
      framewidth=monitor.get_width();
      frameheight = monitor.get_height();
    }
    else{
      throw std::runtime_error("no monitor??");
    }
    if(framewidth <0 || frameheight <0){
      throw xn_Error("something weird happened");
    }
    std::cerr<<"framewidth :"<<framewidth <<"\n frameheight : "<<frameheight
	     <<std::endl;
    return uint_pair_type(framewidth, frameheight);
  }

  size_t ButtonFrame::boxsize(size_t whichbox){
    if(whichbox >= num_columns){
      throw xn_Error("index outside range");
    }
    const Gtk::VButtonBox* box= buttonboxes[whichbox];
    return box->children().size();
  }

  void ButtonFrame::add_button(Gtk::Button* button){
    Gtk::ButtonBox* box = get_emptiest();
    box->pack_start(*button,Gtk::PACK_SHRINK);
  }

  Gtk::ButtonBox* ButtonFrame::get_emptiest(){
    if(num_columns==1){return buttonboxes[0];}
    size_t largest = (buttonboxes[0])->children().size();
    for(size_t i=1;i< num_columns;i++){
      if((buttonboxes[i])->children().size() < largest){
	return buttonboxes[i];
      }
    }
    return buttonboxes[0];
  }
  ButtonFrame::ButtonFrame(size_t numcols ): num_columns(numcols){ 
    add_boxes();
    show_all_children();
  }

  void ButtonFrame::add_boxes(){
    for(size_t i=0;i < num_columns;i++){
      Gtk::VButtonBox* vbox = Gtk::manage(new Gtk::VButtonBox());
      pack_start(*vbox, Gtk::PACK_SHRINK);
      buttonboxes[i]=vbox;
    }
  }

  void ButtonFrame::add_buttons(const std::map<std::string, sigc::slot<void> >& slots){
    //size_t count=0;
    //size_t sz=slots.size();
    for(const_slotmap_iterator w = slots.begin();w==slots.end();w++){
      //	size_t boxnum = (w-slots.begin())% num_columns;
      Gtk::Button* button = Gtk::manage(new Gtk::Button((*w).first));
      button->signal_clicked().connect((*w).second);
      Gtk::ButtonBox* box = get_emptiest();
      box->pack_start(*button,Gtk::PACK_SHRINK);
      button->show();
    }
  }

  ButtonFrame::ButtonFrame(const std::map<std::string, sigc::slot<void> >& slots,
			   size_t numcols ):
    num_columns(numcols){ 
    add_boxes();
    add_buttons(slots);
    show_all_children();
  }

GuiElement::GuiElement(Glib::RefPtr<Gtk::Widget> widget,
	       bool indep):
      packwidget(widget),
      independent(indep){}

  void GuiElement::hide(){
      if(!packwidget){
	throw xn_Error("hide(): packwidget NULL");
      }
      packwidget->hide();
    }

  void GuiElement::show(){
      if(!packwidget){
	throw xn_Error("show(): packwidget NULL");
      }
      packwidget->show();
    }

bool GuiElement::is_pattern_independent()const{
      return independent;
    }

XndrawPatternType GuiElement::get_patterntype()const{
      if(independent){
	return XNDRAW_PATTERN_NONE;
      }
      return XNDRAW_PATTERN_TYPEERROR;
    }

  PatternGui::PatternGui(Glib::RefPtr<Gtk::Widget> widget,
	       XndrawPatternType t_pattern):
      GuiElement(widget,false),
      patterntype(t_pattern){}

XndrawPatternType PatternGui::get_patterntype()const{
      return (XndrawPatternType)patterntype;
    }


  MosaicControls::MosaicControls(RosyDraw* draww,
		   bool horiz):
      PatternGui(Glib::RefPtr<Gtk::Widget>(NULL), XNDRAW_PATTERN_MOSAIC),
      draw(draww),
      ringratio_adjust(XNDRAW_GOLDEN_MEAN_INVERSE,0.01, 2, 0.01),
      ringoffsetangle_adjust(XNDRAW_GOLDEN_MEAN,0.01, 2*M_PI, 0.01),
      colourcount_adjust(3,2,60,1)
    {
      if (horiz){
	m_box = Gtk::manage(new Gtk::HBox());
      }
      else{m_box = Gtk::manage(new Gtk::VBox);}
      packwidget=Glib::RefPtr<Gtk::Widget>(m_box);
      m_box->pack_start(ringratio);
      m_box->pack_start(ringoffsetangle);
    }

int MosaicControls::get_colourcount(const NCOORDS::npatternrosette& gen)const{
      return gen.get_colourcount();
    }

    void MosaicControls::on_ringratio_changed(){
      double val = ringratio_adjust.get_value();
      npatternrosette& r = draw->get_rosette();
      r.set_scale_ratio(val);
    }

void MosaicControls::on_ringoffsetangle_changed(){
      double val = ringoffsetangle_adjust.get_value();
      npatternrosette& r = draw->get_rosette();
      r.set_ringoffset_angle(val);
    }
  void XnWindowBase::on_pattern_set(XndrawPatternType ptype){
      for(std::deque<GuiElement*>::iterator w = guielements.begin();w !=guielements.end();w++){
	GuiElement* elem= *w;
	if(!elem || !(elem->get_patterntype() &  ptype)){
	  if(elem){elem->hide();/*unmap()?*/}
	  w= guielements.erase(w);
	  w--;
	}
      }
}

  XnWindowBase::XnWindowBase(Gtk::WindowType type):
          Gtk::Window(type){}

}
