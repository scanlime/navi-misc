noinst_LIBRARIES = libNet.a

if BUGGY_MIPSPRO
MIPSPRO_CXXFLAGS = -LANG:std
else
MIPSPRO_CXXFLAGS = @CXXFLAGS@
endif

AM_CXXFLAGS = $(MIPSPRO_CXXFLAGS)

MAINTAINERCLEANFILES = \
	Makefile.in

libNet_a_SOURCES =			\
	Address.cpp			\
	AdnsHandler.cpp			\
	Pack.cpp			\
	Ping.cpp			\
	multicast.cpp			\
	udpnetwork.cpp			\
	network.cpp

EXTRA_DIST = \
	README