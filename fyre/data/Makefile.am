fyredatadir = $(datadir)/fyre
fyredata_DATA = 		\
	explorer.glade		\
	animation-render.glade	\
	metadata-emblem.png	\
	about-box.fa		\
	fyre-48x48.png          \
        fyre-32x32.png          \
        fyre-16x16.png

desktopdir = $(datadir)/applications
fyre.desktop:	fyre.desktop.in
	sed 's|\@DATADIR\@|$(datadir)/fyre/|' $< > $@
desktop_DATA = fyre.desktop

mimedir = $(datadir)/mime/packages
mime_DATA = fyre.xml

mimeinfodir = $(datadir)/mime-info
mimeinfo_DATA = fyre.keys

EXTRA_DIST =			\
	$(fyredata_DATA)	\
	fyre-48x48.xcf.gz       \
        fyre-32x32.xcf.gz       \
        fyre-16x16.xcf.gz       \
        fyre-win32-icons.xcf.gz	\
	fyre.desktop.in		\
	fyre.keys		\
	x-fyre-animation.xml
