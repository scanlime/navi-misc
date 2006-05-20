AC_INIT([XChat-GNOME IRC Chat],[0.12svn],[http://bugzilla.gnome.org/enter_bug.cgi?product=xchat-gnome],[xchat-gnome])

AC_PREREQ([2.59])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([configure.ac])

AM_INIT_AUTOMAKE([1.9 dist-bzip2 check-news subdir-objects no-define])

AM_MAINTAINER_MODE
if test "x$enable_maintainer_mode" = xyes; then
	AC_DEFINE([MAINTAINER_MODE],[1],[Define to enable 'maintainer-only' behaviour])
	enable_debug=yes
	MORE_WARN_FLAGS=
	DEPRECATION_FLAGS="-DG_DISABLE_DEPRECATED -DGDK_DISABLE_DEPRECATED -DGDK_PIXBUF_DISABLE_DEPRECATED -DGCONF_DISABLE_DEPRECATED -DGNOME_VFS_DISABLE_DEPRECATED -DBONOBO_UI_DISABLE_DEPRECATED -DBONOBO_DISABLE_DEPRECATED -DLIBGLADE_DISABLE_DEPRECATED -DPANGO_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED -DGNOME_DISABLE_DEPRECATED"
fi

IT_PROG_INTLTOOL([0.35.0])

GLIB_REQUIRED=2.8.0
GTK_REQUIRED=2.8.0
LIBGLADE_REQUIRED=2.3.0
LIBGNOMEVFS_REQUIRED=2.9.2
LIBGNOMEUI_REQUIRED=2.6.0
GCONF_REQUIRED=2.8.0
DBUS_REQUIRED=0.35
DBUS_GLIB_REQUIRED=0.35
LIBSEXY_REQUIRED=0.1.7
LIBNOTIFY_REQUIRED=0.3.2


AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_CPP
AM_PROG_AS
AC_PROG_LIBTOOL
AC_PATH_PROG([GLIB_GENMARSHAL], [glib-genmarshal])

AC_C_CONST
AC_TYPE_SIZE_T
AC_EXEEXT
AC_HEADER_STDC

AC_CHECK_FUNCS(snprintf vsnprintf)
AC_CHECK_FUNCS(memrchr strtoull atoll)

GNOME_COMMON_INIT
GNOME_MAINTAINER_MODE_DEFINES
GNOME_DEBUG_CHECK
GNOME_COMPILE_WARNINGS([maximum])

AC_PATH_PROG([PKG_CONFIG], [pkg-config], [no])

# Set up gnome-doc-utils
GNOME_DOC_INIT

# Hard dependencies
PKG_CHECK_MODULES([DEPENDENCIES], [\
                  glib-2.0 >= $GLIB_REQUIRED \
                  gtk+-2.0 >= $GTK_REQUIRED \
                  libglade-2.0 >= $LIBGLADE_REQUIRED \
                  gnome-vfs-2.0 >= $LIBGNOMEVFS_REQUIRED \
                  libgnomeui-2.0 >= $LIBGNOMEUI_REQUIRED \
                  gconf-2.0 >= $GCONF_REQUIRED \
                  ])

# We can do better option parsing if we have libgnome >= 2.14
# Once 2.14 is out and relatively common, we should probably eliminate this
# and just force everyone to it.
PKG_CHECK_MODULES([LIBGNOME], [libgnome-2.0 >= 2.13], newlibgnome=yes, newlibgnome=no)
if test "x$newlibgnome" = xyes; then
	AC_DEFINE(HAVE_LIBGNOME_214, 1, [Enable 2.14-specific code])
fi

#D-Bus
AC_MSG_CHECKING([whether dbus support is requested])
AC_ARG_ENABLE([dbus],
	AS_HELP_STRING([--disable-dbus],[Disable dbus support]),
	[enable_dbus=$enableval dbus=$enableval],
	[enable_dbus=autodetect dbus=yes])
AC_MSG_RESULT([$enable_dbus])

if test "x$enable_dbus" != "xno"; then
# D-BUS soft dependency
	PKG_CHECK_MODULES([DBUS], [\
	                  dbus-1 >= $DBUS_REQUIRED \
	                  dbus-glib-1 >= $DBUS_GLIB_REQUIRED \
			  ], dbus=yes, dbus=no)
	if test "x$dbus" = "xno"; then
		AC_MSG_WARN([Library requirements (dbus-1 >= $DBUS_REQUIRED dbus-glib-1 >= $DBUS_GLIB_REQUIRED) not met; consider adjusting the PKG_CONFIG_PATH environment variable if your libraries are in a nonstandard prefix so pkg-config can find them.])
	fi
	AC_SUBST([DBUS_CFLAGS])
	AC_SUBST([DBUS_LIBS])
	AC_PATH_PROG([DBUS_BINDING_TOOL],[dbus-binding-tool],[no])
	if test "x$DBUS_BINDING_TOOL" = "xno"; then
		dbus="no"
	fi
fi

if test "x$dbus" != "xyes"; then
	if test "x$enable_dbus" = "xyes"; then
		AC_MSG_ERROR([DBus not found])
	elif test "x$enable_dbus" = "xautodetect"; then
		AC_MSG_WARN([DBus not found, disabling dbus support])
	fi
fi

AM_CONDITIONAL([ENABLE_DBUS],[test "x$dbus" = "xyes"])

# libsexy soft dependency
PKG_CHECK_MODULES([LIBSEXY], [libsexy >= $LIBSEXY_REQUIRED], libsexy=yes, libsexy=no)
AC_SUBST([LIBSEXY_CFLAGS])
AC_SUBST([LIBSEXY_LIBS])
AM_CONDITIONAL([ENABLE_LIBSEXY],[test "x$libsexy" = "xyes"])
if test "x$libsexy" = "xyes"; then
	AC_DEFINE(HAVE_LIBSEXY, 1, [Enable libsexy])
else
	AC_MSG_WARN([Library requirements (libsexy >= $LIBSEXY_REQUIRED) not met; consider adjusting the PKG_CONFIG_PATH environment variable if your libraries are in a nonstandard prefix so pkg-config can find them.])
fi

# libnotify soft dependency
PKG_CHECK_MODULES([LIBNOTIFY], [libnotify >= $LIBNOTIFY_REQUIRED], libnotify=yes, libnotify=no)
AC_SUBST([LIBNOTIFY_CFLAGS])
AC_SUBST([LIBNOTIFY_LIBS])
AM_CONDITIONAL([ENABLE_LIBNOTIFY],[test "x$libnotify" = "xyes"])
if test "x$libnotify" != "xyes"; then
	AC_MSG_WARN([Library requirements (libnotify >= $LIBNOTIFY_REQUIRED) not met; consider adjusting the PKG_CONFIG_PATH environment variable if your libraries are in a nonstandard prefix so pkg-config can find them.])
fi

# GConf
AM_GCONF_SOURCE_2
AC_PATH_PROG(GCONFTOOL, gconftool-2, no)

# SOCKS5
AC_CHECK_LIB(socks5, SOCKSconnect, have_socks=yes, have_socks=no)
AC_CHECK_HEADERS(socks.h, , have_socks=no)
if test "x$have_socks" = xyes; then
	AC_DEFINE(SOCKS, [], [Whether to include SOCKS5 proxy support])
	DEPENDENCIES_LIBS="$DEPENDENCIES_LIBS -lsocks5"
fi

# Perl
AC_MSG_CHECKING([whether Perl support is requested])
AC_ARG_ENABLE([perl],
	AS_HELP_STRING([--disable-perl],[Disable perl support]),
	[enable_perl=$enableval have_perl=$enableval],
	[enable_perl=autodetect have_perl=yes])
AC_MSG_RESULT([$enable_perl])

if test "x$have_perl" != "xno"; then
	AC_PATH_PROG(PERL, perl perl5)
	if test -z $PERL ; then
		AC_MSG_WARN([Perl interpreter not found, disabling perl support])
		have_perl=no
	fi
fi

if test "x$have_perl" != "xno"; then
	AC_MSG_CHECKING(for Perl header files)
	PERL_HEADER_TEST=`PT=perltest.c ; echo "#include <EXTERN.h>" > $PT;  echo "#include <perl.h>" >> $PT;  echo "#include <XSUB.h>" >> $PT ;  echo "int main() { return 0; }" >> $PT ; $CC -Wall $PT -o $PT.out $($PERL -MExtUtils::Embed -e ccopts) 1>/dev/null 2>&1; echo $?; rm -f $PT $PT.out 1>/dev/null 2>&1`

	if test "x$PERL_HEADER_TEST" = "x0" ; then
		PERL_CFLAGS=`$PERL -MExtUtils::Embed -e ccopts`
		AC_MSG_RESULT(found)
	else
		AC_MSG_WARN([Perl headers not found, disabling perl support])
		have_perl=no
	fi
fi

if test "x$have_perl" != "xno"; then
	AC_MSG_CHECKING(for Perl library)
	PERL_LIB_TEST=`PT=perltest.c ; echo "int main() { return 0; }" > $PT ; $CC -Wall $PT -o $PT.out $($PERL -MExtUtils::Embed -e ldopts) 1>/dev/null 2>&1; echo $?; rm -f $PT $PT.out 1>/dev/null 2>&1`

	if test "x$PERL_LIB_TEST" = "x0" ; then
		PERL_LDFLAGS=`$PERL -MExtUtils::Embed -e ldopts`
		AC_MSG_RESULT(found)
		AC_SUBST(PERL_CFLAGS)
		AC_SUBST(PERL_LDFLAGS)
		AC_DEFINE([ENABLE_PERL],[1],[Define to compile with perl support])
		enable_perl=yes
	else
		AC_MSG_WARN([Perl library not found, disabling perl support])
	fi

fi

# Python
AC_MSG_CHECKING([whether Python support is requested])
AC_ARG_ENABLE([python],
	AS_HELP_STRING([--disable-python],[Disable python support]),
	[enable_python=$enableval have_python=$enableval],
	[enable_python=autodetect have_python=yes])
AC_MSG_RESULT([$enable_python])

if test "x$have_python" != "xno"; then
	AM_PATH_PYTHON([2.3],[],[no])

	if test "x$PYTHON" = "x:"; then
		have_python=no
	fi
fi

if test "x$have_python" != "xno"; then
	AM_CHECK_PYTHON_HEADERS([],[have_python=no])
fi

if test "x$have_python" != "xno"; then
	PY_PREFIX=`$PYTHON -c 'import sys ; print sys.prefix'`
	PY_EXEC_PREFIX=`$PYTHON -c 'import sys ; print sys.exec_prefix'`
	PYTHON_LIBS="-lpython$PYTHON_VERSION"
	PYTHON_LIB_LOC="-L$PY_EXEC_PREFIX/lib/python$PYTHON_VERSION/config"
	PYTHON_CFLAGS="-I$PY_PREFIX/include/python$PYTHON_VERSION"
	PYTHON_MAKEFILE="$PY_EXEC_PREFIX/lib/python$PYTHON_VERSION/config/Makefile"
	PYTHON_LOCALMODLIBS=`sed -n -e 's/^LOCALMODLIBS=\(.*\)/\1/p' $PYTHON_MAKEFILE`
	PYTHON_BASEMODLIBS=`sed -n -e 's/^BASEMODLIBS=\(.*\)/\1/p' $PYTHON_MAKEFILE`
	PYTHON_OTHER_LIBS=`sed -n -e 's/^LIBS=\(.*\)/\1/p' $PYTHON_MAKEFILE`
	PYTHON_EXTRA_LIBS="$PYTHON_LOCALMODLIBS $PYTHON_BASEMODLIBS $PYTHON_OTHER_LIBS"
	AC_SUBST([PYTHON_LIBS])
	AC_SUBST([PYTHON_LIB_LOC])
	AC_SUBST([PYTHON_CFLAGS])
	AC_SUBST([PYTHON_EXTRA_LIBS])
fi

if test "x$have_python" != "xyes"; then
	if test "x$enable_python" = "xyes"; then
		AC_MSG_ERROR([Python not found])
	elif test "x$enable_python" = "xautodetect"; then
		enable_python=no
		AC_MSG_WARN([Python not found, disabling python support])
	fi
elif test "x$enable_python" != "xno"; then
	enable_python=yes
	AC_DEFINE([ENABLE_PYTHON],[1],[Define to compile with python support])
fi

# TCL
AC_MSG_CHECKING([whether TCL support is requested])
AC_ARG_ENABLE([tcl],
	AS_HELP_STRING([--disable-tcl],[Disable tcl support]),
	[enable_tcl=$enableval have_tcl=$enableval],
	[enable_tcl=autodetect have_tcl=yes])
AC_MSG_RESULT([$enable_tcl])

if test "x$have_tcl" != "xno"; then
	have_tcl=no
	for dir in $tcl /lib /usr/lib /usr/tcl/lib /usr/local/lib /usr/local/tcl-8.4/lib /usr/local/tcl/lib /opt/lib; do
		if test -f $dir/tclConfig.sh; then
			. $dir/tclConfig.sh
			TCL_LIBS="$TCL_LIB_SPEC $TCL_LIBS"
			TCL_CFLAGS="-I${TCL_PREFIX}/include $TCL_INCLUDE_SPEC"
			AC_SUBST([TCL_LIBS])
			AC_SUBST([TCL_CFLAGS])
			have_tcl=yes
			enable_tcl=yes
			AC_MSG_RESULT($dir/tclConfig.sh)
			break
		fi
	done
	if test "x$have_tcl" == "xno"; then
		AC_MSG_RESULT([tclConfig.sh not found.  Try --enable-tcl=/path/to/])
	fi
fi

# openssl
PKG_CHECK_MODULES([OPENSSL], [openssl], openssl=yes, openssl=no)
if test "x$openssl" = xyes; then
	AC_SUBST([OPENSSL_CFLAGS])
	AC_SUBST([OPENSSL_LIBS])
	AC_DEFINE(USE_OPENSSL, [], [Whether to use OpenSSL])
else
	AC_MSG_WARN([Library requirements (openssl) not met; consider adjusting the PKG_CONFIG_PATH environment variable if your libraries are in a nonstandard prefix so pkg-config can find them.])
fi
AM_CONDITIONAL(USE_OPENSSL, test "x$openssl" = "xyes")

# i18n
GETTEXT_PACKAGE=xchat-gnome
AC_SUBST([GETTEXT_PACKAGE])
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE],["$GETTEXT_PACKAGE"],[Gettext package])
AM_GLIB_GNU_GETTEXT
AM_GLIB_DEFINE_LOCALEDIR([GNOMELOCALEDIR])

