bin_PROGRAMS = xchat-gnome

INCLUDES = $(GUI_CFLAGS)

if USE_MMX
mmx_cmod_S = 				\
	mmx_cmod.h			\
	mmx_cmod.S
endif

schemadir	= $(GCONF_SCHEMA_FILE_DIR)
schema_DATA	= apps_xchat.schemas.in.in

# @INTLTOOL_SCHEMAS_RULE@

desktopdir = $(datadir)/applications
desktop_DATA = xchat-gnome.desktop

xchat_gnome_DATA =			\
	channel-list.glade		\
	transfers.glade			\
	setup-dialog.glade		\
	topic-change.glade		\
	xchat-gnome.glade		\
	xchat-gnome-ui.xml
xchat_gnomedir = $(datadir)/xchat

install-data-local:
	if test -z "$(DESTDIR)" ; then						\
		for p in $(schema_DATA) ; do					\
			$(GCONFTOOL) --install-schema-file=$(srcdir)/$$p;	\
		done								\
	fi

EXTRA_DIST =				\
	BUGS				\
	about.h				\
	about.c				\
	channel_list.h			\
	channel_list.c			\
	connect_dialog.h		\
	connect_dialog.c		\
	fe-gnome.c			\
	gtkutil.h			\
	gtkutil.c			\
	gui.h				\
	gui.c				\
	main_window.h			\
	main_window.c			\
	navigation_tree.h		\
	navigation_tree.c		\
	palette.h			\
	palette.c			\
	pixmaps.h			\
	pixmaps.c			\
	preferences.h			\
	preferences.c			\
	preferences_colors_page.h	\
	preferences_colors_page.c	\
	preferences_dialog.h		\
	preferences_dialog.c		\
	preferences_irc_page.h		\
	preferences_irc_page.c		\
	preferences_plugins_page.h	\
	preferences_plugins_page.c	\
	preferences_servers_page.h	\
	preferences_servers_page.c	\
	setup_dialog.h			\
	setup_dialog.c			\
	textgui.h			\
	textgui.c			\
	transfers.h			\
	transfers.c			\
	userlist.h			\
	userlist.c			\
	userlist_gui.h			\
	userlist_gui.c			\
	$(xchat_gnome_DATA)		\
	$(desktop_DATA)			\
	apps_xchat.schemas.in.in

xchat_gnome_LDADD = ../common/libxchatcommon.a $(GUI_LIBS)
xchat_gnome_SOURCES =			\
	about.h				\
	about.c				\
	channel_list.h			\
	channel_list.c			\
	connect_dialog.h		\
	connect_dialog.c		\
	fe-gnome.c			\
	gtkutil.h			\
	gtkutil.c			\
	gui.h				\
	gui.c				\
	main_window.h			\
	main_window.c			\
	$(mmx_cmod_S)			\
	navigation_tree.h		\
	navigation_tree.c		\
	palette.h			\
	palette.c			\
	pixmaps.h			\
	pixmaps.c			\
	preferences.h			\
	preferences.c			\
	preferences_colors_page.h	\
	preferences_colors_page.c	\
	preferences_dialog.h		\
	preferences_dialog.c		\
	preferences_irc_page.h		\
	preferences_irc_page.c		\
	preferences_plugins_page.h	\
	preferences_plugins_page.c	\
	preferences_servers_page.h	\
	preferences_servers_page.c	\
	setup_dialog.h			\
	setup_dialog.c			\
	textgui.h			\
	textgui.c			\
	transfers.h			\
	transfers.c			\
	userlist.h			\
	userlist.c			\
	userlist_gui.h			\
	userlist_gui.c			\
	xtext.h				\
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

about.o:	$(srcdir)/about.c
	$(COMPILE) -DXCHATSHAREDIR=\"$(xchat_gnomedir)\" -c $(srcdir)/about.c

pixmaps.o:	$(srcdir)/pixmaps.c
	$(COMPILE) -DXCHATSHAREDIR=\"$(xchat_gnomedir)\" -c $(srcdir)/pixmaps.c

gui.o:		$(srcdir)/gui.c
	$(COMPILE) -DXCHATSHAREDIR=\"$(xchat_gnomedir)\" -c $(srcdir)/gui.c

channel_list.o:	$(srcdir)/channel_list.c
	$(COMPILE) -DXCHATSHAREDIR=\"$(xchat_gnomedir)\" -c $(srcdir)/channel_list.c

transfers.o:	$(srcdir)/transfers.c
	$(COMPILE) -DXCHATSHAREDIR=\"$(xchat_gnomedir)\" -c $(srcdir)/transfers.c

guir.o:		$(srcdir)/guir.c
	$(COMPILE) -DXCHATSHAREDIR=\"$(xchat_gnomedir)\" -c $(srcdir)/guir.c

setup_dialog.o:		$(srcdir)/setup_dialog.c
	$(COMPILE) -DXCHATSHAREDIR=\"$(xchat_gnomedir)\" -c $(srcdir)/setup_dialog.c

main_window.o:	$(srcdir)/main_window.c
	$(COMPILE) -DXCHATSHAREDIR=\"$(xchat_gnomedir)\" -c $(srcdir)/main_window.c

preferences_plugins_page.o: $(srcdir)/preferences_plugins_page.c
	$(COMPILE) -DXCHATLIBDIR=\"$(xchatlibdir)\" -c $(srcdir)/preferences_plugins_page.c

SUBDIRS = data pixmaps
