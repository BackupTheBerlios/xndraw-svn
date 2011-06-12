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

#ifndef SNARKY_APPCOMMON_CLIENTCONNECT_H 
#define SNARKY_APPCOMMON_CLIENTCONNECT_H

  //#define DEBUG_FILE 1

#ifdef HAVE_CONFIG_H
#include "appcommonconfig.h"
#endif
#include "fileexists.h"
#include <netdb.h>
#include "socketclass.h"


NAMESPACE_APPCOMMON{
  class ClientSocket: public SocketClass{
    std::string mhost;
    std::string mservport;
public:
  ClientSocket(const std::string& host, const std::string& port):
    SocketClass(-1),
mhost(host), mservport(port){}
 ClientSocket(const char* host, const  char* port): SocketClass(-1),
mhost(host), mservport(port){}
  ClientSocket(const char* host,unsigned short port): SocketClass(-1),
mhost(host){
      std::ostringstream ostmp;
      ostmp << port;
      mservport= ostmp.str();
    }
  //returns the socket fd, or throws an Error
    int do_connect(){
      return connect_to_server(mhost.c_str(), mservport.c_str());
    }

    int connect_to_server(const char* host, const char* service__port);

  };
  }
#endif
