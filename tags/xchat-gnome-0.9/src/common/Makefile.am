if USE_OPENSSL
ssl_sources = ssl.c ssl.h
endif

noinst_LTLIBRARIES = libxchatcommon.la
libxchatcommon_la_SOURCES =		\
	cfgfiles.c			\
	cfgfiles.h			\
	ctcp.c				\
	ctcp.h				\
	dcc.c				\
	dcc.h				\
	fe.h				\
	history.c			\
	history.h			\
	ignore.c			\
	ignore.h			\
	inbound.c			\
	inbound.h			\
	inet.h				\
	modes.c				\
	modes.h				\
	network.c			\
	network.h			\
	notify.c			\
	notify.h			\
	outbound.c			\
	outbound.h			\
	plugin-timer.c			\
	plugin-timer.h			\
	plugin.c			\
	plugin.h			\
	proto-irc.c			\
	proto-irc.h			\
	server.c			\
	server.h			\
	servlist.c			\
	servlist.h			\
	text.c				\
	text.h				\
	textenums.h			\
	textevents.h			\
	tree.c				\
	tree.h				\
	url.c				\
	url.h				\
	userlist.c			\
	userlist.h			\
	util.c				\
	util.h				\
	xchat-plugin.h			\
	xchat.c				\
	xchat.h				\
	xchatc.h			\
	$(ssl_sources)

libxchatcommon_la_LDFLAGS = -static
INCLUDES =		\
	$(DEPENDENCIES_CFLAGS)		\
	$(OPENSSL_CFLAGS)		\
	-DXCHATLIBDIR="\"$(XCHATLIBDIR)\""

textevents: make-te
	./make-te < textevents.in > textevents.h 2> textenums.h
