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

#ifndef X3DRAW_DRAWTOOL_TCC
#define X3DRAW_DRAWTOOL_TCC

namespace xNDRAW{

template <typename Arg1, typename Arg2>
bool TransformSignal<Arg1, Arg2>::start(const std::pair<Arg1, Arg2>& startdata){
      in_use= true;
      startpos = startdata;
      connect_slots();
      return true;
    }

template <typename Arg1, typename Arg2>
xnToolEventFlags TransformSignal<Arg1, Arg2>::get_action_flags()const{
      return action_flags;
    }

template <typename Arg1, typename Arg2>
void TransformSignal<Arg1, Arg2>::clear_action_flags(){
      action_flags = XNTOOLEVENTFLAG_NONE;
    }

template <typename Arg1, typename Arg2>
void TransformSignal<Arg1, Arg2>::set_action_flags(xnToolEventFlags flags){
      action_flags = flags;
    }

template <typename Arg1, typename Arg2>
void TransformSignal<Arg1, Arg2>::set_slots(const std::pair<typename TransformSignal<Arg1, Arg2>::slot_type, typename TransformSignal<Arg1, Arg2>::slot_type>& newslots){
      slots= newslots;
    }

template <typename Arg1, typename Arg2>
Arg2& TransformSignal<Arg1, Arg2>::flag_manipulate_arg(Arg2& rawarg, xnToolEventFlags flags){
      return rawarg;
    }

template <typename Arg1, typename Arg2>
void TransformSignal<Arg1, Arg2>::disconnect_slots(){
  disconnect_real();
  disconnect_preview();
}

template <typename Arg1, typename Arg2>
bool TransformSignal<Arg1, Arg2>::finish(const std::pair< Arg1, Arg2>& data){
if(!in_use){//throw xn_Error("only call finish() after a call to start(), b4 the next call to finish()");
    return false;
  }
  if(real_slot().empty()){return false;}
  Arg1 val = data.first - startpos.first;
 std::pair<Arg1, Arg2> emitarg(val, flag_manipulate_arg(startpos.second, action_flags));
 //std::cerr<<"real.emit(emitarg);\n";
 real.emit(emitarg);
 //std::cerr<<"real.emit(emitarg):  --- done"<<std::endl;
  in_use= false;
      disconnect_slots();
      return true;
    }


template <typename Arg1, typename Arg2>
void TransformSignal<Arg1, Arg2>::set_reference_slot(typename TransformSignal<Arg1, Arg2>::refpt_slot_type ref_slot){
      reference_slot = ref_slot;
    }

template <typename Arg1, typename Arg2>
TransformSignal<Arg1, Arg2>::TransformSignal():
  in_use(false),
      action_flags(  XNTOOLEVENTFLAG_NONE){}

template <typename Arg1, typename Arg2>
TransformSignal<Arg1, Arg2>::TransformSignal(const std::pair<typename TransformSignal<Arg1, Arg2>::slot_type, typename TransformSignal<Arg1, Arg2>::slot_type>& newslots,
					     typename TransformSignal<Arg1, Arg2>::refpt_slot_type ref_slot):
      in_use(false), slots(newslots),
      reference_slot(ref_slot),
      action_flags(  XNTOOLEVENTFLAG_NONE)
{}

template <typename Arg1, typename Arg2>
void TransformSignal<Arg1, Arg2>::connect_slots(){
      if(!real_slot().empty()){
	connect_real(real_slot());
	if(!preview_slot().empty()){
	  connect_preview(preview_slot());
	}
      }
}

template <typename Arg1, typename Arg2>
bool TransformSignal< Arg1, Arg2>::update_preview(const std::pair< Arg1, Arg2>& data){
  if(!in_use){//throw xn_Error("only call update_preview() after a call to start(), b4 the next call to finish()");
    return false;
  }
  if(preview_slot().empty()){return false;}
Arg1 val = data.first - startpos.first;
 std::pair<Arg1, Arg2> emitarg(val, flag_manipulate_arg(startpos.second, action_flags));
  preview.emit(emitarg);
      return true;
    }



template<typename SIGNALRV, typename SIGNALARG>
sigc::connection SignalPair<SIGNALRV,SIGNALARG>::connect_real(typename SignalPair<SIGNALRV,SIGNALARG>::slot_type slot){
	disconnect_real();
	connections.first= real.connect(slot);
	return real_connection();
}

template<typename SIGNALRV, typename SIGNALARG>
void SignalPair<SIGNALRV,SIGNALARG>::disconnect_preview(){
      if(preview_connection().connected()){
	preview_connection().disconnect();
      }
    }

template<typename SIGNALRV, typename SIGNALARG>
    SignalPair<SIGNALRV,SIGNALARG>::SignalPair():
      real(SignalPair<SIGNALRV,SIGNALARG>::pair_type::first),
      preview(SignalPair<SIGNALRV,SIGNALARG>::pair_type::second){}

template<typename SIGNALRV, typename SIGNALARG>
    SignalPair<SIGNALRV,SIGNALARG>::SignalPair(typename SignalPair<SIGNALRV,SIGNALARG>::signal_type r, typename SignalPair<SIGNALRV,SIGNALARG>::signal_type p):
      SignalPair<SIGNALRV,SIGNALARG>::pair_type(r, p), 
      real(SignalPair<SIGNALRV,SIGNALARG>::pair_type::first),
      preview(SignalPair<SIGNALRV,SIGNALARG>::pair_type::second){}

template<typename SIGNALRV, typename SIGNALARG>
void SignalPair<SIGNALRV,SIGNALARG>::disconnect_real(){
      if(real_connection().connected()){
	real_connection().disconnect();
      }
    }

template<typename SIGNALRV, typename SIGNALARG>
sigc::connection SignalPair<SIGNALRV,SIGNALARG>::connect_preview(typename SignalPair<SIGNALRV,SIGNALARG>::slot_type slot){  
	disconnect_preview();
connections.second= preview.connect(slot);
 return preview_connection();
}
}

#endif
