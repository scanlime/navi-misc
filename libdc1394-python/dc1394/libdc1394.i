/*
 * Low-level SWIG interface for libdc1394
 */

%module libdc1394

%{
#include <libdc1394/dc1394_control.h>
#include <libraw1394/raw1394.h>
#include <Numeric/arrayobject.h>
%}

%include carrays.i

%include /usr/include/libdc1394/dc1394_control.h
%include /usr/include/libraw1394/raw1394.h

%array_functions(int, intArray)
%array_functions(nodeid_t, nodeArray)
%array_functions(dc1394_cameracapture, cameraCaptureArray)
%array_functions(dc1394_camerainfo, cameraInfoArray)

%inline %{
  PyObject* get_capture_array(dc1394_cameracapture *capture) {
    int dims[3];
    PyArrayObject *arr;

    import_array();

    dims[0] = capture->frame_width;
    dims[1] = capture->frame_height;
    dims[2] = 3;

    arr = (PyArrayObject*) PyArray_FromDimsAndData(3, dims, PyArray_UBYTE, (char*) capture->capture_buffer);
    if (arr) {
      arr->strides[0] = 3;
      arr->strides[1] = 640*3;
      arr->strides[2] = 1;
      arr->flags = OWN_DIMENSIONS | OWN_STRIDES | SAVESPACE;
    }
    return (PyObject*) arr;
  }
%}
