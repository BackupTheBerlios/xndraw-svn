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
#include "drawtool.h"

using NCOORDS::coords;
//todo : put inside namespace xNDRAW;
using namespace xNDRAW;
using namespace NCOORDS;
drawtoolbase::~drawtoolbase(){}
NCOORDS::coords  Bezier_drawtool::arbitrary_coords(-1e6,-1e6);

xnToolEventFlags& xNDRAW::operator^= (xnToolEventFlags& left, 
				      xnToolEventFlags right){
  int tmp=left ^ right;
  left = (xnToolEventFlags)tmp;
  //left=tmp;
  return left;
}
xnToolEventFlags& xNDRAW::operator|= (xnToolEventFlags& left, 
				      xnToolEventFlags right){
  int tmp=left | right;
  left = (xnToolEventFlags)tmp;
  return left;
}

bool Pencil::moved_past_threshold(const double& x, const double& y)const{
  const  coords& currpt = this->get_current_point();
  double dx = currpt.x -x;
  double dy = currpt.y - y;
  return this->is_threshold(dx, dy);
}


bool Pencil::is_threshold(const double& dx, const double& dy)const{
  return min_distance(pow(dx,2.0) + pow(dy, 2.0));
}

Drawtool::Drawtool(Gtk::Widget* W, double min,double ex , double why )
  : 
  parent(W),  
  min_distance(min),
  point(ex, why),
  previous(ex, why),
  status(DRAW_IS_SUCCESS), 
  initialclicksent(false),
  mouse_depressed(false),
  eventflags(  XNTOOLEVENTFLAG_NONE)
{ }

sigc::connection Drawtool::get_connection(int which){
  if(connections.find(which) != connections.end()){
    return connections[which];
  }
  return sigc::connection();
}

NCOORDS::coords* Drawtool::get_next(GdkEventButton* event){
  previous.set(point);
  point.set(event->x, event->y);
  return &point;
}
NCOORDS::coords* Drawtool::get_next(GdkEventMotion* event){
  previous.set(point);
  point.x = event->x;
  point.y = event->y;
  return &point;
}

   
bool Drawtool::on_drawbutton_release(GdkEventButton* event){
  mouse_depressed = false;
  std::cout << "drawtool_on_drawbutton_release called"<< std::endl;
  return true;
}

Gtk::Widget* Drawtool::get_parent(){
  return parent;
}

double Drawtool::get_x()const { return point.x;}

double Drawtool::get_y()const{ return point.y;}

void Drawtool::set_x(double ex){ point.x = ex;}//WHY?

void Drawtool::set_y(double why){ point.y = why;}
inline      NCOORDS::coords Drawtool::get_coords()const {return point;}
const NCOORDS::coords& Drawtool::Drawtool::get_previous()const { return previous;}
NCOORDS::coords& Drawtool::get_previous(){ return previous;}
const NCOORDS::coords& Drawtool::get_current_point()const{return point ;}
NCOORDS::coords& Drawtool::get_current_point(){return point ;}
void Drawtool::set_previous(NCOORDS::coords* newold){previous.set(*newold);}

bool Drawtool::on_originbutton_press(GdkEventButton* event){
  if(eventflags){
    handle_eventflags();
    return true;
  }
  previous.set(point);
  point.x = event->x;
  point.y=event->y;
  coords_signal_draworigin().emit(value_type(&point,ORIGIN_CLICK));
  return true;
}

GdkEventButton* xNDRAW::as_EventButton(GdkEventAny* ev){
  if (ev->type & (GDK_BUTTON_PRESS| GDK_2BUTTON_PRESS| 
		  GDK_3BUTTON_PRESS | GDK_BUTTON_RELEASE)){
    GdkEventButton* retval= reinterpret_cast<GdkEventButton*>(ev);
    if(!retval){
      throw NCOORDS::xn_Error("as_EventButton failed");
    }
    return retval;
  }
  else{throw NCOORDS::xn_Error("not an eventbutton");}
}

GdkEventKey* xNDRAW::as_EventKey(GdkEventAny* ev){
  if (ev->type & (GDK_KEY_PRESS | GDK_KEY_RELEASE)){
    GdkEventKey* retval= reinterpret_cast<GdkEventKey*>(ev);
    if(!retval){
      throw NCOORDS::xn_Error("as_EventKey failed: not a GDK_KEY_PRESS | GDK_KEY_RELEASE");
    }
    return retval;
  }
  else{throw xn_Error("not an eventbutton");}
}

