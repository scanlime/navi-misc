%module trackd
%include carrays.i

%array_functions(float, floatArray)

extern const char* getTrackdAPIVersion();

extern void* trackdInitTrackerReader(int);
extern int trackdGetNumberOfSensors(void*);
extern void trackdGetPosition(void*, int, float*);
extern void trackdGetEulerAngles(void*, int, float*);
extern void trackdGetMatrix(void*, int, float mat[4][4]);

extern void* trackdInitControllerReader(int);
extern int trackdGetNumberOfValuators(void*);
extern int trackdGetNumberOfButtons(void*);
extern float trackdGetValuator(void*, int);
extern int trackdGetButton(void*, int);
