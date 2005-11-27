xchat_gnomedir = $(datadir)/$(PACKAGE)

if USE_MMX
mmx_cmod_S = mmx_cmod.S
mmx_cmod_h = mmx_cmod.h
endif

schemadir	= $(GCONF_SCHEMA_FILE_DIR)
schema_DATA	= apps_xchat.schemas

install-data-local:
	@GCONF_SCHEMAS_INSTALL_TRUE@ \
	if test -z "$(DESTDIR)" ; then						\
		for p in $(schema_DATA) ; do					\
			$(GCONFTOOL) --install-schema-file=$(srcdir)/$$p;	\
		done								\
	fi

bin_PROGRAMS = xchat-gnome
xchat_gnome_LDFLAGS = -export-dynamic
xchat_gnome_LDADD =				\
	../common/libxchatcommon.la		\
	$(DEPENDENCIES_LIBS)			\
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
	dcc-window.c				\
	dcc-window.h				\
	fe-gnome.c				\
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
	textgui.c				\
	textgui.h				\
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
	marshallers.h

CLEANFILES = $(BUILT_SOURCES)

marshallers.h: marshallers.list
	$(GLIB_GENMARSHAL)  --header --body $< > $@

EXTRA_DIST =					\
	$(xchat_gnome_SOURCES)			\
	marshallers.list			\
	apps_xchat.schemas
