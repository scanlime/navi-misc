SUBDIRS = data src contrib

desktopdir = $(datadir)/applications
fyre.desktop: fyre.desktop.in
	sed 's|\@DATADIR\@|$(datadir)/fyre/|' $< > $@
desktop_DATA = fyre.desktop

EXTRA_DIST = \
	AUTHORS NEWS INSTALL COPYING README \
	ChangeLog \
	autogen.sh \
	configure.ac \
	fyre.desktop.in
