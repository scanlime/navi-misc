/* bzflag
* Copyright (c) 1993 - 2004 Tim Riker
*
* This package is free software;  you can redistribute it and/or
* modify it under the terms of the license found in the file
* named LICENSE that should have accompanied this file.
*
* THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
* WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
*/
// GeoUtils.h

#ifndef _GEO_UTILS_H_
#define _GEO_UTILS_H_

#ifndef DEG_RAD
#define DEG_RAD 0.017453f
#endif

#ifndef REALY_SMALL
#define REALY_SMALL 0.0001f
#endif

#define PI						3.141592653589793238f
#define	TWO_PI				(PI*2.0f)
#define rad_con				(PI/180.0f)		// conversion from degrees to radians
#define deg_con				(180.0f/PI)		// conversion from radians to degrees

#define	TWO_PI				(PI*2.0f)
#define LIN_TOL				0.00005f		// linear tolerance, in inches (50 millionths of an inch)

// 3d variables
typedef struct
{
	float	x,y,z;
}trVertex3D;

typedef struct
{
	float	s,t;
}trVertex2D;

// 3D Point clas
class C3DVertex
{
public:
	C3DVertex();
	C3DVertex(const trVertex3D &rVert);
	C3DVertex(float x , float y, float z = 0);
	C3DVertex(float *pos);
	virtual ~C3DVertex();

	void Set (const trVertex3D &rVert);
	void Set (float x = 0, float y = 0, float z = 0);
	void Set (float* p);

	float SetX ( float x );
	float SetY ( float y );
	float SetZ ( float z );

	float X (void) const;
	float Y (void) const;
	float Z (void) const;

	void Get ( float* pos );

	// functions
	float Dot (const C3DVertex &p0) const;
	float Dot2D (const C3DVertex &p0) const;

	void Cross (const C3DVertex &p0, const C3DVertex &p1);
	float Cross2D (const C3DVertex &p0) const;

	float GetMag (void) const;
	float GetMag2D (void) const;
	float GetMagSq (void) const;

	void SetMag (float scaler);
	void SetMag2D (float scaler);

	void Normalise (void)							{ SetMag(1);	}
	void Normalise2D (void)							{ SetMag2D(1);	}

	bool Close (const C3DVertex &p0);
	bool Close2D (const C3DVertex &p0); 

	// operators
	C3DVertex& operator = (const C3DVertex &p0);
	C3DVertex& operator = (const trVertex3D &p0);
	C3DVertex operator - (void) const;
	C3DVertex operator - (const C3DVertex &p0) const;
	C3DVertex operator + (const C3DVertex &p0) const;
	C3DVertex operator * (float scaler) const;
	C3DVertex operator / (float scaler) const;

	void operator -= (const C3DVertex &p0);
	void operator += (const C3DVertex &p0);
	void operator *= (const C3DVertex &p0);
	void operator *= (float scaler);
	void operator /= (const C3DVertex &p0);
	void operator /= (float scaler);

	operator trVertex3D& (void)						{ return m_rVertex; }
	operator const trVertex3D& (void) const			{ return m_rVertex; }

	trVertex3D* GetVertAddress ( void ) { return &m_rVertex; }

	trVertex3D m_rVertex;
};


typedef struct
{
	long			aVerts[3];
	long			aNorms[3];
	long			aTCs[3];
	trVertex3D		rFaceNormal;
}trTriangle;

#include <math.h>

/*--------------------------------- trVertex3D operators -----------------------------------------*/

/*--------------------------------- operator - -----------------------------------------*/

inline trVertex3D operator - (const trVertex3D &p0)
{
	trVertex3D p1;

	p1.x = - p0.x;
	p1.y = - p0.y;
	p1.z = - p0.z;

	return p1;
}

/*--------------------------------- operator + -----------------------------------------*/

inline trVertex3D operator + (const trVertex3D &p0, const trVertex3D &p1)
{
	trVertex3D p2;

	p2.x = p0.x + p1.x;
	p2.y = p0.y + p1.y;
	p2.z = p0.z + p1.z;

	return p2;
}

/*--------------------------------- operator - -----------------------------------------*/

