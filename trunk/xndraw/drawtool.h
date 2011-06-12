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


#ifndef X3DRAW_DRAWTOOL_H
#define X3DRAW_DRAWTOOL_H
#include "xndrawexceptions.h"
#if HAVE_CONFIG_H
#include "config.h"
#endif

#include "defines.h"
//TODO - include defines.h from config.h

#include <iostream>
#include <sigc++/slot.h>
#include <sigc++/class_slot.h>
#include "xnsignals.h"
#include <queue>
#include <gtkmm.h>
#include <glibmm.h>
#include "ncoords.h"
#include "beziercoords.h"
#include "drawing.h"
//#include "click_pusher.h"

#define IS_META(E) META_press((GdkEventAny*)E)
#define IS_CONTROL(E) CONTROL_press((GdkEventAny*)E)
#define IS_SUPER(E) SUPER_press((GdkEventAny*)E)
#define IS_SHIFT(E) SHIFT_press((GdkEventAny*)E)
#define IS_HYPER(E) HYPER_press((GdkEventAny*)E)
#define HAS_MODIFIERS(E) (IS_META(E)||IS_CONTROL(E)||IS_SUPER(E)	\
			  ||IS_SHIFT(E)||IS_HYPER(E))

#define PRINT_MODIFIERS(E) if(!HAS_MODIFIERS(E)){			\
    std::cerr<<"no modifier keys"<<std::endl;}				\
  else{std::cerr<<"modifier keys:\t"<<(IS_META(E) ? "Meta\t": " ")	\
		<< (IS_CONTROL(E)? "Control\t" : " ")<< (IS_SUPER(E)? "Super\t" :" ") \
		<< (IS_SHIFT(E) ? "Shift\t" : " ") << (IS_HYPER(E) ? "Hyper": " ") \
		<<std::endl;}

#define ARBITRARY_COORDS arbitrary_coords;

namespace xNDRAW{
 


  //template<class T>
  //typedef NCOORDS::threshold threshold;
  using  NCOORDS::threshold;
  //  using  NCOORDS::threshold<double>;
  
#define DRAW_IS_SUCCESS 0 // another enum? - for more than two
#define DRAW_IS_FAILURE 1

  enum xnToolEventFlags{
    XNTOOLEVENTFLAG_NONE =0,
    XNTOOLEVENTFLAG_TRANSLATE = 1,
    XNTOOLEVENTFLAG_SCALE =1<<1,
    XNTOOLEVENTFLAG_ROTATE=1 << 2,
    XNTOOLEVENTFLAG_CENTRED = 1 <<3,
    XNTOOLEVENTFLAG_CONSTRAINED_XY = 1 << 4,
    XNTOOLEVENTFLAG_CONSTRAINED_X = 1 << 5,
    XNTOOLEVENTFLAG_CONSTRAINED_Y= 1 << 6
  };

  xnToolEventFlags& operator^= (xnToolEventFlags& left, 
				xnToolEventFlags right);
  xnToolEventFlags& operator|= (xnToolEventFlags& left, 
				xnToolEventFlags right);

  
  bool CONTROL_press(GdkEventAny* event);
  bool SHIFT_press(GdkEventAny* event);
  bool SUPER_press(GdkEventAny* event);
  bool HYPER_press(GdkEventAny* event);
  bool META_press(GdkEventAny* event);

  GdkEventButton* as_EventButton(GdkEventAny* ev);
  GdkEventKey* as_EventKey(GdkEventAny* ev);

  template<typename SIGNALRV, typename SIGNALARG>
  struct SignalPair:
    public std::pair<sigc::signal<SIGNALRV,SIGNALARG> ,
		     sigc::signal<SIGNALRV,SIGNALARG> >{

  public:
    typedef sigc::signal<SIGNALRV,SIGNALARG> signal_type;
    typedef sigc::slot<SIGNALRV,SIGNALARG> slot_type;
    typedef std::pair<signal_type, signal_type> pair_type;

  protected:
    signal_type& real; //first
    signal_type& preview; //second
    std::pair<sigc::connection, sigc::connection> connections;
    //subtype for multiple connections per signal
   
  public:
 SignalPair();
    SignalPair(signal_type r, signal_type p);

    sigc::connection& real_connection(){return connections.first;}
    sigc::connection& preview_connection(){return connections.second;}
    void real_emit(SIGNALARG arg){real.emit(arg);}
    void preview_emit(SIGNALARG arg){preview.emit(arg);}

    sigc::connection connect_real(slot_type slot);
    sigc::connection connect_preview(slot_type slot);
    void disconnect_preview();
    void disconnect_real();

  };

