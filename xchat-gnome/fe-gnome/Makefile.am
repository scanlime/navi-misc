bin_PROGRAMS = xchat-gnome

INCLUDES = $(GUI_CFLAGS)

if USE_MMX
mmx_cmod_S = 				\
	mmx_cmod.h			\
	mmx_cmod.S
endif

EXTRA_DIST =				\
	about.h				\
	about.c				\
	channel_list.h			\
	channel_list.c			\
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
	userlist.h			\
	userlist.c			\
	channel-list.glade		\
	xchat-gnome.glade

xchat_gnome_LDADD = ../common/libxchatcommon.a $(GUI_LIBS)
xchat_gnome_SOURCES =			\
	about.h				\
	about.c				\
	channel_list.h			\
	channel_list.c			\
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
	userlist.h			\
	userlist.c			\
	xtext.h				\
	xtext.c

xchat_gnome_DATA =			\
	channel-list.glade		\
	xchat-gnome.glade
xchat_gnomedir = $(xchatsharedir)

about.c:	xtext.h

xtext.h: @top_srcdir@/src/fe-gtk/xtext.h
	cp -pR @top_srcdir@/src/fe-gtk/xtext.h .

xtext.c: @top_srcdir@/src/fe-gtk/xtext.c xtext.h
	cp -pR @top_srcdir@/src/fe-gtk/xtext.c .

mmx_cmod.h:	@top_srcdir@/src/fe-gtk/mmx_cmod.h
	cp -pR @top_srcdir@/src/fe-gtk/mmx_cmod.h .

mmx_cmod.S:	@top_srcdir@/src/fe-gtk/mmx_cmod.S mmx_cmod.h
	cp -pR @top_srcdir@/src/fe-gtk/mmx_cmod.S .

pixmaps.o:	$(srcdir)/pixmaps.c
	$(COMPILE) -DXCHATSHAREDIR=\"$(xchatsharedir)\" -c $(srcdir)/pixmaps.c

gui.o:		$(srcdir)/gui.c
	$(COMPILE)  -DXCHATSHAREDIR=\"$(xchatsharedir)\" -c $(srcdir)/gui.c

channel_list.o:	$(srcdir)/channel_list.c
	$(COMPILE)  -DXCHATSHAREDIR=\"$(xchatsharedir)\" -c $(srcdir)/channel_list.c

SUBDIRS = data
