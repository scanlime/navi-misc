# $Id: Makefile.am,v 1.1.1.1 2002/05/26 00:50:03 chipx86 Exp $

SUBDIRS = src

EXTRA_DIST = \
	AUTHORS NEWS INSTALL COPYING \
	ChangeLog \
	autogen.sh \
	tvbuggy.spec.in \
	configure.in

dist-hook: tvbuggy.spec
	cp tvbuggy.spec $(distdir)

release:
	$(MAKE) dist distdir=$(PACKAGE)`date +"%y%m%d"`
