SUBDIRS = data help plugins po src

EXTRA_DIST =			\
	gnome-doc-utils.make	\
	intltool-extract.in	\
	intltool-merge.in	\
	intltool-update.in

CLEANFILES =			\
	intltool-extract	\
	intltool-merge		\
	intltool-update

DISTCLEANFILES =		\
	gnome-doc-utils.make

DISTCHECK_CONFIGURE_FLAGS = \
	--disable-scrollkeeper	\
	--disable-schemas-install
