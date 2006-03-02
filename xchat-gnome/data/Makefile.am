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
	xchat-gnome-small.png		\
	xchat-gnome-ui.xml		\
	xchat-gnome.glade		\
	xchat-gnome.png

xchat_gnomedir = $(datadir)/xchat-gnome

icondir = $(datadir)/pixmaps
icon_DATA = xchat-gnome.png

desktopdir = $(datadir)/applications
desktop_in_files = xchat-gnome.desktop.in
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)
@INTLTOOL_DESKTOP_RULE@

CLEANFILES = $(desktop_DATA)

EXTRA_DIST = 				\
	$(xchat_gnome_DATA)		\
	$(icon_DATA)			\
	$(desktop_in_files)
