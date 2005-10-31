bin_PROGRAMS = xchat-gnome

xchat_gnomedir = $(datadir)/xchat
INCLUDES =					\
	$(GUI_CFLAGS)				\
	$(LIBSEXY_CFLAGS)			\
	-DXCHATSHAREDIR="\"$(xchat_gnomedir)\""	\
	-DXCHATLIBDIR="\"$(xchatlibdir)\""

if USE_MMX
mmx_cmod_S = 					\
	mmx_cmod.h				\
	mmx_cmod.S
endif

schemadir	= $(GCONF_SCHEMA_FILE_DIR)
schema_DATA	= apps_xchat.schemas

# @INTLTOOL_SCHEMAS_RULE@

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

install-data-local:
	if test -z "$(DESTDIR)" ; then						\
		for p in $(schema_DATA) ; do					\
			$(GCONFTOOL) --install-schema-file=$(srcdir)/$$p;	\
		done								\
	fi

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

our_sources =					\
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
	$(plugin_sources)

xchat_sources =					\
	$(mmx_cmod_S)				\
	xtext.h					\
	xtext.c

xchat_gnome_SOURCES =				\
	$(our_sources)				\
	$(xchat_sources)

EXTRA_DIST =					\
	BUGS					\
	$(our_sources)				\
	$(xchat_gnome_DATA)			\
	$(desktop_DATA)				\
	apps_xchat.schemas

BUILT_SOURCES =					\
	mmx_cmod.h				\
	mmx_cmod.S				\
	xtext.h					\
	xtext.c

xtext.h: @top_srcdir@/src/fe-gtk/xtext.h
	cp -pR @top_srcdir@/src/fe-gtk/xtext.h .

xtext.c: @top_srcdir@/src/fe-gtk/xtext.c
	cp -pR @top_srcdir@/src/fe-gtk/xtext.c .

mmx_cmod.h:	@top_srcdir@/src/fe-gtk/mmx_cmod.h
	cp -pR @top_srcdir@/src/fe-gtk/mmx_cmod.h .

mmx_cmod.S:	@top_srcdir@/src/fe-gtk/mmx_cmod.S
	cp -pR @top_srcdir@/src/fe-gtk/mmx_cmod.S .

SUBDIRS = data
