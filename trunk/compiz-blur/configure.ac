dnl Process this file with autoconf to create configure.

dnl ################################################################
dnl # Initialize autoconf
dnl ################################################################
AC_INIT(compiz-blur, 0.0.1, trowbrds@gmail.com)
AC_PREREQ(2.50)
AC_CONFIG_SRCDIR(config.h.in)
AC_COPYRIGHT([Copyright 2006 David Trowbridge])

dnl ################################################################
dnl # Version information
dnl ################################################################
COMPIZ_BLUR_MAJOR_VERSION=0
COMPIZ_BLUR_MINOR_VERSION=0
COMPIZ_BLUR_MICRO_VERSION=1
COMPIZ_BLUR_DEVEL_VERSION=0

COMPIZ_BLUR_VERSION=$COMPIZ_BLUR_MAJOR_VERSION.$COMPIZ_BLUR_MINOR_VERSION.$COMPIZ_BLUR_MICRO_VERSION

if test "x$COMPIZ_BLUR_DEVEL_VERSION" != "x0"; then
	COMPIZ_BLUR_VERSION=$COMPIZ_BLUR_VERSION.$COMPIZ_BLUR_DEVEL_VERSION
fi

dnl ################################################################
dnl # Initialize automake
dnl ################################################################
VERSION=$COMPIZ_BLUR_VERSION
PACKAGE=compiz-blur

AM_INIT_AUTOMAKE($PACKAGE, $VERSION)

dnl ################################################################
dnl # Specify a header configuration file
dnl ################################################################
AM_CONFIG_HEADER(config.h)

dnl ################################################################
dnl # Check for some standard stuff.
dnl ################################################################
AC_PROG_CC
AC_PROG_CPP
AC_PROG_MAKE_SET
AC_PROG_LN_S
AC_PROG_INSTALL

AC_C_CONST
AC_TYPE_SIZE_T
AC_EXEEXT
AC_HEADER_STDC

AC_PROG_LIBTOOL

GLIB_REQUIRED_VER="2.2.2"
COMPIZ_REQUIRED_VER="0.0.4"
PKG_CHECK_MODULES(PACKAGE, \
        glib-2.0 >= $GLIB_REQUIRED_VER \
	compiz >= $COMPIZ_REQUIRED_VER)

AC_SUBST(PACKAGE_CFLAGS)
AC_SUBST(PACKAGE_LIBS)

dnl # Use wall if we have GCC
if test "x$GCC" = "xyes"; then
	CFLAGS="$CFLAGS -Wall"
fi

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

echo prefix............... : $prefix

echo
echo "Now type make to compile"
echo "Then su to root and type: make install"
echo
