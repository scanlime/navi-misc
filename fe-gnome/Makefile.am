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
	completion-store.h		\
	completion-store.c		\
	connect_dialog.h		\
	connect_dialog.c		\
	fe-gnome.c			\
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
	setup_druid.h			\
	setup_druid.c			\
	textgui.h			\
	textgui.c			\
	transfers.h			\
	transfers.c			\
	userlist.h			\
	userlist.c			\
	userlist_gui.h			\
	userlist_gui.c			\
	xtext2.h			\
	xtext2.c			\
	xtextbuffer.h			\
	xtextbuffer.c			\
	channel-list.glade		\
	transfers.glade			\
	xchat-gnome.glade		\
	apps_xchat.schemas.in.in

xchat_gnome_LDADD = ../common/libxchatcommon.a $(GUI_LIBS)
xchat_gnome_SOURCES =			\
	about.h				\
	about.c				\
	channel_list.h			\
	channel_list.c			\
	completion-store.h		\
	completion-store.c		\
	connect_dialog.h		\
	connect_dialog.c		\
	fe-gnome.c			\
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
	setup_druid.h			\
	setup_druid.c			\
	textgui.h			\
	textgui.c			\
	transfers.h			\
	transfers.c			\
	userlist.h			\
	userlist.c			\
	userlist_gui.h			\
	userlist_gui.c			\
	xtext2.h			\
	xtext2.c			\
	xtextbuffer.h			\
	xtextbuffer.c

xchat_gnome_DATA =			\
	channel-list.glade		\
	transfers.glade			\
	xchat-gnome.glade
xchat_gnomedir = $(datadir)/xchat

mmx_cmod.h:	@top_srcdir@/src/fe-gtk/mmx_cmod.h
	cp -pR @top_srcdir@/src/fe-gtk/mmx_cmod.h .

mmx_cmod.S:	@top_srcdir@/src/fe-gtk/mmx_cmod.S mmx_cmod.h
	cp -pR @top_srcdir@/src/fe-gtk/mmx_cmod.S .

about.o:	$(srcdir)/about.c
	$(COMPILE) -DXCHATSHAREDIR=\"$(xchat_gnomedir)\" -c $(srcdir)/about.c

pixmaps.o:	$(srcdir)/pixmaps.c
	$(COMPILE) -DXCHATSHAREDIR=\"$(xchat_gnomedir)\" -c $(srcdir)/pixmaps.c

gui.o:		$(srcdir)/gui.c
	$(COMPILE)  -DXCHATSHAREDIR=\"$(xchat_gnomedir)\" -c $(srcdir)/gui.c

channel_list.o:	$(srcdir)/channel_list.c
	$(COMPILE)  -DXCHATSHAREDIR=\"$(xchat_gnomedir)\" -c $(srcdir)/channel_list.c

transfers.o:	$(srcdir)/transfers.c
	$(COMPILE)  -DXCHATSHAREDIR=\"$(xchat_gnomedir)\" -c $(srcdir)/transfers.c

guir.o:		$(srcdir)/guir.c
	$(COMPILE)  -DXCHATSHAREDIR=\"$(xchat_gnomedir)\" -c $(srcdir)/guir.c

SUBDIRS = data pixmaps
