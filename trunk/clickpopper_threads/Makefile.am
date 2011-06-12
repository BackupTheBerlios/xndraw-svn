lib_LTLIBRARIES = libclickpopper.la

##auto remake libtool
LIBTOOL_DEPS = @LIBTOOL_DEPS@
libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck
###

libclickpopper_la_SOURCES = io_uncollide.cc io_uncollide.h

libclickpopper_la_CFLAGS =  -O2 -funroll-loops -pipe -g -ggdb -Wall

libclickpopper_la_LDFLAGS = @ncoords_glibmm_LIBS@ @xndraw_GTHREAD_LIBS@

libclickpopper_la_LIBADD = $(top_builddir)/ncoords/libNcoords.la $(top_builddir)/appcommon/libstupidapp.la

AM_CPPFLAGS                =  -I$(top_builddir)  -I$(top_builddir)/appcommon -I$(top_srcdir) -I$(top_srcdir)/appcommon -I$(top_srcdir)/ncoords   -O2 -funroll-loops -pipe -g -ggdb -Wall  -fno-inline @ncoords_glibmm_CFLAGS@ @xndraw_GTHREAD_CFLAGS@


#LOCALCLEANFILES = $(CLEANFILES)
LOCALCLEANFILES = 
edit-clean-local:
	for f in $(LOCALCLEANFILES); do if test -f $$f; then rm -f $$f; fi; done
	rm -f $(srcdir)/*~

clean-local:	edit-clean-local