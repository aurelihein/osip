EXTRA_DIST = README BUGS

SUBDIRS = include src scripts help platform debian

rpm-package:
	cd platform && $(MAKE) rpm-package

deb:
	dpkg-buildpackage -rfakeroot

indent:
	indent */*/*.c
	indent */*/*.h
	rm -rf */*/*.c~
	rm -rf */*/*.h~


