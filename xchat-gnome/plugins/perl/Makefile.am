libdir = $(XCHATLIBDIR)/plugins

lib_LTLIBRARIES = perl.la
perl_la_SOURCES = perl.c
perl_la_LDFLAGS =		\
	-avoid-version -module	\
	$(PERL_LDFLAGS)
BUILT_SOURCES = xchat.pm.h
CLEANFILES = $(BUILT_SOURCES)

INCLUDES =			\
	$(PERL_CFLAGS)		\
	$(DEPENDENCIES_CFLAGS)  \
        -I$(srcdir)/..

xchat.pm.h: Xchat.pm IRC.pm
	perl $(srcdir)/generate_header $(srcdir)/Xchat.pm $(srcdir)/IRC.pm

EXTRA_DIST =			\
	generate_header		\
	IRC.pm			\
	Xchat.pm		\
	Xchat.pod		\
	xchat2-perl.html	\
	xchat2-perldocs.html
