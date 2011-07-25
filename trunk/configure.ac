
dnl Process this file with autoconf to produce a configure script.



AC_PREREQ(2.61)


m4_define([xndraw_pkg_name], [xndraw])
#shifted version info to right for first public showing
m4_define([xndraw_major_version], [0])
m4_define([xndraw_minor_version], [2])
m4_define([xndraw_micro_version], [5])
m4_define([xndraw_version], [xndraw_major_version.xndraw_minor_version.xndraw_micro_version])

#if using date in distname, shouldn't it be calculated by make not configure?
#since `make dist` makes the distribution
#configure could run some otheer time
#distdate=`date +%d%m%Y`

#change bugaddress
AC_INIT(xndraw_pkg_name,xndraw_version,[biggerbumme@yahoo.co.in])

AC_CONFIG_AUX_DIR([config])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([1.9])
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADER([config.h])
AC_CONFIG_SUBDIRS([appcommon])
#AH_TOP([#ifndef XNDRAW_CONFIG_H
#define XNDRAW_CONFIG_H])
AH_BOTTOM([#include "defines.h"])
##endif])
AC_DEFINE([XNDRAW_NAME], xndraw_pkg_name, [Program name])
AC_DEFINE([XNDRAW_VERSION], xndraw_version, [Program version])

AC_DEFINE([XNDRAW_YEARS], ["2006-2011"], [years of development, for the copyright string])
AC_DEFINE([XNDRAW_PACKAGE_AUTHORS],["Johno O'Driscoll"],[author(s) of the package- any recruits?])
AC_DEFINE([XNDRAW_MAJOR_VERSION], xndraw_major_version, [xNDRAW major version])
AC_DEFINE([XNDRAW_MINOR_VERSION], xndraw_minor_version, [xNDRAW minor version])
AC_DEFINE([XNDRAW_MICRO_VERSION], xndraw_micro_version, [xNDRAW micro version])

#MAYBE A mistake to do it this way, probably just use 'namespace APPCOMMON' instead
AC_DEFINE([NAMESPACE_APPCOMMON], [namespace xNDRAW], [namespace for ./appcommon/])
AC_DEFINE([NAMESPACE], [xNDRAW],  [namespace for ./appcommon/])

################hosttype - no crosscompiling, at least  yet
AC_CANONICAL_HOST
#does this work - extend to cover *linux*gnu* and *(bsd|BSD)* ?
#echo "KACK_CHYMAERICAL_HOST -( host) is ${host}"

xndraw_ac_host=$host
AC_SUBST([xndraw_ac_host])
UNM_S="`uname -s`"
UNM_P="`uname -p`"
UNM_N="`uname -n`"
xndraw_uname_host="${UNM_S}-${UNM_P}-${UNM_N}"
AC_SUBST([xndraw_uname_host])
AC_MSG_CHECKING([what host?])
AC_MSG_RESULT(["ok : ==> ${xndraw_uname_host}"])

 
case $host in 
	*windows*) $ECHO "you trying to tell me there's some other OS @ $host"
snarky_on_mswindozer=yes;;
        *bsd*)$ECHO "the Borg are BSD @ $host"
snarky_on_mswindozer=no ;;
	*linux*)$ECHO "penguins ahoy!! @ $host"
snarky_on_mswindozer=no ;;
	*) snarky_on_mswindozer=no ;;
esac
if test x"${snarky_on_mswindozer}" = xyes
then
AC_DEFINE([SNARKY_WINDOWS],[1],[xndraw running on some microsoft platform])
AC_DEFINE([PATH_SEP], ['\\'],[platform path separator as char])
AC_DEFINE([PATH_SEP_PTR], ["\\"],[platform path separator as char*])
else
AC_DEFINE([PATH_SEP], ['/'],[platform path separator as char])
AC_DEFINE([PATH_SEP_PTR], ["/"],[platform path separator as char*])
fi
#################################################

dnl whether to compile extra verbosity
AC_ARG_ENABLE(extra-verbiage,
	  [AS_HELP_STRING([--enable-extra-verbiage],
	  	[extra print statements and other perhaps less than useful verbosity, (and possibly other mayhem as seldom compiled for this option! - not;) in particular if threads are properly fixed , where all the std::cerr etc calls should be marshaled by a  threadsafe general-see iouncollide.h ])],
	 [xndraw_verbose=$enableval],
	 [xndraw_verbose=no])

if test  x"$xndraw_verbose" = xyes; then

