#include "3dtypes.h"

Point3D Point3D::operator = (const Point3D &p) {
  this->x = p.x;
  this->y = p.y;
  this->z = p.z;
  return *this;
}

Point3D cross(const Point3D &lhs, const Point3D &rhs, Point3D &result) {
  result.x = lhs.z * rhs.y - lhs.y * rhs.z;
  result.y = lhs.z * rhs.x - lhs.x * rhs.z;
  result.z = lhs.x * rhs.y - lhs.y * rhs.x;
  return result;
}
