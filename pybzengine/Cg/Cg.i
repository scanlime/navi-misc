%module Cg
%{
#include <cg.h>
%}
%include carrays.i

%array_functions(char*, stringArray)

// context functions
CGcontext cgCreateContext(void);
void cgDestroyContext(CGcontext);
int cgIsContext(CGcontext);
char *cgGetLastListing(CGcontext);

// program functions
CGprogram cgCreateProgram(CGcontext, int, const char *, int, const char *,  const char **);
CGprogram cgCreateProgramFromFile(CGcontext, int, const char *, int, const char *, const char **);
CGprogram cgCopyProgram(CGprogram);
void cgDestroyProgram(CGprogram);
CGprogram cgGetFirstProgram(CGcontext);
CGprogram cgGetNextProgram(CGprogram);
CGcontext cgGetProgramContext(CGprogram);
int cgIsProgram(CGprogram);
void cgCompileProgram(CGprogram);
int cgIsProgramCompiled(CGprogram);
const char *cgGetProgramString(CGprogram, int);
int cgGetProgramProfile(CGprogram);

// parameter functions
CGparameter cgGetNamedParameter(CGprogram, const char *);
CGparameter cgGetFirstParameter(CGprogram, int);
CGparameter cgGetNextParameter(CGparameter);
CGparameter cgGetFirstLeafParameter(CGprogram, int);
CGparameter cgGetNextLeafParameter(CGparameter);
CGparameter cgGetFirstStructParameter(CGparameter);
CGparameter cgGetFirstDependentParameter(CGparameter);
CGparameter cgGetArrayParameter(CGparameter, int);
int cgGetArrayDimension(CGparameter);
int cgGetArraySize(CGparameter, int);
CGprogram cgGetParameterProgram(CGparameter prog);
int cgIsParameter(CGparameter);
const char *cgGetParameterName(CGparameter);
int cgGetParameterType(CGparameter);
const char *cgGetParameterSemantic(CGparameter);
CGresource cgGetParameterResource(CGparameter);
CGresource cgGetParameterBaseResource(CGparameter);
unsigned long cgGetParameterResourceIndex(CGparameter);
int cgGetParameterVariability(CGparameter);
int cgGetParameterDirection(CGparameter);
int cgIsParameterReferenced(CGparameter);
const double *cgGetParameterValues(CGparameter, int, int *);
int cgGetParameterOrdinalNumber(CGparameter);

// type functions
const char *cgGetTypeString(int);
int cgGetType(const char *);

// resource functions
const char *cgGetResourceString(CGresource);
CGresource cgGetResource(const char *);

// profile functions
const char *cgGetProfileString(int);
int cgGetProfile(const char *);

// error functions
int cgGetError(void);
const char *cgGetErrorString(int);
void cgSetErrorCallback(CGerrorCallbackFunc);
int cgGetErrorCallback();
