#include <GL/gl.h>
#include "camera.h"

Camera::Camera()
{
  fx = fy = 0; fz = 4.0;
  d = 750;
  rot = 45;
  ele = -75;
  focus = true;
}

void Camera::setup()
{
  glLoadIdentity();
  glTranslatef(0, 0, -d);
  glRotatef(ele, 1.0, 0.0, 0.0);
  glRotatef(rot, 0.0, 0.0, 1.0);
  if(focus)
    glTranslatef(-fx, -fy, -fz);
}
