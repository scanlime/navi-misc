noinst_LTLIBRARIES = libxchatcommon.la

if USE_DBUS
dbusdir = dbus
endif
SUBDIRS = $(dbusdir) .

libxchatcommon_la_SOURCES =			\
	cfgfiles.c				\
	cfgfiles.h				\
	ctcp.c					\
	ctcp.h					\
	dcc.c					\
	dcc.h					\
	fe.h					\
	history.c				\
	history.h				\
	ignore.c				\
	ignore.h				\
	inbound.c				\
	inbound.h				\
	inet.h					\
	modes.c					\
	modes.h					\
	network.c				\
	network.h				\
	notify.c				\
	notify.h				\
	outbound.c				\
	outbound.h				\
	plugin-timer.c				\
	plugin-timer.h				\
	plugin.c				\
	plugin.h				\
	proto-irc.c				\
	proto-irc.h				\
	server.c				\
	server.h				\
	servlist.c				\
	servlist.h				\
	text.c					\
	text.h					\
	textenums.h				\
	textevents.h				\
	tree.c					\
	tree.h					\
	url.c					\
	url.h					\
	userlist.c				\
	userlist.h				\
	util.c					\
	util.h					\
	xchat-plugin.h				\
	xchat.c					\
	xchat.h					\
	xchatc.h

libxchatcommon_la_CPPFLAGS = 			\
	-DXCHATLIBDIR="\"$(pkglibdir)\""	\
	$(AM_CPPFLAGS)

libxchatcommon_la_CFLAGS = 			\
	-I$(top_srcdir)/src/common		\
	$(DEPENDENCIES_CFLAGS)			\
	$(OPENSSL_CFLAGS)			\
	$(AM_CFLAGS)

libxchatcommon_la_LDFLAGS =			\
	-static					\
	$(AM_LDFLAGS)

libxchatcommon_la_LIBADD =

if USE_OPENSSL
libxchatcommon_la_SOURCES += 			\
	ssl.c					\
 	ssl.h
endif

if USE_DBUS
libxchatcommon_la_CFLAGS +=				\
	-I$(top_builddir)/src/common/dbus
libxchatcommon_la_LIBADD +=				\
	$(top_builddir)/src/common/dbus/libxchatdbus.la
endif

textevents: make-te
	./make-te < textevents.in > textevents.h 2> textenums.h
