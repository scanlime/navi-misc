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
	connect_dialog.h		\
	connect_dialog.c		\
	fe-gnome.c			\
	gui.h				\
	gui.c				\
	main_window.h			\
	main_window.c			\
	palette.h			\
	palette.c			\
	preferences_dialog.h		\
	preferences_dialog.c		\
	preferences_servers_page.h	\
	preferences_servers_page.c

xchat_gnome_LDADD = ../common/libxchatcommon.a $(GUI_LIBS)
xchat_gnome_SOURCES =			\
	about.h				\
	about.c				\
	connect_dialog.h		\
	connect_dialog.c		\
	fe-gnome.c			\
	gui.h				\
	gui.c				\
	main_window.h			\
	main_window.c			\
	$(mmx_cmod_S)			\
	palette.h			\
	palette.c			\
	preferences_dialog.h		\
	preferences_dialog.c		\
	preferences_servers_page.h	\
	preferences_servers_page.c	\
	xtext.h				\
	xtext.c

xtext.h: @top_srcdir@/src/fe-gtk/xtext.h
	cp -pR @top_srcdir@/src/fe-gtk/xtext.h .

xtext.c: @top_srcdir@/src/fe-gtk/xtext.c xtext.h
	cp -pR @top_srcdir@/src/fe-gtk/xtext.c .

mmx_cmod.h:	@top_srcdir@/src/fe-gtk/mmx_cmod.h
	cp -pR @top_srcdir@/src/fe-gtk/mmx_cmod.h .

mmx_cmod.S:	@top_srcdir@/src/fe-gtk/mmx_cmod.S mmx_cmod.h
	cp -pR @top_srcdir@/src/fe-gtk/mmx_cmod.S .
