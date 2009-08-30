# Process this file with autoconf to produce a configure script.
AC_INIT
AM_INIT_AUTOMAKE(fx2pipe,0.7)
AC_CONFIG_SRCDIR([fx2pipe/fx2pipe.cc])
AM_CONFIG_HEADER(config.h)

# Checks for programs.
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_CXX
AC_PROG_RANLIB

# Checks for header files.
AC_HEADER_STDC

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_CHECK_TYPE(ssize_t,int)

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_PROG_GCC_TRADITIONAL
#AC_FUNC_MALLOC

AC_CONFIG_FILES([Makefile 
	lib/Makefile 
	usb_io/Makefile 
	fx2pipe/Makefile ])
AC_OUTPUT
