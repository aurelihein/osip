EXTRA_DIST = README BUGS

SUBDIRS = include src scripts help platform

rpm:
	cd platform && $(MAKE) rpm

deb:
	cd platform && $(MAKE) deb

indent:
	indent */*/*.c
	indent */*/*.h
	rm -rf */*/*.c~
	rm -rf */*/*.h~


