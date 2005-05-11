#include <stdlib.h>
#include "vect.h"

#ifndef RUNGE_H
#define RUNGE_H

vect k1(const vect &x, double h);
vect k2(const vect &x, double h, const vect &k1_);
vect k3(const vect &x, double h, const vect &k2_);
vect k4(const vect &x, double h, const vect &k3_);

vect runge(const vect &x, double h,double time);

#endif