  template <typename Arg1, typename Arg2>
  class TransformSignal: public SignalPair<void, std::pair< Arg1, Arg2>& >{
  
  public:
    typedef SignalPair<void, std::pair< Arg1, Arg2>& > signalpair_type;
    typedef typename signalpair_type::slot_type slot_type;
    typedef sigc::slot<coords> refpt_slot_type;
  protected:
    using signalpair_type::real;
    using signalpair_type::preview;
    using signalpair_type::disconnect_real;
    using signalpair_type::disconnect_preview;
   
    std::pair< Arg1, Arg2> startpos;
    bool in_use;
    std::pair<slot_type, slot_type> slots;
    refpt_slot_type reference_slot;
    xnToolEventFlags action_flags;
  
  public:

    virtual Arg2& flag_manipulate_arg(Arg2& rawarg, xnToolEventFlags flags);

    slot_type& real_slot(){return slots.first;}
    slot_type& preview_slot(){return slots.second;}
    void set_real_slot(slot_type slot){ slots.first = slot; }
    void set_preview_slot(slot_type slot){ slots.second = slot; }
    void set_slots(const std::pair<slot_type, slot_type>& newslots);
    void set_action_flags(xnToolEventFlags flags);
    void clear_action_flags();
    xnToolEventFlags get_action_flags()const;

    void set_reference_slot(refpt_slot_type ref_slot);

    TransformSignal();
    TransformSignal(const std::pair<slot_type, slot_type>& newslots,
		    refpt_slot_type ref_slot= refpt_slot_type());
    virtual ~TransformSignal(){}

    virtual bool start(const std::pair< Arg1, Arg2>& startdata);
    void connect_slots();
    
    virtual void disconnect_slots();
    virtual bool update_preview(const std::pair< Arg1, Arg2>& data);

    virtual bool really_start(const coords& p){return false;}
    virtual bool really_update(const coords& p){return false;}
    virtual bool really_finish(const coords& p){return false;}

    virtual bool finish(const std::pair< Arg1, Arg2>& data);


  };

  struct RotateSignaller: public TransformSignal<double, coords>{
    typedef TransformSignal<double, coords> transform_signal_type;
    typedef TransformSignal<double, coords>::slot_type slot_type;
    using  transform_signal_type::startpos;
    using  transform_signal_type::in_use;
  protected:
    //coords* reference_point;
 
  public:

    RotateSignaller(const coords& centre,
		    std::pair<slot_type,slot_type> slots =std::pair<slot_type,slot_type>());

    RotateSignaller(const std::pair<slot_type,slot_type>& slots);
    RotateSignaller();
    virtual ~RotateSignaller();

    coords get_reference_point()const;
    //   void set_reference_point(const coords* pt);
#define ROT_x(S, pt) std::cerr<<"RotateSignaller::"<< S<<"\npt: "<<pt.x<<" "<<pt.y<<std::endl;;
    virtual bool really_start(const coords& p){
      ROT_x("really_start",p);
      return do_start(p);
    }
    virtual bool really_update(const coords& p){
      ROT_x("really_update",p);
      return do_update(p);
    }
    virtual bool really_finish(const coords& p){
      ROT_x("really_finish",p);
      return do_finish(p);
    }

    virtual coords& flag_manipulate_arg(coords& rawarg, xnToolEventFlags flags);

    bool do_start(const coords& click, const coords* refpt=NULL);
    bool do_update(const coords& click);
    bool do_finish(const coords& click);

    double get_angle(const coords& pt)const;
    
  };

  struct TranslateSignaller:
    public TransformSignal<coords, int>{
    typedef TransformSignal<coords, int> transform_signal_type;
    typedef TransformSignal<coords, int>::slot_type slot_type;

    TranslateSignaller(const std::pair<slot_type,slot_type>& slots);
    TranslateSignaller();
    virtual ~TranslateSignaller();

    virtual bool really_start(const coords& p);
    virtual bool really_update(const coords& p);
    virtual bool really_finish(const coords& p);
  };

