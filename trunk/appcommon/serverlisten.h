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
 * @file serverlisten.h
 * bind to socket and listen for connections
 */

#ifndef SNARKY_APPCOMMON_SERVERLISTEN_H 
#define SNARKY_APPCOMMON_SERVERLISTEN_H



#ifdef HAVE_CONFIG_H
#include "appcommonconfig.h"
#endif
#include "fileexists.h"
#include "socketclass.h"
#include <netdb.h>
#include <map>

/*pthread_attr_init(3),  pthread_atfork(3),
  pthread_cancel(3),  pthread_cleanup_push(3), pthread_cond_signal(3), pthread_cond_wait(3),
  pthread_create(3), pthread_detach(3), pthread_equal(3), pthread_exit(3),  pthread_key_cre‐
  ate(3),  pthread_kill(3), pthread_mutex_lock(3), pthread_mutex_unlock(3), pthread_once(3),
  pthread_setcancelstate(3), pthread_setcanceltype(3), pthread_setspecific(3),  pthread_sig‐
  mask(3), and pthread_testcancel(3)*/

NAMESPACE_APPCOMMON{

  class ServerConnection{
    int connectfd;
    sockaddr* clientaddress;
    socklen_t addrlen;
  public:
    virtual ~ServerConnection(){
      if(clientaddress){
	delete clientaddress;
	close(connectfd);
      }
    }
  };
  //todo:as templates, on Mutex, Lock, Thread etc
  template <typename NETAPP>
  class Acceptor{
    int fd_listener;
    addrinfo listenerinfo;
    //is this helpful?
    //or just a lot of locking etc for no good reason
    //connction mapped to its socket
    //if we need this class
    // APPCOMMON_MAP<int, ServerConnection*> connections;

    typedef  APPCOMMON_MAP<int, ServerConnection*> connection_map_type;
    typedef connection_map_type::iterator map_iterator;
 typedef connection_map_type::const_iterator const_map_iterator;

  public:
    Acceptor(int sock, const addrinfo& listenerinf):fd_listener(sock), 
						    listenerinfo(listenerinf){}
    virtual ~Acceptor(){
      //derived class destructor called first
      //if listeners is needed! - must delete any connection as it closes,
      //while holding a lock on its mutex
      //      for(map_iterator w = listeners.begin(); w!= listeners.end();w++){
      //delete (*w).second;//it closes the socket in its dtor
      //}
}

    virtual void handle_connection(int fd, 
				   std::pair<sockaddr*, socklen_t> sockargs) {

    }
  };

  template <typename NETAPP>
    class SocketListener: public SocketClass{
  public:
    typedef NETAPP peer_application_type;
  protected:
    unsigned short port;
    std::string  servicename;
    std::string namepath;
    std::map<int, Acceptor<NETAPP>* > listeners;
    NETAPP* app;
  public:
    typedef  std::deque<int>::size_type size_type;
    size_type size()const{
      return listeners.size();
    }

  public:
    size_t get_max_connections()const{return 256;}
    void reset_hint (addrinfo& hint, bool unused=true)const{
      //use base class version to create a non-passive socket
      SocketClass::reset_hint(hint, true);
    }
    void create_acceptor(int listenersocket, const addrinfo& listenerinfo);
    int create_listener(bool registered = false);

    template<typename THREADKLASS, typename SOCKETTHREADFACTORY>
    void acceptloop(int sockfd, addrinfo& hint){
      THREADKLASS* thread= SOCKETTHREADFACTORY::create_thread(&SocketListener::do_accept, sockfd, hint);
      //int connection= do_accept(sockfd, hint);
    }
    int do_accept(int sockfd, addrinfo& ainfo){
      int connection= accept(sockfd, NULL,NULL);
      //int connection= accept(sockfd, ainfo.ai_addr, ainfo.ai_addrlen);
      if(connection<0){throw Error("accept failed");}
      return connection;
    }

  };

template <typename NETAPP>
  void SocketListener<NETAPP>::create_acceptor(int listenersocket, const addrinfo& listenerinfo){
    //listeners[listenersocket] = listenerinfo;
    //start a thread, which will start threads(from a pool)


  }

}


#endif
