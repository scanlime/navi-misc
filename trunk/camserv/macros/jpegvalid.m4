dnl
dnl Check for struct linger
dnl
AC_DEFUN(CAMSERV_JPEG_VALID, [
av_jpeg_valid=no
AC_MSG_CHECKING(JPEG library is valid)
AC_TRY_RUN([
#include <stdio.h>
#include <stdlib.h>

#include <jpeglib.h>

main ()
{
    struct jpeg_common_struct foo;
 
    foo.client_data = NULL;
    exit (0);
}
],[
AC_DEFINE(HAVE_JPEG_VALID)
av_jpeg_valid=yes
],[
av_jpeg_valid=no
],[
av_jpeg_valid=no
])
AC_MSG_RESULT($av_jpeg_valid)
])
