xchat_gnome_DATA = 			\
	channel-list.glade		\
	color.png			\
	connect-dialog.glade		\
	dcc-window.glade		\
	dcc.png				\
	effects.png			\
	global-message.png		\
	hop.png				\
	irc-network-editor.glade	\
	irc.png				\
	logout.png			\
	message.wav			\
	newdata.png			\
	nicksaid.png			\
	notauthorized.png		\
	op.png				\
	plugin-manager.png		\
	preferences-dialog.glade	\
	preferences-keybindings.png	\
	purple.png			\
	red.png				\
	servers.png			\
	setup-dialog.glade		\
	soap.png			\
	stock_down.png			\
	stock_link.png			\
	stock_up.png			\
	toilet-paper.png		\
	topic-change.glade		\
	users.png			\
	voice.png			\
	wrap.png			\
	xchat-gnome-ui.xml		\
	xchat-gnome.glade		\
	xchat-gnome.png			\
	spellcheck.png

xchat_gnomedir = $(datadir)/xchat-gnome

icondir = $(datadir)/icons/hicolor/48x48/apps
icon_DATA = xchat-gnome.png

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
@INTLTOOL_DESKTOP_RULE@

DISTCLEANFILES = $(desktop_DATA)

EXTRA_DIST = 				\
	$(xchat_gnome_DATA)		\
	$(icon_DATA)			\
	$(desktop_in_files)
