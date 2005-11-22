libdir = $(XCHATLIBDIR)/plugins

EXTRA_DIST =				\
	dbus-plugin.xml			\
	apps_xchat_url_handler.schemas	\
	marshallers.list		\
	README

BUILT_SOURCES =				\
	marshallers.h			\
	dbus-plugin-glue.h

CLEANFILES = $(BUILT_SOURCES)

lib_LTLIBRARIES = dbus.la
dbus_la_SOURCES = dbus-plugin.c
dbus_la_LDFLAGS = -avoid-version -module
dbus_la_LIBADD = $(DBUS_LIBS)
dbus_la_CFLAGS = 		\
	$(DEPENDENCIES_CFLAGS)	\
	$(DBUS_CFLAGS)		\
	-I$(srcdir)/..

bin_PROGRAMS = xchat-remote
xchat_remote_SOURCES = xchat-remote.c
xchat_remote_LDADD = $(DBUS_LIBS)
xchat_remote_CFLAGS = 				\
	$(DBUS_CFLAGS)				\
	-DLOCALEDIR="\"$(GNOMELOCALEDIR)\""	\
	-DAPP="\"$(bindir)/$(PACKAGE)\""

noinst_PROGRAMS = example
example_SOURCES = example.c
example_LDADD = $(DBUS_LIBS)
example_CFLAGS = $(DBUS_CFLAGS)

dbus-plugin-glue.h: dbus-plugin.xml
	$(DBUS_BINDING_TOOL) --prefix=remote_object --mode=glib-server --output=$@ $<

marshallers.h: marshallers.list
	$(GLIB_GENMARSHAL)  --header --body $< > $@

schemadir   = $(GCONF_SCHEMA_FILE_DIR)
schema_DATA = apps_xchat_url_handler.schemas
install-data-local:
	@GCONF_SCHEMAS_INSTALL_TRUE@ \
	if test -z "$(DESTDIR)" ; then \
		for p in $(schema_DATA) ; do \
			GCONF_CONFIG_SOURCE=$(GCONF_SCHEMA_CONFIG_SOURCE) $(GCONFTOOL) --makefile-install-rule $(srcdir)/$$p; \
		done \
	fi
