dnl ################################################################
dnl # Initialize autoconf
dnl ################################################################
AC_INIT(sheepsweeper, 0.0.99, chipx86@chipx86.com)
AC_PREREQ(2.50)
AC_CONFIG_SRCDIR(config.h.in)
AC_REVISION($Revision: 1.2 $)
AC_COPYRIGHT([Copyright 2003 Christian Hammond])

VERSION=0.0.99
PACKAGE=sheepsweeper

AM_INIT_AUTOMAKE($PACKAGE, $VERSION)

AM_CONFIG_HEADER(config.h)

AC_CANONICAL_HOST
AC_CANONICAL_TARGET
case "$target" in
	alpha*-*-linux*)
		CXXFLAGS="$CXXFLAGS -mcpu=ev4 -Wa,-mall"
		;;
esac

AC_PROG_MAKE_SET
AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LN_S

AM_PATH_SDL(1.2.5,, AC_MSG_ERROR([Required SDL version not found!]))
AC_CHECK_LIB(SDL_image, IMG_Load, , AC_MSG_ERROR([SDL_image not found!]))

AC_SUBST(SDL_CFLAGS)
AC_SUBST(SDL_LIBS)


AC_CONFIG_FILES([
Makefile
images/Makefile
images/tiles/Makefile
src/Makefile
])

AC_OUTPUT
