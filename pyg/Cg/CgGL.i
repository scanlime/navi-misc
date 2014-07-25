%module CgGL
%{
#include <cgGL.h>
%}

// profile functions
int cgGLIsProfileSupported(int profile);
void cgGLEnableProfile(int);
void cgGLDisableProfile(int);
int cgGLGetLatestProfile(int);
void cgGLSetOptimalOptions(int);

// program management functions
void cgGLLoadProgram(CGprogram);
void cgGLBindProgram(CGprogram);

// parameter management functions
void cgGLSetParameter1f(CGparameter, float);
void cgGLSetParameter2f(CGparameter, float, float);
void cgGLSetParameter3f(CGparameter, float, float, float);
void cgGLSetParameter4f(CGparameter, float, float, float, float);
void cgGLSetParameter1fv(CGparameter, const float *);
void cgGLSetParameter2fv(CGparameter, const float *);
void cgGLSetParameter3fv(CGparameter, const float *);
void cgGLSetParameter4fv(CGparameter, const float *);
void cgGLSetParameter1d(CGparameter, double);
void cgGLSetParameter2d(CGparameter, double, double);
void cgGLSetParameter3d(CGparameter, double, double, double);
void cgGLSetParameter4d(CGparameter, double, double, double, double);
void cgGLSetParameter1dv(CGparameter, const double *);
void cgGLSetParameter2dv(CGparameter, const double *);
void cgGLSetParameter3dv(CGparameter, const double *);
void cgGLSetParameter4dv(CGparameter, const double *);

void cgGLGetParameter1f(CGparameter param, float *v);
void cgGLGetParameter2f(CGparameter param, float *v);
void cgGLGetParameter3f(CGparameter param, float *v);
void cgGLGetParameter4f(CGparameter param, float *v);
void cgGLGetParameter1d(CGparameter param, double *v);
void cgGLGetParameter2d(CGparameter param, double *v);
void cgGLGetParameter3d(CGparameter param, double *v);
void cgGLGetParameter4d(CGparameter param, double *v);

void cgGLSetParameterArray1f(CGparameter, long, long, const float *);
void cgGLSetParameterArray2f(CGparameter, long, long, const float *);
void cgGLSetParameterArray3f(CGparameter, long, long, const float *);
void cgGLSetParameterArray4f(CGparameter, long, long, const float *);
void cgGLSetParameterArray1d(CGparameter, long, long, const double *);
void cgGLSetParameterArray2d(CGparameter, long, long, const double *);
void cgGLSetParameterArray3d(CGparameter, long, long, const double *);
void cgGLSetParameterArray4d(CGparameter, long, long, const double *);

void cgGLGetParameterArray1f(CGparameter, long, long, float *);
void cgGLGetParameterArray2f(CGparameter, long, long, float *);
void cgGLGetParameterArray3f(CGparameter, long, long, float *);
void cgGLGetParameterArray4f(CGparameter, long, long, float *);
void cgGLGetParameterArray1d(CGparameter, long, long, double *);
void cgGLGetParameterArray2d(CGparameter, long, long, double *);
void cgGLGetParameterArray3d(CGparameter, long, long, double *);
void cgGLGetParameterArray4d(CGparameter, long, long, double *);
void cgGLSetParameterPointer(CGparameter, GLint, GLenum, GLsizei, GLvoid *);

void cgGLEnableClientState(CGparameter);
void cgGLDisableClientState(CGparameter);

// matrix parameter management functions
void cgGLSetMatrixParameterdr(CGparameter, const double *);
void cgGLSetMatrixParameterfr(CGparameter, const float *);
void cgGLSetMatrixParameterdc(CGparameter, const double *);
void cgGLSetMatrixParameterfc(CGparameter, const float *);

void cgGLGetMatrixParameterdr(CGparameter, double *);
void cgGLGetMatrixParameterfr(CGparameter, float *);
void cgGLGetMatrixParameterdc(CGparameter, double *);
void cgGLGetMatrixParameterfc(CGparameter, float *);

void cgGLSetStateMatrixParameter(CGparameter, int, int);
void cgGLSetMatrixParameterArrayfc(CGparameter, long, long, const float *);
void cgGLSetMatrixParameterArrayfr(CGparameter, long, long, const float *);
void cgGLSetMatrixParameterArraydc(CGparameter, long, long, const double *);
void cgGLSetMatrixParameterArraydr(CGparameter, long, long, const double *);

void cgGLGetMatrixParameterArrayfc(CGparameter, long, long, float *);
void cgGLGetMatrixParameterArrayfr(CGparameter, long, long, float *);
void cgGLGetMatrixParameterArraydc(CGparameter, long, long, double *);
void cgGLGetMatrixParameterArraydr(CGparameter, long, long, double *);

// texture parameter management functions
void cgGLSetTextureParameter(CGparameter, GLuint);
GLuint cgGLGetTextureParameter(CGparameter);
void cgGLEnableTextureParameter(CGparameter);
void cgGLDisableTextureParameter(CGparameter);
GLenum cgGLGetTextureEnum(CGparameter);
