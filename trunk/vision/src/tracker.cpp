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

  for (int i=0; i<n_cameras; i++) {
    const CvCamera *cvcam = calib->GetCameraParams(i);
    cam_info[i].valid = true;
    cam_info[i].principal_point.x = cvcam->matrix[2];
    cam_info[i].principal_point.y = cvcam->matrix[5];
    cam_info[i].mat[0][0] = cvcam->rotMatr[0];
    cam_info[i].mat[1][0] = cvcam->rotMatr[1];
    cam_info[i].mat[2][0] = cvcam->rotMatr[2];
    cam_info[i].mat[3][0] = cvcam->transVect[0];
    cam_info[i].mat[0][1] = cvcam->rotMatr[3];
    cam_info[i].mat[1][1] = cvcam->rotMatr[4];
    cam_info[i].mat[2][1] = cvcam->rotMatr[5];
    cam_info[i].mat[3][1] = cvcam->transVect[1];
    cam_info[i].mat[0][2] = cvcam->rotMatr[6];
    cam_info[i].mat[1][2] = cvcam->rotMatr[7];
    cam_info[i].mat[2][2] = cvcam->rotMatr[8];
    cam_info[i].mat[3][2] = cvcam->transVect[2];
    cam_info[i].mat[0][3] = 0;
    cam_info[i].mat[1][3] = 0;
    cam_info[i].mat[2][3] = 0;
    cam_info[i].mat[3][3] = 1;

    printf("Camera %d:\n"
	   "  principal point (%f,%f)\n"
	   "  matrix:\n"
	   "\t %f %f %f %f\n"
	   "\t %f %f %f %f\n"
	   "\t %f %f %f %f\n"
	   "\t %f %f %f %f\n\n", i,
	   cam_info[i].principal_point.x, cam_info[i].principal_point.y,
	   cam_info[i].mat[0][0], cam_info[i].mat[1][0], cam_info[i].mat[2][0], cam_info[i].mat[3][0],
	   cam_info[i].mat[0][1], cam_info[i].mat[1][1], cam_info[i].mat[2][1], cam_info[i].mat[3][1],
	   cam_info[i].mat[0][2], cam_info[i].mat[1][2], cam_info[i].mat[2][2], cam_info[i].mat[3][2],
	   cam_info[i].mat[0][3], cam_info[i].mat[1][3], cam_info[i].mat[2][3], cam_info[i].mat[3][3]);
  }

  while (1) {
    images = cv_dc1394_capture_yuv(n_cameras);
    calib->Undistort(images, images);

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
