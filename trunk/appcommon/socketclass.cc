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
#include "socketclass.h"

NAMESPACE_APPCOMMON{
  void SocketClass::reset_hint(addrinfo& sockethint, bool passive)const{
    memset(&sockethint, 0, sizeof(sockethint));
 
    sockethint.ai_family= PF_UNSPEC;
    sockethint.ai_socktype=SOCK_STREAM;
    if(passive){
      sockethint.ai_flags=AI_PASSIVE;
    }
  }

std::pair<sockaddr*,socklen_t> 
SocketClass::get_empty_sockaddr(int family){
      std::pair<sockaddr*,socklen_t> res(NULL, 0);
      //switch(listenerinfo.ai_family){
      switch(family){
      case PF_INET:
	res.first=reinterpret_cast<sockaddr*>(new sockaddr_in);
	res.second = sizeof( sockaddr_in);
	break;
      case PF_INET6:
	res.first=reinterpret_cast<sockaddr*>(new sockaddr_in6);
	res.second = sizeof( sockaddr_in6);
	break;
	//todo: PF_LOCAL etc
	//does a list exist: otherwise pick through /usr/include

      default:
	break;
      }
      if(res.second){
	memset(res.first, 0, res.second);
      }
      return res;
    }

 ssize_t SocketClass::senddata(int sockfd, char* cardata){
   char* data =cardata;
   //convert wchar_t to char first
   if(!data){throw Error("NULL data passed to senddata()");}
#if HAVE_STRNLEN
   do{
     ssize_t data_len=strnlen(data, data_max);
#else
     ssize_t data_len=strlen(data);
#endif
     ssize_t remaining= data_len;
     ssize_t sent= 0;
     void* d_start=reinterpret_cast<void*>(data);

     while (sent < data_len){
#ifdef WIN32
       ssize_t this_send=send(sockfd, d_start, remaining,0);
#else
       ssize_t this_send=write(sockfd,d_start, remaining);
#endif
       if(this_send<0){//error
	 if(errno==EINTR||errno==EAGAIN){continue;}
	 throw Error("send failed");
       }
       sent +=this_send;
       remaining=data_len-sent;
       d_start+=this_send;
     }
#if HAVE_STRNLEN
     //data +=data_max;}
     //while((data_len==data_max) && (*data != 0)){}
#endif
 }

  ssize_t SocketClass::readfd(void* buf, size_t count){
   
      ssize_t remaining= count;
      ssize_t rcvd= 0;
      void* d_start=buf;

      while (rcvd < count){
#ifdef WIN32
	ssize_t this_recv=recv(sockfd, d_start, remaining,0);
#else
	ssize_t this_recv=read(sockfd,d_start, remaining);
#endif
	if(this_recv<0){//error
	  if(errno==EINTR||errno==EAGAIN){continue;}
	  throw Error("send failed");
	}
	else if(this_recv==0){
	  break;
	}
	rcvd +=this_recv;
	remaining=count-rcvd;
	d_start+=this_recv;
      }
      return rcvd;
   }

ssize_t SocketClass::writefd(void* buf, size_t count){
 ssize_t remaining= count;
      ssize_t sent= 0;
      void* d_start=buf;

      while (sent < count){
#ifdef WIN32
	ssize_t this_send=send(sockfd, d_start, remaining,0);
#else
	ssize_t this_send=write(sockfd,d_start, remaining);
#endif
	if(this_send<0){//error
	  if(errno==EINTR||errno==EAGAIN){continue;}
	  throw Error("send failed");
	}
	sent +=this_send;
	//all_sent+=this_send;
	remaining=count-sent;
	d_start+=this_send;
      }
      return sent;
}
}
