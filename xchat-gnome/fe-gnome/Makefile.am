bin_PROGRAMS = xchat-gnome

INCLUDES =					\
	$(GUI_CFLAGS)				\
	-DXCHATSHAREDIR="\"$(xchat_gnomedir)\""	\
	-DXCHATLIBDIR="\"$(xchatlibdir)\""

if USE_MMX
mmx_cmod_S = 					\
	mmx_cmod.h				\
	mmx_cmod.S
endif

schemadir	= $(GCONF_SCHEMA_FILE_DIR)
schema_DATA	= apps_xchat.schemas.in.in

# @INTLTOOL_SCHEMAS_RULE@

desktopdir = $(datadir)/applications
desktop_DATA = xchat-gnome.desktop

xchat_gnome_DATA =				\
	channel-list.glade			\
	irc-network-editor.glade		\
	transfers.glade				\
	setup-dialog.glade			\
	topic-change.glade			\
	xchat-gnome.glade			\
	xchat-gnome-ui.xml
xchat_gnomedir = $(datadir)/xchat

install-data-local:
	if test -z "$(DESTDIR)" ; then						\
		for p in $(schema_DATA) ; do					\
			$(GCONFTOOL) --install-schema-file=$(srcdir)/$$p;	\
		done								\
	fi

EXTRA_DIST =					\
	BUGS					\
	about.h					\
	about.c					\
	channel-list.h				\
	channel-list.c				\
	connect-dialog.h			\
	connect-dialog.c			\
	eggaccelerators.c			\
	eggaccelerators.h			\
	eggcellrendererkeys.c			\
	eggcellrendererkeys.h			\
	fe-gnome.c				\
	irc-network.c				\
	irc-network.h				\
	irc-network-editor.c			\
	irc-network-editor.h			\
	gui.h					\
	gui.c					\
	main-window.h				\
	main-window.c				\
	navigation-tree.h			\
	navigation-tree.c			\
	palette.h				\
	palette.c				\
	pixmaps.h				\
	pixmaps.c				\
	preferences.h				\
	preferences.c				\
	preferences-colors-page.h		\
	preferences-colors-page.c		\
	preferences-dialog.h			\
	preferences-dialog.c			\
	preferences-irc-page.h			\
	preferences-irc-page.c			\
	preferences-keybindings-page.c		\
	preferences-keybindings-page.h		\
	preferences-plugins-page.h		\
	preferences-plugins-page.c		\
	preferences-servers-page.h		\
	preferences-servers-page.c		\
	setup-dialog.h				\
	setup-dialog.c				\
	textgui.h				\
	textgui.c				\
	transfers.h				\
	transfers.c				\
	userlist.h				\
	userlist.c				\
	userlist-gui.h				\
	userlist-gui.c				\
	util.h					\
	util.c					\
	$(xchat_gnome_DATA)			\
	$(desktop_DATA)				\
	apps_xchat.schemas.in.in

xchat_gnome_LDADD = ../common/libxchatcommon.a $(GUI_LIBS)
xchat_gnome_SOURCES =				\
	about.h					\
	about.c					\
	channel-list.h				\
	channel-list.c				\
	connect-dialog.h			\
	connect-dialog.c			\
	eggaccelerators.c			\
	eggaccelerators.h			\
	eggcellrendererkeys.c			\
	eggcellrendererkeys.h			\
	fe-gnome.c				\
	irc-network.c				\
	irc-network.h				\
	irc-network-editor.c			\
	irc-network-editor.h			\
	gui.h					\
	gui.c					\
	main-window.h				\
	main-window.c				\
	$(mmx_cmod_S)				\
	navigation-tree.h			\
	navigation-tree.c			\
	palette.h				\
	palette.c				\
	pixmaps.h				\
	pixmaps.c				\
	preferences.h				\
	preferences.c				\
	preferences-colors-page.h		\
	preferences-colors-page.c		\
	preferences-dialog.h			\
	preferences-dialog.c			\
	preferences-irc-page.h			\
	preferences-irc-page.c			\
	preferences-keybindings-page.c		\
	preferences-keybindings-page.h		\
	preferences-plugins-page.h		\
	preferences-plugins-page.c		\
	preferences-servers-page.h		\
	preferences-servers-page.c		\
	setup-dialog.h				\
	setup-dialog.c				\
	textgui.h				\
	textgui.c				\
	transfers.h				\
	transfers.c				\
	userlist.h				\
	userlist.c				\
	userlist-gui.h				\
	userlist-gui.c				\
	util.h					\
	util.c					\
	xtext.h					\
	xtext.c

about.c:	xtext.h

xtext.h: @top_srcdir@/src/fe-gtk/xtext.h
	cp -pR @top_srcdir@/src/fe-gtk/xtext.h .

xtext.c: @top_srcdir@/src/fe-gtk/xtext.c xtext.h
	cp -pR @top_srcdir@/src/fe-gtk/xtext.c .

mmx_cmod.h:	@top_srcdir@/src/fe-gtk/mmx_cmod.h
	cp -pR @top_srcdir@/src/fe-gtk/mmx_cmod.h .

mmx_cmod.S:	@top_srcdir@/src/fe-gtk/mmx_cmod.S mmx_cmod.h
	cp -pR @top_srcdir@/src/fe-gtk/mmx_cmod.S .

SUBDIRS = data
