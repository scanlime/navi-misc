bin_PROGRAMS = xchat-gnome-remote
xchat_gnome_remote_SOURCES =			\
	xchat-remote.c
xchat_gnome_remote_LDADD =			\
	$(DBUS_LIBS)				\
	$(DEPENDENCIES_LIBS)
xchat_gnome_remote_CPPFLAGS =			\
	-DLOCALEDIR="\"$(datadir)/locale\""	\
	-DPREFIX="\"$(prefix)\""		\
	$(AM_CPPFLAGS)
xchat_gnome_remote_CFLAGS =			\
	$(DEPENDENCIES_CFLAGS)			\
	$(DBUS_CFLAGS)				\
	$(AM_CFLAGS)


noinst_PROGRAMS = example
example_SOURCES =				\
	example.c
example_LDADD =					\
	$(DBUS_LIBS)				\
	$(DEPENDENCIES_LIBS)
example_CPPFLAGS =				\
	$(AM_CPPFLAGS)
example_CFLAGS =				\
	$(DEPENDENCIES_CFLAGS)			\
	$(DBUS_CFLAGS)				\
	$(AM_CFLAGS)

noinst_LTLIBRARIES = libxchatdbus.la
libxchatdbus_la_SOURCES =			\
	dbus-plugin.c				\
	dbus-plugin.h
libxchatdbus_la_CPPFLAGS =			\
	-I$(srcdir)/..				\
	$(AM_CPPFLAGS)
libxchatdbus_la_CFLAGS =			\
	$(DBUS_CFLAGS)				\
	$(AM_CFLAGS)
libxchatdbus_la_LIBADD =			\
	$(DBUS_LIBS)

dbus-plugin-glue.h: dbus-plugin.xml
	$(LIBTOOL) --mode=execute $(DBUS_BINDING_TOOL) --prefix=remote_object --mode=glib-server --output=$@ $<

marshallers.h: marshallers.list
	$(LIBTOOL) --mode=execute $(GLIB_GENMARSHAL)  --header --body $< > $@

schemadir = $(GCONF_SCHEMA_FILE_DIR)
schema_in_files = url_handler.schemas.in
schema_DATA = $(schema_in_files:.schemas.in=.schemas)
@INTLTOOL_SCHEMAS_RULE@

if GCONF_SCHEMAS_INSTALL
install-data-local:
	if test -z "$(DESTDIR)" ; then \
		for p in $(schema_DATA) ; do \
			GCONF_CONFIG_SOURCE=$(GCONF_SCHEMA_CONFIG_SOURCE) $(GCONFTOOL) --makefile-install-rule $(srcdir)/$$p; \
		done \
	fi
endif

EXTRA_DIST =			\
	url_handler.schemas.in	\
	dbus-plugin.xml		\
	marshallers.list	\
	example.py		\
	README

BUILT_SOURCES =			\
	dbus-plugin-glue.h	\
	marshallers.h		\
	url_handler.schemas

CLEANFILES = $(BUILT_SOURCES)
