dnl Process this file with autoconf to create configure.

dnl ################################################################
dnl # Initialize autoconf
dnl ################################################################
AC_INIT(fyre, 1.1.0, trowbrds@cs.colorado.edu)
AC_PREREQ(2.50)
AC_CONFIG_SRCDIR(config.h.in)
AC_COPYRIGHT([Copyright 2004-2005 David Trowbridge and Micah Dowty])

dnl ################################################################
dnl # Initialize automake
dnl ################################################################
VERSION=1.1.0
PACKAGE=fyre
AM_INIT_AUTOMAKE($PACKAGE, $VERSION)

AM_CONFIG_HEADER(config.h)

dnl ################################################################
dnl # Check for some standard stuff
dnl ################################################################
AC_PROG_INSTALL
AC_PROG_CC
AC_ISC_POSIX
AC_HEADER_STDC

AM_DISABLE_STATIC
AM_PROG_LIBTOOL

AC_PATH_PROG(PKG_CONFIG, pkg-config, no)
if test "x$PKG_CONFIG" = "xno"; then
        AC_MSG_ERROR([You need to install pkg-config])
fi

AC_PATH_PROG(MCS, mcs)
if test "x$MCS" = "x" ; then
        AC_MSG_ERROR([Can't find "mcs" in your PATH])
fi

PKG_CHECK_MODULES(FYRE, gtk-sharp-2.0 glade-sharp-2.0 gnome-sharp-2.0)
AC_SUBST(FYRE_LIBS)

dnl ################################################################
dnl # Output the Makefiles
dnl ################################################################
AC_OUTPUT([
Makefile
data/Makefile
src/Makefile
src/Plugins/Makefile
src/Plugins/Basic/Makefile
src/Plugins/Basic/data/Makefile
src/Plugins/HistogramImager/Makefile
src/Plugins/HistogramImager/data/Makefile
])