  class ScaleSignaller: 
    public TransformSignal<coords, coords>{
  public:
    //first - the scale
    //second - origin
    typedef TransformSignal<coords, coords> transform_signal_type;
    typedef TransformSignal<coords, coords>::slot_type slot_type;
    ScaleSignaller(const coords& ref,
		   std::pair<slot_type,slot_type> slots = std::pair<slot_type,slot_type>());
    ScaleSignaller(const std::pair<slot_type,slot_type>& slots);
    ScaleSignaller();
    virtual ~ScaleSignaller();
  protected:
    //coords* reference_point;
 
  public:
    coords& fix_scale(coords& curscale);

    virtual coords& flag_manipulate_arg(coords& rawarg, xnToolEventFlags flags);

    virtual bool really_start(const coords& p){return do_start(p);}
    virtual bool really_update(const coords& p){return do_update(p);}
    virtual bool really_finish(const coords& p){return do_finish(p);}
    bool do_start(const coords& click, const coords* refpt=NULL);
    bool do_update(const coords& click);
    bool do_finish(const coords& click);
    coords get_reference_point()const;
    //void set_reference_point(const coords* pt);
    //    void destroy_reference_point();
   
  };


  class drawtoolbase : public Glib::Object{
    
    virtual bool on_drawbutton_press(GdkEventButton* event)=0;
    virtual int tooltype()const = 0;
    virtual bool on_drawbutton_release(GdkEventButton* event)=0;
  public:
    drawtoolbase(){}    
    
    virtual ~drawtoolbase()=0;
  };
  
  class TransformTool{
  public:
    typedef std::pair<double, coords> rotation_value_type;// angle, centre
    typedef std::pair<coords,coords> scale_value_type;
    typedef std::pair<coords,int> translate_value_type;
    typedef sigc::signal<void,rotation_value_type&> rotation_signal_type;
    typedef sigc::signal<void,scale_value_type&> scale_signal_type;

    typedef sigc::signal<void, translate_value_type&> point_signal_type;;
    
    
    //all the rest to vanish
    //send transform signals and calls to a base_class or  subobject
    typedef ScaleSignaller::slot_type scale_slot_type;
    typedef RotateSignaller::slot_type rotate_slot_type;
    typedef TranslateSignaller::slot_type translate_slot_type;
    typedef std::pair<scale_slot_type, scale_slot_type> scale_slotpair_type;
    typedef std::pair<rotate_slot_type, rotate_slot_type> rotate_slotpair_type;
    typedef std::pair<translate_slot_type, translate_slot_type> translate_slotpair_type;

    void set_slots(const rotate_slotpair_type& rslots, 
		   const scale_slotpair_type& scslots,
		   const translate_slotpair_type& translots);
  protected:
    RotateSignaller rotate_signal;
    ScaleSignaller scale_signal;
    TranslateSignaller  translate_signal;

  public:
    void set_action_flags(xnToolEventFlags flags){
      if(flags & XNTOOLEVENTFLAG_TRANSLATE){
	translate_signal.set_action_flags( flags);
      }
      if(flags & XNTOOLEVENTFLAG_SCALE){
	scale_signal.set_action_flags( flags);
      }
      if(flags & XNTOOLEVENTFLAG_ROTATE){
	rotate_signal.set_action_flags( flags);
      }
    }

    void clear_action_flags(){
      scale_signal.clear_action_flags();
      translate_signal.clear_action_flags();
      rotate_signal.clear_action_flags();
    }

    coords get_reference_point()const{
      return scale_signal.get_reference_point();
    }

    void set_reference_slot(sigc::slot<coords> slot){
      rotate_signal.set_reference_slot(slot);
      scale_signal.set_reference_slot(slot);
    }

    TransformTool(rotate_slotpair_type rotslots=rotate_slotpair_type(),

		  scale_slotpair_type scaleslots = scale_slotpair_type(),
		  translate_slotpair_type transslots=translate_slotpair_type(),
		  sigc::slot<coords> refpt_slot = sigc::slot<coords>()):
      rotate_signal(rotslots),
      scale_signal(scaleslots),
      translate_signal(transslots){
      set_reference_slot(refpt_slot);
    }


