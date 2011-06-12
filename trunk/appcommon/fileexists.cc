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



#define COMPILING_STUPIDAPP 1

#include "fileexists.h"

#include "preferences.h"
#include <iostream>
#include <cstdlib>
#include <stdexcept>
#include <sstream>
#if HAVE_STRUCT_DIRENT_D_TYPE
#else
#include "unistd.h" //stat needs this
#endif

NAMESPACE_APPCOMMON{ /// or just put it in that namespace

  void snarky_clear_errno(){
    if(errno){
      int e_now = errno;
      errno = 0;
#ifdef DEBUG_META
      std::cerr <<"errno was set to indicate '" << SNARKY_STRERROR(e_now);
      if(errno){ 
	std::cerr <<"\nand again(: strerror threw an err0r\n";
      }
#endif
      errno=0;
    }
  }

  std::string get_working_dir(){
#ifdef DEBUG_FILE
    print_error_clear();
#endif
    snarky_clear_errno();
    std::string result(".");
    char* sysresult = NULL;
#ifdef _GNU_SOURCE
    //conditionalise on the availability of the functions as determined by configure, rather than feature_test_macro?
    sysresult =get_current_dir_name();
    if(sysresult){
      result = sysresult;
      free(sysresult);
      sysresult = NULL;
    }
  
#else //!_GNU_SOURCE
#if defined _BSD_SOURCE || ANDROID || _XOPEN_SOURCE >= 500
    sysresult = getcwd(NULL, 0);
    if(sysresult){
      result = sysresult;
      free(sysresult);
      sysresult = NULL;
    }
    else if(errno){
      std::cerr<<"\n"<<strerror(errno);
      errno = 0;
    }
#else //!(_BSD_SOURCE || _XOPEN_SOURCE >= 500)
    char buf[PATH_MAX +1];
    sysresult = buf;
    sysresult = getwd(sysresult);
    if(sysresult){
      result = sysresult;
    }
#endif //!(_BSD_SOURCE || _XOPEN_SOURCE >= 500)
#endif//!_GNU_SOURCE
    if(!result.size()){result = ".";}
    if(*(result.end()-1)!= PATH_SEP){ //SNARKY_DIR_SEP would be better name
      result += PATH_SEP_PTR;
    }
    return result;
  }

  void print_error_clear(){
    if(errno){
      std::cerr << "errno is set: "<< strerror(errno)<<"\nclearing it";
      errno = 0;
    }
  }

std::string get_user_home(){
const char* home = getenv("HOME");
  if(!home){throw Error("HOME not set in environment");}
  std::string h(home);
  return h;
}

/*const char* get_homepath(){
   std::string home(get_user_home());
   return home.c_str();
   }*/

  void print_error_clear(const char* caller){
    if(errno){
      std::cerr << "errno is set at beginning of "<< caller<< " : "<< strerror(errno)<<"\nclearing it";
      errno = 0;
    }
  }

  bool direxists(const char* fspath){
    if(!fspath){
      throw Error("fspath is NULL");
    }
#ifdef DEBUG_FILE
    print_error_clear("direxists");
#endif
    if(errno){errno=0;}
    DIR* dtest = opendir(fspath);
    if (dtest){
      closedir(dtest);
#ifdef DEBUG_META
      std::cerr<< "fspath "<< fspath <<" is dir";
#endif
      return true;
    }
    else if(errno == ENOENT){
      errno = 0;
      return false;
    }
    else{
      std::cerr << "eRROr opening directory: "<< fspath<< "\nerrno set---> '" << strerror(errno)<< "'\nclearing errno...";
      errno = 0;
    }
    return false;
  }

  DIR* get_apphomedir( bool create_if_missing){
    std::string dirpath(get_user_prefsdirpath());
    if(!direxists(dirpath.c_str())){
      snarky_createdir(dirpath.c_str());
    } 
    DIR* prefsdir = opendir(dirpath.c_str());
    if(!prefsdir){throw WordchopError(strerror(errno));}
    return prefsdir;
  }

  int clear_error_if(int errok){
    if (errno == errok){
#ifdef DEBUG
      std::cerr << "clearing error matching "<< SNARKY_STRERROR(errok)<< std::endl;
#endif
      errno = 0;
    }
    else{
      std::cerr << "Error '" << SNARKY_STRERROR(errno) << "doesn't match " 
		<< SNARKY_STRERROR(errok)<<"\nnot clearing it..." << std::endl;
    }
    return errno;
  }

  int snarky_createdir(const char* s){
    if(!s){
      throw Error("snarky_createdir(const char* s) => s is NULL");
    }
#ifdef DEBUG_FILE
    print_error_clear("snarky_createdir");
#endif
    errno = 0;
    if(direxists(s)){// what about some other type of file existing
      return 0; // kind a bad, but at least it's directory - might be usable
    }
    int dir_createperms = S_IRUSR |S_IWUSR| S_IXUSR| S_IRGRP| S_IXGRP |S_IROTH| S_IXOTH;
    int result = mkdir(s,dir_createperms );
    if(result){
      int e = errno;
      if(e){
	if(e == ENOENT){
	  std::cerr << "nosuch f or d - use snarky_createdirRecursive ";
	  errno = 0;
	  return -1;
	}
      }
      std::cerr << "error creating directory '" << s << "'\n";
      std::string errstr("mkdir: ");

      errstr += s;
      throw WordchopError(errstr);
    }
    return result;
  }

  bool file_regular(const char* dirpath, const char* f){
    if((!dirpath) || (!f)){
      if(!dirpath){
	throw Error("file_regular(const char* dirpath, const char* f):\ndirpath is NULL");
      }
      else{
	throw Error("file_regular(const char* dirpath, const char* f):\nf is NULL");
      }
    }
    DIR* folder = opendir(dirpath);
    if(!folder){
      std::string s("error opening directory: ");
      s += dirpath;
      std::cerr << s<< "\nbecause '"<< strerror(errno) << "'\n";
      throw WordchopError(s);
    }
    dirent* f_entry = get_direntry(folder, f);
    if(!f_entry){ 
      std::ostringstream o;
      o << "file '" << f << "' in dir '"<< dirpath <<"' doesn't exist";
      std::string errmsg = o.str();
      throw WordchopError(errmsg.c_str());
    }
    bool result = false;
    //#ifdef  _DIRENT_HAVE_D_TYPE /// glibc macro
    // may need to define _BSD_SOURCE to ensure seeing DT_REG and friends
    // on linux - how is the value of dirent::d_type determined/tested 
    //against otherwise, since linux always has that?
#if HAVE_STRUCT_DIRENT_D_TYPE
    result = (f_entry->d_type == DT_REG); //regular file
#else
    struct stat m_stat;
    std::string p(dirpath);
    if(!p.size()){ p = "."; p+= PATH_SEP_PTR;}
    else if(*(p.end() -1) != PATH_SEP){ p += PATH_SEP_PTR;}
    p += f;
    if(stat(p.c_str(), &m_stat)){
      throw std::runtime_error("stat failed");
    }
    result = S_ISREG(m_stat.st_mode);
#endif

    closedir(folder);
#ifdef DEBUG
    std::cerr <<"file_regular returning " << (result?"false" : "true");
#endif
    return result;
  }

  std::deque<std::string> get_dirlisting(const char* d){
    if(!d){
      throw Error("get_dirlisting(const char* d) => d is NULL");
    }
    DIR* directory = opendir(d);
    std::deque<std::string> result;
    if (!directory){
      std::cerr <<"no directory " << d << std::endl;
      std::cerr << strerror(errno) <<std::endl;
      errno = 0;
      return result;
    }
    std::string s;
    rewinddir(directory); // list from beginning - unnecessary
    if(errno){
      std::cerr << "\nclearing an error before calling readdir\n"<< strerror(errno);
      errno = 0;//
    }
    dirent* entry = readdir(directory);
    while(entry){
      s = entry->d_name;
      if(s != "." && s != ".."){
	result.push_back(s);
      }
      entry = readdir(directory);
    }
    if(errno){// can't determine if readdir threw error from returnval
      throw std::runtime_error(strerror(errno));
    }
    if(closedir(directory)== -1){//be good
      throw std::runtime_error(strerror(errno));
    }
    return result;
  }





  dirent* get_direntry(DIR* d, const char* name){
    if((!d) || (!name)){
      if(!d ){throw Error("get_direntry(DIR* d, const char* name) - d is NULL");}
      else{throw Error("get_direntry(DIR* d, const char* name) -name is NULL");}
    }
    rewinddir(d);
    std::string namecheck(name);
    if(errno){
      std::cerr << "\nclearing an error before calling readdir\n"<< strerror(errno);
      errno = 0;//
    }
    dirent* entry = readdir(d);
    while(entry){
      if (namecheck == entry->d_name){
	return entry;
      }
      entry = readdir(d);
    }
    if(errno){
      throw std::runtime_error(strerror(errno));
    }
    closedir(d);
    return NULL;

  }

  bool fileexists(const std::string& name){
    char* nmcopy= strdup(name.c_str());
    char* pbase = basename(nmcopy);
    char* pdir = dirname(nmcopy);
    free(nmcopy);
    std::string bname, dname;
    if(pbase){bname=pbase;}
    if(pdir){dname=pdir;}
    if(!(bname.size())){
      throw WordchopError("no filename to check");
    }
    if (!dname.size()){dname = ".";}
    return file_in_dir(dname.c_str(),bname);
  }

  bool file_in_dir(const char * dirname, const std::string& name){
    if(errno){
#ifdef DEBUG_META
      std::cerr<< ":file_in_dir,.... clearing errno"<< std::endl;
#endif
      errno=0;
    }
    DIR* directory = opendir(dirname);
    if(!directory){
      std::ostringstream ostmp;
      ostmp << " WORDCHOP::file_in_dir("<< dirname << ","<< name<< ")\nError opening directory";
      std::string s = ostmp.str();
      throw WordchopError(s);
    }
    rewinddir(directory); // list from beginning - is this necessary?
    if(errno){
      std::cerr << "\nclearing error before calling readdir: '" << strerror(errno)<<"'";
      errno = 0;
    }
    dirent* entry = readdir(directory);
    while(entry){
      std::cout << "entry-->: " << *entry << std::endl;
      if (name == entry->d_name){
	if (entry->d_type == DT_REG){// unportable - extract the stat code above into a separate function
	  closedir(directory);
	  return true;
	}
	else{
	  std::cerr << "file: " << name << std::endl;
	  std::cerr << "file type not DT_REG but --> " << int(entry->d_type)<< std::endl;
	  closedir(directory);
	  return false;
	}
      }
      entry = readdir(directory);
    }
    if(errno){
      throw std::runtime_error(strerror(errno));
    }
    closedir(directory);
    return false;
  }
  //TODO - check if d.d_type defined with _DIRENT_HAVE_D_TYPE
  //and use stat instead
  bool file_is_file(const dirent& d){
    if(!(&d)){throw Error("dirent&: reference to NULL pointer");}
#if HAVE_STRUCT_DIRENT_D_TYPE
    return d.d_type == DT_REG; 
#else
#error "!  _DIRENT_HAVE_D_TYPE ! dirent has no d.type =>use stat"
#endif
  }
  bool file_is_dir(const dirent& d){
    if(!(&d)){throw Error("dirent&: reference to NULL pointer");}
#if HAVE_STRUCT_DIRENT_D_TYPE
    return d.d_type == DT_DIR; 
#else
#error "!  _DIRENT_HAVE_D_TYPE ! dirent has no d.type =>use stat"
#endif
  }

  bool file_is_socket(const dirent& d){
    if(!(&d)){throw Error("dirent&: reference to NULL pointer");}
#if HAVE_STRUCT_DIRENT_D_TYPE
    return d.d_type == DT_SOCK; 
#else
#error "!  _DIRENT_HAVE_D_TYPE ! dirent has no d.type =>use stat"
#endif
  }
  //see serverlisten/clientconnrct.h
  int get_stream_socket(int domain, bool block){
#if defined SOCK_CLOEXEC
    if(block){ return socket(domain, SOCK_STREAM|SOCK_CLOEXEC|SOCK_NONBLOCK, 0);}
    else{    return socket(domain, SOCK_STREAM|SOCK_CLOEXEC, 0);}
#else
    return socket(domain, SOCK_STREAM,0);
#endif
  } 

  // istr must contain a valid inet4 quad next
  std::istream& operator>>( std::istream& istr, in_addr& i4address){
    unsigned int first(0), second(0), third(0), fourth(0);
    const char const_dot('.');
    char dot=0;
    istr >> first >> dot;
    if (!istr.good()|| dot != const_dot){/*do some dramatic*/}
    dot=0;
    istr >> second >> dot;
    if (!istr.good()|| dot != const_dot){/*do some dramatic*/}
    istr >> third >> dot;
    if (!istr.good()|| dot != const_dot){/*do some dramatic*/}
    istr >> fourth;
    if(first > 255|| second   > 255||  third > 255||  fourth > 255)
      {/*do some dramatic*/}
    i4address.s_addr=htonl((((((((unsigned char)first) <<8) |((unsigned char)second) )<<8 )| 
			     ((unsigned char)third))<<8 ) | fourth) ;
    return istr;
  }

#define IS_LOCAL_SOCKET(sokkaddr) (((sockaddr_in&)(sockaddr_in)).sa_family == PF_LOCAL)
#define IS_INET4_SOCKET(sokkaddr) (((sockaddr_in&)(sockaddr_in)).sa_family == PF_INET)
#define IS_INET6_SOCKET(sokkaddr) (((sockaddr_in&)(sockaddr_in)).sa_family == PF_INET6)

  bool path_isAbsolute(const char* path){
    if(!path){throw WordchopError("NULL char* passed to path_isAbsolute");}
    if(!strlen(path)){return false;}
#ifdef _WINDOWS_
    return *(path +1) == ':';
#else
    return *path == '/';
#endif
  }


  std::ostream& operator<<(std::ostream& os,const dirent& d){
    if(!(&d)){throw Error("dirent&: reference to NULL pointer");}
 
    os << "dirent ... \nname: "<< d.d_name <<"\ntype: ";
    switch(d.d_type){
    case DT_DIR:
      return os << "directory"<< std::endl;
    case DT_REG:
      return os << "regular file"<< std::endl;
    case	DT_CHR:
      return os << "character device - a tty etc"<< std::endl;
    case DT_BLK:
      return os << "block device"<< std::endl;
    case	DT_LNK:
      return os << "link / shortcut"<< std::endl;
    case DT_SOCK :
      return os << "socket"<< std::endl;
    case DT_WHT: 
      return os << "what is DT_WHT: ie: "<<  DT_WHT<<std::endl;
    default:
      return os << int(d.d_type) << std::endl;
    }
  }

  std::ostream& operator<<(std::ostream& os, const std::deque<std::string>& d){
    typedef std::deque<std::string>::const_iterator iterator;
    for(iterator i = d.begin(); i != d.end(); i++){
      os << *i<< "\n";
    }
    return os << std::endl;
  }
} //namespace NAMESPACE (or whatever)


