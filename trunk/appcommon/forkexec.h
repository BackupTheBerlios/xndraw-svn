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
 * @file forkexec.h 
 * fork(), wait() etc
 */

#ifndef APPCOMMON_FORKEXEC_H 
#define APPCOMMON_FORKEXEC_H



#ifdef HAVE_CONFIG_H
#include "appcommonconfig.h"
#endif

#include "fileexists.h"
#include "exceptions.h"

NAMESPACE_APPCOMMON{

  enum ForkProcess{
    PROCESS_NONE=0,
    PROCESS_PARENT=1,
    PROCESS_CHILD=2
  };

  struct ForkInfo{ 
  ForkInfo():parent(-1),child(-1),
      parent_read_fd(-1), 
      parent_write_fd(-1),
      child_read_fd(-1),
      child_write_fd(-1){}
  ForkInfo(pid_t parent_, pid_t child_):parent(parent_),child(child_),
      parent_read_fd(-1), 
      parent_write_fd(-1),
      child_read_fd(-1),
      child_write_fd(-1){}
    bool initialised()const{
      return (parent >0) && (child >0);
    }
    pid_t parent;
    pid_t child;
    int parent_read_fd;
    int parent_write_fd;
    int child_read_fd;
    int child_write_fd;
  };

  class Forker{
    // bool fork_called;
  public:
    //typedef ForkProcess::PROCESS_NONE PROCESS_NONE;
    //typedef ForkProcess::PROCESS_PARENT PROCESS_PARENT ;
    //typedef ForkProcess::PROCESS_CHILD PROCESS_CHILD;
 
    static Forker* the_forker;
    ForkProcess process_domain;//Forker represents parent or child?
    
    pid_t self_id_; //pid
    ForkInfo info; 
    bool with_pipe;
    bool pipefromparent;
    std::deque<Forker*>* instances;//only the main instance gets one
  protected:
    Forker(bool readchild=true, bool readparent=false);

  public:
    ~Forker(){
      close_fds();
    }
    //what if the child wants to fork() again - 
    //need multiple copies
    Forker* create(bool pipefromC=true, 
		   bool pipefromP=false,
		   bool callfork=false); 
		   
    void close_unused_fds();
    void close_parent_fds();
    void close_child_fds();
    void close_fds();

    void create_pipes();

    void add_instance(Forker* newinst);
    static Forker& get_the_forker();

    /*
      options to waitpid etc (as of linux2.6.32)
      WNOHANG     return immediately if no child has exited.

      WUNTRACED   also return if a child has stopped (not traced via ptrace)
      WCONTINUED (+ return if a stopped child has been resumed by delivery of SIGCONT.
      /////////////////////////////////////////
      macros to call on status
      WIFEXITED(status) - true  child  terminated  normally, 
      ie by calling exit(3) or_exit(2), or by returning from main().
      WEXITSTATUS(status)-   exit status of child. ie the smallest  8 bits of rv 
      or argument to exit()/_exit().   only use if WIFEXITED returned true.
      WIFSIGNALED(status) true if the child process was terminated by a signal.
      WTERMSIG(status)-signal that terminated process -only use if WIFSIGNALED true.
      WCOREDUMP(status) -did the child core dump.    only use if  WIFSIGNALED  true.
      not portable, so enclose in #ifdef WCOREDUMP ... #endif
      WIFSTOPPED(status) - true if the child process was stopped by a signal; 
      only possible if call was done using WUNTRACED or child  is  being  ptrace()d
      WSTOPSIG(status) - signal which stopped the child only call if WIFSTOPPED true.
      WIFCONTINUED(status)- (since Linux 2.6.10) was process was resumed by SIGCONT.
    */

    //CALL macros on wait(pid)?() status out_argument
    static bool have_WIFSTOPPED();
    static bool child_stopped(int signal);

    static int child_stopsignal(int signal);
    static bool have_WIFCONTINUED();
    static bool child_continued(int signal);
    static bool have_WCOREDUMP();
    
    static bool child_dumped_core(int status);
    static bool have_WIFSIGNALLED();
    static bool child_terminated(int status);
    static int child_termsignal(int status);
    static bool have_WIFEXITED();
    static bool child_exited(int status);

    static int child_exit_status(int status);
    ///

    std::pair<pid_t,int> waitchild(int options);
    ///
    int get_pipereadfd();
    int get_pipewritefd();
    
    pid_t do_fork();

    void set_info(int forkres);

    bool fork_called()const{return info.initialised();}
    pid_t self_id()const{return self_id_;} 
  };
  //
} //NAMESPACE_APPCOMMON
#endif


