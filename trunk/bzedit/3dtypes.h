#ifndef __3D_TYPES_H__
#define __3D_TYPES_H__

class Point3D {
  public:
    Point3D operator = (const Point3D &p);

    float x, y, z;
};

typedef struct {
  float s, t;
} Vertex2D;

typedef struct {
  float r, g, b;
} Color;

typedef struct {
  long verts[3];
  long norms[3];
  long TCs[3];
  Point3D faceNormal;
} Triangle;

Point3D cross(const Point3D &lhs, const Point3D &rhs, Point3D &result);

#endif