# ipv6
AC_CHECK_LIB(socket, select)

AC_CHECK_FUNCS(getaddrinfo, have_getaddrinfo=yes)
AC_MSG_CHECKING(whether to enable IPv6 support)
if test "x$have_getaddrinfo" = xyes; then
	AC_MSG_RESULT(yes)
	AC_DEFINE(USE_IPV6, [], [Whether to use ipv6])
else
	AC_MSG_RESULT(no)
fi

# Xlib & Xshm (for xtext tinting)
AC_CHECK_LIB(X11, XOpenDisplay, xlib=yes, xlib=no)
if test "x$xlib" = xyes; then
	AC_DEFINE(USE_XLIB, [1], [Use Xlib])
fi

AC_CHECK_LIB(Xext, XShmAttach, shm=yes, shm=no)
AC_CHECK_HEADERS(sys/ipc.h, , shm=no)
AC_CHECK_HEADERS(sys/shm.h, , shm=no)
if test "x$shm" = xyes; then
	DEPENDENCIES_LIBS="$DEPENDENCIES_LIBS -lX11 -lXext"
	AC_DEFINE(USE_SHM, [1], [Whether to use Xshm tinting])
fi

# MMX (for xtext tinting)
AC_ARG_ENABLE(mmx,
[  --disable-mmx           disable MMX assembly routines],
        mmx=$enableval, mmx=yes)
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
		AC_DEFINE(USE_MMX, 1, [Use MMX assembly routines])
	fi
