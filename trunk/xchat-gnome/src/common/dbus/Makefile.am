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
	dbus-plugin.h				\
	dbus-client.c				\
	dbus-client.h
libxchatdbus_la_CPPFLAGS =			\
	-I$(srcdir)/..				\
	$(AM_CPPFLAGS)
libxchatdbus_la_CFLAGS =			\
	$(DBUS_CFLAGS)				\
	$(AM_CFLAGS)
libxchatdbus_la_LIBADD =			\
	$(DBUS_LIBS)

remote-object-glue.h: remote-object.xml
	$(LIBTOOL) --mode=execute $(DBUS_BINDING_TOOL) --prefix=remote_object --mode=glib-server --output=$@ $<

marshallers.h: marshallers.list
	$(LIBTOOL) --mode=execute $(GLIB_GENMARSHAL)  --header --body $< > $@

# Dbus service file
servicedir = $(DBUS_SERVICES_DIR)
service_in_files = org.gnome.Xchat.service.in
service_DATA = $(service_in_files:.service.in=.service)

# Rule to make the service file with bindir expanded
$(service_DATA): $(service_in_files) Makefile
	@sed -e "s|\@bindir\@|$(bindir)|" $< > $@

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

EXTRA_DIST =				\
	url_handler.schemas.in		\
	remote-object.xml		\
	marshallers.list		\
	example.py			\
	org.gnome.Xchat.service.in	\
	README

BUILT_SOURCES =				\
	remote-object-glue.h		\
	marshallers.h			\
	org.gnome.Xchat.service		\
	url_handler.schemas

CLEANFILES = $(BUILT_SOURCES)
