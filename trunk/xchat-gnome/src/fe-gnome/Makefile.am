bin_PROGRAMS = xchat-gnome

xchat_gnomedir = $(datadir)/xchat

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

desktopdir = $(datadir)/applications
desktop_DATA = xchat-gnome.desktop

noinst_HEADERS =				\
	about.h					\
	channel-list.h				\
	connect-dialog.h			\
	dcc-window.h				\
	eggaccelerators.h			\
	eggcellrendererkeys.h			\
	irc-network.h				\
	irc-network-editor.h			\
	gui.h					\
	main-window.h				\
	navigation-tree.h			\
	palette.h				\
	pixmaps.h				\
	plugins.h				\
	preferences.h				\
	preferences-dialog.h			\
	preferences-keybindings-page.h		\
	preferences-page-colors.h		\
	preferences-page-effects.h		\
	preferences-page-dcc.h			\
	preferences-page-irc.h			\
	preferences-page-plugins.h		\
	preferences-page-networks.h		\
	setup-dialog.h				\
	textgui.h				\
	userlist.h				\
	userlist-gui.h				\
	util.h					\
	xg-plugin.h				\
	xtext.h					\
	$(mmx_cmod_h)

xchat_gnome_LDADD =				\
	../common/libxchatcommon.a		\
	$(DEPENDENCIES_LIBS)			\
	$(LIBSEXY_LIBS)

xchat_gnome_SOURCES =				\
	about.c					\
	channel-list.c				\
	connect-dialog.c			\
	dcc-window.c				\
	eggaccelerators.c			\
	eggcellrendererkeys.c			\
	fe-gnome.c				\
	irc-network.c				\
	irc-network-editor.c			\
	gui.c					\
	main-window.c				\
	navigation-tree.c			\
	palette.c				\
	pixmaps.c				\
	plugins.c				\
	preferences.c				\
	preferences-dialog.c			\
	preferences-keybindings-page.c		\
	preferences-page-colors.c		\
	preferences-page-effects.c		\
	preferences-page-dcc.c			\
	preferences-page-irc.c			\
	preferences-page-plugins.c		\
	preferences-page-networks.c		\
	setup-dialog.c				\
	textgui.c				\
	userlist.c				\
	userlist-gui.c				\
	util.c					\
	xtext.c					\
	$(mmx_cmod_S)

xchat_gnome_CFLAGS =				\
	$(DEPENDENCIES_CFLAGS)			\
	$(LIBSEXY_CFLAGS)			\
	-DSYSCONFDIR="\"$(sysconfdir)\""	\
	-DDATADIR="\"$(xchat_gnomedir)\""	\
	-DLIBDIR="\"$(xchatlibdir)\""		\
	-DPREFIX="\"$(prefix)\""		\
	-DXCHATSHAREDIR="\"$(xchat_gnomedir)\""	\
	-DXCHATLIBDIR="\"$(xchatlibdir)\""

EXTRA_DIST =					\
	$(xchat_gnome_SOURCES)			\
	$(xchat_gnome_DATA)			\
	$(desktop_DATA)				\
	apps_xchat.schemas
