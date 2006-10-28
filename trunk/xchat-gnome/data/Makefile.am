SUBDIRS = icons

xchat_gnome_DATA = 			\
	channel-list.glade		\
	connect-dialog.glade		\
	dcc-window.glade		\
	dcc.png				\
	irc-network-editor.glade	\
	message.wav			\
	preferences-dialog.glade	\
	setup-dialog.glade		\
	topic-change.glade		\
	voice.png			\
	xchat-gnome-ui.xml		\
	xchat-gnome.glade

xchat_gnomedir = $(datadir)/xchat-gnome

gtk_update_icon_cache = gtk-update-icon-cache -f -t $(datadir)/icons/hicolor

install-data-hook:
	@-if test -z "$(DESTDIR)"; then \
		echo "Updating Gtk icon cache."; \
		$(gtk_update_icon_cache); \
	else \
		echo "*** Icon cache not updated.  After install, run this:"; \
		echo "***   $(gtk_update_icon_cache)"; \
	fi

desktopdir = $(datadir)/applications
desktop_in_files = xchat-gnome.desktop.in
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)
man_MANS = xchat-gnome.1
@INTLTOOL_DESKTOP_RULE@

DISTCLEANFILES = $(desktop_DATA)

EXTRA_DIST = 				\
	$(xchat_gnome_DATA)		\
	$(icon_DATA)			\
	$(desktop_in_files)		\
	$(man_MANS)
