EXTRA_DIST = osip.sgml

mandir = @mandir@/man1

install-data-local: osip.1
	$(mkinstalldirs) $(DESTDIR)$(mandir)
	@chmod 0755 $(DESTDIR)$(mandir)
	@echo "Installing man page: osip.1"
	$(INSTALL_DATA) $(top_srcdir)/doc/osip.1 $(DESTDIR)$(mandir);

osip.1: osip.sgml
	docbook-to-man $< > $@
