dnl Process this file with autoconf to create configure.

dnl ################################################################
dnl # Initialize autoconf
dnl ################################################################
AC_INIT(fyre, 0.7-pre, trowbrds@cs.colorado.edu)
AC_PREREQ(2.50)
AC_CONFIG_SRCDIR(config.h.in)
AC_COPYRIGHT([Copyright 2004 David Trowbridge and Micah Dowty])

dnl ################################################################
dnl # Initialize automake
dnl ################################################################
VERSION=0.7-pre
PACKAGE=fyre
AM_INIT_AUTOMAKE($PACKAGE, $VERSION)

AM_CONFIG_HEADER(config.h)

dnl ################################################################
dnl # Check for some standard stuff
dnl ################################################################
AC_PROG_CC
AC_PROG_CXX
AC_PROG_MAKE_SET
AC_PROG_LN_S
AC_PROG_INSTALL

AC_C_CONST
AC_TYPE_SIZE_T
AC_EXEEXT
AC_FUNC_FORK

AM_BINRELOC

pkg_modules="glib-2.0 >= 2.0.0, gtk+-2.0 >= 2.0.0, libglade-2.0 >= 2.0"
PKG_CHECK_MODULES(PACKAGE, [$pkg_modules])
AC_SUBST(PACKAGE_CFLAGS)
AC_SUBST(PACKAGE_LIBS)

# Check for OpenEXR- we support saving OpenEXR files if the library is available
PKG_CHECK_MODULES(EXR, OpenEXR, have_exr=yes, have_exr=no)
AC_SUBST(EXR_CFLAGS)
AC_SUBST(EXR_LIBS)
if test "x$have_exr" = "xyes"; then
	AC_DEFINE(HAVE_EXR, 1,[compile in OpenEXR support])
fi
AM_CONDITIONAL([HAVE_EXR], [test "x$have_exr" = "xyes"])

# Check for gnet, required for cluster and remote control support
PKG_CHECK_MODULES(GNET, gnet-2.0, have_gnet=yes, have_gnet=no)
AC_SUBST(GNET_CFLAGS)
AC_SUBST(GNET_LIBS)
if test "x$have_gnet" = "xyes"; then
	AC_DEFINE(HAVE_GNET, 1,[compile in gnet support])
fi
AM_CONDITIONAL([HAVE_GNET], [test "x$have_gnet" = "xyes"])

dnl # Use wall if we have GCC
dnl # Also disable glibc versions of functions that have faster versions
dnl # versions as gcc inlines. This should speed up trig on some systems.
if test "x$GCC" = "xyes"; then
	CFLAGS="$CFLAGS -D__NO_INLINE__ -Wall"
fi

dnl # -O3 and -ffast-math should make it go faster on any system
dnl # But Intel icc doesn't have -ffast-math, just -ffast, which is creepy
if test "x$CC" = "xicc"; then
	CFLAGS="$CFLAGS -O3"
else
	CFLAGS="$CFLAGS -O3 -ffast-math"
fi

dnl ### FIXME
dnl # -march=i686 speeds this up quite a bit on my machine (even more so
dnl # than -march=athlon-xp) so if this looks like a recent x86 machine,
dnl # stick that in CFLAGS
dnl # CFLAGS  += $(shell if grep mmx /proc/cpuinfo > /dev/null; then echo -march=i686; fi)


dnl ################################################################
dnl # Output the Makefiles
dnl ################################################################
AC_CONFIG_FILES([
Makefile
data/Makefile
src/Makefile
])

AC_OUTPUT

echo
echo $PACKAGE v$VERSION
echo

if test "x$have_exr" != "xyes"; then
   echo "OpenEXR missing, not including support for OpenEXR files"
fi
if test "x$have_gnet" != "xyes"; then
   echo "gnet missing, not including clustering or remote control support"
fi

echo
echo "Now type make to compile"
echo "Then su to root and type: make install"
echo
