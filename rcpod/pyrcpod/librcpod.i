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
  /* Nonzero if an error occurred, pyrcpod_errorBuffer will contain
   * a malloc'ed string with an error message.
   */
  int pyrcpod_errorOccurred = 0;
  char *pyrcpod_errorBuffer = NULL;

  /* An error callback that sets the above variable */
  void pyrcpod_ErrorHandler(const char *function, int err, const char *message) {
    if (pyrcpod_errorBuffer) {
      free(pyrcpod_errorBuffer);
      pyrcpod_errorBuffer = NULL;
    }
    pyrcpod_errorBuffer = strdup(message);
    pyrcpod_errorOccurred = 1;
  }
  %}

/*************************************** Functions with exception handling */

/* Include all rcpod.h functions, wrapping them with a proper exception handler.
 * We also release the python global lock, so other python threads can run while
 * we do our blocking I/O.
 */
%exception {
  Py_BEGIN_ALLOW_THREADS
  $action
  Py_END_ALLOW_THREADS

  if (pyrcpod_errorOccurred) {
    pyrcpod_errorOccurred = 0;
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

/* usb_device helpers */
%inline %{
  struct usb_device *pyrcpod_nextDevice(struct usb_device *dev) {
    return dev->next;
  }

  struct usb_device *pyrcpod_copyDevice(struct usb_device *dev) {
    struct usb_device *newdev = malloc(sizeof(struct usb_device));
    if (newdev) {
      memcpy(newdev, dev, sizeof(struct usb_device));
      newdev->next = NULL;
      newdev->prev = NULL;
    }
    return newdev;
  }

  void pyrcpod_deleteDevice(struct usb_device *dev) {
    free(dev);
  }
  %}

/* Helpers for creating and destroying rcpod_i2c_dev structures */
%inline %{
  rcpod_i2c_dev *new_i2cDev(int speed, rcpod_pin clock, rcpod_pin data, unsigned char address) {
    rcpod_i2c_dev *idev = malloc(sizeof(rcpod_i2c_dev));
    if (idev) {
      idev->speed = speed;
      idev->clock = clock;
      idev->data = data;
      idev->address = address;
    }
    return idev;
  }

  void delete_i2cDev(rcpod_i2c_dev *idev) {
    free(idev);
  }
%}

/* Helpers for looking at the descriptors of devices not yet opened */
%inline %{
  int usbdev_getDeviceVersion(struct usb_device *dev) {
    return dev->descriptor.bcdDevice;
  }

  int usbdev_getSerialIndex(struct usb_device *dev) {
    return dev->descriptor.iSerialNumber;
  }

  const char* usbdev_getFilename(struct usb_device *dev) {
    return dev->filename;
  }

  const char* usbdev_getDirname(struct usb_device *dev) {
    return dev->bus->dirname;
  }
  %}

/* You have to open devices before retrieving string descriptors */
%inline %{
  const char* usbdevh_getString(rcpod_dev *dev, int index) {
    /* swig will copy this to a new python string, and we're holding
     * the interpreter lock- so this is safe, although a bit suboptimal.
     */
    static char buffer[256];
    buffer[0] = 0;
    buffer[sizeof(buffer)-1] = 0;
    usb_get_string_simple(rcpod_GetDevHandle(dev), index, buffer, sizeof(buffer));
    return buffer;
  }
  %}

/* The End */
