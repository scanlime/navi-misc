/*
 * SWIG interface for librcpod.
 * This generates the 'librcpod' python module, providing
 * a very non-pythonesque raw interface to the rcpod API.
 *
 */

%module librcpod

%{
#include <rcpod.h>
#include <stdlib.h>
%}

%include rcpod.h

/* Array helper functions */
%inline %{
  unsigned char *uchar_array(int size) {
     return (unsigned char *) malloc(size);
  }
  unsigned char uchar_array_get(unsigned char *array, int index) {
     return array[index];
  }
  void free_array(unsigned char *a) {
     free(a);
  }
%}