bool xNDRAW::CONTROL_press(GdkEventAny* event){
  if (event->type & (GDK_BUTTON_PRESS| GDK_2BUTTON_PRESS| 
		     GDK_3BUTTON_PRESS | GDK_BUTTON_RELEASE)){
    GdkEventButton* ev= as_EventButton(event);
    return ev->state & GDK_CONTROL_MASK;
  }
  else if(event->type & (GDK_KEY_PRESS | GDK_KEY_RELEASE)){
    GdkEventKey* key= as_EventKey(event);
    return key->state & GDK_CONTROL_MASK;
  }
  return false; //why bother with an error?
}

bool xNDRAW::SHIFT_press(GdkEventAny* ev){
  if (ev->type & (GDK_BUTTON_PRESS| GDK_2BUTTON_PRESS| 
		  GDK_3BUTTON_PRESS | GDK_BUTTON_RELEASE)){
    GdkEventButton* event= as_EventButton(ev);
    return event->state & GDK_SHIFT_MASK;
  }
  else if(ev->type & (GDK_KEY_PRESS | GDK_KEY_RELEASE)){
    GdkEventKey* key= as_EventKey(ev);
    return key->state & GDK_SHIFT_MASK;
  }
  return false;
}
bool xNDRAW::SUPER_press(GdkEventAny* ev){
  if (ev->type & (GDK_BUTTON_PRESS| GDK_2BUTTON_PRESS| 
		  GDK_3BUTTON_PRESS | GDK_BUTTON_RELEASE)){
    GdkEventButton* event= as_EventButton(ev);
    return event->state & GDK_SUPER_MASK;
  }
  else if(ev->type & (GDK_KEY_PRESS | GDK_KEY_RELEASE)){
    GdkEventKey* key= as_EventKey(ev);
    return key->state & GDK_SUPER_MASK;
  }
  return false;
}

bool xNDRAW::HYPER_press(GdkEventAny* ev){
  if (ev->type & (GDK_BUTTON_PRESS| GDK_2BUTTON_PRESS| 
		  GDK_3BUTTON_PRESS | GDK_BUTTON_RELEASE)){
    GdkEventButton* event= as_EventButton(ev);
    return event->state & GDK_HYPER_MASK;
  }
  else if(ev->type & (GDK_KEY_PRESS | GDK_KEY_RELEASE)){
    GdkEventKey* key= as_EventKey(ev);
    return key->state & GDK_HYPER_MASK;
  }
  return false;
}

bool xNDRAW::META_press(GdkEventAny* ev){
  if (ev->type & (GDK_BUTTON_PRESS| GDK_2BUTTON_PRESS| 
		  GDK_3BUTTON_PRESS | GDK_BUTTON_RELEASE)){
    GdkEventButton* event= as_EventButton(ev);
    return event->state & GDK_META_MASK;
  }
  else if(ev->type & (GDK_KEY_PRESS | GDK_KEY_RELEASE)){
    GdkEventKey* key= as_EventKey(ev);
    return key->state & GDK_META_MASK;
  }
  return false;
}

bool Drawtool::on_drawbutton_press(GdkEventButton* event){
  if(eventflags){
    handle_eventflags();
    return true;
  }
  double dx = event->x - point.x;
  double dy = event->y - point.y;   
  if(min_distance(pow(dx,2.0) + pow(dy, 2.0)) || (!initialclicksent)){
    previous.set(point);
    point.x = event->x;
    point.y=event->y;
    if(!initialclicksent){
      coords_signal_draw().emit(value_type(&point, MOVE_CLICK));
      initialclicksent = true;
    }
    else if (event->button == 1){ //left button
      coords_signal_draw().emit(value_type(&point, LINE_CLICK));
    }
    else if (event->button == 2){ //middle button
      //BETTER -binary OR the values together and add a check to the drawing/history saving routine, ND ONLY EMIT ONCEA
      coords_signal_draw().emit(value_type(&point, LINE_CLICK));
      coords_signal_draw().emit(value_type(&point, CLOSE_PATH));;
    }
    else if(event->button == 3 ){ //right button -todo: other buttons
      coords_signal_draw().emit(value_type(&point, MOVE_CLICK));
    }
  }  
  return true;
}


