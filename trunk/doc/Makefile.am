subdirs = reference .
SUBDIRS = $(subdirs)

#LOCALCLEANFILES = $(LOCALCLEANFILES)
#LOCALCLEANFILES = 
edit-clean-local: edit-clean-local-recursive
	for f in $(LOCALCLEANFILES); do if test -f $$f; then rm -f $$f; fi; done
	rm -f $(srcdir)/*~

edit-clean-local-recursive:
	for d in $(SUBDIRS); do \
		test $$d = . || (cd $(srcdir)/$$d && echo "deleting editbackup files in $$d" && $(MAKE) $(AM_MAKEFLAGS) \
		 edit-clean-local) ; done


clean-local:	edit-clean-local

doc-clean-recursive:
	list='$(SUBDIRS)'; for subdir in $$list; do \
	  test "$$subdir" = . || (cd $$subdir && $(MAKE) $(AM_MAKEFLAGS) doc-clean); \
	done

doc-clean: doc-clean-recursive edit-clean-local

doc-rebuild: doc-clean all

.PHONY: doc-clean doc-clean-recursive doc-rebuild