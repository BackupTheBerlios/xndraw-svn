lib_LTLIBRARIES = libXNinstruction.la

bin_PROGRAMS = testinst

testinst_SOURCES = test.cc

testinst_CFLAGS = $(AM_CPPFLAGS)
testinst_LDFLAGS =  @xndraw_cairomm_LIBS@


libXNinstruction_la_CFLAGS = $(AM_CPPFLAGS)

libXNinstruction_la_LDFLAGS =  @xndraw_cairomm_LIBS@

libXNinstruction_la_SOURCES = boundinstruction.cc boundinstruction.h

AM_CPPFLAGS = -I$(top_srcdir)/ncoords \
		@xndraw_cairomm_CFLAGS@ \
		-I$(top_srcdir)/appcommon -I$(top_builddir)/appcommon 

#LOCALCLEANFILES = $(CLEANFILES)
LOCALCLEANFILES =
edit-clean-local:
	for f in $(LOCALCLEANFILES); do if test -f $$f; then rm -f $$f; fi; done
	rm -f $(srcdir)/*~

clean-local:	edit-clean-local