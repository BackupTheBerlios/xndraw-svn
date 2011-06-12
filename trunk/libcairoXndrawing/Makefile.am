lib_LTLIBRARIES = libxNcairodrawing.la

libxNcairodrawing_la_SOURCES =  drawcolour.cc imagefile.cc cairopath.cc \
transpredicate.cc ratioflower.cc patternrosette.cc pathproducer.cc \
xncairoerror.cc paintcontext.cc pixelregion.cc \
pathqueueeater.cc drawingbase.cc drawing.cc \
drawingset.cc drawfunctors.cc guidraw.cc \
opsequence.cc bufferalloc.cc 

pkginclude_HEADERS =    cairopath.h  drawingbase.h drawcolour.h drawing.h drawingset.h  drawfunctors.h io_defines.h guidraw.h ratioflower.h patternrosette.h pathproducer.h pathqueueeater.h guardedqueue.h opsequence.h  transpredicate.h bufferalloc.h xncairoerror.h pixelregion.h imagefile.h paintcontext.h

##auto remake libtool
LIBTOOL_DEPS = @LIBTOOL_DEPS@
libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck
###

libxNcairodrawing_la_LIBADD = $(top_builddir)/ncoords/libTdeleter.la $(top_builddir)/ncoords/libNcoords.la $(top_builddir)/clickpopper_threads/libclickpopper.la $(top_builddir)/appcommon/libstupidapp.la


libxNcairodrawing_la_CFLAGS = -O0 -pipe -pthread -ggdb -Wall -g  -fno-inline @xndraw_gtkmm_CFLAGS@ @xndraw_librsvg_CFLAGS@ @xndraw_GTHREAD_CFLAGS@ @xndraw_cairomm_CFLAGS@  -g #-O2 -funroll-loops 

libxNcairodrawing_la_LDFLAGS =  @xndraw_gtkmm_LIBS@ @xndraw_librsvg_LIBS@ @xndraw_GTHREAD_LIBS@ @xndraw_cairomm_LIBS@

AM_CPPFLAGS = -g -ggdb  -I$(top_builddir)/appcommon -I$(top_builddir) -I$(top_srcdir) -I$(top_srcdir)/include  -I$(top_srcdir)/appcommon -I$(top_srcdir)/xndraw  -I$(top_srcdir)/ncoords   -I$(top_srcdir)/xn_instruction  -I$(top_srcdir)/clickpopper_threads -I$(top_srcdir)/glitz @xndraw_gtkmm_CFLAGS@ @xndraw_librsvg_CFLAGS@ @xndraw_GTHREAD_CFLAGS@ @xndraw_cairomm_CFLAGS@

AM_LDFLAGS = @xndraw_gtkmm_LIBS@ @xndraw_librsvg_LIBS@ @xndraw_GTHREAD_LIBS@ @xndraw_cairomm_LIBS@

#LOCALCLEANFILES = $(LOCALCLEANFILES)
LOCALCLEANFILES =
edit-clean-local:
	for f in $(LOCALCLEANFILES); do if test -f $$f; then rm -f $$f; fi; done
	rm -f $(srcdir)/*~

clean-local:	edit-clean-local

.PHONY: edit-clean-local