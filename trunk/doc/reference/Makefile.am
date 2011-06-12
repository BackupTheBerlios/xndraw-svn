#cribbed from cairomm/docs/reference/Makefile.am
DOXYGEN_CONFIGFILE = Doxyfile
DOXYGEN_CONFIGFILE_SOURCE = Doxyfile.in

#DON'T put enormous 'html' directory in the regular dist
DIST_SUBDIRS = .
EXTRA_DIST = $(DOXYGEN_CONFIGFILE_SOURCE)
index_html = html/index.html

$(index_html): $(DOXYGEN_CONFIGFILE)
	-rm -rf html
	doxygen $(DOXYGEN_CONFIGFILE) \
	2> doxygen-warnings.txt | tee doxygen-output.txt && cat doxygen-warnings.txt

DOXYGEN_REPORTS = doxygen-output.txt doxygen-warnings.txt undocumented.txt

XNDRAW_SOCALLED_API_REFERENCE_ARCHIVE=$(PACKAGE)-doc.tar.gz

doc-tarball : $(XNDRAW_SOCALLED_API_REFERENCE_ARCHIVE)

$(XNDRAW_SOCALLED_API_REFERENCE_ARCHIVE): $(index_html)
	tar cf - html | gzip -c --best > $@

#LOCALCLEANFILES = $(CLEANFILES) $(XNDRAW_SOCALLED_API_REFERENCE_ARCHIVE)
LOCALCLEANFILES = $(XNDRAW_SOCALLED_API_REFERENCE_ARCHIVE) \
doxygen_output.txt \
doxygen-warnings.txt \
 Doxyfile

edit-clean-local:
	for f in $(LOCALCLEANFILES); do if test -f $$f; then rm -f $$f; \
else if test -d $$f; then rm -rf $$f; fi;fi; done
	rm -f $(srcdir)/*~
	rm -f doxygen_objdb_*tmp

clean-local:	edit-clean-local

doc-clean:
	-rm -rf html
	-rm -f cairomm_doxygen_tags
	-rm -f $(DOXYGEN_REPORTS)

all-local: $(index_html)

maintainer-clean-local: doc-clean
distclean-local: doc-clean

#automake conditional -distribute docs
#if HAVE_BUILTDOCS
#if DISTRIBUTE_BUILT_DOCS
#dist-hook: $(index_html)
#	cp -R html $(distdir)
#endif
#endif

DISTCLEANFILES = $(DOXYGEN_CONFIGFILE) $(DOXYGEN_REPORTS)

.PHONY: doc-clean all-local
