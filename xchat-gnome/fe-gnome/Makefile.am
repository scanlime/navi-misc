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
	channel_list.h				\
	channel_list.c				\
	connect_dialog.h			\
	connect_dialog.c			\
	eggaccelerators.c			\
	eggaccelerators.h			\
	eggcellrendererkeys.c			\
	eggcellrendererkeys.h			\
	error_dialog.c				\
	error_dialog.h				\
	fe-gnome.c				\
	irc-network.c				\
	irc-network.h				\
	irc-network-editor.c			\
	irc-network-editor.h			\
	gui.h					\
	gui.c					\
	main_window.h				\
	main_window.c				\
	navigation_tree.h			\
	navigation_tree.c			\
	palette.h				\
	palette.c				\
	pixmaps.h				\
	pixmaps.c				\
	preferences.h				\
	preferences.c				\
	preferences_colors_page.h		\
	preferences_colors_page.c		\
	preferences_dialog.h			\
	preferences_dialog.c			\
	preferences_irc_page.h			\
	preferences_irc_page.c			\
	preferences_plugins_page.h		\
	preferences_plugins_page.c		\
	preferences_keybindings_page.c		\
	preferences_keybindings_page.h		\
	preferences_servers_page.h		\
	preferences_servers_page.c		\
	setup_dialog.h				\
	setup_dialog.c				\
	textgui.h				\
	textgui.c				\
	transfers.h				\
	transfers.c				\
	userlist.h				\
	userlist.c				\
	userlist_gui.h				\
	userlist_gui.c				\
	$(xchat_gnome_DATA)			\
	$(desktop_DATA)				\
	apps_xchat.schemas.in.in

xchat_gnome_LDADD = ../common/libxchatcommon.a $(GUI_LIBS)
xchat_gnome_SOURCES =				\
	about.h					\
	about.c					\
	channel_list.h				\
	channel_list.c				\
	connect_dialog.h			\
	connect_dialog.c			\
	eggaccelerators.c			\
	eggaccelerators.h			\
	eggcellrendererkeys.c			\
	eggcellrendererkeys.h			\
	error_dialog.c				\
	error_dialog.h				\
	fe-gnome.c				\
	irc-network.c				\
	irc-network.h				\
	irc-network-editor.c			\
	irc-network-editor.h			\
	gui.h					\
	gui.c					\
	main_window.h				\
	main_window.c				\
	$(mmx_cmod_S)				\
	navigation_tree.h			\
	navigation_tree.c			\
	palette.h				\
	palette.c				\
	pixmaps.h				\
	pixmaps.c				\
	preferences.h				\
	preferences.c				\
	preferences_colors_page.h		\
	preferences_colors_page.c		\
	preferences_dialog.h			\
	preferences_dialog.c			\
	preferences_irc_page.h			\
	preferences_irc_page.c			\
	preferences_plugins_page.h		\
	preferences_plugins_page.c		\
	preferences_keybindings_page.c		\
	preferences_keybindings_page.h		\
	preferences_servers_page.h		\
	preferences_servers_page.c		\
	setup_dialog.h				\
	setup_dialog.c				\
	textgui.h				\
	textgui.c				\
	transfers.h				\
	transfers.c				\
	userlist.h				\
	userlist.c				\
	userlist_gui.h				\
	userlist_gui.c				\
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
