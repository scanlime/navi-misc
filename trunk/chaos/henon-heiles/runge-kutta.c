#include <glib.h>
#include <math.h>
#include "runge-kutta.h"

void vector_add (float *a, float *b, float *result, int dim)
{
  int i;
  for (i = 0; i < dim; i++)
    result[i] = a[i] + b[i];
}

void vector_scale (float *a, float  b, float *result, int dim)
{
  int i;
  for (i = 0; i < dim; i++)
    result[i] = a[i] * b;
}

void vector_subtract (float *a, float *b, float *result, int dim)
{
  int i;
  for (i = 0; i < dim; i++)
    result[i] = a[i] - b[i];
}

void vector_normalize (float *a, float *result, int dim)
{
  float l = vector_length (a, dim);
  vector_scale (a, 1/l, result, dim);
}

float vector_length (float *a, int dim)
{
  float b = 0;
  int i;
  for (i = 0; i < dim; i++)
    b += a[i] * a[i];
  return sqrt (b);
}

void k1 (ODEFunc ode, float *xn, float *d, float time, float tdelt)
{
  ode (xn, d, time);
}

void k2 (ODEFunc ode, float *xn, float *d, float *k1, float time, float tdelt, int dim)
{
  float h2 = tdelt / 2.0f;
  float *a = g_new (float, dim);
  vector_scale (k1, h2, a, dim);
  vector_add   (a,  xn, a, dim);
  ode (a, d, time + h2);
  g_free (a);
}

void k3 (ODEFunc ode, float *xn, float *d, float *k2, float time, float tdelt, int dim)
{
  float h2 = tdelt / 2.0f;
  float *a = g_new (float, dim);
  vector_scale (k2, h2, a, dim);
  vector_add   (a,  xn, a, dim);
  ode (a, d, time + h2);
  g_free (a);
}

void k4 (ODEFunc ode, float *xn, float *d, float *k3, float time, float tdelt, int dim)
{
  float *a = g_new (float, dim);
  vector_scale (k3, tdelt, a, dim);
  vector_add   (a,  xn,    a, dim);
  ode (a, d, time + tdelt);
  g_free (a);
}


void rk (ODEFunc ode, float *xn, float *xn1, int dim, float time, float tdelt)
{
  float *kv1, *kv2, *kv3, *kv4;
  float *a;

  kv1 = g_new (float, dim);
  kv2 = g_new (float, dim);
  kv3 = g_new (float, dim);
  kv4 = g_new (float, dim);
  a   = g_new (float, dim);
  k1 (ode, xn, kv1, time, tdelt);
  k2 (ode, xn, kv2, kv1, time, tdelt, dim);
  k3 (ode, xn, kv3, kv2, time, tdelt, dim);
  k4 (ode, xn, kv4, kv3, time, tdelt, dim);

  vector_scale (kv2, 2, kv2, dim);
  vector_scale (kv3, 2, kv3, dim);

  vector_add   (kv1, kv2, a, dim);
  vector_add   (a,   kv3, a, dim);
  vector_add   (a,   kv4, a, dim);
  vector_scale (a,   tdelt / 6.0, a, dim);
  vector_add   (xn, a, xn1, dim);

  g_free (kv1);
  g_free (kv2);
  g_free (kv3);
  g_free (kv4);
  g_free (a);
}
