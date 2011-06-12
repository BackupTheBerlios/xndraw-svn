SUBDIRS = .
ACLOCAL_AMFLAGS =-I m4
noinst_PROGRAMS = existstest regextest

regextest_SOURCES = testreg.cc 

existstest_SOURCES = existstest.cc

existstest_LDADD = libstupidapp.la 

lib_LTLIBRARIES = libstupidapp.la
#multiblockalloc.cc perhaps not needed, multiblockalloc.h all-template
libstupidapp_la_LIBADD = -lpthread
#can you mix c & c++ sourcefiles in one lib?
libstupidapp_la_SOURCES = c_initialiser.c fileexists.cc \
			preferences.cc \
			exceptions.cc \
			stupidiorefs.cc \
multiblockalloc.cc \
			fixwidechar.cc \
                        forkexec.cc cppthreads.cc socketclass.cc clientconnect.cc \
                        serverlisten.cc

#TODO: ../../include/appcommon ?
pkginclude_HEADERS = 	exceptions.h \
			fileexists.h  \
			preferences.h \
			wanthash.h \
			fixwidechar.h \
			stupidiorefs.h \
	                blockalloc.h \
                        multiblockalloc.h \
			peerrank.h \
                        uniqueresource.h \
socketclass.h cppthreads.h \
                        forkexec.h \
                        clientconnect.h \
                        serverlisten.h \
			undefs.h c_initialiser.h container.h


LIBTOOL_DEPS = @LIBTOOL_DEPS@
libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck

AM_CPPFLAGS = -g -ggdb @appcommoncppflags@ -I$(top_builddir) -I$(builddir)
AM_CXXFLAGS =  -g -ggdb @appcommoncxxflags@
AM_LDFLAGS = @appcommonlibflags@

LOCALCLEANFILES =

clean-local: clean_editbackups

edit-clean-local: clean_editbackups


clean_editbackups:
	rm -f *~
	for f in $(LOCALCLEANFILES); do if test -f $$f ; then  rm -f $$f; fi; done
