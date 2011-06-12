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


#ifndef X3DRAW_CLICKHISTORY_TCC
#define X3DRAW_CLICKHISTORY_TCC

//template instantiations from clickhistory.h
//only include this file from (the bottom of) it

#ifndef X3DRAW_CLICKHISTORY_H
//Error
ERROR("this file must be included from 'clickhistory.h'")
#endif

namespace NCOORDS{

   template <class ITER>
    simple_click_history::simple_click_history(ITER start, ITER finish, 
			 std::string path_id):
      pathid(path_id),
      path_cleaner(this){
      if(!pathid.size()){
	std::stringstream ostmp;
	ostmp <<"path#"<< this;
	pathid = ostmp.str();
      }
      std::for_each(start , finish, begin(), std::back_inserter(*this));
    }

  template <typename CHARTYPE, typename TRAITS, typename AL>
  signed char numbercount_line(const std::basic_string<CHARTYPE,TRAITS,AL>& line){
    typedef std::basic_string<CHARTYPE, TRAITS, AL> std_type;
    typedef std::basic_stringstream<CHARTYPE, TRAITS, AL> stringstream_type;
    stringstream_type ostmp;
    ostmp << line;//expected values in the range of +/- screenwidth/height
    ostmp.setf(std::ios_base::skipws);
    const double unlikely_value = 0b1011001110001111;
    signed char res=0;
    while( res <=124 ){      
      if(!ostmp.good()){
	if(ostmp.eof()){return res;}
	if(res){return -res;}
	return -128;
      }
      double v(unlikely_value);
      ostmp >> v;
      if(ostmp.good() || ostmp.eof()){res++;}
      std::cerr<<"numbercount()\n v ="<<v<<"\nunlikely_value"
	       <<unlikely_value <<std::endl;
      if(v == unlikely_value){
	throw xn_Error("an unlikely value!\nPS remove this test measure");
      }
    }
    if( ostmp.eof()){return res;}
    if(ostmp.good()){return -127;}//line too long
    return -res;
  }

  template <typename CHARTYPE>
  bool curstyle_svggood(CHARTYPE s){
    std::string narrowgoodchars("mMlLcCzZ");
    std::locale loc;
    std::basic_string<CHARTYPE> goodchars;
    WIDEN_STRING(narrowgoodchars, goodchars, loc);
    for(size_t i =0; i< goodchars.size();i++){
      if (s == goodchars[i]){return true;}
    }
    return false;
  }

template <typename CHARTYPE>
  int curstyle_to_clicktype(CHARTYPE s){
    std::locale loc;
    const CHARTYPE M_char = CTYPE_FACET(loc).widen('M');
    const CHARTYPE L_char = CTYPE_FACET(loc).widen('L');
    const CHARTYPE C_char = CTYPE_FACET(loc).widen('C');
    const CHARTYPE Z_char = CTYPE_FACET(loc).widen('Z');
    CHARTYPE s_up= std::toupper(s, loc);
    if(s_up == M_char){return MOVE_CLICK;}
    else if(s_up == L_char){return LINE_CLICK;}
    else if(s_up == C_char){return BEZIER_CLICK;}//can't just naively use this
    else if(s_up == Z_char){return CLOSE_PATH;}
    else{
      throw xn_Error("invalid path flag/'point style'");
    }
  }