int Drawtool::tooltype()const{
#ifdef DEBUG
  std::cout << "Drawtool::tooltype() -> XNDRAW_DRAWING_SIMPLE_CLICK" << std::endl;
#endif
  return XNDRAW_DRAWING_SIMPLE_CLICK;
}
int Freedrawtool::tooltype()const{
#ifdef DEBUG
  std::cout << "Freedrawtool::tooltype() -> XNDRAW_DRAWING_FREEHAND" << std::endl;
#endif
  return XNDRAW_DRAWING_FREEHAND;
}

int SelectBoxTool::tooltype()const{
#ifdef DEBUG
  std::cout << "SelectBoxTool::tooltype() -> XNDRAW_SELECT_BOX" << std::endl;
#endif
  return XNDRAW_SELECT_BOX;
}


int Bezier_drawtool::tooltype()const{
#ifdef DEBUG
  std::cout << "Bezier_Bezier_drawtool::tooltype() -> XNDRAW_DRAWING_BEZIER" << std::endl;
#endif
  return XNDRAW_DRAWING_BEZIER;
}
RotateSignaller::RotateSignaller(const std::pair<RotateSignaller::slot_type,
				 RotateSignaller::slot_type>& slots):
  transform_signal_type(slots){}


RotateSignaller::RotateSignaller(){}

RotateSignaller::~RotateSignaller(){}


NCOORDS::coords RotateSignaller::get_reference_point()const{
  if(!reference_slot.empty()){return reference_slot();}
  return coords();//0,0
}

/*
  void RotateSignaller::destroy_reference_point(){
  if(reference_point){
  delete reference_point;
  reference_point=NULL;
  }
  }
*/
double RotateSignaller::get_angle(const coords& pt)const{
  coords ref=get_reference_point();
  return ref.get_angle(&pt);
}


bool RotateSignaller::do_finish(const coords& click){
  double angle= get_angle(click);
  std::cerr<<"RotateSignaller::do_finish(): angle: "<<angle<<std::endl;
  std::pair<double, coords> rotinfo(angle, click);
  return finish(rotinfo);
}

bool RotateSignaller::do_update(const NCOORDS::coords& click){
  double angle= get_angle(click);
  //std::cerr<<"angle: "<<angle<<std::endl;
  std::pair<double, coords> rotinfo(angle, click);
  return update_preview(rotinfo);
}

NCOORDS::coords& 
RotateSignaller::flag_manipulate_arg(NCOORDS::coords& rawarg, 
				     xnToolEventFlags flags){
  if(flags & XNTOOLEVENTFLAG_CENTRED){
    std::cerr<<"setting arg2.x to-100000 to rotate around centre\nFiXME => ME HACK"<<std::endl;
    rawarg.x =-100000;
  }
  return rawarg;
}
   

bool RotateSignaller::do_start(const coords& click, const coords* unused){
  //set_reference_point(refpt);
  double startangle= get_angle(click);
  std::cerr<<"angle: "<<startangle<<std::endl;
  std::pair<double, coords> rotinfo(startangle, click);
  return start(rotinfo);
}

TranslateSignaller::TranslateSignaller(const std::pair<TranslateSignaller::slot_type,
				       TranslateSignaller::slot_type>& slots):
  transform_signal_type(slots){}

TranslateSignaller::TranslateSignaller(){}

TranslateSignaller::~TranslateSignaller(){}

bool TranslateSignaller::really_start(const coords& p){
  std::pair<coords, int> arg(p,0);
  return start(arg);
}

bool TranslateSignaller::really_update(const coords& p){
  std::pair<coords, int> arg(p,0);
  return update_preview(arg);
}

bool TranslateSignaller::really_finish(const coords& p){
  std::pair<coords, int> arg(p,0);
  return finish(arg);
}

NCOORDS::coords ScaleSignaller::get_reference_point()const{
  if(!reference_slot.empty()){return reference_slot();}
  return coords();//0,0
}
/*
  void ScaleSignaller::set_reference_point(const coords* pt){
  destroy_reference_point();
  if(pt){
  reference_point= new coords(*pt);
  }
  }

  void ScaleSignaller::destroy_reference_point(){
  if(reference_point){
  delete reference_point;
  reference_point=NULL;
  }
  }
*/
ScaleSignaller::ScaleSignaller(const std::pair<slot_type,slot_type>& slots):transform_signal_type(slots){}

