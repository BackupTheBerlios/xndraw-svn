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

#if defined(__cplusplus)

#ifndef APPCOMMON_PKGUNDEF_H
#define APPCOMMON_PKGUNDEF_H
//wrong!?
#if APPCOMMON_SUBPKG

#ifdef COMPILING_STUPIDAPP
#define SUBPACKAGE_VERSION PACKAGE_VERSION
#define SUBPACKAGE_STRING PACKAGE_STRING
#define SUBPACKAGE_TARNAME PACKAGE_TARNAME
#define SUBVERSION VERSION
#define SUBPACKAGE_BUGREPORT PACKAGE_BUGREPORT
#define SUBPACKAGE PACKAGE
#define SUBPACKAGE_NAME PACKAGE_NAME

#endif //COMPILING_STUPIDAPP


#undef PACKAGE_VERSION
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME
#undef VERSION
#undef PACKAGE_BUGREPORT
#undef PACKAGE
#undef PACKAGE_NAME
#include "config.h"
#else
#error "testing..."
#endif


#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION SUBPACKAGE_VERSION
#endif

#ifndef PACKAGE_STRING
#define PACKAGE_STRING SUBPACKAGE_STRING
#endif

#ifndef PACKAGE_TARNAME
#define PACKAGE_TARNAME SUBPACKAGE_TARNAME
#endif

#ifndef VERSION
#define VERSION SUBVERSION
#endif

#ifndef PACKAGE_BUGREPORT
#define PACKAGE_BUGREPORT SUBPACKAGE_BUGREPORT
#endif

#ifndef PACKAGE
#define PACKAGE SUBPACKAGE
#endif

#ifndef PACKAGE_NAME
#define PACKAGE_NAME SUBPACKAGE_NAME
#endif


//this means every file in the package includes some <*set> and <*map> header
//move this preprocessor magic to where they are required,
//if this an issue

// hash<SOMETYPE>::operator()(SOMETYPE)
///must be DEFINEd 
//to use SOMETYPE as a key_type in some hash map
//see wanthash.h for examples.
//Obviously keytype should make a fair crack at immutability
//lest the hash mutate also
//#include "wanthash.h"//uncomment if moved out of this configheader
#ifndef SnarkyMap
#define SnarkyMap XndrawMap
#if HAVE_HASHMAP 

#if  HAVE_UNORDERED_MAP
//#define SnarkyMap  std::unordered_map
#include <unordered_map>
#else
#ifdef HAVE_TR1_UNORDERED_MAP
//#define SnarkyMap  std::tr1::unordered_map
#include <tr1/unordered_map>

#else
#ifdef  HAVE_EXT_HASH_MAP
//#define SnarkyMap  __gnu_cxx::hash_map
#include <ext/hash_map>

#else 
#error "HAVE_HASHMAP defined but no candidate indicated. config error"
#endif //USE_EXT_HASH_MAP
#endif //USE_TR1_UNORDERED_MAP
#endif //USE_UNORDERED_MAP
#else
//#define SnarkyMap std::map
#include <map>
#endif // HAVE_HASHMAP
#endif // !SnarkyMap

//// see notes with SnarkyMap for SnarkySet
#ifndef SnarkySet
#define SnarkySet XndrawSet
#if HAVE_HASHSET
#include "wanthash.h"
#if  HAVE_UNORDERED_SET
#include <unordered_set>
#else
#ifdef HAVE_TR1_UNORDERED_SET
#include <tr1/unordered_set>
#else
#ifdef  HAVE_EXT_HASH_SET
#include <ext/hash_set>
#else 
#error "HAVE_HASHSET defined but no candidate indicated. config error"
#endif //USE_EXT_HASH_SET
#endif //USE_TR1_UNORDERED_SET
#endif //USE_UNORDERED_SET
#else
#include <set>
#endif // HAVE_HASHSET
#endif // !SnarkySet
////


#endif
#endif