  template<typename CHARTYPE, typename TRAITS>
  std::basic_istream<CHARTYPE, TRAITS>& simple_click_history::read_datastream(std::basic_istream<CHARTYPE, TRAITS>& bindata){
    typedef std::basic_istream<CHARTYPE, TRAITS> istream_type;
    typedef std::basic_stringstream<CHARTYPE, TRAITS> stringstream_type;
    typedef std::basic_string<CHARTYPE, TRAITS> std_type;//default/any allocator
    typedef typename istream_type::traits_type traits_type;
    typedef typename istream_type::int_type int_type;
#ifdef DEBUG_CLICKHISTORY
    if(size()){
      std::cerr<< " appending to non-empty path"<<std::endl;
    }
#endif
    bindata.setf(std::ios_base::skipws);
#ifdef DEBUG_CLICKHISTORY
    N_COUT( "reading simple_click_history  from stream: ");
#endif
    //simple_click_history result;
    value_type tmpresult;
    std_type s_gotline;
    int numcount(-11);
    try{
      bool reading = true;
      //TODO: name all enums and use names
      while(reading){
	if(bindata.eof() || !bindata.good()){reading=false;break;}
	if(std::getline(bindata, s_gotline) ){
	  if(!s_gotline.size()){
	    reading = false; 
	    break;// blank line means new path
	  }
	  int no_cnt= numbercount_line(s_gotline);
	  if(numcount == -11){
	    if ((no_cnt <2) ||( no_cnt >4)){
	      throw xn_Error("bad file");
	    }
	    numcount = no_cnt;
	  }
	  else if(no_cnt != numcount){
	    throw xn_Error("inconsistent data");
	  }
	  stringstream_type ostmp;
	  ostmp<<    s_gotline;
	  ostmp >> tmpresult.first.x
		>> tmpresult.first.y; 
	  if(numcount==4){
	    int mcoordstate;
	    ostmp >> mcoordstate;
	    tmpresult.first.set_state((coord_state)mcoordstate);
	  }
	  if(numcount >2){	ostmp  >> tmpresult.second;}
	  push_back(tmpresult);
	}
	else{std::cerr <<"getline failed: will drop out of loop next iter\n";}
      }
    }
    catch(std::ios_base::failure& e){
      std::cerr <<" reading path from stream failed, or maybe just eof\n"
		<<e.what()<< "\n size of path after reading: " << size()<<std::endl;
    }
    //check if path got longer when not empty at start
    if(!size()){
      throw PathEmpty();
    }
    return bindata;
  }

  template <typename CHARTYPE, typename TRAITS>
  std::basic_ostream<CHARTYPE, TRAITS>& 
  simple_click_history::write_human_stream(std::basic_ostream<CHARTYPE, TRAITS>& os){
    typedef std::basic_ostream<CHARTYPE, TRAITS> ostream_type;
    typedef std::basic_stringstream<CHARTYPE, TRAITS> stringstream_type;
    typedef std::basic_string<CHARTYPE, TRAITS> std_type;
      std::locale loc;
      CHARTYPE SPACE = std::use_facet<std::ctype<CHARTYPE> >(loc).widen(' ');
      CHARTYPE STRUDEL = std::use_facet<std::ctype<CHARTYPE> >(loc).widen('@');
      CHARTYPE NEWLINE = std::use_facet<std::ctype<CHARTYPE> >(loc).widen('\n');
      stringstream_type ostmp;
      ostmp << NEWLINE ;
      std_type terminatedNEWLINE =ostmp.str();
      std_type wide_id;
      std::string pat=get_pathid();
      WIDEN_STRING(pat, wide_id, loc);
      os.clear();
      os.exceptions(std::ios_base::failbit );
      try{
	os << STRUDEL << wide_id << NEWLINE;
	for (const_iterator walk = begin();walk !=end(); walk++){
	  std_type CLIKTYPE;
	  std::string clicktyp = clicktype_as_string((*walk).second);
	  WIDEN_STRING(clicktyp, CLIKTYPE, loc);
	  os << (*walk).first.x << SPACE << (*walk).first.y << SPACE 
	     << CLIKTYPE << NEWLINE;
	}
	return os << NEWLINE; //terminated? huh
      }
      catch(std::ios_base::failure& fail){
	std::ostringstream ostmp;
	ostmp << "writing data failed: "<< fail.what();
	throw XNIOError(ostmp.str());
      }
      return os;
    }

