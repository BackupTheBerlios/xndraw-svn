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

#ifndef XNDRAW_XNCLIENTSERVER_H
#define XNDRAW_XNCLIENTSERVER_H

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include "socketclass.h"
#include "serverlisten.h"
#include "clientconnect.h"
#include <glibmm/thread.h>
#include <glibmm/threadpool.h>
namespace xNDRAW{

  class Conversation{
    //look at  irc

  };

  class NetMessage{
    typedef int64_t system_identifier_type;//numeric in4/in6 address?
    //what about case of 2 machines on same private address eg-192.168.0.11
    //on 2 different private subnets(connected to internet)
    //can't use private address, but also can't use their gateway
    //as 2 machines behind the same gateway would have identical addresses
    //maybe generate a cryptok key based on their mac address
    //as thats supposed to be unique
    system_identifier_type src_machine;
    system_identifier_type dest_machine;
    pid_t src_process;
    pid_t dest_process;
    uid_t src_user;
    uid_t dest_user;
    long msg_id;
    std::string title;
  };
   



template<typename SHAREDDATA>
class ShareGroup{
  //everyone has their own copy per collaboration,
  //with a local copy of the SHAREDDATA
  class Member{
    std::string name;
    int sock;//-1== offline?
    sockaddr* addr;
  };
  SHAREDDATA shareitem;
  Member localowner;
  std::deque<Member> members;
  void edit_data();
  void share_changes(const std::deque<Member>& m);
  //etc etc
};

class xnServerConnection{
  //send and receive drawing instructions etc - have a conversATION
 
 protected:
  Glib::Mutex mutex;
  int connectfd;
  sockaddr* clientaddress;
  socklen_t addrlen;

 public:
  //uninitialised
  xnServerConnection();
  //return val and return args from accept(2)
  xnServerConnection(int fd,sockaddr* acceptaddr,socklen_t acceptlen);
  xnServerConnection(int fd,std::pair<sockaddr* ,socklen_t> acceptdata);

 protected:
  xnServerConnection(const xnServerConnection&);
  xnServerConnection& operator=(const xnServerConnection&);

 public:
  virtual ~xnServerConnection();

  bool connected()const{
    return connectfd > 0;
  }
  //NEED A SINK_FUNCTION, And source_function()
  void read_write_loop(const char* outbuuf, size_t outbytes,
		       char*& inbuf, size_t inbufsz){
    if(!connected()){
      throw NAMESPACE::Error("not connected");
    }
  }
};
#define   BUFSZ 1024
class basicNetApp{
 
  std::string writebuf;
  char readbuf[BUFSZ];
  bool something_to_report();
  void writefunc(NAMESPACE::SocketClass& sock){
    if(something_to_report()){
      writebuf = "something to report: not much really";
      char* s = strdup(writebuf.c_str());
      sock.writefd((void*)s, writebuf.size());
      free(s);
      writebuf="";
    }
  }
  void readfunc(NAMESPACE::SocketClass& sock){
    while(1){
      char* pread=readbuf;
      ssize_t count= sock.readfd((void*)pread, sizeof(readbuf));
      if(count < 0){
	throw NAMESPACE::Error("readfunc failed");
      }
      else if(!count){
	break;
      }
      else{
	std::cout<< pread<<std::endl;
	memset(readbuf, 0, BUFSZ);
      }
    }
  }
  basicNetApp(){
    memset(readbuf, 0, sizeof(readbuf));
  }

};

template <typename NETAPP>
class xnSocketAcceptor :public NAMESPACE::Acceptor< NETAPP>{
  
  // int fd_listener;
  Glib::Thread * acceptthread;
  Glib::ThreadPool threads;
  //SERVER* server;
   addrinfo listenerinfo;
  //    std::deque<ServerConnection> connections;
  Glib::Mutex settingsmutex;
  bool accept_connections_;
 public:
  xnSocketAcceptor (int sock, const addrinfo& listenerinf);
  ~xnSocketAcceptor(){
    threads.shutdown();
    //call join in another thread?
    // if(acceptthread &&acceptthread->joinable()){
    //	acceptthread->join();
    //  }
  }
  void set_accept_connections(bool val);

  std::pair<Glib::Thread*, xnSocketAcceptor*>
    create_in_thread(int sock, const addrinfo& listenerinf);
    
    
  void handle_connection(int connectfd, 
			 std::pair<sockaddr*,socklen_t>& sockargs){
    xnServerConnection* converser= new xnServerConnection(connectfd, sockargs);
    threads.push(sigc::bind(sigc::mem_fun(*converser, &xnServerConnection::read_write_loop),get_server(), get_readfunc(), get_writefunc()));
  }
  //TODO: conversation handler -REPLACE THESE 3 WITH REAL FUNCCTIONS
  void* get_readfunc(){return NULL;}
  void* get_writefunc(){return NULL;}
  void* get_server(){return NULL;}
  bool accept_connections(){
    Glib::Mutex::Lock lock(settingsmutex);
    return  accept_connections_;
  }
  std::pair<sockaddr*,socklen_t> get_empty_sockaddr()const;

  void acceptloop(){
    //using NAMESPACE::Acceptor<NETAPP>::fd_listener;
    while(accept_connections()){
      std::pair<sockaddr*, socklen_t> sockpair=get_empty_sockaddr();
      int connection =accept(NAMESPACE::Acceptor<NETAPP>::fd_listener, sockpair.first, &sockpair.second);
      if(connection<0){
	switch(errno){
	case EAGAIN:
#if (EAGAIN - EWOULDBLOCK)
	case EWOULDBLOCK
#endif
	case ENETDOWN: 
	case EPROTO:
	case ENOPROTOOPT:
	case  EHOSTDOWN:
	case  ENONET:
	case  EHOSTUNREACH:
	case  EOPNOTSUPP:
	case ENETUNREACH:
	  continue;
	default:
	  throw NAMESPACE::Error("connect failed");
	}
      }
      else{
	//ie create a ServerConnection in its own thread
	handle_connection(connection, sockpair);
      }
    }

  }
};
}

#endif