ScaleSignaller::ScaleSignaller(){}
ScaleSignaller::~ScaleSignaller(){}

NCOORDS::coords& ScaleSignaller::fix_scale(NCOORDS::coords& curscale){
  if(action_flags & XNTOOLEVENTFLAG_CONSTRAINED_XY){
    double xabs= (curscale.x >=0 ? curscale.x : -curscale.x);
    double yabs= (curscale.y >=0 ? curscale.y : -curscale.y);

    if(xabs >yabs){
      curscale.y = curscale.x;
    }
    else{curscale.x = curscale.y;}
  }
  else if(action_flags & XNTOOLEVENTFLAG_CONSTRAINED_X){
    curscale.x=1;
  }
  else if(action_flags & XNTOOLEVENTFLAG_CONSTRAINED_Y){
    curscale.y =1;
  }
  return curscale;
}

bool ScaleSignaller::do_finish(const coords& click){
  coords refpt=get_reference_point();
  coords startdist = startpos.second -refpt;
  coords newdist=  click - refpt;
  coords curscale= newdist /startdist;
  curscale=fix_scale(curscale);
  //
  //XNTOOLEVENTFLAG_CONSTRAINED_XY = 1 << 4,
  //XNTOOLEVENTFLAG_CONSTRAINED_X 
  std::pair<coords, coords> scaleinfo(curscale, click);
  return finish(scaleinfo);
}

NCOORDS::coords& 
ScaleSignaller::flag_manipulate_arg(NCOORDS::coords& rawarg, 
				    xnToolEventFlags flags){
  if(flags & XNTOOLEVENTFLAG_CENTRED){
    std::cerr<<"setting arg2.x to-100000 to rotate around centre\nFiXME => ME HACK"<<std::endl;
    rawarg.x =-100000;
  }
  return rawarg;
}

bool ScaleSignaller::do_update(const coords& click){
  coords refpt=get_reference_point();
  coords startdist = startpos.second -refpt;
  coords newdist=  click - refpt;
  coords curscale= newdist /startdist;
  curscale=fix_scale(curscale);
  std::pair<coords, coords> scaleinfo(curscale, click);
  return update_preview(scaleinfo);
}

bool ScaleSignaller::do_start(const coords& click, const coords* unused){
  // set_reference_point(refpt);
  coords startscale(1,1);
  std::pair<coords, coords> scaleinfo(startscale, click);
  return start(scaleinfo);
}

void  TransformTool::set_slots(const rotate_slotpair_type& rslots, 
			       const scale_slotpair_type& scslots,
			       const translate_slotpair_type& translots){
  rotate_signal.set_slots(rslots);
  scale_signal.set_slots(scslots);
  translate_signal.set_slots(translots);
}

bool Freedrawtool::get_motion(GdkEventMotion* event){
  double xnew=event->x;
  double ynew=event->y;
  double dx =xnew - get_x();
  double dy = ynew - get_y();
  if ((pow(dx,2) + pow(dy, 2)) <= 2){
    return true;//put in drawtool . feed it the events
  }
  
  set_x(xnew);
  set_y(ynew);
  return true;//work this out - for the whole iteration , aybe a coords* parameter
}

bool SelectBoxTool::on_drawbutton_press(GdkEventButton* event){
  set_mouse_depressed(true);
  point.x = previous.x =event->x;
  point.y = previous.y = event->y;
  coords_signal_select().emit(value_type(&point, XNDRAW_POINTER_SELECTBOX_BUTTONDOWN));
  sigc::connection freedraw_connection =Glib::signal_timeout().connect(sigc::mem_fun(*this, &SelectBoxTool::on_freedraw_timeout),10);
  connections[XNDRAW_CONNECT_COORDS_SELECT] = freedraw_connection;
 
#ifdef DEBUG
  std::cout <<"SelectBoxTool connecting draw callback ==>signal_timeout().connect(sig... &SelectBoxTool::on_freedraw_timeout),10);" << std::endl;
#endif
  return true;
}

