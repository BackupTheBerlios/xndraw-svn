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
#include "forkexec.h"
//will need some ifdefs here, guard macros
#include <sys/types.h>
#include <sys/wait.h>

#ifndef DIR_SEP_PTR
#ifdef WINDOWS_NT
#define DIR_SEP_PTR "\\"
#else
#define DIR_SEP_PTR "/"
#endif
#endif

#ifndef DIR_SEP
#ifdef WINDOWS_NT
#define DIR_SEP '\\'
#else
#define DIR_SEP '/'
#endif
#endif

#ifndef EXEEXT
#ifdef WINDOWS_NT
#define EXEEXT ".exe"
#else
//mollify compiler
#define EXEEXT ""
#endif
#endif

NAMESPACE_APPCOMMON{

Forker* Forker::the_forker = NULL;

void Forker::close_fds(){
      close_child_fds();
      close_parent_fds();
    }
int Forker::get_pipereadfd(){

      if(process_domain == PROCESS_PARENT){
	return info.parent_read_fd;
      }
      else if(process_domain == PROCESS_CHILD){
	return info.child_read_fd;
      }
      else{
	throw Error("not initialised proprely:fork() not called?");
      }
    }

bool Forker::child_continued(int status){
#ifdef WIFCONTINUED
      return WIFCONTINUED(status);
#else
      return false;
#endif
    }

int Forker::child_stopsignal(int status){
#ifdef  WSTOPSIG
      if(child_stopped(status)){
	return WSTOPSIG(status);
      }
      throw Error("child not stopped");
#else
      throw Error("WSTOPSIG not available");
#endif
    }

bool Forker::child_stopped(int status){
#ifdef WIFSTOPPED
      return WIFSTOPPED(status);
#else
      return false;
#endif
    }

int Forker::child_termsignal(int status){
#ifdef WTERMSIG
      if(child_terminated(status)){

	return WTERMSIG(status);
      }
      throw Error("child not terminated");
#else
      throw Error("not available");
#endif
    }

bool Forker::child_terminated(int status){
#ifdef WIFSIGNALED
      return WIFSIGNALED(status);
#else
      return false;
#endif
    }

int Forker::child_exit_status(int status){
#ifdef WEXITSTATUS
      if(child_exited(status)){
	return WEXITSTATUS(status);
      }
      throw Error("child did not exit");
#else
      throw Error("not availavlr:");
#endif
    }

 bool Forker::child_exited(int status){
      //status rv from wait*()
#ifdef  WIFEXITED
      return WIFEXITED(status);
#else
      return false;
#endif
    }

bool Forker::child_dumped_core(int status){
#ifdef WCOREDUMP
  if(child_terminated(status)){
     return WCOREDUMP(status);
    }
    throw Error("child not terminated");
#else
      return false;
#endif
    }

 bool Forker::have_WIFEXITED(){
#ifdef WIFEXITED
      return true;
#else
      return false;
#endif
 }

 bool Forker::have_WIFSIGNALLED(){
#ifdef WIFSIGNALLED
      return true;
#else
      return false;
#endif
 }

 bool Forker::have_WCOREDUMP(){
#ifdef WCOREDUMP
      return true;
#else
      return false;
#endif
 }

 bool Forker::have_WIFCONTINUED(){
#ifdef WIFCONTINUED
      return true;
#else
      return false;
#endif
 }

 bool Forker::have_WIFSTOPPED(){
#ifdef WIFSTOPPED
      return true;
#else
      return false;
#endif
    }

std::pair<pid_t,int> Forker::waitchild(int options){
      typedef std::pair<pid_t,int> result_type;
      if(process_domain != PROCESS_PARENT){
	throw Error("must be PROCESS_PARENT to call waitchild()");
      }
      if(info.child==-1){
	throw Error("no child!");
      }
      int status(0);
      pid_t res= waitpid(info.child, &status,options);
      return result_type(res, status);
    }

int Forker::get_pipewritefd(){

      if(process_domain == PROCESS_PARENT){
	return info.parent_write_fd;
      }
      else if(process_domain == PROCESS_CHILD){
	return info.child_write_fd;
      }
      else{
	throw Error("not initialised proprely:fork() not called?");
      }
    }

Forker::Forker(bool readchild, bool readparent):
    process_domain(PROCESS_NONE),
      self_id_(-1),
      with_pipe(readchild),
      pipefromparent(readparent)
      {}

 Forker* Forker::create(bool pipefromC, 
			bool pipefromP,
			bool callfork){
   Forker* res=new Forker( pipefromC, pipefromP);
      Forker& mainforker= get_the_forker();
      mainforker.add_instance(res);
      if(callfork){
	res->do_fork();
      }
      return res;
    }

void Forker::close_unused_fds(){
      if(process_domain == PROCESS_PARENT){
	close_child_fds();
      }
      else if (process_domain == PROCESS_CHILD){
	close_parent_fds();
      }
      else{
	throw Error("not properly initialised");
      }
    }

 void Forker::close_parent_fds(){
      if(info.parent_read_fd !=-1){
	close(info.parent_read_fd);
	info.parent_read_fd=-1;
      }
      if(info.parent_write_fd!=-1){
	close(info.parent_write_fd);
	info.parent_write_fd =-1;
      }
    }

void Forker::close_child_fds(){
      if(info.child_read_fd !=-1){
	close(info.child_read_fd);
	info.child_read_fd=-1;
      }
      if(info.child_write_fd!=-1){
	close(info.child_write_fd);
	info.child_write_fd =-1;
      }
    }

void Forker::create_pipes(){
      int fds[2];
      if(with_pipe){
	if(pipe(fds)==-1){throw Error("pipe()");}
	info.parent_read_fd =fds[0];
	info.child_write_fd=fds[1];
      }
      if(pipefromparent){
	if(pipe(fds)==-1){throw Error("pipe()");}
	info.child_read_fd =fds[0];
	info.parent_write_fd=fds[1];
      }
    }

Forker& Forker::get_the_forker(){
      if(! the_forker){
	static Forker f;
	the_forker =&f;
      }
      return *the_forker;
    }


 void Forker::add_instance(Forker* newinst){
      if(!newinst){
	throw Error("NULL");
      }
      if(!instances){instances = new std::deque<Forker*>();}
      instances->push_back(newinst);
    }
void Forker::set_info(int forkres){
      if( forkres <0){
      throw Error("fork failed");
      }
      self_id_=getpid();
      if( forkres > 0){ 
	info.parent=self_id_;
	info.child=  forkres;
	process_domain=PROCESS_PARENT;
      }
      else{
	info.child= self_id_;
	info.parent=getppid();
	process_domain=PROCESS_CHILD;
	//other_id= getppid();
      }
    }

pid_t Forker::do_fork(){
      create_pipes();
      if (fork_called()){
      //throw an error - fork already called
	throw Error("fork already called: create a new forker");
      }
    pid_t res= fork();
    set_info(res);//throws error if fork() failed
    close_unused_fds();
    return res;
  }


std::string get_python_executable(){
    typedef std::deque<std::string>::const_iterator  const_deque_iterator;
  const char* envpython_ = getenv("PYTHON");
    if(envpython_){
      std::string envpython(envpython_);
      if (fileexists(envpython)){
	return envpython;
      }
    }
    std::string dir_sep(DIR_SEP_PTR);
    std::string sep(PATH_SEP_PTR);
    std::string pythonexe = "python";
    //could alsoi glob/regex for python[0-9][\.][0-9]
    //std::string pythonversionexe =pythonexe + PYTHON_VERSION;
    std::string exeext = EXEEXT;
    if(exeext.size()){
      if(!(*(exeext.begin()) == '.')){
	pythonexe += ".";
	//pythonversionexe+= ".";
      }
      pythonexe +=exeext;
      //pythonversionexe+=exeext;
    }
    //look in environment
    char* pythonenv = NULL;
      
    //look in cwd
    std::string thisdir(".");
    thisdir+= dir_sep;
    std::string p =  thisdir +pythonexe;
    if(fileexists(p)){return p;}
    // p=thisdir +pythonversionexe;
    //   if(fileexists(p)){return p;}
    typedef std::string::const_iterator const_iterator;
    typedef std::deque<std::string>::const_iterator const_deque_iterator;

    //look in the PATH, if one is set    
    pythonenv =  getenv("PATH");
    if(pythonenv){ 
      std::string pathenv(pythonenv);
      pythonenv = NULL;
      std::deque<std::string> pathdirs;
      const_iterator last_mark = pathenv.begin();
      const_iterator w = pathenv.begin();
      for(; w!= pathenv.end();w++){
	if (*w == PATH_SEP){
	  std::string pathstr(last_mark, w);
	  last_mark =w ;
	  if((w+1) != pathenv.end()){last_mark++;/*skip 'sep'*/}
	  pathdirs.push_back(pathstr);
	}
      }
      if(last_mark !=w){
	std::string pathstr(last_mark, w);
	pathdirs.push_back(pathstr);
      }
      for(const_deque_iterator pathiter = pathdirs.begin(); pathiter != pathdirs.end();pathiter++){
	std::string p = *pathiter;
	std::string p1(p);
	if(!p.size()|| p == "."){continue;/*already looked at '.'*/}
	if(*(p.end()) != DIR_SEP){p+= dir_sep;}
	//should be looking for pythonversionexe first
	p+=pythonexe;
	//p1 += pythonversionexe;
	//if(fileexists(p1)){return p1;} 
	if(fileexists(p)){return p;} 
      }
    }
#ifdef LINGO_ON_DARWIN
    //  ../MacOS/lingo sets PYTHON
    std::string framework_python ="/Library/Frameworks/Python.framework/Versions/";
    //also $HOME/Library/Frameworks/Python.framework/Versions/"etc
    //and /System/Library/Frameworks/Python.framework/Versions/"etc
    framework_python+= PYTHON_VERSION;

    framework_python+= "/bin/python";
    if(fileexists(framework_python)){
      return framework_python;
    }
#else
#ifdef LINGO_ON_WINDOWS
    //run around screaming
    //$HOMRDRIVE\Program Files\Python$
#else
#ifdef LINGO_ON_BSD
    return std::string("/usr/local/bin/python");
#endif //!LINGO_ON_BSD - test / development system
#endif //LINGO_ON_WINDOWS
#endif

    //getting desperate now
      pythonenv = getenv("PYTHONHOME");
    if(pythonenv){
      std::string p(pythonenv);
      std::string p1(pythonenv);

      pythonenv = NULL;
      if(p.size() && p != "."){
	if(*(p.end() -1) != DIR_SEP){
	  p += DIR_SEP_PTR;
	  p1 += DIR_SEP_PTR;
	}
	p+= "bin";
	//p1 +=  "bin";
	p+= DIR_SEP_PTR;
	//p1 +=  DIR_SEP_PTR;
	p += pythonexe;
	//p1 +=  pythonversionexe;
	if(fileexists(p)){
	  return p;
	}
	//if(fileexists(p1)){
	//	  return p1;
	//	}
      }
    }
    throw Error("can't find python exe");
  }

}