    void set_scale_slots(const std::pair<scale_slot_type,
			 scale_slot_type>& slots){
      scale_signal.set_slots(slots);
    }
    bool rotate_start(const coords& pt){
      return rotate_signal.really_start(pt);
    }
    bool rotate_continue(const coords& pt){
      return rotate_signal.really_update(pt);
    }

    bool rotate_finish(const coords& pt){
      bool res=rotate_signal.really_finish(pt);
      rotate_signal.clear_action_flags();
      return res;
    }
    bool scale_start(const coords& pt){
      return scale_signal.really_start(pt);
    }
    bool scale_continue(const coords& pt){
      return scale_signal.really_update(pt);
    }

    bool scale_finish(const coords& pt){
      bool res=scale_signal.really_finish(pt);
      scale_signal.clear_action_flags();
      return res;
    }
    bool translate_start(const coords& pt){
      return translate_signal.really_start(pt);
    }
    bool translate_continue(const coords& pt){
      return translate_signal.really_update(pt);
    }

    bool translate_finish(const coords& pt){
      return translate_signal.really_finish(pt);
    }
    void set_translate_slots(const std::pair<translate_slot_type,
			     translate_slot_type>& slots){
      translate_signal.set_slots(slots);
    }

    void set_rotate_slots(const std::pair<rotate_slot_type,
			  rotate_slot_type>& slots){
      rotate_signal.set_slots(slots);
    }
  };
  
  class Drawtool: public drawtoolbase{
  public:
   
    typedef   XndrawMap<int, sigc::connection> connection_map;
    typedef std::pair<coords*, int> value_type;
    typedef std::pair<coords, int> objvalue_type;//to become value_type:passing a pointer too dangerous
    typedef std::pair<double, coords> rotation_value_type;// angle, centre
    typedef std::pair<coords,coords> scale_value_type;
    typedef objvalue_type& objvalue_reference;////to become 'reference'
    typedef const objvalue_type& const_objvalue_reference;
    typedef sigc::signal<void, value_type> DrawSignal;
    //typedef sigc::signal<void, objvalue_reference> point_signal_type;
    typedef sigc::signal<void,rotation_value_type&> rotation_signal_type;
    typedef sigc::signal<void,scale_value_type&> scale_signal_type;

    typedef sigc::signal<void, objvalue_reference> point_signal_type;;
    
    
    //all the rest to vanish
    //send transform signals and calls to a base_class or  subobject
    typedef ScaleSignaller::slot_type scale_slot_type;
    typedef RotateSignaller::slot_type rotate_slot_type;
    typedef TranslateSignaller::slot_type translate_slot_type;
    typedef std::pair<rotate_slot_type, rotate_slot_type> rotate_slotpair_type;
    typedef std::pair<scale_slot_type, scale_slot_type> scale_slotpair_type;
    typedef std::pair<translate_slot_type, translate_slot_type> translate_slotpair_type;
    typedef DrawSignal coordsOriginSignal;
    
  protected:
    Gtk::Widget* parent;
    threshold<double> min_distance;
  
    coords point;
    coords previous;
    std::queue<coords*> clickqueue;
    //hmm - not useful right now - but difficult to remove
    
    int status;
      bool  initialclicksent;
    bool mouse_depressed;
    xnToolEventFlags eventflags;

    connection_map connections;
    //sigc::connection connection;
       
 
    DrawSignal  m_coords_signal_draw;
    coordsOriginSignal m_coords_signal_origin;
    
    DrawSignal  m_coords_signal_select;
    //or in a separate class which just pops up during scale/rotat/trans events
    TransformTool transformer;   

  public:
    virtual int tooltype()const;
    //required by Bezier_drawtool
    virtual void on_path_reset(){ initialclicksent=false;}

    std::queue<coords*>& get_queue(){
      return clickqueue;
    }
    void clear_eventflags();
    
    void set_connection(sigc::connection conn, int which);

    bool mouse_in_parent()const;
    void set_transform_slots(const rotate_slotpair_type& rslots, 
			     const scale_slotpair_type& scslots,
			     const translate_slotpair_type& translots);

