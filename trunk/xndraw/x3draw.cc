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


#include "x3draw.h"
#include "ncoords.h"
#include "fileexists.h"


using namespace xNDRAW;
using namespace NCOORDS;
/*
  const char* xNDRAW::get_user_prefsdirpath(){
  const char* home = getenv("HOME");
  if(!home){throw xn_Error("HOME not set in environment");}
  unsigned pkgnamelength = strlen(PACKAGE_NAME);
  unsigned homelength = strlen(home);
  char* prefsdir = new char[homelength + pkgnamelength +3];
  strcpy(prefsdir, home);

  strcat(PATH_SEP_PTR, prefsdir);
  strcat(".", prefsdir); // hidden directory
  strcat(PACKAGE_NAME, prefsdir);
  return prefsdir;
  }
*/
DIR* xNDRAW::get_xndrawhomedir(const char* dirpath, bool create_if_missing){
  //or DIR* get_xndrawhomedir(const char* pkgdir, const char* homepath = NULL, bool create_if_missing = true)
  DIR* prefsdir = opendir(dirpath);
  if(prefsdir || !create_if_missing){ return prefsdir; }
  if(!system(NULL)){throw xn_Error("command shell not ava}ilable\nBefore restarting xndraw try manually creating  a folder called \".xndraw\"\nin your home directory\n\nheh heh");}
  char* commandstring = new char[strlen(dirpath) + 10];
  strcpy(commandstring, "mkdir -p ");
  strcat(commandstring, dirpath);
  prefsdir = opendir(dirpath);
  return prefsdir;
}

std::string xNDRAW::get_application_home(){
  const char* hm = getenv("HOME");
  std::stringstream ostmp;
  if(hm){
    ostmp << hm;
  }
  else{
#ifdef WINDOWS_NT
    // getenv("HOMEDRIVE") AND  getenv("HOMEPATH")?
    const char* hmdrv =getenv("HOMEDRIVE");
    hm =getenv("HOMEPATH");
    if(hm && hmdrv){
      ostmp <<hmdrv;
      if(res.size() && *(res.end()-1) != '\\'){
	ostmp <<"\\";
      }
      ostmp <<hm;
    }
    else{ throw xn_Error("no HOME that can b determined");}
#else
    throw xn_Error("no HOME defined");
#endif
  }
  std::string peek=ostmp.str();
#ifndef WINDOWS_NT
  if(peek.size()&& *(peek.end()-1) != '/'){
    ostmp << "/";
  }
#else
  if(peek.size()&& *(peek.end()-1) != '\\'){
    ostmp << "\\";
  }
#endif
  ostmp << ".xndraw";
  std::string tmpfile=ostmp.str();
 if(!direxists( tmpfile.c_str())){
   int res =snarky_createdir(tmpfile.c_str());
   if(res){
     throw xn_Error("can't create directory in home: permissions problewms?");
   }
 }
 return ostmp.str();
}


//use libstupiapp here
//as template, to handle widechar filesystem
std::vector<std::string> xNDRAW::list_dir(DIR* d){
  std::string narrowhint("");
  //using NAMESPACE_APPCOMMON;
  return list_dir(d, narrowhint);
}

template <>
X3_draw* Singletonise<X3_draw, commandline_opts>::ptr_to_Instance =NULL;
bool X3_draw::X3_draw_is_verbose = (bool)ENABLE_EXTRA_VERBIAGE;// TODO; implement verbose=no

X3_draw::X3_draw(): 
 
  //Ncoords_base(3),
  author("John O'Driscoll - biggerbumme@yahoo.co.in - PO Box 2080 Strawberry Hills NSW 2012 Australia.send a postcard, or a pattern"),
  helpstring(std::string(PACKAGE_STRING)),
  argcount(1)
{
  load_preferences("prefs");
  std::string pkg(PACKAGE_STRING);
  arguments.push_back( pkg);
  helpstring =helpstring + " is free software.\n\nUsage :   Options\n\n --help  ---  print this text\n\n";
  helpstring = helpstring  + "--version   ---  print virgin information \n\n\n" + author;
}  

X3_draw::X3_draw(commandline_opts& opt):

  author("John O'Driscoll - biggerbumme@yahoo.co.in - PO Box 2080 Strawberry Hills NSW 2012 Australia.send a postcard, or a pattern"),
  helpstring(PACKAGE_STRING),
  argcount(opt.get_argc()),
  arguments(opt.get_argv_strings()){
  load_preferences("prefs");
  helpstring += " is free software.\n\nUsage :   Options\n\n --help  ---  print this text\n\n";
  helpstring += "--version   ---  print version information \n\n\n" + author;
}



void X3_draw::set_colourpref(const char* name, double r, double g, double b, double a){
  Draw_colour dcolour(r,g,b,a);
  std::string s(name);
  set_prefvalue(s, dcolour);
}


