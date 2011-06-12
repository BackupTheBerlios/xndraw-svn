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

#include "xnclientserver.h"

namespace xNDRAW{

  xnServerConnection::xnServerConnection():connectfd(-1),
					   clientaddress(NULL), addrlen(0){}

  xnServerConnection::xnServerConnection(int fd,sockaddr* acceptaddr,socklen_t acceptlen):
    connectfd(fd),
    clientaddress(acceptaddr),
    addrlen(acceptlen){}
  xnServerConnection::xnServerConnection(int fd,
					 std::pair<sockaddr* ,socklen_t> acceptdat):
    connectfd(fd),
    clientaddress(acceptdat.first),
    addrlen(acceptdat.second){}


  xnServerConnection::~xnServerConnection(){
    if(clientaddress){
      delete clientaddress;
    }
    if(connectfd>0){
      close(connectfd);
    }
  }


  template <typename NETAPP>
  xnSocketAcceptor<NETAPP>::xnSocketAcceptor (int sock, 
				      const addrinfo& listenerinf):
    NAMESPACE::Acceptor<NETAPP>(sock), 
    listenerinfo(listenerinf), 
    accept_connections_(true){}
template <typename NETAPP>
  void xnSocketAcceptor<NETAPP>::set_accept_connections(bool val){
    Glib::Mutex::Lock lock(settingsmutex);
    accept_connections_ = val;
  }
 
template <typename NETAPP>
  std::pair<Glib::Thread*, xnSocketAcceptor<NETAPP>*>
  xnSocketAcceptor<NETAPP>::create_in_thread(int sock, const addrinfo& listenerinf){
    xnSocketAcceptor* acceptor = new xnSocketAcceptor(sock, listenerinf);
    Glib::Thread* t =Glib::Thread::create(sigc::mem_fun( *acceptor, & xnSocketAcceptor::acceptloop), false);
    //t->start();
    return std::pair<Glib::Thread*, xnSocketAcceptor*>(t, acceptor);
  }
  template<typename NETAPP>
  std::pair<sockaddr*,socklen_t> 
  xnSocketAcceptor<NETAPP>::get_empty_sockaddr()const{
    std::pair<sockaddr*,socklen_t> res(NULL, 0);
    //switch(listenerinfo.ai_family){
    return SocketClass::get_empty_sockaddr(listenerinfo.ai_family);
     
  }
}//namespace xNDRAW
