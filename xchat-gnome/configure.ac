AC_INIT([XChat-GNOME IRC Chat],[0.8svn],[http://bugzilla.gnome.org/enter_bug.cgi?product=xchat-gnome],[xchat-gnome])
GNOME_COMMON_INIT

AC_PREREQ([2.59])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([configure.ac])

PACKAGE=xchat-gnome
VERSION=0.8svn
AM_INIT_AUTOMAKE($PACKAGE, $VERSION)

AM_MAINTAINER_MODE
if test "x$enable_maintainer_mode" = xyes; then
	AC_DEFINE([MAINTAINER_MODE],[1],[Define to enable 'maintainer-only' behaviour])
	enable_debug=yes
	MORE_WARN_FLAGS=
	DEPRECATION_FLAGS="-DG_DISABLE_DEPRECATED -DGDK_DISABLE_DEPRECATED -DGDK_PIXBUF_DISABLE_DEPRECATED -DGCONF_DISABLE_DEPRECATED -DGNOME_VFS_DISABLE_DEPRECATED -DBONOBO_UI_DISABLE_DEPRECATED -DBONOBO_DISABLE_DEPRECATED -DLIBGLADE_DISABLE_DEPRECATED -DPANGO_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED -DGNOME_DISABLE_DEPRECATED"
fi

AC_PROG_INTLTOOL([0.29])

GLIB_REQUIRED=2.8.0
GTK_REQUIRED=2.8.0
LIBGLADE_REQUIRED=2.3.0
LIBGNOMEVFS_REQUIRED=2.9.2
LIBGNOMEUI_REQUIRED=2.6.0
GCONF_REQUIRED=2.8.0
DBUS_GLIB_REQUIRED=0.35
LIBSEXY_REQUIRED=0.1.4


AC_PROG_CC
AC_PROG_CPP
AM_PROG_AS
AC_PROG_LIBTOOL
AC_PATH_PROG([GLIB_GENMARSHAL], [glib-genmarshal])

AC_C_CONST
AC_TYPE_SIZE_T
AC_EXEEXT
AC_HEADER_STDC

GNOME_DEBUG_CHECK
GNOME_COMPILE_WARNINGS([maximum])

AC_PATH_PROG([PKG_CONFIG], [pkg-config], [no])

# Hard dependencies
PKG_CHECK_MODULES([DEPENDENCIES], [\
                  glib-2.0 >= $GLIB_REQUIRED \
                  gtk+-2.0 >= $GTK_REQUIRED \
                  libglade-2.0 >= $LIBGLADE_REQUIRED \
                  gnome-vfs-2.0 >= $LIBGNOMEVFS_REQUIRED \
                  libgnomeui-2.0 >= $LIBGNOMEUI_REQUIRED \
                  gconf-2.0 >= $GCONF_REQUIRED \
                  ])
AC_SUBST([DEPENDENCIES_CFLAGS])
AC_SUBST([DEPENDENCIES_LIBS])

# D-BUS soft dependency
PKG_CHECK_MODULES([DBUS], [dbus-glib-1 >= $DBUS_GLIB_REQUIRED], dbus=yes, dbus=no)
AC_SUBST([DBUS_CFLAGS])
AC_SUBST([DBUS_LIBS])
AC_PATH_PROG([DBUS_BINDING_TOOL],[dbus-binding-tool],[no])
if test "x$DBUS_BINDING_TOOL" = "xno"; then
	AC_MSG_ERROR([dbus-binding-tool not found])
	dbus="no"
fi
AM_CONDITIONAL([ENABLE_DBUS],[test "x$dbus" = "xyes"])

# libsexy soft dependency
PKG_CHECK_MODULES([LIBSEXY], [libsexy >= $LIBSEXY_REQUIRED], libsexy=yes, libsexy=no)
AC_SUBST([LIBSEXY_CFLAGS])
AC_SUBST([LIBSEXY_LIBS])
AM_CONDITIONAL([ENABLE_LIBSEXY],[test "x$libsexy" = "xyes"])

# libnotify soft dependency
PKG_CHECK_MODULES([LIBNOTIFY], [libnotify], libnotify=yes, libnotify=no)
AC_SUBST([LIBNOTIFY_CFLAGS])
AC_SUBST([LIBNOTIFY_LIBS])
AM_CONDITIONAL([ENABLE_LIBNOTIFY],[test "x$libnotify" = "xyes"])

# GConf
AM_GCONF_SOURCE_2
AC_PATH_PROG(GCONFTOOL, gconftool-2, no)

# i18n
ALL_LINGUAS="`cat "$srcdir/po/LINGUAS" | grep -v '^#'`"

AC_SUBST([CONFIG_STATUS_DEPENDENCIES],['$(top_srcdir)/po/LINGUAS'])

GETTEXT_PACKAGE=xchat-gnome
AC_SUBST([GETTEXT_PACKAGE])
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE],["$GETTEXT_PACKAGE"],[Gettext package])
AM_GLIB_GNU_GETTEXT
AM_GLIB_DEFINE_LOCALEDIR([GNOMELOCALEDIR])

