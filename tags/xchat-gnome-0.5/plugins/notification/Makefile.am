libdir = $(xchatlibdir)/plugins

schemadir = $(GCONF_SCHEMA_FILE_DIR)
schema_DATA = notification.schemas

EXTRA_DIST = notification.schemas

install-local-data:
	if test -z "$(DESTDIR)" ; then \
		for p in $(schema_DATA) ; do \
			$(GCONFTOOL) --install-schema-file=$(srcdir)/$$p; \
		done \
	fi

lib_LTLIBRARIES = notification.la
notification_la_SOURCES =	\
	eggtrayicon.h		\
	eggtrayicon.c		\
	notification.c		\
	xg-plugin.h
notification_la_LDFLAGS = -avoid-version -module

INCLUDES = $(COMMON_CFLAGS) \
		   $(GTK_CFLAGS) \
		   $(GUI_CFLAGS) \
		   -I$(srcdir)/.. \
		   -I$(top_srcdir)/src/fe-gnome \
		   -DEGG_COMPILATION \
		   -DXCHATSHAREDIR="\"$(datadir)/xchat\""