    void set_keymodifier_flags(GdkEventAny* event, const coords& diff);

    void set_reference_point_hook(sigc::slot<coords> slot);

    virtual bool on_key_press_event(GdkEventKey* event);
 
    bool handle_eventflags(GdkEventAny* unused=NULL);

    int event_transformation_flags()const;

    bool on_motion(GdkEventMotion* event){
      return true;//should call on motion event, or nothing
    }
    bool on_motion_event(GdkEventMotion* event);
  
    inline    DrawSignal coords_signal_select(){
      return m_coords_signal_select;
    }
 
    inline      DrawSignal coords_signal_draw(){
      return m_coords_signal_draw;
    }
    
    
    inline coordsOriginSignal coords_signal_draworigin(){
      return m_coords_signal_origin;
    }
    bool on_originbutton_press(GdkEventButton* event);
    
    Gtk::Widget* get_parent();
    
    Drawtool(Gtk::Widget* W, double min  = 0.8,double ex = 0.0, double why = 0.0);
    
    virtual ~Drawtool(){
      disconnect_all();
    }
    void disconnect_all();
    
    sigc::connection get_connection(int which);
  
    inline bool get_mouse_depressed()const{ return mouse_depressed;}
    inline void set_mouse_depressed(bool up_or_down){mouse_depressed = up_or_down;}
    
    coords* get_next(GdkEventButton* event);
    
    coords* get_next(GdkEventMotion* event);
    
    bool on_drawbutton_release(GdkEventButton* event);

    bool on_drawbutton_press(GdkEventButton* event);
    
    void set_connection(int which, sigc::connection conn){
      connections[which] = conn;
    }
    
    
    
    double get_x()const;
    double get_y()const;
    coords get_coords()const;
    const coords& get_previous()const;
    coords& get_previous();
    const coords& get_current_point()const;
    coords& get_current_point();
    void set_previous(coords* newoLD);
    void set_x(double ex);
    void set_y(double why);
    
    friend std::ostream& operator<<(std::ostream& stream, const Drawtool& d){
      return stream << d.get_x() << d.get_y();
    }
    
    inline      threshold<double>& get_threshold(){
      return min_distance;
    }
    
    inline      void set_Threshold(double newmin){
      min_distance.reset(newmin);
    }
    
  };
  
  
  
  class Freedrawtool:public Drawtool{
  protected:
    int usecs;
    //sigc::connection freedraw_connection;
    Glib::RefPtr<Glib::TimeoutSource> timeout_signal;
    
  public:
    int tooltype()const;
    
    //  Freedrawtool(const Drawtool& drawtool, int timing = 100000): Drawtool(drawtool), usecs(timing){}
    
    Freedrawtool(Gtk::Widget* mummy, int timing = 10000/*0.01sec*/, double min = 2.0,double ex =0.0, double why = 0.0);

    virtual ~Freedrawtool(){}
    friend std::ostream& operator<<(std::ostream& stream, const Freedrawtool& d){
      return stream << d.get_x()  <<d.get_y();
    }
    
  public:   
    int get_delay()const { return usecs;}
    void set_delay(int timing){usecs = timing;}
    bool get_motion(GdkEventMotion* event); 
    bool on_drawbutton_release(GdkEventButton* event);
    bool on_drawbutton_press(GdkEventButton* event);  
    virtual bool on_freedraw_timeout();
    
  };

  class SelectBoxTool:public Freedrawtool{
  protected:

    threshold<double> min_selectupdate;
      
 
  public:
    SelectBoxTool(Gtk::Widget* parent_, double min_size= 1.0, double minbox_update= 12.0):
      Freedrawtool(parent_, 10000, min_size), 
      min_selectupdate(minbox_update){
      std::cerr << "SelectBoxTool::SelectBoxTool():parent: "<< parent<<std::endl;
    }

    virtual int tooltype()const;
    bool get_motion(GdkEventMotion* event); 
        bool on_drawbutton_release(GdkEventButton* event);  
    bool on_drawbutton_press(GdkEventButton* event);  