# MMX (for xtext tinting)
if test "$mmx" = "yes"; then
	case $host_cpu in
		i386|i486|i586|i686|i786|k6|k7)
			mmx=yes
			;;
		*)
		mmx=no
	esac
	if test "$system" = "OpenBSD"; then
		dnl openbsd fails because mmx_cmod doesn't prefix its symbols with underscore.
		dnl xtext.o: Undefined symbol `_shade_ximage_15_mmx' referenced from text segment
		mmx=no
	fi
	if test "$mmx" = "yes"; then
		AC_DEFINE([USE_MMX], [], [Use MMX accelerated tinting])
	fi
fi

AM_CONDITIONAL(USE_MMX, test "$mmx" = "yes")

# plugins
AC_MSG_CHECKING([which plugins to build])
ALL_PLUGINS="autoaway notification url_scraper"
DBUS_PLUGINS="dbus net-monitor"
LIBNOTIFY_PLUGINS="notify-osd"
PYTHON_PLUGINS="python"
PERL_PLUGINS="perl"
TCL_PLUGINS="tcl"

DIST_PLUGINS="$ALL_PLUGINS $DBUS_ALL_PLUGINS $LIBNOTIFY_ALL_PLUGINS $PYTHON_PLUGINS $PERL_PLUGINS $TCL_PLUGINS"

# dbus-specific plugins
if test "x$dbus" = "xyes"; then
	ALL_PLUGINS="$ALL_PLUGINS $DBUS_PLUGINS"
else
	AC_MSG_WARN([xchat-gnome compiled without dbus support, plugins $DBUS_PLUGINS will not be available $ENABLE_DBUS])
fi

AC_ARG_WITH([plugins],
            [  --with-plugins=plugin1,plugin2,...
                          build the specified plugins.  Available:
                          autoaway, dbus, net-monitor, notification,
                          notify-osd, perl, python, tcl, url_scraper],
            [plugins=$with_plugins],
            [plugins="default"])
if test "x$with_plugins" = xyes; then
	plugins="default"
fi

PLUGINS=
IFS="${IFS=     }"; plugin_save_ifs="$IFS"; IFS=","
for plugin in $plugins; do
	if echo "$ALL_PLUGINS" | egrep "(^| )$plugin(\$| )" > /dev/null; then
		PLUGINS="$PLUGINS $plugin"
	elif test "x$plugin" = "xdefault"; then
		PLUGINS="$PLUGINS $ALL_PLUGINS"
	fi
done
IFS="$plugin_save_ifs"

AC_SUBST([DIST_PLUGINS])
AC_SUBST([BUILD_PLUGINS],[`echo $PLUGINS | tr ' ' '\n' | sort | uniq | tr '\n' ' '`])
AC_MSG_RESULT([$PLUGINS])

# standard paths
XCHATLIBDIR=${libdir}/xchat
AC_SUBST([XCHATLIBDIR])

AC_CONFIG_FILES([
Makefile
data/Makefile
help/Makefile
help/C/Makefile
help/C/figures/Makefile
plugins/Makefile
plugins/autoaway/Makefile
plugins/dbus/Makefile
plugins/net-monitor/Makefile
plugins/notification/Makefile
plugins/url_scraper/Makefile
po/Makefile.in
src/Makefile
src/common/Makefile
src/fe-gnome/Makefile
])

AC_OUTPUT

echo
echo $PACKAGE $VERSION
echo

echo prefix............... : $prefix

echo
echo "Now type make to compile"
echo "Then su to root and type: make install"
echo