bool SelectBoxTool::on_drawbutton_release(GdkEventButton* event){
  mouse_depressed = false;
#ifdef DEBUG
  std::cout << "SelectBoxTool::on_drawbutton_release called"<< std::endl;
#endif
  point.x = event->x;
  point.y = event->y;
  coords_signal_select().emit(value_type(&point,XNDRAW_POINTER_SELECTBOX_BUTTONUP));
  return true;
}

void  Drawtool::set_transform_slots(const rotate_slotpair_type& rslots, 
				    const scale_slotpair_type& scslots,
				    const translate_slotpair_type& translots){
  transformer.set_slots(rslots, scslots,translots);
}

bool Drawtool::on_motion_event(GdkEventMotion* event){
  bool res(false);
  if(eventflags){
    coords motionpt(event->x, event->y);
    if(eventflags & XNTOOLEVENTFLAG_ROTATE){
      res= transformer.rotate_continue(motionpt);
    }
    if(eventflags & XNTOOLEVENTFLAG_SCALE){
      res= transformer.scale_continue(motionpt);
    }
    if(eventflags & XNTOOLEVENTFLAG_TRANSLATE){
      res= transformer.translate_continue(motionpt);
    }
  }
  return res;
}

void Drawtool::disconnect_all(){
  for (connection_map::iterator m_map = connections.begin(); m_map != connections.end(); m_map++){
    if((*m_map).second.connected()){(*m_map).second.disconnect();}
  }
}

int Drawtool::event_transformation_flags()const{
  int res(0);
  if(eventflags & XNTOOLEVENTFLAG_ROTATE){
    res =  (int)XNTOOLEVENTFLAG_ROTATE;
  }
  if(eventflags & XNTOOLEVENTFLAG_SCALE){
    res |=  (int)XNTOOLEVENTFLAG_SCALE;
  }
  if(eventflags & XNTOOLEVENTFLAG_TRANSLATE){
    res |=  (int)XNTOOLEVENTFLAG_TRANSLATE;
  }
  return res;
}

bool Drawtool::handle_eventflags(GdkEventAny* unused){
  //execute any path manipulation here
  //ie translate/scale/rotate
  int mousex, mousey;
  parent->get_pointer( mousex, mousey);
  coords mouse( mousex, mousey);
  bool inwidget= mouse_in_parent();
  if (!inwidget){
    std::cerr<<"keypress but mouse not in widget: "<<mousex <<", "
	     <<  mousey<<"\nreturning"<< std::endl;
    return false;
  }
  if(eventflags & XNTOOLEVENTFLAG_ROTATE){
    transformer.rotate_finish(mouse);
  }
  if(eventflags & XNTOOLEVENTFLAG_SCALE){
    transformer.scale_finish(mouse);
  } 
  if(eventflags & XNTOOLEVENTFLAG_TRANSLATE){
    transformer.translate_finish(mouse);
  }
  clear_eventflags();
  return true;
}

void Drawtool::set_connection(sigc::connection conn, int which){
  connections[which] = conn;
}

void Drawtool::clear_eventflags(){
  eventflags=(xnToolEventFlags)0;
  transformer.clear_action_flags();
}    

void Drawtool::set_keymodifier_flags(GdkEventAny* event, 
				     const coords& diff){
  bool meta(IS_META(event)), super(IS_SUPER(event)), hyper(IS_HYPER(event)),
    shift(IS_SHIFT(event)), control(IS_CONTROL(event));
  if(shift){
    eventflags |= XNTOOLEVENTFLAG_CENTRED;
  }
  if(control){
    eventflags |=  XNTOOLEVENTFLAG_CONSTRAINED_XY;
  }
  if(meta ||super || hyper){
    if(eventflags&XNTOOLEVENTFLAG_CONSTRAINED_XY){
      eventflags ^=XNTOOLEVENTFLAG_CONSTRAINED_XY;
    }
    coords absdiff(diff);
    absdiff.x=(diff.x >=0?diff.x :-diff.x);
    absdiff.y=(diff.y >=0?diff.y :-diff.y);
    if (diff.x >= diff.y){
      eventflags |= XNTOOLEVENTFLAG_CONSTRAINED_Y;
    }
    else{
      eventflags |= XNTOOLEVENTFLAG_CONSTRAINED_X;
    }
  }
}

