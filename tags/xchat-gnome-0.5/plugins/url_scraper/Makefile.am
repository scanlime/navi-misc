libdir = $(xchatlibdir)/plugins

schemadir = $(GCONF_SCHEMA_FILE_DIR)
schema_DATA = urlscraper.schemas

EXTRA_DIST = urlscraper.schemas

install-data-local:
	if test -z "$(DESTDIR)" ; then \
		for p in $(schema_DATA) ; do \
			$(GCONFTOOL) --install-schema-file=$(srcdir)/$$p; \
		done \
	fi

lib_LTLIBRARIES = urlscraper.la
urlscraper_la_SOURCES = urlscraper.c
urlscraper_la_LDFLAGS = -avoid-version -module
INCLUDES = $(COMMON_CFLAGS) $(GTK_CFLAGS) $(GUI_CFLAGS) -I$(srcdir)/..
