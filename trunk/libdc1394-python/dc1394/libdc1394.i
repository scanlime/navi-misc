/*
 * Low-level SWIG interface for libdc1394
 */

%module libdc1394

%{
#include <libdc1394/dc1394_control.h>
#include <libraw1394/raw1394.h>
%}

%include carrays.i

%include /usr/include/libdc1394/dc1394_control.h
%include /usr/include/libraw1394/raw1394.h

%array_functions(int, intArray)
%array_functions(nodeid_t, nodeArray)
%array_functions(dc1394_cameracapture, cameraCaptureArray)
%array_functions(dc1394_camerainfo, cameraInfoArray)

%inline %{
  void write_image(dc1394_cameracapture *capture) {
    FILE* image = fopen("image.ppm", "wb");
    fprintf(image, "P6\n%d %d\n255\n",  capture->frame_width, capture->frame_height);
    fwrite(capture->capture_buffer, 1, capture->frame_width * capture->frame_height * 3, image);
    fclose(image);
  }
%}