bool Drawtool::on_key_press_event(GdkEventKey* event){
  if(mouse_depressed){
    return false;
  }
  wchar_t c=event->keyval;
  PRINT_MODIFIERS(event)
    std::cerr<<"character: '";
  std::wcerr<<c;
  std::cerr<<"' - as numeric: "  <<(int)event->keyval <<std::endl;
  int mousex, mousey;
  parent->get_pointer( mousex, mousey);
  coords mouse( mousex, mousey);
  coords refpt = transformer.get_reference_point();
  coords diff=mouse-refpt;
  bool inwidget= mouse_in_parent();
  if (!inwidget){
    std::cerr<<"keypress but mouse not in widget: "<<mousex <<", "
	     <<  mousey<<"\nreturning"<< std::endl;
    return false;
  }
  bool handle_eventflags_now(false);
  switch(event->keyval){
  case GDK_Escape:
    clear_eventflags();
    break;//cancel current transformation
  case '\n':
    handle_eventflags_now=true;
    break;
  case 'm':
  case 'M':
    if(!event_transformation_flags()){
      eventflags=	 (xnToolEventFlags)( eventflags |   XNTOOLEVENTFLAG_TRANSLATE);
	  
      transformer.translate_start(mouse);
    }
  break;
  case 's':
  case 'S':

    if(!event_transformation_flags()){
      eventflags= (xnToolEventFlags)(eventflags |   XNTOOLEVENTFLAG_SCALE);
      set_keymodifier_flags((GdkEventAny*)event, diff);
      transformer.set_action_flags(eventflags);
      transformer.scale_start(mouse);;
    }
  break;
  case 'r':
  case 'R':
    if(!event_transformation_flags()){
      eventflags= (xnToolEventFlags)(eventflags |   XNTOOLEVENTFLAG_ROTATE);
      set_keymodifier_flags((GdkEventAny*)event, diff);
      transformer.set_action_flags(eventflags);
      transformer.rotate_start(mouse);;
      break;
    }
  default:
    break;

  }
  if(handle_eventflags_now){
    return handle_eventflags(reinterpret_cast<GdkEventAny*>(event));
  }
  return true;
}
 

void Drawtool::set_reference_point_hook(sigc::slot<coords> slot){
  transformer.set_reference_slot(slot);
}

bool Drawtool::mouse_in_parent()const{
  int m_x, m_y;
  parent->get_pointer(m_x, m_y);
  Gtk::Allocation alloc= parent->get_allocation ();
  return m_x >=0 && m_y >=0 &&  m_x <=alloc.get_width() &&
    m_y <=alloc.get_height();
}

bool Freedrawtool::on_drawbutton_press(GdkEventButton* event){
  if(eventflags){
    handle_eventflags();
    return true;
  }
  set_mouse_depressed(true);
  double dx = event->x - get_x();
  double dy = event->y - get_y();
  if(get_threshold()(pow(dx,2.0) + pow(dy, 2.0))){
    previous.set(get_x(),get_y());
    point.set(event->x, event->y);
    coords_signal_draw().emit(value_type(&point,MOVE_CLICK));
  }
  sigc::connection freedraw_connection =Glib::signal_timeout().connect(sigc::mem_fun(*this, &Freedrawtool::on_freedraw_timeout),10);
  connections[XNDRAW_CONNECT_COORDS_FREEDRAW] = freedraw_connection;
#ifdef DEBUG
  std::cout <<"Freedrawtool connecting draw callback ==>signal_timeout().connect(sig... &Freedrawtool::on_freedraw_timeout),10);" << std::endl;
#endif
  return true;
}

