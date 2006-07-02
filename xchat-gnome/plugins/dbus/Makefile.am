libdir = $(XCHATLIBDIR)/plugins

EXTRA_DIST =					\
	dbus-plugin.xml				\
	xchat_gnome_url_handler.schemas.in	\
	marshallers.list			\
	README

lib_LTLIBRARIES = dbus.la
dbus_la_SOURCES = dbus-plugin.c
dbus_la_LDFLAGS = -avoid-version -module
dbus_la_LIBADD = $(DBUS_LIBS)

bin_PROGRAMS = xchat-gnome-remote
xchat_gnome_remote_SOURCES = xchat-gnome-remote.c
xchat_gnome_remote_LDADD = $(DBUS_LIBS)

noinst_PROGRAMS = example
example_SOURCES = example.c
example_LDADD = $(DBUS_LIBS)

INCLUDES =					\
	$(DEPENDENCIES_CFLAGS)			\
	$(DBUS_CFLAGS)				\
	-DLOCALEDIR="\"$(GNOMELOCALEDIR)\""	\
	-DAPP="\"$(bindir)/$(PACKAGE)\""	\
	-DPREFIX="\"$(prefix)\""		\
	-I$(srcdir)/..


dbus-plugin-glue.h: dbus-plugin.xml
	$(DBUS_BINDING_TOOL) --prefix=remote_object --mode=glib-server --output=$@ $<

marshallers.h: marshallers.list
	$(GLIB_GENMARSHAL)  --header --body $< > $@

schemadir	= $(GCONF_SCHEMA_FILE_DIR)
schema_in_files	= xchat_gnome_url_handler.schemas.in
schema_DATA	= $(schema_in_files:.schemas.in=.schemas)
@INTLTOOL_SCHEMAS_RULE@

if GCONF_SCHEMAS_INSTALL
install-data-local:
	if test -z "$(DESTDIR)" ; then \
		for p in $(schema_DATA) ; do \
			GCONF_CONFIG_SOURCE=$(GCONF_SCHEMA_CONFIG_SOURCE) $(GCONFTOOL) --makefile-install-rule $$p; \
		done \
	fi
endif

BUILT_SOURCES =				\
	marshallers.h			\
	dbus-plugin-glue.h

CLEANFILES = $(BUILT_SOURCES)
DISTCLEANFILES = xchat_gnome_url_handler.schemas
