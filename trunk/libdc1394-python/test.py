#!/usr/bin/env python

from dc1394.libdc1394 import *

# Ask libraw1394 how many ports we have
handle = raw1394_new_handle()
numPorts = raw1394_get_port_info(handle, None, 0)
raw1394_destroy_handle(handle)

# Look for cameras on each port...
for port in xrange(numPorts):
    handle = dc1394_create_handle(port)

    # Get a list of cameras on this port
    numCamerasPointer = new_intArray(1)
    nodeArray = dc1394_get_camera_nodes(handle, numCamerasPointer, 0)
    numCameras = intArray_getitem(numCamerasPointer, 0)
    nodes = [nodeArray_getitem(nodeArray, i) for i in xrange(numCameras)]
    delete_nodeArray(nodeArray)

    # If we found any, we're done...
    if nodes:
        cameraNode = nodes[0]
        break

    dc1394_destroy_handle(handle)

# Print info on this camera
info = new_cameraInfoArray(1)
dc1394_get_camera_info(handle, cameraNode, info)
dc1394_print_camera_info(info)

# Set up a capture
capture = new_cameraCaptureArray(1)
dc1394_setup_capture(handle, cameraNode, 0, FORMAT_VGA_NONCOMPRESSED,
                     MODE_640x480_RGB, SPEED_400, FRAMERATE_15,
                     capture)
dc1394_start_iso_transmission(handle, cameraNode)

dc1394_single_capture(handle, capture)
write_image(capture)
