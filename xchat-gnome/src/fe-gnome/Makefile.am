if USE_MMX
mmx_cmod_S = mmx_cmod.S
mmx_cmod_h = mmx_cmod.h
endif

schemadir	= $(GCONF_SCHEMA_FILE_DIR)
schema_in_files	= apps_xchat.schemas.in
schema_DATA	= $(schema_in_files:.schemas.in=.schemas)
@INTLTOOL_SCHEMAS_RULE@

if GCONF_SCHEMAS_INSTALL
install-data-local:
	if test -z "$(DESTDIR)" ; then \
		for p in $(schema_DATA) ; do \
			GCONF_CONFIG_SOURCE=$(GCONF_SCHEMA_CONFIG_SOURCE) $(GCONFTOOL) --makefile-install-rule $$p; \
		done \
	fi
endif

bin_PROGRAMS = xchat-gnome

xchat_gnome_SOURCES =				\
	$(mmx_cmod_S)				\
	$(mmx_cmod_h)				\
	about.c					\
	about.h					\
	channel-list-window.c			\
	channel-list-window.h				\
	connect-dialog.c			\
	connect-dialog.h			\
	conversation-panel.c			\
	conversation-panel.h			\
	dcc-window.c				\
	dcc-window.h				\
	fe-gnome.c				\
	find-bar.c				\
	find-bar.h				\
	gui.c					\
	gui.h					\
	image-utils.c				\
	image-utils.h				\
	irc-network-editor.c			\
	irc-network-editor.h			\
	irc-network.c				\
	irc-network.h				\
	main-window.c				\
	main-window.h				\
	migration.h				\
	migration.c				\
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
	preferences-page-spellcheck.c		\
	preferences-page-spellcheck.h		\
	preferences-page.h			\
	preferences-page.c			\
	preferences.c				\
	preferences.h				\
	setup-dialog.c				\
	setup-dialog.h				\
	status-bar.h				\
	status-bar.c				\
	text-entry.c				\
	text-entry.h				\
	topic-label.c				\
	topic-label.h				\
	userlist-gui.c				\
	userlist-gui.h				\
	userlist.c				\
	userlist.h				\
	util.c					\
	util.h					\
	xg-plugin.h				\
	xtext.c					\
	xtext.h					\
	$(BUILT_SOURCES)

xchat_gnome_CPPFLAGS = \
	-DTOPSRCDIR="\"$(top_srcdir)\""		\
	-DSYSCONFDIR="\"$(sysconfdir)\""	\
	-DDATADIR="\"$(pkgdatadir)\""		\
	-DXCHATSHAREDIR="\"$(pkgdatadir)\""	\
	-DLIBDIR="\"$(pkglibdir)\""		\
	-DPREFIX="\"$(prefix)\""		\
	-DXCHATLIBDIR="\"$(pkglibdir)\""	\
	-DGNOMELOCALEDIR="\"$(datadir)/locale\""\
	$(AM_CPPFLAGS)

xchat_gnome_CFLAGS = \
	$(DEPENDENCIES_CFLAGS)			\
	$(LIBSEXY_CFLAGS)			\
	$(AM_CFLAGS)

xchat_gnome_LDFLAGS = \
	-export-dynamic	\
	$(AM_LDFLAGS)

xchat_gnome_LDADD = \
	$(top_builddir)/src/common/libxchatcommon.la	\
	$(DEPENDENCIES_LIBS)				\
	$(OPENSSL_LIBS)					\
	$(LIBSEXY_LIBS)

BUILT_SOURCES =	\
	xg-marshal.h	\
	xg-marshal.c

CLEANFILES = $(stamp_files) $(BUILT_SOURCES)
DISTCLEANFILES = $(stamp_files) apps_xchat.schemas $(BUILT_SOURCES)
MAINTAINERCLEANFILES = $(stamp_files) $(BUILT_SOURCES)

stamp_files = \
	stamp-xg-marshal.c		\
	stamp-xg-marshal.h

xg-marshal.c: stamp-xg-marshal.c
	@true
stamp-xg-marshal.c: xg-marshal.list
	$(GLIB_GENMARSHAL) --prefix=xg_marshal $(srcdir)/xg-marshal.list --header --body > xg-marshal.c \
	&& echo timestamp > $(@F)

xg-marshal.h: stamp-xg-marshal.h
	@true
stamp-xg-marshal.h: xg-marshal.list
	$(GLIB_GENMARSHAL) --prefix=xg_marshal $(srcdir)/xg-marshal.list --header > xg-marshal.h \
	&& echo timestamp > $(@F)

EXTRA_DIST =					\
	$(xchat_gnome_SOURCES)			\
	xg-marshal.list				\
	apps_xchat.schemas.in
