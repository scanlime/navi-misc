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

void k2 (ODEFunc ode, float *xn, float *d, float *k1, float time, float tdelt, int dim, float *scratch)
{
	float h2 = tdelt / 2.0f;
	vector_scale (k1, h2, scratch, dim);
	vector_add (scratch, xn, scratch, dim);
	ode (scratch, d, time + h2);
}

void k3 (ODEFunc ode, float *xn, float *d, float *k2, float time, float tdelt, int dim, float *scratch)
{
	float h2 = tdelt / 2.0f;
	vector_scale (k2, h2, scratch, dim);
	vector_add (scratch, xn, scratch, dim);
	ode (scratch, d, time + h2);
}

void k4 (ODEFunc ode, float *xn, float *d, float *k3, float time, float tdelt, int dim, float *scratch)
{
	vector_scale (k3, tdelt, scratch, dim);
	vector_add (scratch, xn, scratch, dim);
	ode (scratch, d, time + tdelt);
}


void rk (ODEFunc ode, float *xn, float *xn1, int dim, float time, float tdelt, float *scratch)
{
	float *kv1, *kv2, *kv3, *kv4, *a, *s;

	kv1 = &scratch[dim * 0];
	kv2 = &scratch[dim * 1];
	kv3 = &scratch[dim * 2];
	kv4 = &scratch[dim * 3];
	a   = &scratch[dim * 4];
	s   = &scratch[dim * 5];
	k1 (ode, xn, kv1, time, tdelt);
	k2 (ode, xn, kv2, kv1, time, tdelt, dim, s);
	k3 (ode, xn, kv3, kv2, time, tdelt, dim, s);
	k4 (ode, xn, kv4, kv3, time, tdelt, dim, s);

	vector_scale (kv2, 2, kv2, dim);
	vector_scale (kv3, 2, kv3, dim);

	vector_add   (kv1, kv2, a, dim);
	vector_add   (a,   kv3, a, dim);
	vector_add   (a,   kv4, a, dim);
	vector_scale (a,   tdelt / 6.0, a, dim);
	vector_add   (xn, a, xn1, dim);
}
