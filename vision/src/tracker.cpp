/* 3D object tracker. Requires a camera calibration file saved by 'camera_calibrator' */

#include <cv_dc1394.h>
#include <cv_sdl.h>
#include <camshift_frontend.h>
#include <cv.hpp>
#include <cvaux.h>


void tracker(int n_cameras, CvCalibFilter *calib) {
  IplImage **images;
  IplImage *rgb_images[n_cameras];
  MultiCamshiftUI *tracker = NULL;
  SDL_Event event;
  Cv3dTrackerCameraInfo cam_info[n_cameras];
  CvBox2D *boxen;
  Cv3dTracker2dTrackedObject object2d[n_cameras];
  Cv3dTrackerTrackedObject object;
  Cv3dTrackerCameraIntrinsics cam_intrinsics[n_cameras];

  memset(rgb_images, 0, sizeof(rgb_images));

  /* Copy camera intrinsics from the saved calibration */
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

  printf("Calibrating 3D tracker, 7x9 checkerboard must be in view of both cameras\n");
  while (1) {
    if (!cv_sdl_process_events())
      return;
    images = cv_dc1394_capture_yuv(n_cameras);

    /* Draw an X or a check on each camera to indicate current calibration status */
    for (int i=0; i<n_cameras; i++) {
      IplImage *image = images[i];
      if (cam_info[i].valid) {
	/* Green check */
	const double yuv_color = CV_RGB(0, 0, 255);
	cvLineAA(image, cvPoint(10, 64), cvPoint(50, 128), yuv_color);
	cvLineAA(image, cvPoint(50, 128), cvPoint(100, 10), yuv_color);
      }
      else {
	/* Red X */
	const double yuv_color = CV_RGB(255, 0, 192);
	cvLineAA(image, cvPoint(10, 10), cvPoint(128, 128), yuv_color);
	cvLineAA(image, cvPoint(10, 128), cvPoint(128, 10), yuv_color);
      }
    }

    cv_sdl_show_yuv_tiles(images, n_cameras, n_cameras);

    /* Calibration should be done in RGB space */
    for (int i=0; i<n_cameras; i++) {
      if (!rgb_images[i])
	rgb_images[i] = cvCreateImage(cvGetSize(images[i]), 8, 3);
      cvCvtColor(images[i], rgb_images[i], CV_YCrCb2RGB);
    }

    /* Try to calibrate. This updates cam_info, and returns true if we're done */
    memset(&cam_info, 0, sizeof(cam_info));
    if (cv3dTrackerCalibrateCameras(n_cameras, cam_intrinsics, cvSize(6,8), 1, rgb_images, cam_info)) {
      printf("Finished calibration\n");
      break;
    }
  }

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