Freedrawtool::Freedrawtool(Gtk::Widget* mummy, int timing, double min,double ex , double why ):
  Drawtool(mummy, min,ex, why), 
  usecs(timing)
{ timeout_signal= Glib::TimeoutSource::create(10);
  //Glib::signal_timeout().connect(sigc::mem_fun(*this,&Freedrawtool::on_freedraw_timeout), 10);
}
bool SelectBoxTool::on_freedraw_timeout(){
  //currently doesn't stop when pointer leaves window
  if (get_mouse_depressed()){
    int xnow,ynow;
    //std::cerr <<"SelectBoxTool::on_freedraw_timeout(): Parent: "<< parent <<std::endl;
    parent->get_pointer(xnow,ynow);
    double dx = xnow - get_x();
    double dy = ynow - get_y();
    if(min_selectupdate(pow(dx,2.0) + pow(dy, 2.0))){
      //std::cerr << " SelectBoxTool::on_freedraw_timeout(): point set: "
      //<< xnow << ", " << ynow<<std::endl;
      //previous.set(point); - leave at box start location
      point.set(xnow,ynow);
      coords_signal_select().emit(value_type(&point, XNDRAW_POINTER_SELECTBOX_DRAGMOTION));
    }
    return true;
  }
  connections[XNDRAW_CONNECT_COORDS_SELECT].disconnect();
  return false;
}


bool Freedrawtool::on_freedraw_timeout(){
#ifdef DEBUG
  std::cout <<" Freedrawtool::on_freedraw_timeout()" << std::endl;
#endif
  if (get_mouse_depressed()){
    int xnow,ynow;
    get_parent()->get_pointer(xnow,ynow);
    double dx = xnow - get_x();
    double dy = ynow - get_y();
    if(get_threshold()(pow(dx,2.0) + pow(dy, 2.0))){
      previous.set(point);
      point.set(xnow,ynow);
      coords_signal_draw().emit(value_type(&point, LINE_CLICK));
    }
    return true;
  }
  connections[XNDRAW_CONNECT_COORDS_FREEDRAW].disconnect();  
  return false;
}

bool Bezier_drawtool::on_drawbutton_press(GdkEventButton* event){
  using namespace NCOORDS;//BEZIER_CLICK etc
  double dx = event->x - point.x;
  double dy = event->y - point.y; 
  if(min_distance(pow(dx,2.0) + pow(dy, 2.0))){
    set_mouse_depressed(true);
    previous.set(point);
    point.x = event->x;
    point.y=event->y;
    if(!initialclicksent){
      saved_handle = point;
      have_saved_handle = true;
      coords_signal_draw().emit(value_type(&point, MOVE_CLICK));
      //move_to not followed by handles, should eliminate nasty crash...
      set_mouse_depressed(false);
      initialclicksent = true;
    }
    else{
      switch(event->button ){
      case 1: //left button
	coords_signal_draw().emit(value_type(&point, BEZIER_CLICK));
	break;
      case 2: //middle button
	//BETTER -binary OR the values together and add a check to the drawing/history saving routine
	coords_signal_draw().emit(value_type(&point, BEZIER_CLICK));
	coords_signal_draw().emit(value_type(&point, CLOSE_PATH));;
	set_mouse_depressed(false);
	break;
      case 3 : //right button -todo: other buttons
      default:
	coords_signal_draw().emit(value_type(&point, MOVE_CLICK));
	saved_handle=point;
	//	saved_handle=arbitrary_coords;//fix bug 
	//	have_saved_handle = false;
	set_mouse_depressed(false);
	break;
      }
    }
  }
  return true;
}
bool Freedrawtool::on_drawbutton_release(GdkEventButton* event){
  return Drawtool::on_drawbutton_release(event);
  //point.x = event->x;
  //point.y = event->y;
  //connections[connections[XNDRAW_CONNECT_COORDS_FREEDRAW].disconnect()
}

void Bezier_drawtool::on_path_reset(){
  Drawtool::on_path_reset();
  have_saved_handle=false;
  saved_handle=arbitrary_coords;
}
bool Bezier_drawtool::on_drawbutton_release(GdkEventButton* event){
  using namespace NCOORDS;//BEZIER_HANDLE
  if(get_mouse_depressed()){
    //should create a 'smooth' point
    coords handle1((2 * point.x) - event->x, (2 * point.y) - event->y);
    if(have_saved_handle && (saved_handle !=arbitrary_coords)){
      coords_signal_draw().emit(value_type(&saved_handle, BEZIER_HANDLE));
    }
    else{
      coords_signal_draw().emit(value_type(&point, BEZIER_HANDLE));
    }
    coords_signal_draw().emit(value_type(&handle1, BEZIER_HANDLE));
    have_saved_handle=true;
    saved_handle = coords(event->x, event->y);
    set_mouse_depressed(false);
  }
  return true;
}
