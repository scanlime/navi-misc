/* 3D object tracker. Requires a camera calibration file saved by 'camera_calibrator' */

#include <cv_dc1394.h>
#include <cv_sdl.h>
#include <camshift_frontend.h>
#include <cv.hpp>
#include <cvaux.h>


void tracker(int n_cameras, CvCalibFilter *calib) {
  IplImage **images;
  MultiCamshiftUI *tracker = NULL;
  SDL_Event event;
  Cv3dTrackerCameraInfo cam_info[n_cameras];
  CvBox2D *boxen;
  Cv3dTracker2dTrackedObject object2d[n_cameras];
  Cv3dTrackerTrackedObject object;
  Cv3dTrackerCameraIntrinsics cam_intrinsics[n_cameras];

  /* Copy camera intrinsics and reset calibration info */
  for (int i=0; i<n_cameras; i++) {
    const CvCamera *cvcam = calib->GetCameraParams(i);
    cam_intrinsics[i].principal_point.x = cvcam->matrix[2];
    cam_intrinsics[i].principal_point.y = cvcam->matrix[5];
    cam_intrinsics[i].focal_length[0] = cvcam->matrix[0];
    cam_intrinsics[i].focal_length[1] = cvcam->matrix[4];
    cam_intrinsics[i].distortion[0] = cvcam->distortion[0];
    cam_intrinsics[i].distortion[1] = cvcam->distortion[1];
    cam_intrinsics[i].distortion[2] = cvcam->distortion[2];
    cam_intrinsics[i].distortion[3] = cvcam->distortion[3];
  }
  memset(&cam_info, 0, sizeof(cam_info));

  printf("Calibrating 3D tracker, 7x9 checkerboard must be in view of both cameras\n");
  do {
    if (!cv_sdl_process_events())
      return;
    images = cv_dc1394_capture_yuv(n_cameras);
    cv_sdl_show_yuv_tiles(images, n_cameras, n_cameras);

    for (int i=0; i<n_cameras; i++)
      cvCvtColor(images[i], images[i], CV_YCrCb2RGB);
  } while (!cv3dTrackerCalibrateCameras(n_cameras, cam_intrinsics, cvSize(6,8), 1, images, cam_info));
  printf("Finished calibration\n");

  while (1) {
    images = cv_dc1394_capture_yuv(n_cameras);
    // calib->Undistort(images, images);

    if (!tracker)
      tracker = new MultiCamshiftUI(n_cameras, cvGetSize(images[0]));

    tracker->updateEventState();

    while (SDL_PollEvent(&event)) {
      if (tracker->processEvent(&event))
	continue;
      if (!cv_sdl_event_handler(&event))
	return;
    }

    boxen = tracker->track(images);
    for (int i=0; i<2; i++) {
      object2d[i].id = 1;
      object2d[i].p.x = (int) boxen[i].center.x;
      object2d[i].p.y = (int) boxen[i].center.y;
    }

    cv3dTrackerLocateObjects(n_cameras, 1, cam_info, object2d, &object);

    printf("%f %f %f\n", object.p.x, object.p.y, object.p.z);
  }
}

int main(int argc, char **argv) {
  CvCalibFilter calib;
  calib.LoadCameraParams("camera.calibration");

  if (!calib.IsCalibrated()) {
    printf("Run camera_calibrator first\n");
    return 1;
  }

  cv_dc1394_init();
  tracker(2, &calib);
  return 0;
}

/* The End */
