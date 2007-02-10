SUBDIRS = data help plugins po src

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST =			\
	gnome-doc-utils.make	\
	intltool-extract.in	\
	intltool-merge.in	\
	intltool-update.in

DISTCLEANFILES =		\
	intltool-extract	\
	intltool-merge		\
	intltool-update		\
	gnome-doc-utils.make

DISTCHECK_CONFIGURE_FLAGS = \
	--disable-scrollkeeper	\
	--disable-schemas-install
