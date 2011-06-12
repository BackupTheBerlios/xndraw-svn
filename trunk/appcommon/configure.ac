#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

m4_define([appcommon_pkgname], [libappcommon])

m4_define([appcommon_major_version],[0])
m4_define([appcommon_minor_version],[0])
m4_define([appcommon_micro_version],[3])

m4_define([appcommon_major_libversion],[0])
m4_define([appcommon_minor_libversion],[0])
m4_define([appcommon_micro_libversion],[2])


m4_define([appcommon_version], [appcommon_major_version.appcommon_minor_version.appcommon_micro_version])
m4_define([appcommon_libversion], [appcommon_major_libversion.appcommon_minor_version.appcommon_micro_version])

m4_define([author],[John O'Driscoll])
m4_define([bugaddress],[biggerbumme@yahoo.co.in])




AC_PREREQ([2.61])
AC_INIT([libappcommon], [appcommon_version], [BUG-REPORT-ADDRESS])

AC_CONFIG_SRCDIR([uniqueresource.h])
AC_CONFIG_HEADERS([appcommonconfig.h])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([.])
#HOW TO PASS args to configure script from metapackage configure script
#which calls it?
AH_TOP([#ifndef APPCOMMON_CONFIG_H
#define APPCOMMON_CONFIG_H])


AH_BOTTOM([#endif
#include "undefs.h"
])
AM_INIT_AUTOMAKE([1.9])
# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

LT_INIT
AC_SUBST([LIBTOOL_DEPS])

# Checks for libraries.
appcommoncxxflags=
appcommoncppflags="-I. -I.. -I../.."
appcommonlibflags=
AC_SUBST([appcommoncxxflags])
AC_SUBST([appcommoncppflags])
AC_SUBST([appcommonlibflags])
# Checks for header files.
AC_CHECK_HEADERS([fcntl.h unistd.h glob.h])
 AC_CHECK_HEADERS([netdb.h])
 AC_CHECK_HEADERS([netinet/in.h])
 AC_CHECK_HEADERS([sys/socket.h])
AC_CHECK_FUNCS([socket])


AC_ARG_ENABLE(appcommon-toplevelpkg,
	  [AS_HELP_STRING([--enable-appcommon-toplevelpkg],
	  	[don't include a toplevel pkg 'config.h'])],
	 [toplevelpkg=$enableval],
	 [toplevelpkg=no])


if test x"${toplevelpkg}" = xno ; then

AC_DEFINE([APPCOMMON_SUBPKG],[1],[is this a subpackage])
else
AC_DEFINE([APPCOMMON_SUBPKG],[0],[is this is not a subpackage])
fi

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_CHECK_TYPES([ptrdiff_t])
AC_TYPE_PID_T
AC_TYPE_SIZE_T

######################################
# dirent etc lifeforms on unixlike oses(everything including windows
# fileexists.cc
 AC_STRUCT_DIRENT_D_TYPE
 AC_STRUCT_DIRENT_D_INO

#   *   *   *   *   *   *

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_STRTOD
AC_CHECK_FUNCS([fchdir getcwd memset mkdir regcomp strdup strerror strtol strtoul uname])
AC_MSG_CHECKING([presence of posix_memalign])
AC_CHECK_FUNCS([posix_memalign memalign valloc],[break]) 

 AC_MSG_RESULT([${ac_cv_func_posix_memalign}])


AC_PROG_RANLIB
 AC_TYPE_UINT16_T
AC_LANG_PUSH([C++])
################################################
#  -*-*-*-*-  hash_map or map?  -*-*-*-*
##############################################


#TODO: test for -std=CXX0X

xndraw_testconfig_tr1_map="yes"
xndraw_testconfig_CXX0X_map="no"



AC_CHECK_HEADERS([ext/hash_map], [snarky_hashmapfile=yes],[snarky_hashmapfile=no])
#requires -std=c++0x or so
AC_CHECK_HEADERS([unordered_map],[xndraw_cxx0x_unordered_map="yes"], [xndraw_cxx0x_unordered_map="no"])
AC_CHECK_HEADERS([unordered_set] ,[xndraw_cxx0x_unordered_set="yes"], [xndraw_cxx0x_unordered_set="no"])
AC_CHECK_HEADERS([tr1/unordered_map],[xndraw_tr1_unordered_map="yes"], [xndraw_tr1_unordered_map="no"])
AC_CHECK_HEADERS([tr1/unordered_set] ,[xndraw_tr1_unordered_set="yes"], [xndraw_tr1_unordered_set="no"])

AC_CHECK_HEADERS([ext/hash_set], [snarky_hashsetfile=yes],[snarky_hashsetfile=no])

#experimental - but doesn't seem to require any crazy compiler switches
AC_CHECK_HEADERS([parallel/parallel.h], [xndraw_maybe_parallel="yes"],[xndraw_maybe_parallel="no"])

# where m4 causes configure to choke by misinterpreting a comma 
# So, use a shell substitution aa workaround

SNARKY_DEPRECATED_HASH_MAP_FOR_TEST="__gnu_cxx::hash_map<int, int>"
SNARKY_DEPRECATED_HASH_MULTIMAP_FOR_TEST="__gnu_cxx::hash_multimap<int, int>"
SNARKY_DEPRECATED_HASH_SET_FOR_TEST="__gnu_cxx::hash_set<int>"
SNARKY_DEPRECATED_HASH_MULTISET_FOR_TEST="__gnu_cxx::hash_multiset<int>"
TR1_MAP_FOR_TEST="std::tr1::unordered_map<int, int>"
TR1_MULTIMAP_FOR_TEST="std::tr1::unordered_multimap<int, int>"
TR1_SET_FOR_TEST="std::tr1::unordered_set<int>"
TR1_MULTISET_FOR_TEST="std::tr1::unordered_multiset<int>"
CXX0X_MAP_FOR_TEST="std::unordered_map<int, int>"
CXX0X_MULTIMAP_FOR_TEST="std::unordered_multimap<int, int>"
CXX0X_SET_FOR_TEST="std::unordered_set<int>"
CXX0X_MULTISET_FOR_TEST="std::unordered_multiset<int>"


if test x"${xndraw_testconfig_CXX0X_map}" = x"yes"; then
if test x"${xndraw_cxx0x_unordered_map}" = x"yes"; then
AC_CHECK_TYPES([${CXX0X_MAP_FOR_TEST}],
		[xndraw_use_cxx0x_unordered_map=yes],
		[xndraw_use_cxx0x_unordered_map=no],
		[#include <unordered_map>])

AC_CHECK_TYPES([${CXX0X_MULTIMAP_FOR_TEST}],
		[xndraw_use_cxx0x_unordered_multimap=yes],
		[xndraw_use_cxx0x_unordered_multimap=no],
		[#include <unordered_map>])

AC_CHECK_TYPES([${CXX0X_SET_FOR_TEST}],
		[xndraw_use_cxx0x_unordered_set=yes],
		[xndraw_use_cxx0x_unordered_set=no],
		[#include <unordered_set>])

AC_CHECK_TYPES([${CXX0X_MULTISET_FOR_TEST}],
		[xndraw_use_cxx0x_unordered_multiset=yes],
		[xndraw_use_cxx0x_unordered_multiset=no],
		[#include <unordered_set>])
fi
fi
#etc - if map there surely set is too, for each alternative(what about non-gcc?)

if test x"${xndraw_testconfig_tr1_map}" = x"yes"; then
if test x"${xndraw_tr1_unordered_map}" = x"yes"; then
AC_CHECK_TYPES([${TR1_MAP_FOR_TEST}],
		[xndraw_use_tr1_unordered_map=yes],
		[xndraw_use_tr1_unordered_map=no],
		[#include <tr1/unordered_map>])

AC_CHECK_TYPES([${TR1_MULTIMAP_FOR_TEST}],
		[xndraw_use_tr1_unordered_multimap=yes],
		[xndraw_use_tr1_unordered_multimap=no],
		[#include <tr1/unordered_map>])
	
AC_CHECK_TYPES([${TR1_SET_FOR_TEST}],
		[xndraw_use_tr1_unordered_set=yes],
		[xndraw_use_tr1_unordered_set=no],
		[#include <tr1/unordered_set>])

AC_CHECK_TYPES([${TR1_MULTISET_FOR_TEST}],
		[xndraw_use_tr1_unordered_multiset=yes],
		[xndraw_use_tr1_unordered_multiset=no],
		[#include <tr1/unordered_set>])
fi
fi

if test x"$snarky_hashmapfile" = xyes
then

#SUBSTITUTION MAGIC REQUIRED HERE m4 (((or is it sh))) will choke on
#AC_CHECK_TYPES([__gnu_cxx::hash_map<int, int>]...

AC_CHECK_TYPES([${SNARKY_DEPRECATED_HASH_MAP_FOR_TEST}],
		[snarky_hashmap=yes],
		[snarky_hashmap=no],
		[#include <ext/hash_map>])

AC_CHECK_TYPES([${SNARKY_DEPRECATED_HASH_MULTIMAP_FOR_TEST}],
		[snarky_hashmultimap=yes],
		[snarky_hashmultimap=no],
		[#include <ext/hash_map>])

fi


if test x"$snarky_hashsetfile" = xyes
then

AC_CHECK_TYPES([${SNARKY_DEPRECATED_HASH_SET_FOR_TEST}],
		[snarky_hashset=yes],
		[snarky_hashset=no],
		[#include <ext/hash_set>])

AC_CHECK_TYPES([${SNARKY_DEPRECATED_HASH_MULTISET_FOR_TEST}],
		[snarky_hashmultiset=yes],
		[snarky_hashmultiset=no],
		[#include <ext/hash_set>])

fi
AC_DEFINE([MAP_TYPE_NONE],[0],[No UNorderED MaP tYPe Available])
AC_DEFINE([MAP_TYPE_HASHMAP],[1],[UNorderED MaP tYPe IS (DEPREcated) __gnu_cxx::hash_map])
AC_DEFINE([MAP_TYPE_TR1_UNORDERED_MAP],[2],[UNorderED MaP tYPe Available - 
std::tr1::unordered_map])
AC_DEFINE([MAP_TYPE_CXXOX_UNORDERED_MAP],[3],[UNorderED MaP tYPe Available - 
std::unordered_map])
AC_DEFINE([SET_TYPE_NONE],[0],[No UNorderED MaP tYPe Available])
AC_DEFINE([SET_TYPE_HASHSET],[1],[UNorderED SET tYPe IS (DEPREcated) __gnu_cxx::hash_set])
AC_DEFINE([SET_TYPE_TR1_UNORDERED_SET],[2],[UNorderED SET tYPe Available - 
std::tr1::unordered_set])
AC_DEFINE([SET_TYPE_CXXOX_UNORDERED_SET],[3],[UNorderED SET tYPe Available - 
std::unordered_set])


#TODO: full template param list
if test x"${xndraw_use_tr1_unordered_map}" = x"yes"; then
SNARKY_PRINT_MAP_TYPE="std::tr1::unordered_map<T,T>"
AC_DEFINE([HAVE_HASHMAP],[MAP_TYPE_TR1_UNORDERED_MAP],[Using std__tr1__unordered_map])
AC_DEFINE([XndrawMap], [std::tr1::unordered_map],[using ${SNARKY_PRINT_MAP_TYPE} for maps])

else
if test x"${xndraw_use_cxx0x_unordered_map}" = x"yes"; then
SNARKY_PRINT_MAP_TYPE="std::unordered_map<T,T>"
AC_DEFINE([HAVE_HASHMAP],[MAP_TYPE_CXX0X_UNORDERED_MAP],[Using std__unordered_map])
AC_DEFINE([XndrawMap], [std::unordered_map],[using ${SNARKY_PRINT_MAP_TYPE} for maps])


else
if test x"$snarky_hashmap" = xyes
then


SNARKY_PRINT_MAP_TYPE="__gnu_cxx::hash_map<T,T>"

AC_DEFINE([HAVE_HASHMAP], [MAP_TYPE_HASHMAP],[using hash_map for map])
AC_DEFINE([XndrawMap], [__gnu_cxx::hash_map],[using ${SNARKY_PRINT_MAP_TYPE} for our maps- 2 X faster])
else
SNARKY_PRINT_MAP_TYPE="std::map<T,T>"

AC_DEFINE([HAVE_HASHMAP],[0],[no hash_map])
#this define only useful where the defaults for predicates etc are ok
AC_DEFINE([XndrawMap], [std::map],[using ${SNARKY_PRINT_MAP_TYPE} as fallback])
fi
fi
fi
################# => unordered_set

################
if test x"${xndraw_use_tr1_unordered_set}" = x"yes"; then
SNARKY_PRINT_SET_TYPE="std::tr1::unordered_set<T>"
AC_DEFINE([HAVE_HASHSET],[SET_TYPE_TR1_UNORDERED_SET],[Using std__tr1__unordered_set])
AC_DEFINE([XndrawSet], [std::tr1::unordered_set],[using ${SNARKY_PRINT_SET_TYPE} for maps])

else
if test x"${xndraw_use_cxx0x_unordered_set}" = x"yes"; then
SNARKY_PRINT_SET_TYPE="std::unordered_set<T>"
AC_DEFINE([HAVE_HASHSET],[SET_TYPE_CXX0X_UNORDERED_SET],[Using hash set ${SNARKY_PRINT_SET_TYPE}])
AC_DEFINE([XndrawSet], [std::unordered_set],[using ${SNARKY_PRINT_SET_TYPE} for maps])
else
if test x"$snarky_hashset" = xyes
then
SNARKY_PRINT_SET_TYPE="__gnu_cxx::hash_set<T>"

AC_DEFINE([HAVE_HASHSET], [1],[ CAN USE ${SNARKY_PRINT_SET_TYPE}])
AC_DEFINE([XndrawSet], [__gnu_cxx::hash_set],[using ${SNARKY_PRINT_SET_TYPE} for our sets- possibly 2 X faster])
else
SNARKY_PRINT_SET_TYPE="std::set<T>"

AC_DEFINE([HAVE_HASHSET],[0],[no hash_set])
AC_DEFINE([XndrawSet], [std::set],[using ${SNARKY_PRINT_SET_TYPE} as fallback])




fi
fi
fi
AC_DEFINE([APPCOMMON_MAP],[XndrawMap],[a lib specific name for the  hash map type discovered by configure])
##########################end unordered_set determination
# -*-*-*-*   end map test -*-*-*
#we only use unordered_map of these types.
#MOVED TO APPCOMMON/configure.ac
#############################################################
AC_LANG_POP([C++])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
