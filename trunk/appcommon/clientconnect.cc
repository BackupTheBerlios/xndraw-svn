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
#include "clientconnect.h"  



NAMESPACE_APPCOMMON{



  int ClientSocket::connect_to_server(const char* host, const char* service__port){
    addrinfo hint;
    addrinfo* outinfo(NULL), *outinfo0(NULL);
    reset_hint(hint, false);
   
   
    std::ostringstream ostmp;
    int socket_fd=-1;
    int err = getaddrinfo(host, service__port, &hint, &outinfo0);
    if(err){
      ostmp <<"getaddrinfo("<<host<<", "<<service__port<<"...)\n"<<gai_strerror(err);
      throw Error(ostmp.str());
    }
   
    for (outinfo=outinfo0; outinfo;outinfo=outinfo->ai_next){
      int socket_fd = socket(outinfo->ai_family, outinfo->ai_socktype, outinfo->ai_protocol);
      if(socket_fd<0){
	ostmp << "failed to create socket\n";
	/*try again with next, if one exists*/
	continue;
      }
      if(connect(socket_fd, outinfo->ai_addr, outinfo->ai_addrlen)<0){
	ostmp <<"connect failed"<<std::endl;
	close(socket_fd);
	continue;
      }

   
 
    }
    freeaddrinfo(outinfo0);
    if(socket_fd<0){
      //TODO: keep a string, or an Error
      throw Error("couldn't connect");
    }
    return socket_fd;
  }

}
