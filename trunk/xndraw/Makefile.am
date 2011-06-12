# Makefile.am for xndraw-x.y.z/xndraw


##auto remake libtool
LIBTOOL_DEPS = @LIBTOOL_DEPS@
libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck
###

bin_PROGRAMS=xndraw drawaddikt xnsimple npathframetest
npathframetest_SOURCES = x3draw.cc npathframe.cc guiutils.cc npathframetest.cc pingbase.cc

xnsimple_SOURCES=  simpleguimain.cc simplegui.cc simplegui.h drawtool.cc x3draw.cc pngsave_window.cc drawbuffer.cc

xndraw_SOURCES=  	drawbuffer.cc \
		 	drawtool.cc \
			bufferedarea.cc \
			bufferedarea.h \
			colourchooser.cc \
                        pingbase.cc \
			main.cc \
			pngsave_window.cc \
			linewidth.cc  \
			x3draw.cc \
			x3drawtoolbox.cc \
			fmatrix.cc \
			dataview.cc \
			xndrawdata.cc \
			drewadit.cc  \
			xnmenus.cc \
			drawdeque.cc \
			imageview.cc \
			ping_window.cc \
			pingbuffer.cc  \
			editmodes.cc  \
			hframe.cc \
			pathview.cc \
			guiutils.cc \
			xnclientserver.cc \
                       npathframe.cc

pkginclude_HEADERS =drewadit.h drawbuffer.h drawtool.h  drawtool.tcc colourchooser.h hframe.h pngsave_window.h pngsave_window.tcc linewidth.h x3drawtoolbox.h fmatrix.h xndrawdata.h x3draw.h dataview.h drawdeque.h pingbase.h ping_window.h pingbuffer.h editmodes.h treepoo.h xnmenus.h imageview.h xnsignals.h pathview.h guiutils.h xnclientserver.h npathframe.h



# fileops.cc fileops.h #removed

#xndraw_HEADERS = drawingbase.h drawcolour.h drawing.h drewadit.h drawbuffer.h drawingset.h drawstore.h drawtool.h colourchooser.h ping_window.h pingbuffer.h paste_ping.h hframe.h  pngsave_window.h linewidth.h x3drawtoolbox.h fmatrix.h typedefs.h  xndrawdata.h  x3draw.h dataview.h  clickhistory.h drawdeque.h pasteframe.h surfacepair.h click_pusher.h click_popper.h threaddraw.h defines.h io_uncollide.h pingswitcher.h ../ncoords/defines.h selection.h fileops.h

drawaddikt_SOURCES=drewadit.cc drawtool.cc drawaddictmain.cc 

xnsimple_CFLAGS = @xndraw_gtkmm_CFLAGS@ @xndraw_cairomm_CFLAGS@

xnsimple_LDADD = $(top_builddir)/ncoords/libNcoords.la \
		$(top_builddir)/libcairoXndrawing/libxNcairodrawing.la \
		$(top_builddir)/appcommon/libstupidapp.la 

xnsimple_LDFLAGS =  @xndraw_gtkmm_LIBS@  @xndraw_cairomm_LIBS@

npathframetest_LDFLAGS =  @xndraw_gtkmm_LIBS@  @xndraw_cairomm_LIBS@ @xndraw_librsvg_LIBS@ @xndraw_GTHREAD_LIBS@
npathframetest_LDADD = $(top_builddir)/ncoords/libNcoords.la \
		$(top_builddir)/libcairoXndrawing/libxNcairodrawing.la \
		$(top_builddir)/appcommon/libstupidapp.la 
npathframetest_CFLAGS = -O2 -funroll-loops -pipe  -ggdb -Wall -g  -fno-inline $(GTK_CPPFLAGS)@xndraw_gtkmm_CFLAGS@ @xndraw_librsvg_CFLAGS@ @xndraw_GTHREAD_CFLAGS@ @xndraw_cairomm_CFLAGS@

xndraw_CFLAGS = -O2 -funroll-loops -pipe  -ggdb -Wall -g  -fno-inline $(GTK_CPPFLAGS)@xndraw_gtkmm_CFLAGS@ @xndraw_librsvg_CFLAGS@ @xndraw_GTHREAD_CFLAGS@ @xndraw_cairomm_CFLAGS@

drawaddikt_CFLAGS =  -O2 -funroll-loops -pipe  -ggdb -Wall -g  -fno-inline $(GTK_CPPFLAGS) @xndraw_gtkmm_CFLAGS@ @xndraw_librsvg_CFLAGS@ @xndraw_GTHREAD_CFLAGS@

drawaddikt_LDFLAGS =  @xndraw_gtkmm_LIBS@ @xndraw_librsvg_LIBS@ @xndraw_GTHREAD_LIBS@ @xndraw_cairomm_LIBS@

drawaddikt_LDADD = $(top_builddir)/ncoords/libNcoords.la  \
		$(top_builddir)/appcommon/libstupidapp.la 


xndraw_LDFLAGS =  @xndraw_gtkmm_LIBS@ @xndraw_librsvg_LIBS@ @xndraw_GTHREAD_LIBS@ @xndraw_cairomm_LIBS@


xndraw_LDADD = $(top_builddir)/ncoords/libNcoords.la   \
		$(top_builddir)/clickpopper_threads/libclickpopper.la \
		$(top_builddir)/libcairoXndrawing/libxNcairodrawing.la  \
		$(top_builddir)/appcommon/libstupidapp.la 
#$(top_builddir)/glitz/libxndrawglitz.la

AM_CPPFLAGS                =  -I$(top_builddir)  -I$(top_builddir)/appcommon -I$(top_srcdir) -I$(top_srcdir)/include  -I$(top_srcdir)/appcommon -I$(top_srcdir)/xn_instruction  -I$(top_srcdir)/ncoords    -I$(top_srcdir)/appcommon -I$(top_srcdir)/clickpopper_threads -I$(top_srcdir)/glitz   -I$(top_srcdir)/libcairoXndrawing $(GTK_CPPFLAGS)   -pipe  -Wall -ggdb  -O2 -funroll-loops  -g  -fno-inline @xndraw_gtkmm_CFLAGS@ @xndraw_librsvg_CFLAGS@ @xndraw_GTHREAD_CFLAGS@ @xndraw_cairomm_CFLAGS@


#LOCALCLEANFILES = $(CLEANFILES)
LOCALCLEANFILES =
edit-clean-local:
	for f in $(LOCALCLEANFILES); do if test -f $$f; then rm -f $$f; fi; done
	rm -f $(srcdir)/*~

clean-local:	edit-clean-local

