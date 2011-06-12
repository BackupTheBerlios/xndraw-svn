extra_DIST = test_ncoords_rotate_path.cc

dist_bin_SCRIPTS=svgread.py

lib_LTLIBRARIES      = libTdeleter.la libNcoords.la libxNdimensional.la

libxNdimensional_la_SOURCES = ndimensional.cc

libxNdimensional_la_CPPFLAGS =   -I$(top_srcdir)  -I$(top_builddir)/appcommon -I$(top_srcdir)/appcommon -O2 -funroll-loops -pipe -pthread -g -ggdb -Wall  -fno-inline @ncoords_glibmm_CFLAGS@  # -ggdb -g  -fno-inline for debug could also remove -O2 -funroll-loops for more clarity

##auto remake libtool
LIBTOOL_DEPS = @LIBTOOL_DEPS@
libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck
###

#move to ../test
noinst_PROGRAMS = testNodedeqoperator #test_matrixrotate
testdir = .
testNodedeqoperator_SOURCES = testoperatorin.cc

testNodedeqoperator_CFLAGS = @ncoords_glibmm_CFLAGS@

testNodedeqoperator_LIBS = @ncoords_glibmm_LIBS@

testNodedeqoperator_LDADD = libNcoords.la



libNcoords_la_SOURCES = basecoords.cc coords.cc ncoords.cc \
	beziercoords.cc clickhistory.cc npatterncoords.cc \
	nodes.cc nodesmap.cc selection.cc xndrawexceptions.cc  \
	linesegment.cc segmentshape.cc threadklass.cc pathshape.cc \
        subpath.cc bouncenav.cc


pkginclude_HEADERS = basecoords.h \
			 coords.h \
			ncoords.h  \
			beziercoords.h  \
			clickhistory.h \
                       clickhistory.tcc \
			npatterncoords.h  \
			nodes.h \
			coordinatefunctors.h \
			nodesmap.h \
			selection.h \
			xndrawexceptions.h \
			typedefs.h \
			ndimensional.h \
linesegment.h linesegment.tcc threadklass.h \
pathshape.h pathsection.h pathsection.tcc subpath.h \
bouncenav.h


libTdeleter_la_SOURCES = deleteptr.cc deleteptr.h

libTdeleter_la_CFLAGS = -O2 -funroll-loops -pipe -pthread -DXTHREADS -DXUSE_MTSAFE_API -g -ggdb -Wall  -fno-inline @ncoords_glibmm_CFLAGS@ #@xndraw_cairomm_CFLAGS@



libNcoords_la_CFLAGS =  -O2 -funroll-loops -pipe -pthread -g -ggdb -Wall  -fno-inline @ncoords_glibmm_CFLAGS@  # -ggdb -g  -fno-inline for debug could also remove -O2 -funroll-loops for more clarity



libNcoords_la_LIBADD = $(top_builddir)/ncoords/libTdeleter.la

AM_CPPFLAGS                =  -I$(top_builddir) -I$(top_srcdir) -I.  -I$(top_builddir)/appcommon   -I$(top_srcdir)/xndraw   -I$(top_srcdir)/appcommon  $(GTHREAD_CFLAGS) @ncoords_glibmm_CFLAGS@ -O2 -funroll-loops -pipe -g -ggdb -Wall  -fno-inline #-I$(top_srcdir)/libcairoXndrawing

AM_LDFLAGS  =  	@ncoords_glibmm_LIBS@  

#LOCALCLEANFILES = $(LOCALCLEANFILES)
LOCALCLEANFILES = 
edit-clean-local:
	for f in $(LOCALCLEANFILES); do if test -f $$f; then rm -f $$f; fi; done
	rm -f $(srcdir)/*~

clean-local:	edit-clean-local