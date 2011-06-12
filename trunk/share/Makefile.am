SUBDIRS = xnicons .

extra_DIST = xndraw.desktop.in ui_info.xml

#TODO: sed command for xndraw.desktop

LOCALCLEANFILES = xndraw.desktop


pkgxmldatadir = $(pkgdatadir)/xml

xndraw_pkgxmldatadir = $(pkgxmldatadir)

xndraw_pkgxmldata_DATA = $(srcdir)/ui_info.xml
XML_FILES = $(srcdir)/ui_info.xml

#xndraw_DATA = $(XML_FILES)


edit-clean-local:
	for f in $(LOCALCLEANFILES); do if test -f $$f; then rm -f $$f; fi; done
	rm -f $(srcdir)/*~

clean-local:	edit-clean-local

.PHONY: edit-clean-local
