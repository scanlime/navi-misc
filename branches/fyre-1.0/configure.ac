dnl Process this file with autoconf to create configure.

dnl ################################################################
dnl # Initialize autoconf
dnl ################################################################
AC_INIT(fyre, 1.0.1, trowbrds@gmail.com)
AC_PREREQ(2.50)
AC_CONFIG_SRCDIR(config.h.in)
AC_COPYRIGHT([Copyright 2004-2006 David Trowbridge and Micah Dowty])

dnl ################################################################
dnl # Initialize automake
dnl ################################################################
VERSION=1.0.1
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
AM_CONDITIONAL([HAVE_BINRELOC], [ test "x$br_cv_binreloc" = "xyes" ])

AC_PATH_PROG(XDGMIME, update-mime-database, no)
if test "x$XDGMIME" = "xno"; then
	enable_xdgmime="no"
else
	enable_xdgmime="yes"
fi
AM_CONDITIONAL(ENABLE_XDGMIME, test "x$enable_xdgmime" = "xyes")

AC_PATH_PROG(FDODESKTOP, update-desktop-database, no)
if test "x$FDODESKTOP" = "xno"; then
	enable_fdodesktop="no"
else
	enable_fdodesktop="yes"
fi
AM_CONDITIONAL(ENABLE_FDODESKTOP, test "x$enable_fdodesktop" = "xyes")

pkg_modules="glib-2.0 >= 2.0.0, gtk+-2.0 >= 2.0.0, libglade-2.0 >= 2.0"
PKG_CHECK_MODULES(PACKAGE, [$pkg_modules])
AC_SUBST(PACKAGE_CFLAGS)
AC_SUBST(PACKAGE_LIBS)

# Check for windows, enable compiling windows resources if we find it
AC_MSG_CHECKING([for Win32])
case "$host" in
  *mingw*)
    WIN32_LIBS=fyre-win32-res.o
    ;;
  *)
    WIN32_LIBS=
    ;;
esac
AC_SUBST(WIN32_LIBS)

# Check for getopt. If we're missing it, include our own version
AC_CHECK_FUNC(getopt_long, have_getopt=yes)
AM_CONDITIONAL([HAVE_GETOPT], [test "x$have_getopt" = "xyes"])

# Check for OpenEXR- we support saving OpenEXR files if the library is available
AC_ARG_ENABLE(openexr, [  --disable-openexr       build without OpenEXR support])
if test x$enable_openexr != xno; then
	PKG_CHECK_MODULES(EXR, OpenEXR, have_exr=yes, have_exr=no)
	AC_SUBST(EXR_CFLAGS)
	AC_SUBST(EXR_LIBS)
	if test "x$have_exr" = "xyes"; then
		AC_DEFINE(HAVE_EXR, 1,[compile in OpenEXR support])
	fi
	AM_CONDITIONAL([HAVE_EXR], [test "x$have_exr" = "xyes"])
else
	AM_CONDITIONAL([HAVE_EXR], false)
fi

# Check for gnet, required for cluster and remote control support
AC_ARG_ENABLE(gnet, [  --disable-gnet          build without clustering/remote control])
if test x$enable_gnet != xno; then
	PKG_CHECK_MODULES(GNET, gnet-2.0, have_gnet=yes, have_gnet=no)
	AC_SUBST(GNET_CFLAGS)
	AC_SUBST(GNET_LIBS)
	if test "x$have_gnet" = "xyes"; then
		AC_DEFINE(HAVE_GNET, 1,[compile in gnet support])
	fi
	AM_CONDITIONAL([HAVE_GNET], [test "x$have_gnet" = "xyes"])
else
	AM_CONDITIONAL([HAVE_GNET], false)
fi

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
autopackage/default.apspec
data/Makefile
data/icons/Makefile
data/icons/16x16/Makefile
data/icons/22x22/Makefile
data/icons/24x24/Makefile
data/icons/scalable/Makefile
src/Makefile
contrib/Makefile
])

AC_OUTPUT

echo
echo $PACKAGE v$VERSION
echo

if test "x$have_exr" != "xyes"; then
	echo "Not including support for OpenEXR files"
else
	echo "Including support for OpenEXR files"
fi
if test "x$have_gnet" != "xyes"; then
	echo "Not including clustering or remote control support"
else
	echo "Including clustering or remote control support"
fi

echo
echo "Now type make to compile"
echo "Then su to root and type: make install"
echo