AC_DEFINE([ENABLE_EXTRA_VERBIAGE], 1, [if you want it define ENABLE_EXTRA_VERBIAGE to 1])
else
AC_DEFINE([ENABLE_EXTRA_VERBIAGE], 0, [if you want it define ENABLE_EXTRA_VERBIAGE to 1])
fi
AC_ARG_ENABLE([matrix-in-reverse],
	[AS_HELP_STRING([--enable-matrix-in-reverse],
	[matrix-in-reverse: order in which matrix operations applied
deafault is in reverse 
--enable-matrix-in-reverse=no to change to forwards])],
	[xndraw_matrix_reverse=$enableval],
	[xndraw_matrix_reverse=yes])

if test x"${xndraw_matrix_reverse}" = x"yes" ; then
AC_DEFINE([XNDRAW_MATRIX_REVERSE], [1], [multiply etc matrixes in reverse order to what they are to take effect])
else
AC_DEFINE([XNDRAW_MATRIX_REVERSE], [0], [multiply etc matrixes in same order to what they are to take effect])
fi

#AC_ARG_ENABLE([glitz],
#	[AS_HELP_STRING([--enable-glitz],
#	[uses glitz for cairo etc output via openGL on X, requires
 #           glitz-glx and  cairo-glitz as well as glitz itself. dON'T USE for now as
#glitz directory not present - in any case the option has been forcibly disabled until glitz dir has been cleaned of non-copywrong material])],
#	[xndraw_use_glitz=no], #xndraw_use_glitz=$enableval
#	[xndraw_use_glitz=no])

#AM_CONDITIONAL([USE_GLITZ], [test "x$xndraw_use_glitz" = "xyes"])



#if ! test x"${xndraw_use_glitz}" = xno
#then

#AC_MSG_WARN([using -lglitz for output to openGL-driven devices])
#else
#AC_MSG_WARN([_NOT_ using -lglitz])
#fi

#TODO: make svg compulsory - probably won't compile without this option
AC_ARG_ENABLE([svg-output],
	[AS_HELP_STRING([ --enable-svg-output],
	[librsvg2 output if available - the default])],
	[xndraw_use_svg=$enableval],
	[xndraw_use_svg=yes])

dnl configure debug
AC_ARG_ENABLE([debug],
            [AS_HELP_STRING([--enable-debug],
               [extra debug info and members (default is no)])],
            [xndraw_debug=$enableval],
            [xndraw_debug=no])

dnl fix the build so xndraw_build_docs=no is honoured
dnl TODO: PERL_PATH
AC_ARG_ENABLE([docs],
	[AS_HELP_STRING([--enable-docs],
	[build api documentation (requires doxygen - and perl)])],
	[xndraw_build_docs=$enableval],
            [xndraw_build_docs=no])


AC_DEFINE([XNDRAW_GOLDEN_MEAN], [((pow(5, 0.5)-1)/2.0)],[the golden mean])
AC_DEFINE([XNDRAW_GOLDEN_MEAN_INVERSE],[(1.0 / XNDRAW_GOLDEN_MEAN)],[the golden mean])


if test x"$xndraw_debug" = xyes
then 
AC_DEFINE([COUNT_COORDS],[1], [extra members to count the instances])
AC_DEFINE([DEBUG],[1],[DEBUG])
if test "x$xndraw_verbose" = "xyes"; then
AC_DEFINE([DEBUG_VERBOSE],[1],[extra DEBUG verbosity])
fi
fi
bugflags="$xndraw_debug"
case $bugflags in
      *pathsection*)
AC_DEFINE([DEBUG_SUBPATH_SECTION], [1], [debug bezier drawing])
AC_DEFINE([CHECK_SUBPATH_SECTION], [1], [debug bezier drawing])
echo "debugging bezier";;
     *) ;;
esac
case $bugflags in
      *bezier*)
AC_DEFINE([DEBUG_BEZIER], [1], [debug bezier drawing])
echo "debugging bezier";;
     *) ;;
esac
case $bugflags in
      *pathqueue*)
AC_DEFINE([DEBUG_PATHQUEUE], [1], [debug PATHQUEUe])
echo "debugging PATHQUEUE";;
     *) ;;
esac

#how to allow say
# --enable-debug=bezier,pathqueue,window
# so that
##this causesDEBUG_BEZIER, DEBUG_PATHQUEUE, nad DEBUG_WINDOW to be AC_DEFINED

