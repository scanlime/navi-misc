fyredatadir = $(datadir)/fyre
fyredata_DATA = 		\
	explorer.glade		\
	animation-render.glade	\
	metadata-emblem.png	\
	about-box.fa		\
	fyre-48x48.png          \
        fyre-32x32.png          \
        fyre-16x16.png		\
	x-fyre-animation.png

desktopdir = $(datadir)/applications
%.desktop:	%.desktop.in
	sed 's|\@DATADIR\@|$(datadir)/fyre/|' $< > $@
desktop_DATA =			\
	fyre.desktop		\
	fyre-fa.desktop

mimedir = $(datadir)/mime/packages
mime_DATA = fyre.xml

EXTRA_DIST =			\
	$(fyredata_DATA)	\
	fyre-48x48.xcf.gz       \
        fyre-32x32.xcf.gz       \
        fyre-16x16.xcf.gz       \
        fyre-win32-icons.xcf.gz	\
	fyre.desktop.in		\
	fyre-fa.desktop.in	\
	x-fyre-animation.xml	\
	x-fyre-animation.png