fi

AM_CONDITIONAL(USE_MMX, test "x$mmx" = "xyes")

# plugins
AC_MSG_CHECKING([which plugins to build])
ALL_PLUGINS="autoaway notification url_scraper sound-notification"
DBUS_PLUGINS="dbus net-monitor"
LIBNOTIFY_PLUGINS="notify-osd"
PYTHON_PLUGINS="python"
PERL_PLUGINS="perl"
TCL_PLUGINS="tcl"

DIST_PLUGINS="$ALL_PLUGINS $DBUS_PLUGINS $LIBNOTIFY_PLUGINS $PYTHON_PLUGINS $PERL_PLUGINS $TCL_PLUGINS"

# dbus-specific plugins
if test "x$dbus" = "xyes"; then
	ALL_PLUGINS="$ALL_PLUGINS $DBUS_PLUGINS"
else
	AC_MSG_WARN([xchat-gnome compiled without dbus support, plugins $DBUS_PLUGINS will not be available])
fi

# libnotify-specific plugins
if test "x$libnotify" = "xyes"; then
	ALL_PLUGINS="$ALL_PLUGINS $LIBNOTIFY_PLUGINS"
else
	AC_MSG_WARN([xchat-gnome compiled without libnotify support, plugins $LIBNOTIFY_PLUGINS will not be available])
