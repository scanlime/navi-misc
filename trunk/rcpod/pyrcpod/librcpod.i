/*
 * SWIG interface for librcpod.
 * This generates the 'librcpod' python module, providing
 * a very non-pythonesque raw interface to the rcpod API.
 *
 */

%module librcpod

%{
#include <rcpod.h>
#include <usb.h>
#include <stdlib.h>
#include <string.h>
%}

%include carrays.i
%include exception.i

/*************************************** Error handler */

%inline %{
  /* Set to a malloc'ed string if an error has occurred, NULL normally */
  char *pyrcpod_errorBuffer = NULL;

  /* An error callback that sets the above variable */
  void pyrcpod_ErrorHandler(const char *function, int err, const char *message) {
    if (pyrcpod_errorBuffer) {
      free(pyrcpod_errorBuffer);
      pyrcpod_errorBuffer = NULL;
    }
    pyrcpod_errorBuffer = strdup(message);
  }
  %}

/*************************************** Functions with exception handling */

/* Include all rcpod.h functions, wrapping them with a proper exception handler */
%exception {
  $action
  if (pyrcpod_errorBuffer) {
    SWIG_exception(SWIG_IOError, pyrcpod_errorBuffer);
  }
}

%include rcpod.h

%inline %{
  void pyrcpod_init(void) {
    /* Set up our error handler */
    rcpod_SetErrorHandler(pyrcpod_ErrorHandler);

    usb_init();
    usb_find_busses();
    rcpod_Init();
  }

  void pyrcpod_findDevices(void) {
    /* Ask libusb and librcpod to look for devices */
    usb_find_devices();
    rcpod_FindDevices();
  }
  %}

%exception;

/*************************************** Accessor functions */

/* Include functions from the SWIG standard library to access
 * the unsigned character arrays used by librcpod as buffers
 */
%array_functions(unsigned char, ucharArray)

/* Helper to follow the 'next' pointer in librcpod's device linked list */
%inline %{
  struct usb_device *pyrcpod_nextDevice(struct usb_device *dev) {
    return dev->next;
  }
  %}

/* The End */