inline trVertex3D operator - (const trVertex3D &p0, const trVertex3D &p1)
{
	trVertex3D p2;

	p2.x = p0.x - p1.x;
	p2.y = p0.y - p1.y;
	p2.z = p0.z - p1.z;

	return p2;
}

/*--------------------------------- operator * -----------------------------------------*/

inline trVertex3D operator * (const trVertex3D &p0, float scalar)
{
	trVertex3D p2;

	p2.x = p0.x * scalar;
	p2.y = p0.y * scalar;
	p2.z = p0.z * scalar;

	return p2;
}

/*--------------------------------- operator / -----------------------------------------*/

inline trVertex3D operator / (const trVertex3D &p0, float scalar)
{
	trVertex3D p2;

	scalar = 1.0f / scalar;
	p2.x = p0.x * scalar;
	p2.y = p0.y * scalar;
	p2.z = p0.z * scalar;

	return p2;
}

/*--------------------------------- operator * -----------------------------------------*/

inline trVertex3D operator * (float scalar, const trVertex3D &p0)
{
	trVertex3D p2;

	p2.x = p0.x * scalar;
	p2.y = p0.y * scalar;
	p2.z = p0.z * scalar;

	return p2;
}

/*--------------------------------- operator / -----------------------------------------*/

inline trVertex3D operator / (float scalar, const trVertex3D &p0)
{
	trVertex3D p2;

	scalar = 1.0f / scalar;
	p2.x = p0.x * scalar;
	p2.y = p0.y * scalar;
	p2.z = p0.z * scalar;

	return p2;
}

/*--------------------------------- operator += -----------------------------------------*/

inline void operator += (trVertex3D &p0, const trVertex3D &p1)
{
	p0.x += p1.x;
	p0.y += p1.y;
	p0.z += p1.z;
}

/*--------------------------------- operator -= -----------------------------------------*/

inline void operator -= (trVertex3D &p0, const trVertex3D &p1)
{
	p0.x -= p1.x;
	p0.y -= p1.y;
	p0.z -= p1.z;
}

/*--------------------------------- operator *= -----------------------------------------*/

inline void operator *= (trVertex3D &p0, const trVertex3D &p1)
{
	p0.x *= p1.x;
	p0.y *= p1.y;
	p0.z *= p1.z;
}

/*--------------------------------- operator /= -----------------------------------------*/

inline void operator /= (trVertex3D &p0, const trVertex3D &p1)
{
	p0.x /= p1.x;
	p0.y /= p1.y;
	p0.z /= p1.z;
}

/*--------------------------------- operator *= -----------------------------------------*/

inline void operator *= (trVertex3D &p0, float scalar)
{
	p0.x *= scalar;
	p0.y *= scalar;
	p0.z *= scalar;
}

/*--------------------------------- operator /= -----------------------------------------*/

inline void operator /= (trVertex3D &p0, float scalar)
{
	scalar = 1.0f / scalar;
	p0.x *= scalar;
	p0.y *= scalar;
	p0.z *= scalar;
}

/*------------------------------------- dot ---------------------------------------------*/

