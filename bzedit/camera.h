#ifndef __CAMERA_H__
#define __CAMERA_H__

class Camera {
  public:
    Camera();

    void setup();

    float d, rot, ele, fx, fy, fz;
    bool focus;
};

#endif
