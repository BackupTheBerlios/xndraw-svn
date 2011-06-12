// -*- C++ -*-

/* Copyright (C)2006-2011 John O'Driscoll - biggerbumme@ yahoo.co.in
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#include "pathshape.h"

namespace NCOORDS{
 

  DoublePath::DoublePath(simple_click_history* src,
			 bool frontapex, double rotation):
    basepath(src),
    frontend(frontapex),//frontend ==false weird when drawing interactively
    rotangle(rotation){}

  coords DoublePath::get_last_point()const{
    if(!basepath){throw xn_Error("get_last_point():basepath NULL");}
    size_t sz= basepath->size();
    if(!sz){throw xn_Error("0 length path");}
    else if(sz==1 || frontend){
      return (*(basepath->end()-1)).first;
    }
    coords res = (*(basepath->begin())).first;
    coords apex = get_apex();
    return apex.rotate(&res, rotangle);
  }

  coords  DoublePath::get_middle_point()const{
    if(!basepath){throw xn_Error("get_middle_point():basepath NULL");}
    size_t sz= basepath->size();
    if(!sz){throw xn_Error("0 length path");}
    else if(sz==1 || frontend){
      return   (*(basepath->begin())).first;
    }
    return (*(basepath->end()-1)).first;
  }

  coords  DoublePath::get_first_point()const{
    if(!basepath){throw xn_Error("get_first_point(): basepath NULL");}
    size_t sz= basepath->size();
    if(!sz){throw xn_Error("0 length path");}
    else if(sz==1 || !frontend){
      return (*(basepath->begin())).first;
    }
    coords res = (*(basepath->end()-1)).first;
    coords apex = get_apex();
    return apex.rotate(&res, rotangle);
  }
  simple_click_history DoublePath::get_doppelganger()const{
    if(!basepath){throw xn_Error("get_doppelganger():basepath NULL");}
    simple_click_history res(*basepath);
    coords apex(get_apex());
    res.rotate(apex, rotangle);
    return res; //reverse path ?
  }

  simple_click_history DoublePath::as_path()const{
    if(!basepath){throw xn_Error("as_path():basepath NULL");}
    std::cerr<<"basepath.size(): "<< basepath->size()<<std::endl;
    if(basepath->size()<2){return *basepath;}
    typedef simple_click_history::const_reverse_iterator reverse_iterator;
    simple_click_history result;
    if(!frontend){      
result= *basepath;
}
    simple_click_history doublepath=get_doppelganger();
    //std::cerr<<"path from get_doppelganger();"<<doublepath<<std::endl;
    reverse_iterator r_w = doublepath.rbegin();
    std::pair<coords, int> first_one = *r_w++;
    if(frontend){
      first_one.second= MOVE_CLICK;
      result.push_back(first_one);
    }
    for(;r_w != doublepath.rend();r_w++){
      result.push_back(*r_w);
    }
    (*(result.end()-1)).second = LINE_CLICK;
    if(frontend && basepath->size()>1){
      for(const_iterator walk = basepath->begin()+1;walk != basepath->end(); walk++){
	result.push_back(*walk);
      }
    }
    return result;
  }

  QuadPath::QuadPath(DoublePath& p, double angle, bool use_backend_for_apex):
    srcpath(&p),
    rotangle(angle),
    backend(use_backend_for_apex){
    if (!srcpath){
      throw xn_Error("QuadPath::QuadPath():DoublePath NULL");
    }
  }

  coords QuadPath::get_apex()const{
    if(!srcpath){throw xn_Error("srcpath NULL");}
    if(backend){
      return srcpath->get_last_point();
    }
    else {return srcpath->get_first_point();}
  }

  simple_click_history QuadPath::as_path() const{
    std::cerr<<" QuadPath::as_path() const{"<<std::endl;
    if(!srcpath){throw xn_Error("srcpath NULL");}
    simple_click_history half = srcpath->as_path();
    if(!(half.size() > 1)){return half;}
    simple_click_history tmp(half), result;
    coords apex=get_apex();
    tmp.rotate(apex, rotangle);
    const_reverse_iterator r_w = tmp.rbegin();
    if(backend){
      result = half;
      r_w++;
      for(;r_w != tmp.rend(); r_w++){
	result.push_back(*r_w);
      }
      (*(result.end()-1)).second = LINE_CLICK;
    }
    else{
      point_type pt0 = *r_w++;
      pt0.second = MOVE_CLICK;
      result.push_back(pt0);
      for(;r_w != tmp.rend(); r_w++){
	result.push_back(*r_w);
      }
      (*(result.end()-1)).second = LINE_CLICK;
      if(half.size() >1){
      for(const_iterator w = half.begin()+1; w != half.end(); w++){
	result.push_back(*w);
      }
      }
    }
    return result;
  }

  const simple_click_history& nPathShape::get_basepath()const{
    return path.get_basepath();
  }
  simple_click_history& nPathShape::get_basepath(){
    return path.get_basepath();
  }
  double nPathShape::get_quadpath_angle()const{
    //feeling way at the moment
    unsigned int base= get_base();
    double stepangle= get_step();
  
    if(base==3){return stepangle;}  //2 *M_PI / 3
    else{return M_PI;}
  }
  double nPathShape::get_corner_angle(bool with_quadpath)const{
    unsigned int base=get_base();
    if(base==2){return  M_PI;}
    else if(base==3){
      if(with_quadpath){return 2 * M_PI/3.0;}
      else{return M_PI/3.0;}
    }
    else if(base==4){return M_PI/2.0;}
    //multiple possibilities
    else{return 2.0 *M_PI/3.0;}

  }

  DoublePath::operator bool()const{return basepath && basepath->size();}
    
  size_t DoublePath::basesize()const{
    if(!basepath){throw xn_Error("DoublePath::basesize(): basepath NULL");}
return basepath->size();}

  simple_click_history& DoublePath::get_basepath(){
 if(!basepath){throw xn_Error("DoublePath::get_basepath()): basepath NULL");}
    return *basepath;
  }

  const simple_click_history& DoublePath::get_basepath()const{
  if(!basepath){throw xn_Error("DoublePath::get_basepath()): basepath NULL");}
  return *basepath;
  }

  bool DoublePath::is_frontend()const{return frontend;}

  void DoublePath::set_basepath(simple_click_history* p){
    basepath =p;
  }

  simple_click_history nPathShape::draw_shape(bool with_quadpath){
    std::cerr<<"nPathShape::draw_shape"<<std::endl;
    if(!path){return simple_click_history();}
    if(path.basesize()==1){return path.as_path();}
    unsigned int base = get_base();
    double cornerangle= get_corner_angle();
    simple_click_history side ;
    if(with_quadpath){
      std::cerr<< "quad side"<<std::endl;
      QuadPath qpath(path, get_quadpath_angle(), true);
      side=qpath.as_path();
    }
    else{
      //std::cerr<< "double side"<<std::endl;
      if(base==3){cornerangle=M_PI/3.0;}
      side=path.as_path();
    }
    //std::cerr<<"side : \n"<<side<<std::endl;
    simple_click_history& p_test =path.get_basepath();
    //std::cerr << "P-test: "<< p_test<<std::endl;
    if(side.size() < p_test.size()){
      throw xn_Error("generated path shorter than source");
    }
    simple_click_history shapepath(side);
    coords corner,	 scratchstart,offset;
    for(unsigned int i =1;i<base;i++){
      simple_click_history scratch(side);
      
      scratchstart=(*(scratch.begin())).first;
      corner=(*(shapepath.end()-1)).first;
      scratch.rotate(scratchstart, i* cornerangle);
      offset= corner -scratchstart;
      if(scratch.size()<2){throw xn_Error("scratch too small");}
      for(const_iterator w= scratch.begin()+1;w != scratch.end();w++){
	coords p_pt=(*w ).first;
	p_pt+= offset;
	shapepath.push_back(point_type(p_pt, (*w).second));
      }
    }
    //offset path so its bounding box starts from 0,0
    //move to its own function
    if(shapepath.size()){
      cRect shapebox;
      shapepath.get_extents(shapebox);
      coords topL(shapebox.x(), shapebox.y());
      shapepath -= topL;
    }
    return shapepath;
  }
  //side2.reverse();

  simple_click_history nPathShape::angleshape(double firstangle,
					      double secondangle){
    if((firstangle < -(2*M_PI)) ||(firstangle > (2 *M_PI)) 
       ||(secondangle < -(2*M_PI)) ||secondangle > 2 *M_PI){
      throw xn_Error("angle out of range");
    }
    if(!path){return simple_click_history();}
    if(path.basesize()==1){return path.as_path();}
    unsigned int base = get_base();
    simple_click_history side(path.as_path()) ,scratch,shapepath;
    shapepath=side;
    coords corner,	 scratchstart,offset;
    double scratchangle=0;
    for(unsigned int i =1;i<base;i++){
      scratch=side;
      if(i%2){   
	scratchangle += firstangle;
	scratchstart=(*(scratch.rbegin())).first;}
      else{
	scratchangle += secondangle;
	scratchstart=(*(scratch.begin())).first;
      }
      corner=(*(shapepath.end()-1)).first;

      scratch.rotate(scratchstart, scratchangle);
      offset= corner -scratchstart;
      if(scratch.size() > 1){
      if(i%2){   
	for(const_reverse_iterator r_w= scratch.rbegin()+1;r_w != side.rend();r_w++){
	  shapepath.push_back(point_type((*r_w ).first + offset, (*r_w).second));
	}
      }
      else{
	for(const_iterator w= scratch.begin()+1;w != side.end();w++){
	  shapepath.push_back(point_type((*w ).first + offset, (*w).second));
	}
      }
      }
    }
    return shapepath;
  }

	


  double nPathShape::get_doublepath_angle()const{
    unsigned int base= get_base();
    double stepangle= get_step();
    if(base ==3){
      return (2 * M_PI) - stepangle;
    }
    return M_PI;
  }
  coords DoublePath::get_apex()const{

    if(!basepath||!basepath->size()){
      throw xn_Error("NULL or path empty");
    }
    if(frontend){ return (*(basepath->begin())).first;}
    return (*(basepath->end()-1)).first;
  }

  DoublePath nPathShape::create_doublepath(double angle){
    if(angle==0){angle = get_doublepath_angle();}
    DoublePath res( &(get_basepath()), true, angle);
    return res;
  }

  DoublePath nPathShape::create_doublepath(path_type& basepath, double angle){
    if(angle==0){angle = get_doublepath_angle();}
    DoublePath res( &basepath, true, angle);
    return res;
  }
  
  QuadPath  nPathShape::create_quadpath(double quadangle, double doubleangle){
    if(quadangle==0){quadangle= get_quadpath_angle();}
    return QuadPath(path,quadangle, doubleangle);
  }

  void nPathShape::set_basepath(simple_click_history* p){
    path.set_basepath(p);
  }

  simple_click_history nPathShape::as_path(){
    return draw_shape();//true or false - for quadpath
  }

  nPathShape::nPathShape(const ncoords& origin, 
			 simple_click_history& basepath):
    ncoords(origin),
    path(&basepath, true, get_doublepath_angle()){}

  bool nPathShape::is_tileable()const{
    //FIXME
    throw xn_Error("FIXME");;
  }
}
