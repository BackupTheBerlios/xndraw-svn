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



/**
 * @file fileexists.h 
 * reading DIRs to see what is There
 */

#ifndef SNARKY_APPCOMMON_FILEEXISTS_H 
#define SNARKY_APPCOMMON_FILEEXISTS_H

#define DEBUG_FILE 1

#ifdef HAVE_CONFIG_H
#include "appcommonconfig.h"
#endif

#ifndef NAMESPACE_APPCOMMON
#define NAMESPACE_APPCOMMON namespace APPCOMMON
#define NAMESPACE APPCOMMON
#endif

#ifndef SNARKY_STRERROR
#define SNARKY_STRERROR strerror
#endif

#ifndef CTYPE_FACET 
#define CTYPE_FACET std::use_facet<std::ctype<CHARTYPE> >
#endif

#ifndef TYPE_STD_TYPE 
#define TYPE_STD_TYPE typedef std::basic_string<CHARTYPE> std_type
#endif




/*** fromm the autoconf manual
 -- Macro: AC_HEADER_DIRENT
     Check for the following header files.  For the first one that is
     found and defines `DIR', define the listed C preprocessor macro:

     `dirent.h'     `HAVE_DIRENT_H'
     `sys/ndir.h'   `HAVE_SYS_NDIR_H'
     `sys/dir.h'    `HAVE_SYS_DIR_H'
     `ndir.h'       `HAVE_NDIR_H'

     The directory-library declarations in your source code should look
     something like the following:
*/
#include <sys/types.h>
#include <sys/stat.h>

#if HAVE_GLOB_H
#include <glob.h>
#endif
#include <cerrno>
#include <cstring> // strerror
#include <cstdlib>
#include <string>
#include <deque>
#include <vector>
#include <locale>
#include <iosfwd>
#include <fstream>
#include "exceptions.h"
#include "stupidiorefs.h"
#if defined ANDROID
namespace{
#include <stdlib.h>
#include <malloc.h>
}
#endif
#include <cstdlib>
#include <cctype>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#ifdef HAVE_DIRENT_H
# include <dirent.h>
# define NAMLEN(dirent) strlen ((dirent)->d_name)
#else
# define dirent direct
# define NAMLEN(dirent) ((dirent)->d_namlen)
# ifdef HAVE_SYS_NDIR_H
#  include <sys/ndir.h>
# endif
# ifdef HAVE_SYS_DIR_H
#  include <sys/dir.h>
# endif
# ifdef HAVE_NDIR_H
#  include <ndir.h>
# endif
#endif

//Todo : supply my own dirname
#include <libgen.h> // dirname


#if defined _BSD_SOURCE || _XOPEN_SOURCE >= 500
#include <fcntl.h>


#endif