  template <typename CHARTYPE, typename TRAITS>
  std::basic_ostream<CHARTYPE, TRAITS>& simple_click_history::write_datastream(std::basic_ostream<CHARTYPE, TRAITS>& os){
    typedef std::basic_ostream<CHARTYPE, TRAITS> ostream_type;
    typedef std::basic_stringstream<CHARTYPE, TRAITS> stringstream_type;
    typedef std::basic_string<CHARTYPE, TRAITS> std_type;
      std::locale loc;
      CHARTYPE SPACE = std::use_facet<std::ctype<CHARTYPE> >(loc).widen(' ');
      CHARTYPE STRUDEL = std::use_facet<std::ctype<CHARTYPE> >(loc).widen('@');

      CHARTYPE NEWLINE = std::use_facet<std::ctype<CHARTYPE> >(loc).widen('\n');
      stringstream_type ostmp;
      ostmp << NEWLINE ;
      std_type terminatedNEWLINE =ostmp.str();
      std_type wide_id;
      std::string pat=get_pathid();
      WIDEN_STRING(pat, wide_id, loc);
      os.clear();
      os.exceptions(std::ios_base::failbit );
      try{
	os << STRUDEL << wide_id << NEWLINE;
	for (const_iterator walk = begin();walk !=end(); walk++){
	  os << (*walk).first.x << SPACE << (*walk).first.y << SPACE 
	     <<(*walk).second << NEWLINE;
	}
	return os << NEWLINE; //terminated? huh
      }
      catch(std::ios_base::failure& fail){
	std::ostringstream ostmp;
	ostmp << "writing data failed: "<< fail.what();
	throw XNIOError(ostmp.str());
      }
      return os;
    }
    
  //read from a stream into a path
  //call repeatedly
  //like so
  ////
  
 //  std::deque<simple_click_history> paths;
  //  try{

  //  --- then ---
  //
  //simple_click_history pathempty;
  //while(!inputstream.eof()){//or something
  // paths.push_back( pathempty);
  //simple_click_history& p = *(paths.rbegin());
  //inputstream >> p;
  //}