#####
dnl #######################
#AC_ARG_ENABLE([gimp-plugin],
#	[AS_HELP_STRING([--enable-gimp-plugin],
#	[Danger! VORSICHT! WARNING: broken - _and_ -requires gimp source,
#and handconfigured path to them])],
#	[xndraw_build_gimp_plugin=$enableval],
#            [xndraw_build_gimp_plugin=no])

#if test "x$xndraw_build_gimp_plugin" = "xyes"
#then
#echo "need the gimp sources"

#if ! test -f gimp/external/somedir/gimp${xndraw_gimp_version}-tar.bz2
#then
#complain and set
#xndraw_build_gimp_plugin="no"
#or stop build to either get the source file, stop the build
# or change gimp version or file location- 
#some env var or switch, but needto set up the underlying make targets 
#so they will use the file so set - GIMP_VERSION should be easier
#fi
#fi
#AM_CONDITIONAL([BUILD_GIMP_PLUGIN],[test "x$xndraw_build_gimp_plugin" = "xyes"])
#AC_SUBST([GIMP_VERSION],[xndraw_gimp_version])



AC_PROG_CXX
AC_PROG_CC

AC_PROG_CPP
AC_PROG_CXXCPP
AC_LANG([C++])
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET



AC_ARG_WITH([doxygen], 
            [AS_HELP_STRING([--with-doxygen],
            [path to doxygen - required to build(optional)
	    API documentation])],
	    [xndraw_doxygen=${with_DOXYGEN}],
[xndraw_doxygen=no])

if ! test ${xndraw_doxygen} = no
then
if ! test -x  ${xndraw_doxygen}
then 
xndraw_doxygen=no
fi
fi

if test "x${xndraw_build_docs}" = "xyes"
then

dnl some Makefile variable to enable/disable actually building the docs required
dnl check for doxygen
AC_PATH_PROG([PERL_PATH], [perl])
if ! test ${xndraw_doxygen} = no
then
DOXYGEN=${xndraw_doxygen}

else
AC_PATH_PROG([DOXYGEN], [doxygen], [no])
fi
if test x"${DOXYGEN}" = x"no"
then
AC_MSG_WARN([can't find doxygen. 
            If it is in a non-standard location(not in path)
	    rerun configure with the argument --with-doxygen="/path/to/doxygen"
Not building documentation])
xndraw_build_docs="no"
fi
fi
#distribute the built docs?
#AM_CONDITIONAL([HAVE_BUILTDOCS],[test x"${xndraw_build_docs}" = x"yes"])
#AM_CONDITIONAL([DISTRIBUTE_BUILT_DOCS], [! test x"${xndraw_distribute_built_docs}" = x"no"])


for top_builddir in . .. ../.. $ac_auxdir $ac_auxdir/..; do
  test -f $top_builddir/configure && break
done


LT_INIT
AC_SUBST([LIBTOOL_DEPS])

#how many flags can be or`ed into an int
AC_CHECK_SIZEOF(int, 4)


# Checks for libraries.
# FIXME: Replace `main' with a function in `-lX11':
AC_CHECK_LIB([X11], [main])
# FIXME: Replace `main' with a function in `-lXcursor':
AC_CHECK_LIB([Xcursor], [main])
# FIXME: Replace `main' with a function in `-lXext':
AC_CHECK_LIB([Xext], [main])
# FIXME: Replace `main' with a function in `-lXfixes':
AC_CHECK_LIB([Xfixes], [main])
# FIXME: Replace `main' with a function in `-lXi':
AC_CHECK_LIB([Xi], [main])
# FIXME: Replace `main' with a function in `-lXinerama':
AC_CHECK_LIB([Xinerama], [main])
# FIXME: Replace `main' with a function in `-lXrandr':
AC_CHECK_LIB([Xrandr], [main])
# FIXME: Replace `main' with a function in `-lXrender':
AC_CHECK_LIB([Xrender], [main])
# FIXME: Replace `main' with a function in `-lcairo':

# FIXME: Replace `main' with a function in `-lfontconfig':
AC_CHECK_LIB([fontconfig], [main])
# FIXME: Replace `main' with a function in `-lfreetype':
AC_CHECK_LIB([freetype], [main])
# FIXME: Replace `main' with a function in `-liconv':
AC_CHECK_LIB([iconv], [main])
# FIXME: Replace `main' with a function in `-lm':
AC_CHECK_LIB([m], [main])
# FIXME: Replace `main' with a function in `-lz':
AC_CHECK_LIB([z], [main])


# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_HEADER_DIRENT
AC_C_CONST
AC_HEADER_STDC
AC_CHECK_HEADERS([argz.h malloc.h memory.h stdlib.h string.h strings.h unistd.h fcntl.h])

#defines pid_t if its not in the headers
AC_TYPE_PID_T
#TODO: handle unlikely case of no fork() - we don't use currently, so
#its not a gamebreaking issue
#if HAVE_WORKING_FORK
#...
#endif
AC_FUNC_FORK
#AC_FUNK_VFORK
AC_FUNC_STRTOD
#todO: GET STRTOD.C from libiberty, and arrange to build and link it if required
#same for malloc()
AC_FUNC_MALLOC

#hash map tests moved to appcommon configure script


AC_CHECK_TYPES([ptrdiff_t])

AM_MAINTAINER_MODE

# Checks for library functions.
AC_CHECK_FUNCS([memmove memset ])
AC_CHECK_FUNCS([strerror strchr strdup strrchr strtol strtoul])
AC_CHECK_FUNCS([sqrt  pow ])
AC_CHECK_FUNCS([uname fchdir getcwd mkdir])


AC_FUNC_CLOSEDIR_VOID
AC_FUNC_REALLOC

AC_STRUCT_TM
AC_TYPE_SIZE_T
AC_C_INLINE
AC_TYPE_SIGNAL

PKG_CHECK_MODULES([xndraw_gtkmm], [gtkmm-2.4 >= 2.8 ])
PKG_CHECK_MODULES([xndraw_cairomm], [cairomm-1.0 >= 1.2])
PKG_CHECK_MODULES([ncoords_glibmm], [glibmm-2.4 >= 2.8])
PKG_CHECK_MODULES([xndraw_GTHREAD], [gthread-2.0])

dnl PKG_CHECK_MODULES([xndraw_GIMP],
dnl  gimp-2.0 >= $GIMP_REQUIRED_VERSION gimpui-2.0 >= $GIMP_REQUIRED_VERSION)


#svg to become mandatory
if test  x"$xndraw_use_svg" = "xyes"; then
PKG_CHECK_MODULES([xndraw_librsvg],[librsvg-2.0],[xndraw_do_build_svg=yes],[xndraw_do_build_svg=no])
else
xndraw_do_build_svg=no
xndraw_librsvg_CFLAGS=
xndraw_librsvg_LDFLAGS=

AC_SUBST([xndraw_librsvg_CFLAGS])
AC_SUBST([xndraw_librsvg_LDFLAGS])
fi

AM_CONDITIONAL([XNDRAW_BUILD_LIBRSVG], [test "x${xndraw_do_build_svg}" = "xyes"])




dnl write a condition

if test "x${xndraw_use_glitz}" = "xyes"
then
dnl PKG_CHECK_MODULES([xndraw_GLITZ],[ glitz-glx cairo-glitz gdk-x11-2.0])
xndraw_glitz_CFLAGS="`pkg-config glitz-glx cairo-glitz gdk-x11-2.0 glitz --cflags`"
xndraw_glitz_LDFLAGS="`pkg-config glitz-glx cairo-glitz gdk-x11-2.0 glitz --libs`"
else
xndraw_glitz_CFLAGS=
xndraw_glitz_LDFLAGS=
fi

AC_SUBST([xndraw_glitz_CFLAGS])
AC_SUBST([xndraw_glitz_LDFLAGS])

dnl 		glitz/Makefile \
dnl		glitz-test/Makefile \

AC_CONFIG_FILES([Makefile \
		ncoords/Makefile \
		xn_instruction/Makefile \
		libcairoXndrawing/Makefile \
		xndraw/Makefile \
		share/Makefile \
		share/xnicons/Makefile \
		share/xndraw.desktop \
		test/Makefile \
		DEBIAN/Makefile \
		DEBIAN/control \
		doc/Makefile \
clickpopper_threads/Makefile \
		doc/reference/Makefile \
		doc/reference/Doxyfile])

		
	#	gimp/Makefile \
	#	gimp/libgimpslave/Makefile \
	#	gimp/libgimpslave/mirrorsrctree/Makefile \
	#	gimp/libgimpslave/external/Makefile \
	#	gimp/libgimpslave/external/gimp-tar/Makefile \
	#	glitz/Makefile \
#got its own configure script(do I leave in SUBDIRS in Makefile.am?)
#	appcommon/Makefile \
AC_OUTPUT