///this kludge assumes a lot about the file - add a mode member, since it closes and opens the file
NAMESPACE_APPCOMMON{

  void snarky_clear_errno();

  enum{
    SNARKYPATH_NETWORK = 1,
    SNARKYPATH_UNIX = 2,
    SNARKYPATH_WINDOWS=4
  };
  std::istream& operator>>( std::istream& istr, in_addr& i4address);
  
template <typename CHARTYPE>
std::basic_string<CHARTYPE> path_join(const std::basic_string<CHARTYPE>& dirprefix, 
				      const std::basic_string<CHARTYPE>& pathtojoin, 
				      std::locale loc,
				      int flags =SNARKYPATH_NETWORK ){
  //this doesn't really handle windows - some re magic might be the thing
  typedef std::basic_string<CHARTYPE> std_type;
  if(!dirprefix.size()){return pathtojoin;}
  std_type result (dirprefix);
  CHARTYPE sep;
  if(flags & SNARKYPATH_NETWORK || !(flags & SNARKYPATH_WINDOWS)){
    sep = std::use_facet<std::ctype<CHARTYPE> >(loc).widen('/');
  }
  else{
    sep =  std::use_facet<std::ctype<CHARTYPE> >(loc).widen('\\');
  }
  std::basic_ostringstream<CHARTYPE> OSTMP;
  OSTMP << sep;

  if(*(result.end()-1) != sep){
    result += OSTMP.str();
}
  if(pathtojoin.size() && *(pathtojoin.begin()) == sep){
  //TODO: move some versatile functions in appcommon, eg widenstring
    //std::ostringstream ostmp;
    //ostmp <<"attempt to join absolute path '"<< widenstring<CHARTYPE, char>(pathtojoin)
    //	  <<"' to directory '" <<widenstring<CHARTYPE, char>(dirprefix)<<"'"<<std::endl
    std::cerr <<"attempt to join absolute path '";
    get_CERR<CHARTYPE>()<<pathtojoin;
    std::cerr <<"' to directory '";
    get_CERR<CHARTYPE>()<<dirprefix;
    std::cerr <<"'"<< std::endl;
    throw PathError("attempt to join absolute path - see the error trac");
  }
  return result +pathtojoin;
}

template <typename CHARTYPE>
int snarky_wget_file(const std::basic_string<CHARTYPE>& remoteuri, const std::basic_string<CHARTYPE>& workdir){
  TYPE_STD_TYPE;
  int result(0); 

  //chdir to output directory
 int chdir_return(0);
#if defined  _BSD_SOURCE || _XOPEN_SOURCE >= 500 
      int curdir = open(".", O_RDONLY);
      if(curdir == -1){
	throw SystemError("open returned an error");
      }
      int newdir = open(workdir.c_str(), O_RDONLY);
      if(newdir == -1){
	throw SystemError("open returned an error");
      }
      chdir_return= fchdir(newdir);
      if(chdir_return == -1){
	throw SystemError("fchdir returned an error");
      }
#else
      const char* curdir= getcwd(NULL, 0);
      chdir_return= chdir(workdir.c_str());
      if(chdir_return == -1){
	throw SystemError("chdir returned an error");
      }
#endif

      std::string wget = "wget --random-wait ";
      //std_type wget_path_options = snarky_get_wgetstring();
      std::basic_ostringstream<CHARTYPE> ostmp;
      ostmp << wget << remoteuri;
      wget = ostmp.str();
      int sysret = std::system(wget.c_str());//only for char 
      result= sysret;
      //problem: can't use UriBase to split filename elementt
      //      result.second =path_join(workdir, 
#if defined  _BSD_SOURCE || _XOPEN_SOURCE >= 500 
      chdir_return = fchdir(curdir);
      if(chdir_return == -1){
	throw SystemError("fchdir returned an error getting back to curdir");
      }
      if(close(curdir)==-1){
	throw SystemError("error closing fd");
      }
      if(close(newdir)==-1){
	throw SystemError("error closing fd");
      }
#else
      chdir_return =chdir(curdir);
      if(chdir_return == -1){
	throw SystemError("chdir returned an errorgetting back to curdir");
      }
      ///TODO: choose whether to use char* or void* in configure
      free(const_cast<char*>(curdir));
#endif
      return result;
}
 
template <typename CONTAINER>
 void glob_files(CONTAINER& container, const char* s_glob, const char* workingdir){
      //TODO: change 1 varname to 2 so as not to declare diiferently typed 'curdir' depending on cpp preprocessor
      int chdir_return(0);
#if defined  _BSD_SOURCE || _XOPEN_SOURCE >= 500 
      int curdir = open(".", O_RDONLY);
      if(curdir == -1){
	throw WordchopError("open returned an error");
      }
      int newdir = open(workingdir, O_RDONLY);
      if(newdir == -1){
	throw WordchopError("open returned an error");
      }
      chdir_return= fchdir(newdir);
      if(chdir_return == -1){
	throw WordchopError("fchdir returned an error");
      }
#else
      const char* curdir= getcwd(NULL, 0);
      chdir_return= chdir(workingdir);
      if(chdir_return == -1){
	throw WordchopError("chdir returned an error");
      }
#endif
      glob_t gbuf;
      gbuf.gl_offs=0;
      int rv = glob(s_glob,GLOB_DOOFFS, NULL, &gbuf);
      if(!rv){
	for(unsigned int i=0; i< gbuf.gl_pathc;i++){
	  container.push_back(std::string(gbuf.gl_pathv[i]));
	}
      }
      else if(rv == GLOB_NOMATCH){
	std::cerr << "no files match '"<< s_glob << "'"<<std::endl;
      }
      else if (rv== GLOB_NOSPACE){
	std::string s("glob ran out of memory");
	throw NoMemoryError(s);
      }
      else{
	std::cerr << "an error occurred while globbing"<<std::endl;
      }
#if defined  _BSD_SOURCE || _XOPEN_SOURCE >= 500 
      chdir_return = fchdir(curdir);
      if(chdir_return == -1){
	throw WordchopError("fchdir returned an error getting back to curdir");
      }
      if(close(curdir)==-1){
	throw WordchopError("error closing fd");
      }
      if(close(newdir)==-1){
	throw WordchopError("error closing fd");
      }
#else
      chdir_return =chdir(curdir);
      if(chdir_return == -1){
	throw WordchopError("chdir returned an errorgetting back to curdir");
      }
      ///TODO: choose whether to use char* or void* in configure
      free(const_cast<char*>(curdir));
#endif
    }
    

  int snarky_createdir(const char* dirpath);
  bool direxists(const char* fspath);
  bool file_regular(const char* dirpath, const char* f);
DIR* get_apphomedir(bool create_if_missing = true);
 const char* get_homepath();
  std::string get_user_home();
  //DEBUG functions
  void print_error_clear();
  void print_error_clear(const char* caller);
  int clear_error_if(int errok);
std::deque<std::string> get_dirlisting(const char* directory);
    bool path_isAbsolute(const char* path);
  std::ostream& operator<<(std::ostream& os, const std::deque<std::string>& d);
  bool fileexists(const std::string& name);
  bool file_in_dir(const char* dirname, const std::string& name);

  bool file_is_file(const dirent& d);
  bool file_is_dir(const dirent& d);

  dirent* get_direntry(DIR* d, const char* name);
  std::ostream& operator<<(std::ostream& os,const dirent& d);



  template <typename CHARTYPE>
  bool stream_good(const std::basic_istream<CHARTYPE>& f, const char* caller){
    if(f.good()){
      return true;
    }
    int fstate = f.rdstate();
std::cerr<< caller << " : error with file... ";
    if(fstate | std::ios_base::failbit){
      std::cerr<< "failbit set...";
    }
if(fstate | std::ios_base::badbit){
  std::cerr<< "badbit set...";
    }
  
  if(errno){
    std::cerr<< SNARKY_STRERROR(errno) << std::endl;
  }
  return false;
}

  template <class CHARTYPE>
    bool file_is_text(std::basic_ifstream<CHARTYPE>& f, const std::string& fname,std::locale loc){
    typedef typename std::basic_ifstream<CHARTYPE>::traits_type traits_type;
    typedef typename traits_type::int_type int_type;
    register int_type cache;
    register CHARTYPE c;
    for (int i = 0; i < 100; i++){
      cache = f.get();

      if (cache == traits_type::eof()){
	f.close();
	f.open(fname.c_str());//close and reopen to return pointer to start of file!
	return true;
      }
      c = traits_type::to_char_type(cache);
    if (! std::isprint(c, loc)){
      f.close();
f.open(fname.c_str());
      return false;
    }
    }
    f.close();
f.open(fname.c_str());
    return true;
  }
  
  
  
  //wrap in class for access to a set locale
  template <class CHARTYPE>
    std::vector<std::basic_string<CHARTYPE> > list_dir(DIR* d, std::basic_string<CHARTYPE> s_unused , int filetype = DT_REG){ 
    /** list an open  directory.
     * @param d --> directory to list
     * @param s_unused --> a clue for the compiler(for template function type deduction)
     * @param filetype -->type of the file:default is DT_REG ie regular file
     * @return a vector of strings with the file names
     **/
    typedef std::basic_string<CHARTYPE> std_type;
    typedef std::vector<std_type> return_type;
    return_type result;
    std_type s;
    CHARTYPE* tmp = NULL;
    rewinddir(d); // list from beginning
    dirent* entry = readdir(d);
    while(entry){
      std::string  c_s(entry->d_name);
      if(!(filetype & entry->d_type)){
	continue;
      }
     
      std::basic_ostringstream<CHARTYPE> ostmp;
      for(std::string::const_iterator wit = c_s.begin(); wit != c_s.end(); wit++){
	ostmp << std::use_facet<std::ctype<CHARTYPE> >(get_COUT<CHARTYPE>().getloc()).widen(*wit);
      }
      s = ostmp.str();
      if(s != "." && s != ".."){
	result.push_back(s);
      }
      entry =  readdir(d);
  }
    return result;
  }
  

template <class CHARTYPE>
    std::vector<std::basic_string<CHARTYPE> > list_ofdirs(DIR* d, std::basic_string<CHARTYPE> s_unused ){
    return  list_dir<CHARTYPE>(d, s_unused, DT_DIR);
  }


}
#endif //SNARKY_APPCOMMON_FILEEXISTS_H