inline float dot(const trVertex3D &lhs, const trVertex3D &rhs)
{
	return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

/*------------------------------------- dot_2d ---------------------------------------------*/

inline float dot_2d(const trVertex3D &lhs, const trVertex3D &rhs)
{
	return lhs.x * rhs.x + lhs.y * rhs.y;
}

/*------------------------------------- cross ---------------------------------------------*/

inline trVertex3D& cross(const trVertex3D &lhs, const trVertex3D &rhs, trVertex3D &result)
{
	float temp_x, temp_y;		// this allows result to be same as lhs or rhs

	temp_x = lhs.z * rhs.y - lhs.y * rhs.z;
	temp_y = lhs.z * rhs.x - lhs.x * rhs.z;
	result.z = lhs.x * rhs.y - lhs.y * rhs.x;
	result.x = temp_x;
	result.y = temp_y;
	return result;
}

/*------------------------------------- cross_2d ---------------------------------------------*/

inline float cross_2d(const trVertex3D &lhs, const trVertex3D &rhs)
{
	return lhs.x * rhs.y - lhs.y * rhs.x;
}

/*------------------------------------- get_mag ---------------------------------------------*/

inline float get_mag(const trVertex3D &vec)
{
	return (float)sqrt(dot(vec, vec));
}

/*------------------------------------- get_mag_2d ---------------------------------------------*/

inline float get_mag_2d(const trVertex3D &vec)
{
	return (float)sqrt(dot_2d(vec, vec));
}

/*------------------------------------- set_mag ---------------------------------------------*/

inline bool set_mag(trVertex3D &vec, float mag)
{
	float old_mag = get_mag(vec);
	if (old_mag < 1.0e-15)
	{
		vec.x = mag;
		vec.z = vec.y = 0;
		return false;
	}
	vec *= mag / old_mag;
	return true;
}

/*------------------------------------- set_mag_2d ---------------------------------------------*/

inline bool set_mag_2d(trVertex3D &vec, float mag)
{
	float old_mag = get_mag_2d(vec);
	if (old_mag < 1.0e-15)
	{
		vec.x = mag;
		vec.z = vec.y = 0;
		return false;
	}
	mag /= old_mag;
	vec.x *= mag;
	vec.y *= mag;
	return true;
}

/*------------------------------------- close ---------------------------------------------*/
inline bool close(const trVertex3D &vec1, const trVertex3D &vec2)
{
	if ( fabs(vec1.x-vec2.x)> REALY_SMALL)
		return false;

	if ( fabs(vec1.y-vec2.y)> REALY_SMALL)
		return false;

	if ( fabs(vec1.z-vec2.z)> REALY_SMALL)
		return false;

	return true;
}

/*------------------------------------- close_2d ---------------------------------------------*/
inline bool close_2d(const trVertex3D &vec1, const trVertex3D &vec2)
{
	if ( fabs(vec1.x-vec2.x)> REALY_SMALL)
		return false;

	if ( fabs(vec1.y-vec2.y)> REALY_SMALL)
		return false;

	return true;
}


// matrix classes
#define XFORM_TRANS		1
#define XFORM_SCALE		2
#define XFORM_ROT		4

typedef struct
{
	char flags;
	float el[4][4];
}trMatrix;

typedef enum
{
	eXY, 
	eXZ, 
	eYZ,
	eXR,
	eXD,
	eNonPrimary
}tePlanes;

trMatrix *ClassifyMatrix(trMatrix*);
trMatrix *NormalizeMatrix(trMatrix*);
trMatrix *GetIdentityMatrix(trMatrix*);
bool MatrixPlanar(const trMatrix*, bool*);
tePlanes MatrixPlane(const trMatrix*);
bool MatrixScaleNonUniform(const trMatrix*);
bool MatrixScaleNonUniform2d(const trMatrix*);
trVertex3D *MatrixScale(const trMatrix*, trVertex3D*);
bool EqualMatrix(const trMatrix*, const trMatrix*);
bool SamePlaneMatrix(const trMatrix*, const trMatrix*, bool*);
trMatrix *TranslateMatrix(trMatrix*, const trVertex3D*);
trMatrix *TranslationMatrix(trMatrix*, const trVertex3D*);
trMatrix *ScaleMatrix(trMatrix*, const trVertex3D*);
trMatrix *ScalingMatrix(trMatrix*, const trVertex3D*);
trMatrix *RotationMatrix(trMatrix*, const trVertex3D*, float, float);
trMatrix *RotationMatrix(trMatrix*, float, float);
trMatrix *Rotate2dMatrix(trMatrix*, float, const trVertex3D*);
trMatrix *MirrorMatrix(trMatrix*, const trVertex3D*, const trVertex3D*);
trMatrix *ForceDepthMatrix(trMatrix*, double);
trMatrix *ConcatMatrices(trMatrix*, const trMatrix*);
trMatrix *InvertMatrix(trMatrix*);
trVertex3D *TransformPos(trVertex3D*, const trMatrix*);
trVertex3D *TransformPos2d(trVertex3D*, const trMatrix*);
trVertex3D *TransformNorm(trVertex3D*, const trMatrix*);
trVertex3D *TransformNorm2d(trVertex3D*, const trMatrix*);
trMatrix *MatrixFromAxes(trMatrix*, const trVertex3D*, const trVertex3D*, const trVertex3D*);
trMatrix *DifferenceMatrix(trMatrix*, const trVertex3D*, const trVertex3D*, const trVertex3D*, const trVertex3D*, const trVertex3D*, const trVertex3D*);


class CMatrix
{
public:
		CMatrix();
		CMatrix(const trMatrix &m);
		CMatrix(const CMatrix &m);
		CMatrix(const float *m , const char flags);
		virtual ~CMatrix();

		char GetFlags ( void ) { return matrix.flags; }
		void SetFlags ( char flag ) { matrix.flags = flag; }
		void AddFlag ( char flag ) { matrix.flags |= flag; }

		float GetElement ( int row, int col );
		float SetElement ( float val, int row, int col );

		float *GetElements ( void );
		void GetElements ( float *m );

		void SetElements ( float *m );

		void Classify( void );
		void Normalize( void );
		void GetIdentity( void );
		bool Planar ( void );
		tePlanes Plane( void );

		bool ScaleNonUniform( void );
		bool ScaleNonUniform2d( void );
		trVertex3D *MatrixScale( trVertex3D *p);

		bool operator == ( const CMatrix &m );	// equal matrix
		void operator += (const CMatrix &m);	// concat
		void Concat (const CMatrix &m);	// concat

		bool SamePlane( const CMatrix &m );

		void Invert( void );
		void Translate( const trVertex3D* p);
		void Scale( const trVertex3D* p);
		void Rotate2d(float angle, const trVertex3D* p);
		void Mirror(const trVertex3D* p, const trVertex3D* p2);

		void TranslationMatrix( const trVertex3D* p);
		void ScalingMatrix( const trVertex3D* p);
		void RotationMatrix( const trVertex3D* p, float a, float b);
		void RotationMatrix( float a, float b);
		void ForceDepthMatrix( float depth );

		C3DVertex& TransformPos ( C3DVertex& point );
		C3DVertex& TransformPos2d ( C3DVertex& point );
		C3DVertex& TransformNorm ( C3DVertex& point );
		C3DVertex& TransformNorm2d ( C3DVertex& point );

protected:
	trMatrix		matrix;
};

// frustum classes
class ViewPlane
{
public:
	ViewPlane();
	~ViewPlane();

	bool PointBehind ( float fX, float fY, float fZ );
	float PointDist ( float fX, float fY, float fZ );
	void Set ( float fA, float fB, float fC , float fD );

	float GetA ( void ){return m_rNorm.x;}
	float GetB ( void ){return m_rNorm.y;}
	float GetC ( void ){return m_rNorm.z;}
	float GetD ( void ){return m_fODist;}

	trVertex3D	m_rNorm;
	float		m_fODist;

	void Normalise();

private:
	bool	m_bSet;
};

typedef enum 
{
	eFrustRight	= 0,
	eFrustLeft,		
	eFrustBotom,	
	eFrustTop,		
	eFrustBack,		
	eFrustFront,
	eLastFrustSide
}eFrustumSides; 

class BaseFrustum 
{
public:
	BaseFrustum();
	virtual ~BaseFrustum();
	
	// functions to set the frustum
	void SetPlane ( eFrustumSides side, float a, float b, float c, float d );
	void Set ( float fovH, float fovV, C3DVertex &pos, C3DVertex & rot );

	// tests
	virtual bool PointIn(float fX, float fY, float fZ);
	virtual bool SphereIn(float fX, float fY, float fZ, float fRad);
	virtual bool CubeIn( float fX, float fY, float fZ, float fSize );
	virtual bool BoxIn( float fX, float fY, float fZ ,float fXSize, float fYSize, float fZSize );

	virtual bool QuadOut ( float *fp1, float *fp2, float *fp3, float *fp4 );

protected:
	bool		m_bSet;
	ViewPlane m_aPlanes[eLastFrustSide];
};


#endif//_GEO_UTILS_H_
