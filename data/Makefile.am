fyredatadir = $(datadir)/fyre
fyredata_DATA = 		\
	explorer.glade		\
	animation-render.glade	\
	metadata-emblem.png	\
	about-box.fa		\
	fyre-48x48.png          \
        fyre-32x32.png          \
        fyre-16x16.png

if ENABLE_FDODESKTOP
desktopdir = $(datadir)/applications
%.desktop:	%.desktop.in
	sed 's|\@DATADIR\@|$(datadir)/fyre/|' $< > $@
desktop_DATA =			\
	fyre.desktop		\
	fyre-fa.desktop		\
	fyre-png.desktop
update_fdodesktop = update-desktop-database
else
update_fdodesktop = /bin/true
endif

if ENABLE_XDGMIME
xdgmimedir = $(datadir)/mime/packages
xdgmime_DATA = fyre.xml
update_xdgmime = update-mime-database $(datadir)/mime

xdgicondir = $(datadir)/icons/hicolor/48x48/mimetypes
xdgicon_DATA = application-x-fyre-animation.png
else
update_xdgmime = /bin/true
endif

install-data-hook:
	$(update_xdgmime) || true
	$(update_fdodesktop) || true

uninstall-hook:
	$(update_xdgmime) || true
	$(update_fdodesktop) || true

EXTRA_DIST =			\
	$(fyredata_DATA)	\
	fyre-48x48.xcf.gz       \
        fyre-32x32.xcf.gz       \
        fyre-16x16.xcf.gz       \
        fyre-win32-icons.xcf.gz	\
	fyre.desktop.in		\
	fyre-fa.desktop.in	\
	fyre-png.desktop.in	\
	fyre.xml		\
	application-x-fyre-animation.png
