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
	fyre-fa.desktop		\
	fyre-png.desktop

xdgmimedir = $(datadir)/mime/packages
xdgmime_DATA = fyre.xml

install-data-hook:
	update-mime-database $(datadir)/mime
	update-desktop-database

uninstall-hook:
	update-mime-database $(datadir)/mime
	update-desktop-database

EXTRA_DIST =			\
	$(fyredata_DATA)	\
	fyre-48x48.xcf.gz       \
        fyre-32x32.xcf.gz       \
        fyre-16x16.xcf.gz       \
        fyre-win32-icons.xcf.gz	\
	fyre.desktop.in		\
	fyre-fa.desktop.in	\
	fyre-png.desktop.in	\
	x-fyre-animation.xml	\
	x-fyre-animation.png
