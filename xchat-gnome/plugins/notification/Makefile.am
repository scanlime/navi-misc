libdir = $(XCHATLIBDIR)/plugins

schemadir	= $(GCONF_SCHEMA_FILE_DIR)
schema_in_files	= notification.schemas.in
schema_DATA	= $(schema_in_files:.schemas.in=.schemas)
@INTLTOOL_SCHEMAS_RULE@

EXTRA_DIST = notification.schemas.in

if GCONF_SCHEMAS_INSTALL
install-data-local:
	if test -z "$(DESTDIR)" ; then \
		for p in $(schema_DATA) ; do \
			GCONF_CONFIG_SOURCE=$(GCONF_SCHEMA_CONFIG_SOURCE) $(GCONFTOOL) --makefile-install-rule $$p; \
		done \
	fi
endif

BUILT_SOURCES = notification.schemas
DISTCLEANFILES = $(BUILT_SOURCES)

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
