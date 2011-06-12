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

#ifndef XNDRAW_X3DRAW_H
#define XNDRAW_X3DRAW_H
#include "exceptions.h"
#if HAVE_CONFIG_H
#include "config.h"
#endif


#include "xndrawexceptions.h"
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <dirent.h>
#include <exception>
#include <glibmm/thread.h>
#include <glibmm/object.h>
#include <glibmm/optioncontext.h>
#include "drawcolour.h"

#include "preferences.h"
//name disagrees with python usage, where:
// os.sep == "/" or "\\"
// and os.path_sep(or something) == ":" and ";" or whatever
#ifndef PATH_SEP
#ifdef _WINDOWS_
#define PATH_SEP '\\'
#else// !_WINDOWS_
#define PATH_SEP '/'
#endif // !_WINDOWS_
#endif //!PATH_SEP

#ifndef PATH_SEP_PTR
#ifdef _WINDOWS_
#define PATH_SEP_PTR "\\"
#else// !_WINDOWS_
#define PATH_SEP_PTR "/"
#endif // !_WINDOWS_
#endif

namespace xNDRAW{

  //  const char* get_user_prefsdirpath();

  DIR* get_xndrawhomedir(const char* dirpath, bool create_if_missing = true);

  std::string get_application_home();

  std::vector<std::string> list_dir(DIR* d);
  //TODO: - use/share preference classes from stupidword
  //  typedef WORDCHOP::Preferencebase Preferencebase;
  // typedef WORDCHOP::Preferences Preferences;

  //template <typename VALUETYPE>
  //typedef WORDCHOP::Preference<VALUETYPE> Preference<VALUETYPE>;

  
 
    
    class commandline_opts: public Glib::OptionContext{
      int argcount;
      std::vector<std::string>  argvalues;
    public:
      commandline_opts(int argc, char** argv): argcount(argc) {
	for (int i = 0; i < argcount; i++){
	  argvalues.push_back(argv[i]);
	}
      }
      commandline_opts(const commandline_opts& src):argcount(src.argcount), argvalues(src.argvalues){}
      virtual ~commandline_opts(){}
      
      int get_argc(){return argcount;}
      std::vector<std::string>&  get_argv_strings(){return argvalues;}
      
      char** get_argv();
      
    };
    
    template<class SINGLE, class ARG> class Singletonise : public Glib::Object{
      Singletonise(const Singletonise&);
      Singletonise& operator=(const Singletonise&);
      static SINGLE* ptr_to_Instance;
    protected:
      Singletonise() {}
      virtual ~Singletonise() {}
    public:
      static SINGLE& instance() {
	if (ptr_to_Instance == NULL){
	  static SINGLE theInstance;
	  ptr_to_Instance = &theInstance;
	}
	return *ptr_to_Instance;
      }
      static SINGLE& instance(ARG& arg){
	if (ptr_to_Instance == NULL){
	  static SINGLE theInstance(arg);
	  ptr_to_Instance = &theInstance;
	}
	return *ptr_to_Instance;
      }
      
      
    };
    
    
  class AboutPackage {
    const std::string package;
    const std::string author;
    const std::string email;
    const std::string copyleft_dates;

    std::string get_short_info()const;
    std::string get_non_warranty()const;
    std::string get_COPYING()const;


  };

  

    class X3_draw:public Singletonise<X3_draw, commandline_opts> {
    protected:
      friend class Singletonise<X3_draw, commandline_opts>;
      
      
      std::string author;
      std::string helpstring;
      
      Preferences preferences;
      int argcount;
      std::vector<std::string> arguments;
      static bool X3_draw_is_verbose;
      //static unsigned int Ncoords_base;
      Glib::Mutex mutex;
      X3_draw operator=(X3_draw& src);//no no no
      
      
    public:
      
      bool  get_verbose(){
	return X3_draw_is_verbose;
      }
      
      void set_verbose(bool verbose_or_no){
	X3_draw_is_verbose = verbose_or_no;
      }
      
      void shut_up_maybe(){
	X3_draw_is_verbose =!X3_draw_is_verbose;
      }
    protected: 
      const Preferencebase& get_preference(const std::string& name)const {
	return preferences.get_preference( name);
      }
      
      Preferencebase& get_preference(const std::string& name){
	return preferences.get_preference( name);
      }
    public:
      template <class VALUETYPE>
      VALUETYPE get_prefvalue(const std::string& name){
	//overload for any VALUETYPE withoout default ctor...
	VALUETYPE result;
	preferences.get_value(name, result);
	return result;
      }
	
      // gcc 4.2 can't deduce template param from return type expected - coercion could be involved...,
      //so we give a template argument
      template <class VALUETYPE>
      VALUETYPE get_prefvalue(std::string& name, VALUETYPE compilehint = VALUETYPE() ){
	preferences.get_value(name, compilehint);
	return compilehint;
      }

      template <class VALUETYPE>
      VALUETYPE get_prefvalue(const char* prefname, VALUETYPE compilehint = VALUETYPE() ){
	std::string s(prefname);
	return get_prefvalue(s, compilehint);
      }


      template <class VALUETYPE>
      void set_prefvalue(const std::string& name, const VALUETYPE& v){
	preferences.set_value(name, v);
      }

      template <class VALUETYPE>
      void set_prefvalue(const char* name, const VALUETYPE& v){
	std::string s(name);
	set_prefvalue(s, v);
      }

      bool have_pref(const char* prefname)const{return preferences.have_pref(prefname);}
      bool have_pref(const std::string& prefname)const{return preferences.have_pref(prefname);}
      void set_colourpref(const char* name, double r, double g, double b, double a = 1.0);

      int drawprog(X3_draw* drawprogram);///?what?
    protected:
      X3_draw();
      X3_draw(int argc, char** argv);
      X3_draw(commandline_opts& opt);
      
      X3_draw(X3_draw& src);//not allowed
      X3_draw operator=(const X3_draw&);//not allowed
      virtual    ~X3_draw(){}
    public:
      std::string get_author(){
	return author;
      }
      std::string get_version(){
	return std::string(PACKAGE_VERSION);
      }
      void set_helpstring();
      void get_help(std::string& helptext);
      std::string get_progname(){
	return std::string(PACKAGE_NAME);
      }
      
      std::string get_progname_and_version(){
	return std::string(PACKAGE_STRING);
      }
      
      
      void load_preferences(const char* filename = NULL);
      //TODO: a file parser to read prefsfile, or find the right portable xml parsing lib
    

    void get_help(){
      std::cout << helpstring << std::endl;
    }

    int get_argc(){
      return argcount;
    }

    char** get_argv(){
      char** temp= new char*[argcount];
      for (int i = 0; i < argcount; i++){
	temp[i] =const_cast<char*>(arguments[i].c_str());
      }
      return temp;
    }
    
     void std_out(std::ostringstream& os) {

    std::cout << os.str();
  }

    void complain_usage(){
      using namespace std;
      cout << "______________________________________\nUSAGE\n\n______________________" << endl;
      get_help();
    }
  };
  
}
  
#endif /*XNDRAW_X3DRAW_H*/