fi

# perl-specific plugins
if test "x$enable_perl" = "xyes"; then
	ALL_PLUGINS="$ALL_PLUGINS $PERL_PLUGINS"
else
	AC_MSG_WARN([xchat-gnome compiled without perl support, plugins $PERL_PLUGINS will not be available])
fi

# python-specific plugins
if test "x$enable_python" = "xyes"; then
	ALL_PLUGINS="$ALL_PLUGINS $PYTHON_PLUGINS"
else
	AC_MSG_WARN([xchat-gnome compiled without python support, plugins $PYTHON_PLUGINS will not be available])
fi

# tcl-specific plugins
if test "x$enable_tcl" = "xyes"; then
	ALL_PLUGINS="$ALL_PLUGINS $TCL_PLUGINS"
else
	AC_MSG_WARN([xchat-gnome compiled without tcl support, plugins $TCL_PLUGINS will not be available])
fi

AC_ARG_WITH([plugins],
            [  --with-plugins=plugin1,plugin2,...
                          build the specified plugins.  Available:
                          autoaway, dbus, net-monitor, notification,
                          notify-osd, perl, python, tcl, url_scraper,
                          sound-notification],
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
AC_DEFINE(USE_PLUGIN, [], [Use plugins])
AC_DEFINE(USE_GMODULE, [], [Use gmodule for loading plugins])

# standard paths
XCHATLIBDIR=${libdir}/$PACKAGE
AC_SUBST([XCHATLIBDIR])
XCHATSHAREDIR=${datadir}/$PACKAGE
AC_SUBST([XCHATSHAREDIR])

# warning flags
AM_CPPFLAGS="$AM_CPPFLAGS $DEPRECATION_FLAGS"
AM_CFLAGS="$AM_CFLAGS $WARN_CFLAGS $MORE_WARN_FLAGS"
AC_SUBST([AM_CPPFLAGS])
AC_SUBST([AM_CFLAGS])

# various and sundry defines and flags
AC_DEFINE(_GNU_SOURCE, [], [Use GNU source])
AC_SUBST([DEPENDENCIES_CFLAGS])
AC_SUBST([DEPENDENCIES_LIBS])

AC_CONFIG_FILES([
Makefile
data/Makefile
help/Makefile
plugins/Makefile
plugins/autoaway/Makefile
plugins/dbus/Makefile
plugins/net-monitor/Makefile
plugins/notification/Makefile
plugins/notify-osd/Makefile
plugins/perl/Makefile
plugins/python/Makefile
plugins/sound-notification/Makefile
plugins/tcl/Makefile
plugins/url_scraper/Makefile
po/Makefile.in
src/Makefile
src/common/Makefile
src/fe-gnome/Makefile
data/xchat-gnome.desktop.in
])

AC_OUTPUT

echo
echo $PACKAGE $VERSION
echo

echo prefix............... : $prefix
