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
 * @file socketclass.h 
 * common methods with sockets
 */

#ifndef SNARKY_APPCOMMON_SOCKETCLASS_H 
#define SNARKY_APPCOMMON_SOCKETCLASS_H

//#define DEBUG_FILE 1

#ifdef HAVE_CONFIG_H
#include "appcommonconfig.h"
#endif
#include "fileexists.h"

#include "netdb.h"
NAMESPACE_APPCOMMON{

  class SocketService;



  class SocketClass{
  protected:
    int sockfd;
    //max 'individual' write
    //(loops over data with strnlen when greater
    //which still could read endlessly wi
    unsigned int data_max;
    SocketClass(const SocketClass&);
    SocketClass& operator=(const SocketClass&);
  public:
    SocketClass(int fd): sockfd(fd), data_max(1 <<18/*256k*/){}
    virtual ~SocketClass(){}
    void reset_hint(addrinfo& sockethint, bool passive=true) const;

    //void set_acceptfilter(acceptfilter& filter
    ssize_t senddata(int sockfd, char* data);

    //delete the returned sockaDDR AFTER USE
    static std::pair<sockaddr*,socklen_t> 
    get_empty_sockaddr(int family);
    ssize_t readfd(void* buf, size_t count);
    ///whats' a good count? - a page?
    virtual ssize_t readsocket(void* buf, size_t count){
      return readfd(buf, count);
    }

    ssize_t writefd(void* buf, size_t count);
    virtual ssize_t writesocket(void* buf, size_t count){
      return writefd(buf, count);
    }


  };

  template <typename NETAPP>
    class Connection: public SocketClass{
  protected:
    NETAPP* app;
  public:
    Connection(int fd, NETAPP* a):SocketClass(fd), app(a){}
  protected:
    Connection(const Connection&);
    Connection& operator=(const Connection&) ;
  };
  
}
#endif
