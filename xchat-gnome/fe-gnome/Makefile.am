bin_PROGRAMS = xchat-gnome

xchat_gnomedir = $(datadir)/xchat
INCLUDES =					\
	$(GUI_CFLAGS)				\
	$(LIBSEXY_CFLAGS)			\
	-DSYSCONFDIR="\"$(sysconfdir)\""	\
	-DDATADIR="\"$(xchat_gnomedir)\""	\
	-DLIBDIR="\"$(xchatlibdir)\""		\
	-DXCHATSHAREDIR="\"$(xchat_gnomedir)\""	\
	-DXCHATLIBDIR="\"$(xchatlibdir)\""

if USE_MMX
mmx_cmod_S = 					\
	mmx_cmod.h				\
	mmx_cmod.S
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

xchat_gnome_DATA =				\
	channel-list.glade			\
	connect-dialog.glade			\
	dcc-window.glade			\
	irc-network-editor.glade		\
	preferences-dialog.glade		\
	setup-dialog.glade			\
	topic-change.glade			\
	xchat-gnome.glade			\
	xchat-gnome-ui.xml

xchat_gnome_LDADD =				\
	../common/libxchatcommon.a		\
	$(GUI_LIBS)				\
	$(LIBSEXY_LIBS)

if DO_PLUGIN
plugin_sources =				\
	plugins.c				\
	plugins.h				\
	preferences-page-plugins.c		\
	preferences-page-plugins.h
endif

xchat_gnome_SOURCES =				\
	about.c					\
	about.h					\
	channel-list.c				\
	channel-list.h				\
	connect-dialog.c			\
	connect-dialog.h			\
	dcc-window.c				\
	dcc-window.h				\
	eggaccelerators.c			\
	eggaccelerators.h			\
	eggcellrendererkeys.c			\
	eggcellrendererkeys.h			\
	fe-gnome.c				\
	irc-network.c				\
	irc-network.h				\
	irc-network-editor.c			\
	irc-network-editor.h			\
	gui.c					\
	gui.h					\
	main-window.c				\
	main-window.h				\
	navigation-tree.c			\
	navigation-tree.h			\
	palette.c				\
	palette.h				\
	pixmaps.c				\
	pixmaps.h				\
	preferences.c				\
	preferences.h				\
	preferences-dialog.c			\
	preferences-dialog.h			\
	preferences-page-colors.c		\
	preferences-page-colors.h		\
	preferences-page-effects.c		\
	preferences-page-effects.h		\
	preferences-page-dcc.c			\
	preferences-page-dcc.h			\
	preferences-page-irc.c			\
	preferences-page-irc.h			\
	preferences-keybindings-page.h		\
	preferences-keybindings-page.c		\
	preferences-page-networks.h		\
	preferences-page-networks.c		\
	setup-dialog.c				\
	setup-dialog.h				\
	textgui.c				\
	textgui.h				\
	userlist.c				\
	userlist.h				\
	userlist-gui.c				\
	userlist-gui.h				\
	util.c					\
	util.h					\
	xg-plugin.h				\
	xtext.h					\
	xtext.c					\
	$(mmx_cmod_S)				\
	$(plugin_sources)

EXTRA_DIST =					\
	$(xchat_gnome_SOURCES)			\
	$(xchat_gnome_DATA)			\
	$(desktop_DATA)				\
	apps_xchat.schemas

SUBDIRS = data help
