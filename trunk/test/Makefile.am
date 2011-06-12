SUBDIRS = .

check_PROGRAMS = functortest

functortest_SOURCES = functortest.cc
functortest_LDFLAGS= $(AM_LDFLAGS)
functortest_CPPFLAGS = $(AM_CPPFLAGS)


AM_LDFLAGS= -L$(top_builddir)/ncoords $(top_builddir)/ncoords/libNcoords.la @ncoords_glibmm_LIBS@  

AM_CPPFLAGS = -I$(top_srcdir)/ncoords -I$(top_srcdir)/appcommon  -I$(top_builddir)/appcommon @ncoords_glibmm_CFLAGS@

test:functortest