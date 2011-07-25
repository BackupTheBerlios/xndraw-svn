
LIBTOOL_DEPS = @LIBTOOL_DEPS@
libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck

dnl TODO: make distcheck claims that uninstall leaves the actual .so.* and .a libraries behind
dnl TODO: how to fix that 


ACLOCAL_AMFLAGS= -I m4
nodist_pkginclude_HEADERS = defines.h

#pkgdatadir= @pkgdatadir@
pkgxmldir= $(pkgdatadir)/xml
pkgicondir= $(pkgdatadir)/icons
xnPREFIX= $(prefix)

#causes an error
#defines_h_SOURCES = defines.h.in 
# so...
noinst_HEADERS = defines.h.in

editdefines = sed \
	-e 's|@pkgdatadir[@]|$(pkgdatadir)|g' \
	-e 's|@PREFIX[@]|$(PREFIX)|g' \
	-e 's|@pkgicondir[@]|$(pkgicondir)|g' \
	-e 's|@pkgxmldir[@]|$(pkgxmldir)|g' 

defines.h: $(srcdir)/defines.h.in
	rm -f $@ $@.tmp
	$(editdefines) $(srcdir)/$@.in > $@.tmp
	mv $@.tmp $@

all-local: defines.h

SUBDIRS = . appcommon ncoords clickpopper_threads  xn_instruction libcairoXndrawing xndraw doc share DEBIAN test #glitz-test gimp glitz 
#todo libgimpslave to override tool 'brush movement' data in gimp, since this is not exposed for a plugin - a feature request might be a better idea!

DIST_SUBDIRS = $(SUBDIRS)
EXTRA_DIST = reconf.sh xndraw-1.pc.in
#breaks on windows?
#format date from most to least significant for sorting
XN_DATE = `date  +%Y%m%d`
DATEDIST_ARCHIVES = $(PACKAGE)-$(XN_DATE).tar.gz

AM_CPPFLAGS                =  -I$(top_builddir) -I$(top_srcdir) -I$(top_srcdir)/include  -I$(top_srcdir)/xndraw  -I$(top_srcdir)/ncoords -pipe  -Wall -ggdb -g -O0

doc-clean-recursive:
	(cd doc && $(MAKE) $(AM_MAKEFLAGS) doc-clean)

doc-clean: doc-clean-recursive

date-dist: dist

	mv $(DIST_ARCHIVES) $(DATEDIST_ARCHIVES)
#mv $(DIST_ARCHIVES) $(PACKAGE)-`date  +%d%m%Y`.tar.gz
LOCALCLEANFILES = defines.h xndraw-1.pc

edit-clean-local:
	for f in $(LOCALCLEANFILES); do if test -f $$f; then rm -f $$f; fi; done
	rm -f $(srcdir)/*~
edit-clean-local-recursive: edit-clean-local
	for d in $(SUBDIRS); do \
		test $$d = . || (cd $(builddir)/$$d && echo "deleting files in $$d" && $(MAKE) $(AM_MAKEFLAGS) \
		 edit-clean-local) ; done

clean-local:	edit-clean-local-recursive

.PHONY: doc-clean edit-clean-local all-local
