man_MANS = osip.1

EXTRA_DIST = osip.sgml $(man_MANS)

osip.1: osip.sgml
	docbook-to-man $< > $@
