dnl Process this file with autoconf to create configure.

dnl ################################################################
dnl # Initialize autoconf
dnl ################################################################
AC_INIT(rtgraph, 0.1, micah@navi.cx)
AC_PREREQ(2.50)
AC_CONFIG_SRCDIR(config.h.in)
AC_COPYRIGHT([Copyright 2004 Micah Dowty])

dnl ################################################################
dnl # Initialize automake
dnl ################################################################
VERSION=0.1
PACKAGE=rtgraph
AM_INIT_AUTOMAKE($PACKAGE, $VERSION)

AM_CONFIG_HEADER(config.h)

dnl ################################################################
dnl # Check for some standard stuff
dnl ################################################################
AC_PROG_CC
AC_PROG_MAKE_SET
AC_PROG_LN_S
AC_PROG_INSTALL

AC_C_CONST
AC_TYPE_SIZE_T
AC_EXEEXT

pkg_modules="glib-2.0 >= 2.0.0, gtk+-2.0 >= 2.0.0"
PKG_CHECK_MODULES(PACKAGE, [$pkg_modules])
AC_SUBST(PACKAGE_CFLAGS)
AC_SUBST(PACKAGE_LIBS)

dnl # Make it faster, yo
CFLAGS="$CFLAGS -O3"

dnl ################################################################
dnl # Output the Makefiles
dnl ################################################################
AC_CONFIG_FILES([
Makefile
src/Makefile
])

AC_OUTPUT

echo
echo $PACKAGE v$VERSION

echo
echo "Now type make to compile"
echo "Then su to root and type: make install"
echo
