xchat_gnomedir = $(datadir)/$(PACKAGE)

if USE_MMX
mmx_cmod_S = mmx_cmod.S
mmx_cmod_h = mmx_cmod.h
endif

schemadir	= $(GCONF_SCHEMA_FILE_DIR)
schema_in_files	= apps_xchat.schemas.in
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

bin_PROGRAMS = xchat-gnome
xchat_gnome_LDFLAGS = -export-dynamic
xchat_gnome_LDADD =				\
	../common/libxchatcommon.la		\
	$(DEPENDENCIES_LIBS)			\
	$(OPENSSL_LIBS)				\
	$(LIBSEXY_LIBS)

xchat_gnome_SOURCES =				\
	$(mmx_cmod_S)				\
	$(mmx_cmod_h)				\
	about.c					\
	about.h					\
	channel-list.c				\
	channel-list.h				\
	connect-dialog.c			\
	connect-dialog.h			\
	conversation-panel.c			\
	conversation-panel.h			\
	dcc-window.c				\
	dcc-window.h				\
	fe-gnome.c				\
	find-bar.c				\
	find-bar.h				\
	gui.c					\
	gui.h					\
	irc-network-editor.c			\
	irc-network-editor.h			\
	irc-network.c				\
	irc-network.h				\
	main-window.c				\
	main-window.h				\
	navigation-tree.c			\
	navigation-tree.h			\
	palette.c				\
	palette.h				\
	pixmaps.c				\
	pixmaps.h				\
	plugins.c				\
	plugins.h				\
	preferences-dialog.c			\
	preferences-dialog.h			\
	preferences-page-colors.c		\
	preferences-page-colors.h		\
	preferences-page-dcc.c			\
	preferences-page-dcc.h			\
	preferences-page-effects.c		\
	preferences-page-effects.h		\
	preferences-page-irc.c			\
	preferences-page-irc.h			\
	preferences-page-networks.c		\
	preferences-page-networks.h		\
	preferences-page-plugins.c		\
	preferences-page-plugins.h		\
	preferences.c				\
	preferences.h				\
	setup-dialog.c				\
	setup-dialog.h				\
	status-bar.h				\
	status-bar.c				\
	text-entry.c				\
	text-entry.h				\
	topic-label.c				\
	topic-label.h				\
	userlist-gui.c				\
	userlist-gui.h				\
	userlist.c				\
	userlist.h				\
	util.c					\
	util.h					\
	xg-plugin.h				\
	xtext.c					\
	xtext.h

INCLUDES =					\
	$(DEPENDENCIES_CFLAGS)			\
	$(LIBSEXY_CFLAGS)			\
	-DSYSCONFDIR="\"$(sysconfdir)\""	\
	-DDATADIR="\"$(xchat_gnomedir)\""	\
	-DLIBDIR="\"$(libdir)\""		\
	-DPREFIX="\"$(prefix)\""		\
	-DXCHATLIBDIR="\"$(XCHATLIBDIR)\""	\
	-DXCHATSHAREDIR="\"$(XCHATSHAREDIR)\""

BUILT_SOURCES =					\
	apps_xchat.schemas			\
	marshallers.h

CLEANFILES = $(BUILT_SOURCES)

marshallers.h: marshallers.list
	$(GLIB_GENMARSHAL)  --header --body $< > $@

EXTRA_DIST =					\
	$(xchat_gnome_SOURCES)			\
	marshallers.list			\
	apps_xchat.schemas.in
