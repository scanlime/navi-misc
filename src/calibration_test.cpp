#include <cv_dc1394.h>
#include <cv_sdl.h>
#include <cv.hpp>
#include <cvaux.h>

CvCalibFilter* calibrate() {
  CvCalibFilter *calib = new CvCalibFilter();
  IplImage** images;
  double etalonParams[3];

  calib->LoadCameraParams("camera.calibration");
  if (calib->IsCalibrated()) {
    printf("Loading saved calibration\n");
  }
  else {
    printf("Stereo calibration, 7x9 checkerboard must be in view\n");

    calib->SetCameraCount(2);

    /* We have a 7x9 checkerboard of unit squares */
    etalonParams[0] = 7;
    etalonParams[1] = 9;
    etalonParams[2] = 1;
    calib->SetEtalon(CV_CALIB_ETALON_CHECKERBOARD, etalonParams);

    while (!calib->IsCalibrated()) {
      images = cv_dc1394_capture_rgb(2);

      if (calib->FindEtalon(images)) {
	/* We found our checkerboard points in this image, use it to calibrate */
	CvPoint2D32f *points[2];
	int pointCount;
	bool found;
	calib->GetLatestPoints(0, &points[0], &pointCount, &found);
	calib->GetLatestPoints(1, &points[1], &pointCount, &found);
	printf("Accepted frame %d\n", calib->GetFrameCount()+1);
	calib->Push((const CvPoint2D32f**) points);
      }

      /* Draw points even if we haven't found the complete checkerboard yet */
      calib->DrawPoints(images);

      cvCvtColor(images[0], images[0], CV_RGB2YCrCb);
      cvCvtColor(images[1], images[1], CV_RGB2YCrCb);
      cv_sdl_show_yuv_tiles(images, 2, 2);
      if (!cv_sdl_process_events())
	return NULL;
    }
    calib->SaveCameraParams("camera.calibration");
    printf("Calibrated\n");
  }

  return calib;
}

int main() {
  CvCalibFilter* calib;
  IplImage** images;

  cv_dc1394_init();
  calib = calibrate();

  while (cv_sdl_process_events()) {
    images = cv_dc1394_capture_yuv(2);
    calib->Undistort(images, images);
    cv_sdl_show_yuv_tiles(images, 2, 2);
  }
}

