libdir = $(XCHATLIBDIR)/plugins

schemadir	= $(GCONF_SCHEMA_FILE_DIR)
schema_in_files	= urlscraper.schemas.in
schema_DATA	= $(schema_in_files:.schemas.in=.schemas)
@INTLTOOL_SCHEMAS_RULE@

EXTRA_DIST = urlscraper.schemas.in

BUILT_SOURCES = urlscraper.schemas
DISTCLEANFILES = $(BUILT_SOURCES)

if GCONF_SCHEMAS_INSTALL
install-data-local:
	if test -z "$(DESTDIR)" ; then \
		for p in $(schema_DATA) ; do \
			GCONF_CONFIG_SOURCE=$(GCONF_SCHEMA_CONFIG_SOURCE) $(GCONFTOOL) --makefile-install-rule $$p; \
		done \
	fi
endif

lib_LTLIBRARIES = urlscraper.la
urlscraper_la_SOURCES = urlscraper.c
urlscraper_la_LDFLAGS = -avoid-version -module
INCLUDES =		\
	$(DEPENDENCIES_CFLAGS)	\
	-I$(srcdir)/..
