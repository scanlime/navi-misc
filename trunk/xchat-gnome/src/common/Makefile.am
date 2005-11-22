noinst_LIBRARIES = libxchatcommon.a

LINK = $(LIBTOOL) --mode=link $(CXXLD) $(AM_CFLAGS) $(CFLAGS) $(LDFLAGS) -o $@

noinst_HEADERS =			\
	cfgfiles.h			\
	ctcp.h				\
	dcc.h				\
	fe.h				\
	history.h			\
	ignore.h			\
	inbound.h			\
	inet.h				\
	modes.h				\
	notify.h			\
	network.h			\
	outbound.h			\
	plugin-timer.h			\
	plugin.h			\
	proto-irc.h			\
	server.h			\
	servlist.h			\
	ssl.h				\
	text.h				\
	textevents.h			\
	textenums.h			\
	tree.h				\
	url.h				\
	userlist.h			\
	util.h				\
	xchat-plugin.h			\
	xchat.h				\
	xchatc.h

libxchatcommon_a_SOURCES =		\
	cfgfiles.c			\
	ctcp.c				\
	dcc.c				\
	history.c			\
	ignore.c			\
	inbound.c			\
	modes.c				\
	network.c			\
	notify.c			\
	outbound.c			\
	plugin-timer.c			\
	plugin.c			\
	proto-irc.c			\
	server.c			\
	servlist.c			\
	ssl.c				\
	text.c				\
	tree.c				\
	url.c				\
	userlist.c			\
	util.c				\
	xchat.c

libxchatcommon_a_CFLAGS =		\
	$(DEPENDENCIES_CFLAGS)		\
	-DXCHATLIBDIR="\"$(XCHATLIBDIR)\""
libxchatcommon_a_LDFLAGS = -static

textevents: make-te
	./make-te < textevents.in > textevents.h 2> textenums.h
