#ifndef __RUNGE_KUTTA_H__
#define __RUNGE_KUTTA_H__

typedef void (*ODEFunc)(float *last, float *deriv, float time);

void vector_add       (float *a, float *b, float *result, int dim);
void vector_scale     (float *a, float  b, float *result, int dim);
void vector_subtract  (float *a, float *b, float *result, int dim);
float vector_length   (float *a, int dim);
void vector_normalize (float *a, float *result, int dim);

void k1 (ODEFunc ode, float *xn, float *d, float time, float tdelt);
void k2 (ODEFunc ode, float *xn, float *d, float *k1, float time, float tdelt, int dim);
void k3 (ODEFunc ode, float *xn, float *d, float *k2, float time, float tdelt, int dim);
void k4 (ODEFunc ode, float *xn, float *d, float *k3, float time, float tdelt, int dim);

void rk (ODEFunc ode, float *xn, float *xn1, int dim, float time, float tdelt);

#endif