void X3_draw::load_preferences(const char* filename){
  set_prefvalue("author", std::string("John O'Driscoll - biggerbumme@yahoo.co.in - PO Box 2080 Strawberry Hills NSW 2012 Australia.send a postcard, or a pattern"));
  
  //**********************
  //TODO: check command line and environmaent for any of these values, 
  //particularly check if an alternate preferences file has been nominated
  //TODO: find, read, and load prefernces file
  //ie look in @prefix@/share/xndraw/${PREFERENCES_FILE}
  //then ${HOME}/.xndraw/${PREFERENCES_FILE}
  std::string prefsfile("xndrawprefs.xml");
  set_prefvalue("PREFERENCES_FILE", prefsfile);
  //*******************************
  // colours for edit interface...
  set_colourpref("MAPCOLOUR_POINT_LINE",0.0, 1.0, 0.0, 1.0);//editcolour - coordmap
  set_colourpref("MAPCOLOUR_POINT_LINE_SELECTED", .3, .9, .3, 1);
  set_colourpref("MAPCOLOUR_POINT_FILL",0.3, 1.0, 0.3, 1.0);//editcolour - coordmap
  set_colourpref("MAPCOLOUR_POINT_FILL_SELECTED", .5, 1., .5, 1);
  set_colourpref( "EDITPOINT_COLOUR", .9, .9, .15, 1.);
  set_colourpref("EDITPOINT_COLOUR_SELECTED", 1.,1.,0.,1.);
  set_colourpref( "EDITPOINT_FILL_COLOUR", 0.9, .15, .9, 1.);
  set_colourpref("EDITPOINT_FILL_COLOUR_SELECTED", 1., 0.05, 0.95, 1.0);

  set_colourpref("EDITLINE_COLOUR", .9, .15, .9, 1.);
  set_colourpref("EDITLINE_COLOUR_SELECTED",1, 0., 1, 1.);
  set_colourpref("HANDLE_LINE_COLOUR", 0.6, 0.5, 0.1, 0.8);
  set_colourpref("HANDLE_POINT_COLOUR",.4, .5,0.,.8);
  set_colourpref("HANDLE_POINT_COLOUR_SELECTED", 0.6, 0.9, 0.1, 0.8);
  set_colourpref("HANDLE_POINT_FILL_COLOUR",.4, .5,0.,0.8);
  set_colourpref("HANDLE_POINT_FILL_COLOUR_SELECTED", 0.6, 0.9, 0.1, 0.8);
  set_colourpref("SELECTION_MARCHING_ANTS", .4, .4, .4, .6);
  set_colourpref("RTPREVIEW_PATH", 0,1,0,0.7);
  set_colourpref("RTPREVIEW_BBOX", 0,0, 1,0.7);

  unsigned long nodepatternstyle = XNDRAW_NODEPATTERN_REGULAR;
  //main window buffer size
  set_prefvalue("IMAGEBUFFER_WIDTH", 2048u);
  set_prefvalue("IMAGEBUFFER_HEIGHT",2048u);
  //nPathWindow etc buffer size
set_prefvalue("PATHFRAME_BUFFER_WIDTH", 1200u);
  set_prefvalue("PATHFRAME_BUFFER_HEIGHT",800u);
  set_prefvalue("DRAWINGSET_NODE_PATTERN", nodepatternstyle);
  
  set_prefvalue("DEFAULT_OFFSET_X", double(300));
  set_prefvalue("DEFAULT_OFFSET_Y", double(200));
  set_prefvalue("DRAWINGSET_MAX_NODE_COUNT", MAX_NODE_COUNT);
  set_prefvalue("NCOORDS_BASE", 3u);
  set_prefvalue("NODES_BASE", 3u);
  set_prefvalue("SELECT_RADIUS", double(20));
  set_prefvalue("XNDRAW_INIT_TYPE",   XNDRAW_INIT_AS_DRAWINGSET | XNDRAW_INIT_ON_INIT);
  set_prefvalue("DRAWINGSET_CORRECT", (bool)DRAWINGSET_LINE_UNDER_MOUSE);

  set_prefvalue("XNDRAW_HOME", ".xndraw");
}


char** commandline_opts::get_argv(){//broken, stupid
  using namespace std;
  size_t sz = argvalues.size();
  char** result = new char*[sz];
     
  if (sz == 0){
    return result = NULL;
  }
  int i =0;
  vector<string>::iterator v_walk = argvalues.begin();
  for(; v_walk != argvalues.end();v_walk++){      
    string s = *v_walk;

    const char* word =s.c_str();
    sz =sizeof(*word)/sizeof(char);
    result[i] = new char[sz];
    for (size_t j = 0; j < sz; j++){
      result[i][j] = word[j];
    }
    i++;
  }
  return result;
}

X3_draw::X3_draw(int argc, char** argv)
  :

  author("John O'Driscoll - biggerbumme@yahoo.co.in - PO Box 2080 Strawberry Hills NSW 2012 Australia.send a postcard, or a pattern"),
  helpstring(std::string(PACKAGE_STRING)),
  argcount(argc)
{
  for(int i = 0 ; i < argc; i++){
    arguments.push_back(argv[i]);
  }   
  helpstring =helpstring + " is free software.\n\nUsage :   Options\n\n --help  ---  print this text\n\n";
  helpstring = helpstring  + "--version   ---  print virgin information \n\n\n" + author;
}
 

void X3_draw::set_helpstring(){
  helpstring =get_progname_and_version() + " is free software.\n\nUsage :   Options\n\n --help  ---  print this text\n\n";
  helpstring = helpstring  + "--version   ---  print virgin information \n\n\n" + author;
}

void X3_draw::get_help(std::string& helptext){
  std::cout << helptext<< std::endl;
}

int X3_draw::drawprog(X3_draw* drawprogram){
  if (argcount > 2){
    complain_usage();
    return 1;
  }
  if (arguments[1] == "--version"){
    get_version();
    return 0;
  }
  if (arguments[1] == "--help"){
    get_help();
    return 0;
  }
 
  
  return 0;
}
