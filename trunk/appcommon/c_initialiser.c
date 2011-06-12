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


// todo: check for gcc in configure, and define __USE_GNU 
//in appcommmonconfig.h
#define _GNU_SOURCE

#define COMPILING_STUPIDAPP 1

#include "c_initialiser.h"


pthread_mutex_t init_fastmutex(void){
  pthread_mutex_t m=PTHREAD_MUTEX_INITIALIZER;
  return m;
}
pthread_mutex_t init_recmutex(void){
#if defined(PTHREAD_RECURSIVE_MUTEX_INITIALIZER)
  pthread_mutex_t m=PTHREAD_RECURSIVE_MUTEX_INITIALIZER;
#elif defined(PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP)

  pthread_mutex_t m=PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
#else
#error "no recursive mutex initialiser available"
#endif
  return m;
}
pthread_mutex_t init_checkmutex(void){
#if defined(PTHREAD_ERRORCHECK_MUTEX_INITIALIZER)
  pthread_mutex_t m=PTHREAD_ERRORCHECK_MUTEX_INITIALIZER;
#elif defined(PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP)
 pthread_mutex_t m=PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP;

#else
#error "no errorcheck mutex initialiser available"
#endif
 return m;
}


pthread_cond_t init_pthreadcond(void){
  pthread_cond_t c= PTHREAD_COND_INITIALIZER;
  return c;
}
 