    virtual bool on_freedraw_timeout();
  };
  class Bezier_drawtool:public Freedrawtool{
  protected:
    static coords arbitrary_coords;
    bool have_saved_handle;
    coords saved_handle;
    //moved to baseclass
    //bool initialclicksent;
  public:
    Bezier_drawtool(Gtk::Widget* parent_, 
		    NCOORDS::bez_coords* thisn =NULL , 
		    NCOORDS::bez_coords* thatn=NULL):
      Freedrawtool(parent_),
      have_saved_handle(false),
      saved_handle(arbitrary_coords){}
    
    virtual ~Bezier_drawtool(){}
    virtual int tooltype()const;
    virtual void on_path_reset();
    bool on_drawbutton_press(GdkEventButton* event);

    bool on_drawbutton_release(GdkEventButton* event);
  };




  //////////////////////////////////////////

  struct Pencilbase : public Glib::Object{
    typedef std::pair<coords* , int> value_type;

    
    typedef sigc::signal<void, value_type> DrawSignal;

    virtual value_type  on_press(GdkEventButton* event)=0;
    
    virtual value_type on_release(GdkEventButton* event)=0;
  
    Pencilbase(){}
    
    virtual ~Pencilbase()=0;
  };


  class Pencil: public Pencilbase{
  protected:
    Gtk::Widget* parent;
    threshold<double> min_distance;
    
    coords last_down;
    coords last_up;
    coords cache;

    int status;
    bool mouse_depressed;
    DrawSignal drawsignal;
    sigc::connection connection;
    
  public:
    Pencil(Gtk::Widget* W, double min  = 1.0,double ex = 0.0, double why = 0.0)
      : 
      parent(W),  
      min_distance(min),
      last_down(ex, why),
      last_up(ex, why),
      status(DRAW_IS_SUCCESS), 
      mouse_depressed(false)
    { }
    
    virtual ~Pencil(){};
   
    
    typedef sigc::signal<void, value_type> FreeDrawSignal;
    
  protected:
    FreeDrawSignal  m_coords_signal_freedraw;
    
  public:
    virtual   FreeDrawSignal coords_signal_freedraw(){
      return m_coords_signal_freedraw;
    }
    
    Gtk::Widget* get_parent(){
      return parent;
    }
    
    
    
    sigc::connection get_connection(int which){
      return connection;
    }
    //overload
    virtual    const coords& get_current_point()const{
      return last_down;
    }
    //overload
    virtual coords& get_current_point(){
      return last_down;
    }

    inline bool get_mouse_depressed()const{ return mouse_depressed;}
    inline void set_mouse_depressed(bool up_or_down){mouse_depressed = up_or_down;}
    bool moved_past_threshold(const GdkEventButton* event)const{
      return moved_past_threshold(event->x, event->y);
    }

    bool moved_past_threshold(const GdkEventMotion* event)const{
      return moved_past_threshold(event->x, event->y);
    }

    
    bool moved_past_threshold(const double& x, const double& y)const;
    bool is_threshold(const double& x, const double& y)const;
    
    
    value_type on_press(const GdkEventButton* event){
      using namespace NCOORDS;
      mouse_depressed = true;
      last_down.x = event->x;
      last_down.y = event->y;
      switch(event->button){
      case(1):
	return value_type(&last_down, LINE_CLICK);
      case(3):
      default:
	return value_type(&last_down, MOVE_CLICK);
      }
    }
    
    value_type  on_release(GdkEventButton* event){
      using namespace NCOORDS;
      mouse_depressed = false;
      std::cout << "Pencil::on_release called"<< std::endl;
      return value_type(NULL, NULL_CLICK);
    }
    
    
    void set_connection(sigc::connection conn){
      connection = conn;
    }
    
    
    
    inline      double get_x()const { return get_current_point().x;}
    inline      double get_y()const{ return get_current_point().y;}
    inline      coords* get_coords()const {return new coords(last_down);}
    
    
    friend std::ostream& operator<<(std::ostream& stream, const Pencil& d){
      return stream << d.get_x() << d.get_y();
    }
    
    inline      threshold<double>& get_threshold(){
      return min_distance;
    }
    
    inline      void set_Threshold(double newmin){
      min_distance.reset(newmin);
    }
    
  };
  
}/*namespace xNDRAW*/
#include "drawtool.tcc"

#endif /*X3_DRAW_DRAWTOOL_H*/
