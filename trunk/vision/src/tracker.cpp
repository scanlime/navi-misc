/* 3D object tracker. Requires a camera calibration file saved by 'camera_calibrator' */

#include <cv_dc1394.h>
#include <cv_sdl.h>
#include <camshift_frontend.h>
#include <cv.hpp>
#include <cvaux.h>
#include <math.h>

#define FIELDSENSOR_KLUDGE

#ifdef FIELDSENSOR_KLUDGE
FILE *efs_dev, *efs_output;
#endif

/* Given the user-input origin, X basis, and Y basis points,
 * transform an object's position from an arbitrary coordinate
 * system to the one specified by the user.
 */
CvPoint3D32f object_transform(CvPoint3D32f object,
			      CvPoint3D32f origin,
			      CvPoint3D32f x_basis,
			      CvPoint3D32f y_basis) {
  /* First subtract the origin from everything */
  object = cvPoint3D32f(object.x - origin.x,
			object.y - origin.y,
			object.z - origin.z);
  x_basis = cvPoint3D32f(x_basis.x - origin.x,
			 x_basis.y - origin.y,
			 x_basis.z - origin.z);
  y_basis = cvPoint3D32f(y_basis.x - origin.x,
			 y_basis.y - origin.y,
			 y_basis.z - origin.z);

  /* Calculate a Z basis as the cross product of X and Y */
  CvPoint3D32f z_basis;
  z_basis = cvPoint3D32f( x_basis.y * y_basis.z - x_basis.z * y_basis.y,
			 -x_basis.x * y_basis.z + x_basis.z * y_basis.x,
			  x_basis.x * y_basis.y - x_basis.y * y_basis.x);

  /* Scale the Z basis to have a length halfway between that of the X and Y bases */
  float x_len = sqrt(x_basis.x * x_basis.x + x_basis.y * x_basis.y + x_basis.z * x_basis.z);
  float y_len = sqrt(y_basis.x * y_basis.x + y_basis.y * y_basis.y + y_basis.z * y_basis.z);
  float z_len = sqrt(z_basis.x * z_basis.x + z_basis.y * z_basis.y + z_basis.z * z_basis.z);
  float new_z_len = (x_len + y_len) / 2;
  float z_scale = -new_z_len / z_len;
  z_basis.x *= z_scale;
  z_basis.y *= z_scale;
  z_basis.z *= z_scale;

  /* Now pose the problem as the solution of Ax = b where b is the original
   * object position and A contains the three basis column vectors.
   */
  CvMat *A = cvCreateMat(3, 3, CV_32FC1);
  CvMat *x = cvCreateMat(3, 1, CV_32FC1);
  CvMat *b = cvCreateMat(3, 1, CV_32FC1);
  cvmSet(b, 0, 0, object.x);
  cvmSet(b, 1, 0, object.y);
  cvmSet(b, 2, 0, object.z);
  cvmSet(A, 0, 0, x_basis.x);
  cvmSet(A, 1, 0, x_basis.y);
  cvmSet(A, 2, 0, x_basis.z);
  cvmSet(A, 0, 1, y_basis.x);
  cvmSet(A, 1, 1, y_basis.y);
  cvmSet(A, 2, 1, y_basis.z);
  cvmSet(A, 0, 2, z_basis.x);
  cvmSet(A, 1, 2, z_basis.y);
  cvmSet(A, 2, 2, z_basis.z);
  if (!cvSolve(A, b, x)) {
    printf("Oh no, object transformation is singular\n");
  }
  object = cvPoint3D32f(cvmGet(x, 0, 0),
			cvmGet(x, 1, 0),
			cvmGet(x, 2, 0));
  cvReleaseMat(&A);
  cvReleaseMat(&x);
  cvReleaseMat(&b);

  return object;
}


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
  CvPoint3D32f origin={0,0,0}, x_basis={1,0,0}, y_basis={0,1,0};
  CvPoint3D32f transformed;

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
    calib->Undistort(images, images);

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
    calib->Undistort(images, images);

    if (!tracker)
      tracker = new MultiCamshiftUI(n_cameras, cvGetSize(images[0]));

    tracker->updateEventState();

    while (SDL_PollEvent(&event)) {
      if (tracker->processEvent(&event))
	continue;
      if (!cv_sdl_event_handler(&event))
	return;

      if (event.type == SDL_KEYDOWN)
	switch (event.key.keysym.sym) {

	case 'o':
	  origin = object.p;
	  break;

	case 'x':
	  x_basis = object.p;
	  break;

	case 'y':
	  y_basis = object.p;
	  break;

#ifdef FIELDSENSOR_KLUDGE
	case 'l':
	  {
	    static int sequence = 0;
	    if (efs_output) {
	      fclose(efs_output);
	      efs_output = NULL;
	      printf("Closed log\n");
	    }
	    else {
	      char name[40];
	      sprintf(name, "efs-%d.log", ++sequence);
	      printf("Logging to %s\n", name);
	      efs_output = fopen(name, "w");
	    }
	  }
	  break;
#endif

	}
    }

    boxen = tracker->track(images);
    for (int i=0; i<2; i++) {
      object2d[i].id = 1;
      object2d[i].p.x = (int) boxen[i].center.x;
      object2d[i].p.y = (int) boxen[i].center.y;
    }

    cv3dTrackerLocateObjects(n_cameras, 1, cam_info, object2d, &object);
    transformed = object_transform(object.p, origin, x_basis, y_basis);
    printf("%6.2f %6.2f %6.2f\n", transformed.x, transformed.y, transformed.z);

#ifdef FIELDSENSOR_KLUDGE
    if (efs_output) {
      int efs_packet[9];
      float n_samples;
      fread(efs_packet, 9, sizeof(int), efs_dev);
      n_samples = efs_packet[8];
      if (n_samples > 0) {
	fprintf(efs_output,
		"%f %f %f %f %f %f %f %f %f %f %f\n",
		efs_packet[0] / n_samples,
		efs_packet[1] / n_samples,
		efs_packet[2] / n_samples,
		efs_packet[3] / n_samples,
		efs_packet[4] / n_samples,
		efs_packet[5] / n_samples,
		efs_packet[6] / n_samples,
		efs_packet[7] / n_samples,
		transformed.x, transformed.y, transformed.z);
      }
    }
#endif
  }
}

int main(int argc, char **argv) {
  CvCalibFilter calib;
  calib.LoadCameraParams("camera.calibration");

#ifdef FIELDSENSOR_KLUDGE
  efs_dev = fopen("/dev/usb/efs0", "rb");
#endif

  if (!calib.IsCalibrated()) {
    printf("Run camera_calibrator first\n");
    return 1;
  }

  cv_dc1394_init();
  tracker(2, &calib);
  return 0;
}

/* The End */
