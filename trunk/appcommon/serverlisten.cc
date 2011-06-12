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

#include "serverlisten.h"
 
#define MAXSOCK 30
#define BACKLOG 64

NAMESPACE_APPCOMMON{
  
  //overload where some thread abstraction is visible
  

  //doesnt call accept;
  template <typename NETAPP>
    int SocketListener<NETAPP>::create_listener(bool registered ){
    addrinfo hint;
    addrinfo* iter(NULL), *result(NULL);
    std::ostringstream errtmp;
    int failures(0);
    reset_hint(hint);
    std::string real_servicename(servicename);
    if(!registered || !real_servicename.size()){
      std::ostringstream ostmp;
      ostmp << port;
      real_servicename= ostmp.str();
    }
    int err=getaddrinfo(NULL, real_servicename.c_str(), &hint, &result);
    if(err){
      errtmp << gai_strerror(err);
      throw Error(errtmp.str());
    }
    for( iter = result;iter && (size() < MAXSOCK);iter=iter->ai_next){
      int sockfd= socket(iter->ai_family, iter->ai_socktype, iter->ai_protocol);
      if (sockfd<0){
	failures++;
	errtmp<<"socket of family "<<iter->ai_family<< " bad: socket() failed\n";
	continue;
      }
      if(bind(sockfd,iter->ai_addr, iter->ai_addrlen)<0){
	failures++;
	errtmp<<"socket "<<sockfd << " of family "<<iter->ai_family<< ": bind() failed\n";
	close(sockfd);
	continue;
      }
      listen(sockfd,get_max_connections());
      listeners[sockfd] = *iter;
    }
    freeaddrinfo(result);
    if (!size()){
      std::string s_err("no listening sockets successfully called listen:\n");
      s_err+=errtmp.str();
      throw Error(s_err);
    }
    else if(failures){
      std::cerr<<failures << "sockets failed to start:\n"<< errtmp.str()<<std::endl;
    }
    return 0;
  }

  int daytimelisten(int family, uint16_t port) {
    //only for PF_INET
    register int socket_fd, connection;
   
    struct sockaddr_in saddr;
 socklen_t saddr_sz = sizeof saddr;
    time_t t;
    struct tm *tm;
    FILE *client;
    if ((socket_fd = socket(family, SOCK_STREAM, 0)) < 0) {
      throw Error("socket creation failed");
    }

    memset(reinterpret_cast<void*>(&saddr), 0, sizeof saddr);

    saddr.sin_family = family;
    saddr.sin_port   = htons(port);
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(socket_fd, (struct sockaddr *)&saddr, sizeof saddr) < 0) {
      throw Error("bind");
    }

    switch (fork()) {
    case -1:
      throw Error("fork");

    default:
      close(socket_fd);
      return 0;
      break;
    case 0:
      break;
    }

    listen(socket_fd, BACKLOG);

    for (;;) {
      // b = sizeof sa;

      if ((connection = accept(socket_fd, (sockaddr *)&saddr, &saddr_sz)) < 0) {
	perror("daytimed accept");
	return 4;
      }

      if ((client = fdopen(connection, "w")) == NULL) {
	perror("daytimed fdopen");
	return 5;
      }

      if ((t = time(NULL)) < 0) {
	perror("daytimed time");

	return 6;
      }

      tm = gmtime(&t);
      fprintf(client, "%.4i-%.2i-%.2iT%.2i:%.2i:%.2iZ\n",
	      tm->tm_year + 1900,
	      tm->tm_mon + 1,
	      tm->tm_mday,
	      tm->tm_hour,
	      tm->tm_min,
	      tm->tm_sec);

      fclose(client);
    }
  }




}
