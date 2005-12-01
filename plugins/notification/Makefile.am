libdir = $(XCHATLIBDIR)/plugins

schemadir = $(GCONF_SCHEMA_FILE_DIR)
schema_DATA = notification.schemas
EXTRA_DIST = notification.schemas
install-data-local:
	@GCONF_SCHEMAS_INSTALL_TRUE@ \
	if test -z "$(DESTDIR)" ; then \
		for p in $(schema_DATA) ; do \
			$(GCONFTOOL) --install-schema-file=$(srcdir)/$$p; \
		done \
	fi

lib_LTLIBRARIES = notification.la
notification_la_SOURCES =	\
	eggtrayicon.h		\
	eggtrayicon.c		\
	notification.c
notification_la_LDFLAGS = -avoid-version -module
INCLUDES =		\
	$(DEPENDENCIES_CFLAGS)		\
	-I$(top_srcdir)/src/fe-gnome	\
	-I$(srcdir)/..			\
	-DEGG_COMPILATION		\
	-DXCHATSHAREDIR="\"$(XCHATSHAREDIR)\""