  ////  --- or maybe---
  //
   // inputstream >> paths;
  //
  // ---then ---
  //}
  //catch(...){...}
 template <typename CHARTYPE, typename TRAITS>
  std::basic_istream<CHARTYPE, TRAITS>& operator>>(std::basic_istream<CHARTYPE, TRAITS>& ifs, simple_click_history& dest){
    typedef std::basic_string<CHARTYPE, TRAITS> std_type;
    //doesn't match operator<< -this used to import format from svgread.py
    //TODO: symmetric with operator<<(ostream, simple_click_history&)
    // it should create a _machine_ readable stream
    //TODO: pipe and popen or fork and execve for svgread.py(not here!)
    std::cerr<<"enter operator>>simple_click_history\n";
    std::pair<coords, int> inpoint;
    double x(0.),y(0.);
    std::locale loc; //current locale
    //SVG path point 'flags'
    //call methods of clickhistory if ! 'svgstyle'
    const CHARTYPE M_char= CTYPE_FACET(loc).widen('M');
    const CHARTYPE C_char= CTYPE_FACET(loc).widen('C');
    const CHARTYPE Z_char= CTYPE_FACET(loc).widen('Z');
    const CHARTYPE L_char= CTYPE_FACET(loc).widen('L');
    const CHARTYPE STRUDEL =CTYPE_FACET(loc).widen('@');
    const CHARTYPE SPACECHAR=CTYPE_FACET(loc).widen(' ');
    const CHARTYPE NEWLINE=CTYPE_FACET(loc).widen(' ');
    register CHARTYPE space;
    register CHARTYPE  curstyle = M_char;//start with a move_to()
    register CHARTYPE nextlinestart;
    std::string path_newid;//narrow string
    std::basic_string<CHARTYPE, TRAITS> idline, cur_line;
    bool gotid(false);
    while(ifs.good() && std::getline(ifs, cur_line)){
      
      if(!gotid){
	if(!(cur_line.size())){continue;}
	else{
	  space= *(cur_line.begin());
	  if(std::isdigit(space, loc)){
	    int n_numbers = numbercount_line(cur_line);
	    if( n_numbers>=2 &&n_numbers <= 4){
	      dest.read_datastream(ifs);
	      return ifs;
	    }
	    else throw xn_Error("can't parse file");
	  }
	  //non-svg style ie point.x point.y,[[state],clicktype]
	  //and which may not have id at the top
	  if (space != STRUDEL ){
	    throw xn_Error("path data start with pathid(starts with '@') or point data = numbers");
	  }
	  idline= std_type(cur_line.begin()+1, cur_line.end());
	  WIDEN_STRING(idline, path_newid, loc);
	  dest.set_pathid(path_newid);
	  gotid=true;continue;
	}	
      }
      else{//gotid ==true
	space= *(cur_line.begin());
	if(cur_line.size()!=1  ){
	  if(std::isdigit(space, loc)){
	    int n_numbers = numbercount_line(cur_line);
	    if( n_numbers>=2 &&n_numbers <= 4){
	      dest.read_datastream(ifs);
	      return ifs;
	    }
	    else throw xn_Error("can't parse file");
	  }
	  throw xn_Error("can't parse file");
	}
	else{
	  if(std::toupper(space, loc) != M_char){
	    throw xn_Error("in svgstyle data, actual point data(after optional pathid) must begin with 'M' (ie 'move_to()')on otherwise empty line");
	  }
	  break;
	}	
      }
    }
    //path proper - already read leading 'M'
    if(ifs.eof()){
      throw PathEmpty();
    }
    typedef std::pair<coords, int> pair_type;
    //dest.push_back(pair_type(coords(x,y),MOVE_CLICK));
    while(ifs.good() && std::getline(ifs, cur_line)){    
      if (not cur_line.size()){
	if(!dest.size()){
	  throw PathEmpty();
	}
	std::cerr<<"returning path: "<<dest<<std::endl;
	return ifs;
      }
      else if(cur_line.size()==1){
	curstyle = std::toupper(*(cur_line.begin()),loc);
	if (!curstyle_svggood(curstyle )){//ie 'L','M','C' or 'Z'
	  throw xn_Error("invalid point style");
	}
	continue;
      }
      std::stringstream sstream;
      sstream << cur_line;
      nextlinestart= cur_line[0];
      coords  pt;
      sstream >> pt.x >> pt.y;
      dest.push_back(pair_type(pt, curstyle_to_clicktype(curstyle)));
      if (curstyle == C_char){
	coords handle, handle1;
	sstream >>handle.x>> handle.y
		>>handle1.x>> handle1.y;
	//reverse order of handles
	std::cerr<< "curve point and handles - the input line :\n"<< cur_line
		 <<"\nOUTPUT TO simple_click_history =>\nthe point: "<<pt.x <<", "<< pt.y
		 <<"\nhandle1 : "<< handle1.x << ", " <<handle1.y
		 <<"\nhandle2 :"<< handle.x << ", "<<handle.y<<std::endl;
	dest.push_back(pair_type(handle1 ,BEZIER_HANDLE));
	dest.push_back(pair_type(handle ,BEZIER_HANDLE));
      }
    }
    std::cerr <<"falling off back of inserter:the path"<<dest<<std::endl;
    return ifs;
  }

  
  
 template <typename CHARTYPE, typename TRAITS>
  std::basic_istream<CHARTYPE, TRAITS>& operator>>(std::basic_istream<CHARTYPE, TRAITS>& ifs, std::deque<simple_click_history>& dest){
    while (!ifs.eof()){
      dest.push_back(simple_click_history());
      simple_click_history& path = *(dest.end()-1);
      try{
	ifs >> path;
      }
      catch(const PathEmpty& pe){
	dest.pop_back();
	std::cerr <<"got 'PathEmpty', cleared empty path ...\nreturning"
		  <<std::endl;
	return ifs;
      }
      std::cerr <<"got path: "<< path<<std::endl;
      if (!ifs.good()){return ifs;}
    }
    return ifs;
  }


}
namespace xNDRAW{


}


#endif
