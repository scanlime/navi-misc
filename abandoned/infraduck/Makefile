LDFLAGS = -lpgserver

all: main data/gl_lucid.th

data/gl_lucid.th: media_src/gl_lucid.ths
	cd media_src; themec gl_lucid.ths && mv gl_lucid.th ../data/
