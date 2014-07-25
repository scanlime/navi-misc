/* bzflag
* Copyright (c) 1993 - 2004 Tim Riker
*
* This package is free software;  you can redistribute it and/or
* modify it under the terms of the license found in the file
* named COPYING that should have accompanied this file.
*
* THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
* WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
*/

// 3dutils.cpp

#include "GeoUtils.h"
#include <memory>

#define EQUAL_X(v0, v1, z)	(fabs((v0) - (v1)) < (z))

#define EQUAL_X2(v0, v1, z)	(EQUAL_X((v0).x, (v1).x, z) && EQUAL_X((v0).y, (v1).y, z))
#define EQUAL_X3(v0, v1, z)	(EQUAL_X2(v0, v1, z) && EQUAL_X((v0).z, (v1).z, z))


static void Invert3x3(trMatrix*);
static trMatrix *TransposeMatrix(trMatrix*);
static trMatrix *AddMatrix(trMatrix*, const trMatrix*);
static trMatrix *SubtractMatrix(trMatrix*, const trMatrix*);
static float MatrixDeterminant(const trMatrix*);


/*--------------------------------------- ClassifyMatrix ------------------------------------*/

trMatrix *ClassifyMatrix(trMatrix *prMatrix)
{
	float prec = LIN_TOL / 2.0f;
	float det;

	prMatrix->flags = 0;

	if ((fabs(prMatrix->el[0][3]) >= prec) || (fabs(prMatrix->el[1][3]) >= prec) || (fabs(prMatrix->el[2][3]) >= prec))
		prMatrix->flags |= XFORM_TRANS;

	// 1.0f +- 3 * accy ~= (1.0f +- accy) ** 3, so use 3 * accy...
	prec = 3.0f * LIN_TOL;
	det = MatrixDeterminant(prMatrix);
	if (!EQUAL_X(det, 1.0f, prec))
		prMatrix->flags |= XFORM_SCALE;

#if 0
	// 1.0f +- 2 * accy ~= (1.0f +- accy) ** 2, so use 2 * accy...
	prec = 2.0f * LIN_TOL;

	if (!EQUAL_X(prMatrix->el[0][0] * prMatrix->el[0][0] + prMatrix->el[0][1] * prMatrix->el[0][1] + prMatrix->el[0][2] * prMatrix->el[0][2], 1.0f, prec))
		prMatrix->flags |= XFORM_SCALE;
	else if (!EQUAL_X(prMatrix->el[1][0] * prMatrix->el[1][0] + prMatrix->el[1][1] * prMatrix->el[1][1] + prMatrix->el[1][2] * prMatrix->el[1][2], 1.0f, prec))
		prMatrix->flags |= XFORM_SCALE;
	else if (!EQUAL_X(prMatrix->el[2][0] * prMatrix->el[2][0] + prMatrix->el[2][1] * prMatrix->el[2][1] + prMatrix->el[2][2] * prMatrix->el[2][2], 1.0f, prec))
		prMatrix->flags |= XFORM_SCALE;
#endif

	prec = LIN_TOL;

	if ((prMatrix->el[0][0] < 1.0f - prec) || (fabs(prMatrix->el[0][1]) >= prec) || (fabs(prMatrix->el[0][2]) >= prec))
		prMatrix->flags |= XFORM_ROT;
	else if ((prMatrix->el[1][1] < 1.0f - prec) || (fabs(prMatrix->el[1][0]) >= prec) || (fabs(prMatrix->el[1][2]) >= prec))
		prMatrix->flags |= XFORM_ROT;
	else if ((prMatrix->el[2][2] < 1.0f - prec) || (fabs(prMatrix->el[2][0]) >= prec) || (fabs(prMatrix->el[2][1]) >= prec))
		prMatrix->flags |= XFORM_ROT;

	return prMatrix;
}

/*--------------------------------------- NormalizeMatrix ------------------------------------------*/

trMatrix *NormalizeMatrix(trMatrix *prMatrix)
{
	const short iTimesLimit = 1;
	const short iPowerLimit = 3;
	static float coef[10] = {1.0f, -1/2., 3/8., -5/16., 35/128., -63/256., 231/1024., -429/2048., 6435/32768., -12155/65536.};
	short iTime, iPower;
	trVertex3D rScale;
	trMatrix rMatrix, rTempMatrix;
	trMatrix X, X_power, rSumMatrix;

	/* don't pass scaling matrices in here! */
	/* for algorithm and original code, see graphics gems I, pp 464 & 765 */

	rMatrix = *prMatrix;
	rMatrix.flags = XFORM_ROT;			// do math with 3 x 3, no shortcuts
	rMatrix.el[0][3] = 0.0f;
	rMatrix.el[1][3] = 0.0f;
	rMatrix.el[2][3] = 0.0f;

	for (iTime=0; iTime<iTimesLimit; iTime++)
	{
		rTempMatrix = rMatrix;
		TransposeMatrix(&rTempMatrix);
		ConcatMatrices(&rTempMatrix, &rMatrix);
		X = rTempMatrix;
		SubtractMatrix(&X, GetIdentityMatrix(&rTempMatrix));
		GetIdentityMatrix(&X_power);
		X_power.flags = XFORM_ROT;
		GetIdentityMatrix(&rSumMatrix);
		rSumMatrix.flags = XFORM_ROT;

		for (iPower=1; iPower<=iPowerLimit; iPower++)
		{
			ConcatMatrices(&X_power, &X);
			rTempMatrix = X_power;
			rScale.x = rScale.y = rScale.z = coef[iPower];
			ScaleMatrix(&rTempMatrix, &rScale);
			AddMatrix(&rSumMatrix, &rTempMatrix);
		}

		ConcatMatrices(&rMatrix, &rSumMatrix);
	}

	rMatrix.el[0][3] = prMatrix->el[0][3];
	rMatrix.el[1][3] = prMatrix->el[1][3];
	rMatrix.el[2][3] = prMatrix->el[2][3];
	*prMatrix = rMatrix;
	return ClassifyMatrix(prMatrix);
}

/*--------------------------------------- GetIdentityMatrix ------------------------------------*/

trMatrix *GetIdentityMatrix(trMatrix *prMatrix)
{
	prMatrix->flags = 0;
	prMatrix->el[0][0] = 1.0f;
	prMatrix->el[1][0] = 0.0f;
	prMatrix->el[2][0] = 0.0f;
	prMatrix->el[3][0] = 0.0f;
	prMatrix->el[0][1] = 0.0f;
	prMatrix->el[1][1] = 1.0f;
	prMatrix->el[2][1] = 0.0f;
	prMatrix->el[3][1] = 0.0f;
	prMatrix->el[0][2] = 0.0f;
	prMatrix->el[1][2] = 0.0f;
	prMatrix->el[2][2] = 1.0f;
	prMatrix->el[3][2] = 0.0f;
	prMatrix->el[0][3] = 0.0f;
	prMatrix->el[1][3] = 0.0f;
	prMatrix->el[2][3] = 0.0f;
	prMatrix->el[3][3] = 1.0f;
	return prMatrix;
}

/*--------------------------------------- MatrixPlanar ------------------------------------------*/

bool MatrixPlanar(const trMatrix *prMatrix, bool *pbRevArcs)
{
	trVertex3D rNorm;

	if ((~prMatrix->flags & XFORM_ROT) && (~prMatrix->flags & XFORM_SCALE))
	{
		if (pbRevArcs) *pbRevArcs = false;
		return true;
	}

	rNorm.x = rNorm.y = 0.0f;
	rNorm.z = 1.0f;
	TransformNorm(&rNorm, prMatrix);
	if (prMatrix->flags & XFORM_SCALE)
		set_mag(rNorm, 1.0f);

	if (dot_2d(rNorm, rNorm) > LIN_TOL * LIN_TOL)
		return false;

	if (pbRevArcs)
	{
		*pbRevArcs = (rNorm.z < 0.0f);
		if (prMatrix->flags & XFORM_SCALE)
		{
			// could be left-handed, arcs would be reversed (again)...
			if (MatrixDeterminant(prMatrix) < 0.0f)
				*pbRevArcs = !*pbRevArcs;
		}
	}
	return true;
}

/*--------------------------------------- MatrixPlane ------------------------------------------*/

tePlanes MatrixPlane(const trMatrix *prMatrix)
{
	if (~prMatrix->flags & XFORM_ROT)
		return eXY;
	if ((fabs(prMatrix->el[2][0]) < LIN_TOL) && (fabs(prMatrix->el[2][1]) < LIN_TOL) && (fabs(prMatrix->el[0][2]) < LIN_TOL) && (fabs(prMatrix->el[1][2]) < LIN_TOL))
		return eXY;
	if ((fabs(prMatrix->el[1][0]) < LIN_TOL) && (fabs(prMatrix->el[1][1]) < LIN_TOL) && (fabs(prMatrix->el[0][2]) < LIN_TOL) && (fabs(prMatrix->el[2][2]) < LIN_TOL))
		return eXZ;
	if ((fabs(prMatrix->el[0][0]) < LIN_TOL) && (fabs(prMatrix->el[0][1]) < LIN_TOL) && (fabs(prMatrix->el[1][2]) < LIN_TOL) && (fabs(prMatrix->el[2][2]) < LIN_TOL))
		return eYZ;
	return eNonPrimary;
}

/*--------------------------------------- MatrixScaleNonUniform ------------------------------------------*/

bool MatrixScaleNonUniform(const trMatrix *prMatrix)
{
	float xScale, xTemp;

	if (~prMatrix->flags & XFORM_SCALE)
		return false;

	xScale = prMatrix->el[0][0] * prMatrix->el[0][0] + prMatrix->el[0][1] * prMatrix->el[0][1] + prMatrix->el[0][2] * prMatrix->el[0][2];
	xTemp = prMatrix->el[1][0] * prMatrix->el[1][0] + prMatrix->el[1][1] * prMatrix->el[1][1] + prMatrix->el[1][2] * prMatrix->el[1][2];
	if (!EQUAL_X(xTemp, xScale, LIN_TOL))
		return true;
	xTemp = prMatrix->el[2][0] * prMatrix->el[2][0] + prMatrix->el[2][1] * prMatrix->el[2][1] + prMatrix->el[2][2] * prMatrix->el[2][2];
	return !EQUAL_X(xTemp, xScale, LIN_TOL);
}

/*--------------------------------------- MatrixScaleNonUniform2d ------------------------------------------*/

bool MatrixScaleNonUniform2d(const trMatrix *prMatrix)
{
	float xScale, xTemp;

	if (~prMatrix->flags & XFORM_SCALE)
		return false;

	xScale = prMatrix->el[0][0] * prMatrix->el[0][0] + prMatrix->el[0][1] * prMatrix->el[0][1];
	xTemp = prMatrix->el[1][0] * prMatrix->el[1][0] + prMatrix->el[1][1] * prMatrix->el[1][1];
	return !EQUAL_X(xTemp, xScale, LIN_TOL);
}

/*--------------------------------------- MatrixScale ------------------------------------------*/

trVertex3D *MatrixScale(const trMatrix *prMatrix, trVertex3D *prScale)
{
	float xTemp;

	xTemp = prMatrix->el[0][0] * prMatrix->el[0][0] + prMatrix->el[1][0] * prMatrix->el[1][0] + prMatrix->el[2][0] * prMatrix->el[2][0];
	prScale->x = (float)sqrt(xTemp);
	xTemp = prMatrix->el[0][1] * prMatrix->el[0][1] + prMatrix->el[1][1] * prMatrix->el[1][1] + prMatrix->el[2][1] * prMatrix->el[2][1];
	prScale->y = (float)sqrt(xTemp);
	xTemp = prMatrix->el[0][2] * prMatrix->el[0][2] + prMatrix->el[1][2] * prMatrix->el[1][2] + prMatrix->el[2][2] * prMatrix->el[2][2];
	prScale->z = (float)sqrt(xTemp);

	// if determinant is negative, negate all three scale values...
	if (MatrixDeterminant(prMatrix) < 0.0f)
		*prScale *= - 1.0f;

	return prScale;
}

/*--------------------------------------- MatrixDeterminant ------------------------------------------*/

static float MatrixDeterminant(const trMatrix *prMatrix)
{
	float xTemp;

	xTemp =  prMatrix->el[0][0] * (prMatrix->el[1][1] * prMatrix->el[2][2] - prMatrix->el[2][1] * prMatrix->el[1][2]);
	xTemp -= prMatrix->el[0][1] * (prMatrix->el[1][0] * prMatrix->el[2][2] - prMatrix->el[2][0] * prMatrix->el[1][2]);
	xTemp += prMatrix->el[0][2] * (prMatrix->el[1][0] * prMatrix->el[2][1] - prMatrix->el[2][0] * prMatrix->el[1][1]);

	return xTemp;
}

/*--------------------------------------- EqualMatrix ------------------------------------------*/

bool EqualMatrix(const trMatrix *prMatrix1, const trMatrix *prMatrix2)
{
	if (prMatrix1->flags != prMatrix2->flags)
		return false;

	if (prMatrix1->flags & XFORM_ROT)
	{
		if (!EQUAL_X(prMatrix1->el[0][0], prMatrix2->el[0][0], LIN_TOL)) return false;
		if (!EQUAL_X(prMatrix1->el[0][1], prMatrix2->el[0][1], LIN_TOL)) return false;
		if (!EQUAL_X(prMatrix1->el[0][2], prMatrix2->el[0][2], LIN_TOL)) return false;
		if (!EQUAL_X(prMatrix1->el[1][0], prMatrix2->el[1][0], LIN_TOL)) return false;
		if (!EQUAL_X(prMatrix1->el[1][1], prMatrix2->el[1][1], LIN_TOL)) return false;
		if (!EQUAL_X(prMatrix1->el[1][2], prMatrix2->el[1][2], LIN_TOL)) return false;
		if (!EQUAL_X(prMatrix1->el[2][0], prMatrix2->el[2][0], LIN_TOL)) return false;
		if (!EQUAL_X(prMatrix1->el[2][1], prMatrix2->el[2][1], LIN_TOL)) return false;
		if (!EQUAL_X(prMatrix1->el[2][2], prMatrix2->el[2][2], LIN_TOL)) return false;
	}
	else
	{
		if (prMatrix1->flags & XFORM_SCALE)
		{
			if (!EQUAL_X(prMatrix1->el[0][0], prMatrix2->el[0][0], LIN_TOL)) return false;
			if (!EQUAL_X(prMatrix1->el[1][1], prMatrix2->el[1][1], LIN_TOL)) return false;
			if (!EQUAL_X(prMatrix1->el[2][2], prMatrix2->el[2][2], LIN_TOL)) return false;
		}
	}
	if (prMatrix1->flags & XFORM_TRANS)
	{
		if (!EQUAL_X(prMatrix1->el[0][3], prMatrix2->el[0][3], LIN_TOL)) return false;
		if (!EQUAL_X(prMatrix1->el[1][3], prMatrix2->el[1][3], LIN_TOL)) return false;
		if (!EQUAL_X(prMatrix1->el[2][3], prMatrix2->el[2][3], LIN_TOL)) return false;
	}
	return true;
}

/*--------------------------------------- SamePlaneMatrix ------------------------------------------*/

bool SamePlaneMatrix(const trMatrix *prMatrix1, const trMatrix *prMatrix2, bool *pbReversed)
{
	trVertex3D rNorm1 = {0.0f, 0.0f, 1.0f};
	trVertex3D rNorm2 = {0.0f, 0.0f, 1.0f};
	trVertex3D rNorm3;

	/* returns whether the two matrices have the same planar orientation... */

	TransformNorm(&rNorm1, prMatrix1);
	TransformNorm(&rNorm2, prMatrix2);
	if (pbReversed)
		*pbReversed = (dot(rNorm1, rNorm2) < 0.0f);
	cross(rNorm1, rNorm2, rNorm3);
	return (dot(rNorm3, rNorm3) < LIN_TOL * LIN_TOL);
}

/*--------------------------------------- TranslateMatrix ------------------------------------------*/

trMatrix *TranslateMatrix(trMatrix *prMatrix, const trVertex3D *prVector)
{
	if ((fabs(prVector->x) > 0.00000001f) || (fabs(prVector->y) > 0.00000001f) || (fabs(prVector->z) > 0.00000001f))
	{
		if (prMatrix->flags & XFORM_TRANS)
		{
			prMatrix->el[0][3] += prVector->x;		
			prMatrix->el[1][3] += prVector->y;		
			prMatrix->el[2][3] += prVector->z;	
		}
		else
		{
			prMatrix->el[0][3] = prVector->x;		
			prMatrix->el[1][3] = prVector->y;		
			prMatrix->el[2][3] = prVector->z;	
		}
		prMatrix->flags |= XFORM_TRANS;
	}

	return prMatrix;
}

/*--------------------------------------- TranslationMatrix ------------------------------------------*/

trMatrix *TranslationMatrix(trMatrix *prMatrix, const trVertex3D *prVector)
{
	GetIdentityMatrix(prMatrix);

	if ((fabs(prVector->x) > 0.00000001f) || (fabs(prVector->y) > 0.00000001f) || (fabs(prVector->z) > 0.00000001f))
	{
		prMatrix->flags = XFORM_TRANS;
		prMatrix->el[0][3] = prVector->x;		
		prMatrix->el[1][3] = prVector->y;		
		prMatrix->el[2][3] = prVector->z;	
	}

	return prMatrix;
}

/*--------------------------------------- ScaleMatrix ------------------------------------------*/

trMatrix *ScaleMatrix(trMatrix *prMatrix, const trVertex3D *prScale)
{
	if (prMatrix->flags & XFORM_ROT)
	{
		prMatrix->el[0][0] *= prScale->x;
		prMatrix->el[0][1] *= prScale->x;
		prMatrix->el[0][2] *= prScale->x;
		prMatrix->el[1][0] *= prScale->y;
		prMatrix->el[1][1] *= prScale->y;
		prMatrix->el[1][2] *= prScale->y;
		prMatrix->el[2][0] *= prScale->z;
		prMatrix->el[2][1] *= prScale->z;
		prMatrix->el[2][2] *= prScale->z;
	}
	else
	{
		if (prMatrix->flags & XFORM_SCALE)
		{
			prMatrix->el[0][0] *= prScale->x;
			prMatrix->el[1][1] *= prScale->y;
			prMatrix->el[2][2] *= prScale->z;
		}
		else
		{
			prMatrix->el[0][0] = prScale->x;
			prMatrix->el[1][1] = prScale->y;
			prMatrix->el[2][2] = prScale->z;
		}
	}
	if (prMatrix->flags & XFORM_TRANS)
	{
		prMatrix->el[0][3] *= prScale->x;
		prMatrix->el[1][3] *= prScale->y;
		prMatrix->el[2][3] *= prScale->z;
	}
	prMatrix->flags |= XFORM_SCALE;
	return prMatrix;
}

/*--------------------------------------- ScalingMatrix ------------------------------------------*/

trMatrix *ScalingMatrix(trMatrix *prMatrix, const trVertex3D *prScale)
{
	GetIdentityMatrix(prMatrix);
	prMatrix->flags = XFORM_SCALE;
	prMatrix->el[0][0] = prScale->x;
	prMatrix->el[1][1] = prScale->y;
	prMatrix->el[2][2] = prScale->z;
	return prMatrix;
}

/*--------------------------------------- RotationMatrix ------------------------------------------*/

trMatrix *RotationMatrix(trMatrix *prMatrix, const trVertex3D *prAxis, float s, float c)
{
	float xTemp, t;

	/* *prAxis is a unit vector, s and c are sine and cosine of angle, respectively... */

	GetIdentityMatrix(prMatrix);

	if ((fabs(s) > 0.0000000001f) || (c < 0.0f))
	{
		t = 1.0f - c;
		prMatrix->flags = XFORM_ROT;
		prMatrix->el[0][0] = t * prAxis->x * prAxis->x + c;
		prMatrix->el[1][1] = t * prAxis->y * prAxis->y + c;
		prMatrix->el[2][2] = t * prAxis->z * prAxis->z + c;
		prMatrix->el[0][1] = prMatrix->el[1][0] = t * prAxis->x * prAxis->y;
		xTemp = s * prAxis->z;
		prMatrix->el[0][1] -= xTemp;
		prMatrix->el[1][0] += xTemp;
		prMatrix->el[0][2] = prMatrix->el[2][0] = t * prAxis->x * prAxis->z;
		xTemp = s * prAxis->y;
		prMatrix->el[0][2] += xTemp;
		prMatrix->el[2][0] -= xTemp;
		prMatrix->el[1][2] = prMatrix->el[2][1] = t * prAxis->y * prAxis->z;
		xTemp = s * prAxis->x;
		prMatrix->el[1][2] -= xTemp;
		prMatrix->el[2][1] += xTemp;
	}

	return prMatrix;
}

/*--------------------------------------- RotationMatrix ------------------------------------------*/

trMatrix *RotationMatrix(trMatrix *prMatrix, float s, float c)
{
	/* s and c are sine and cosine of angle, respectively... */

	GetIdentityMatrix(prMatrix);

	if ((fabs(s) > 0.000000001f) || (c < 0.0f))
	{
		prMatrix->flags = XFORM_ROT;
		prMatrix->el[0][0] = c;
		prMatrix->el[1][1] = c;
		prMatrix->el[0][1] = - s;
		prMatrix->el[1][0] = s;
	}

	return prMatrix;
}

/*--------------------------------------- Rotate2dMatrix ------------------------------------------*/

trMatrix *Rotate2dMatrix(trMatrix *prMatrix, float xAngle, const trVertex3D *prOrigin)
{
	trVertex3D rOrigin;
	trMatrix	rTransMatrix;

	xAngle *= rad_con;					// Convert to Radians
	rOrigin.x = - prOrigin->x;
	rOrigin.y = - prOrigin->y;
	rOrigin.z = 0.0f;

	TranslationMatrix(&rTransMatrix, &rOrigin);
	RotationMatrix(prMatrix, (float)sin(xAngle), (float)cos(xAngle));
	ConcatMatrices(prMatrix, &rTransMatrix);
	rOrigin.x = - rOrigin.x;	rOrigin.y = - rOrigin.y;
	TranslateMatrix(prMatrix, &rOrigin);
	return prMatrix;
}

/*--------------------------------------- MirrorMatrix ------------------------------------------*/

trMatrix *MirrorMatrix(trMatrix *prMatrix, const trVertex3D *prAxisPol, const trVertex3D *prMirrorPoint)
{
	GetIdentityMatrix(prMatrix);
	prMatrix->flags = XFORM_ROT | XFORM_TRANS;
	prMatrix->el[0][0] = prAxisPol->x;
	prMatrix->el[1][1] = prAxisPol->y;
	prMatrix->el[2][2] = prAxisPol->z;
	prMatrix->el[0][3] = (1.0f - prAxisPol->x) * prMirrorPoint->x;
	prMatrix->el[1][3] = (1.0f - prAxisPol->y) * prMirrorPoint->y;
	prMatrix->el[2][3] = (1.0f - prAxisPol->z) * prMirrorPoint->z;
	return prMatrix;
}

/*--------------------------------------- ForceDepthMatrix ------------------------------------------*/

trMatrix *ForceDepthMatrix(trMatrix *prMatrix, float xDepth)
{
	GetIdentityMatrix(prMatrix);
	prMatrix->flags = XFORM_ROT | XFORM_TRANS;
	prMatrix->el[2][2] = 0.0f;
	prMatrix->el[2][3] = xDepth;
	return prMatrix;
}

/*--------------------------------------- ConcatMatrices ------------------------------------------*/

trMatrix *ConcatMatrices(trMatrix *prIn2Matrix, const trMatrix *prIn1Matrix)
{
	trMatrix rOutMatrix;
	trMatrix *prOutMatrix = &rOutMatrix;

	/* output goes to prIn2Matrix... */

	if (prIn1Matrix->flags == 0)
		return prIn2Matrix;

	if (prIn2Matrix->flags == 0)
	{
		*prIn2Matrix = *prIn1Matrix;
		return prIn2Matrix;
	}

	prOutMatrix->flags = prIn2Matrix->flags | prIn1Matrix->flags;

	if (prIn2Matrix->flags & XFORM_ROT)
	{
		if (prIn1Matrix->flags & XFORM_ROT)
		{
			/* both rotated... */
			prOutMatrix->el[0][0] = prIn2Matrix->el[0][0] * prIn1Matrix->el[0][0] + prIn2Matrix->el[0][1] * prIn1Matrix->el[1][0] + prIn2Matrix->el[0][2] * prIn1Matrix->el[2][0];
			prOutMatrix->el[1][0] = prIn2Matrix->el[1][0] * prIn1Matrix->el[0][0] + prIn2Matrix->el[1][1] * prIn1Matrix->el[1][0] + prIn2Matrix->el[1][2] * prIn1Matrix->el[2][0];
			prOutMatrix->el[2][0] = prIn2Matrix->el[2][0] * prIn1Matrix->el[0][0] + prIn2Matrix->el[2][1] * prIn1Matrix->el[1][0] + prIn2Matrix->el[2][2] * prIn1Matrix->el[2][0];
			prOutMatrix->el[0][1] = prIn2Matrix->el[0][0] * prIn1Matrix->el[0][1] + prIn2Matrix->el[0][1] * prIn1Matrix->el[1][1] + prIn2Matrix->el[0][2] * prIn1Matrix->el[2][1];
			prOutMatrix->el[1][1] = prIn2Matrix->el[1][0] * prIn1Matrix->el[0][1] + prIn2Matrix->el[1][1] * prIn1Matrix->el[1][1] + prIn2Matrix->el[1][2] * prIn1Matrix->el[2][1];
			prOutMatrix->el[2][1] = prIn2Matrix->el[2][0] * prIn1Matrix->el[0][1] + prIn2Matrix->el[2][1] * prIn1Matrix->el[1][1] + prIn2Matrix->el[2][2] * prIn1Matrix->el[2][1];
			prOutMatrix->el[0][2] = prIn2Matrix->el[0][0] * prIn1Matrix->el[0][2] + prIn2Matrix->el[0][1] * prIn1Matrix->el[1][2] + prIn2Matrix->el[0][2] * prIn1Matrix->el[2][2];
			prOutMatrix->el[1][2] = prIn2Matrix->el[1][0] * prIn1Matrix->el[0][2] + prIn2Matrix->el[1][1] * prIn1Matrix->el[1][2] + prIn2Matrix->el[1][2] * prIn1Matrix->el[2][2];
			prOutMatrix->el[2][2] = prIn2Matrix->el[2][0] * prIn1Matrix->el[0][2] + prIn2Matrix->el[2][1] * prIn1Matrix->el[1][2] + prIn2Matrix->el[2][2] * prIn1Matrix->el[2][2];
		}
		else
		{
			if (prIn1Matrix->flags & XFORM_SCALE)
			{
				/* second rotated, first scaled only... */
				prOutMatrix->el[0][0] = prIn2Matrix->el[0][0] * prIn1Matrix->el[0][0];
				prOutMatrix->el[1][0] = prIn2Matrix->el[1][0] * prIn1Matrix->el[0][0];
				prOutMatrix->el[2][0] = prIn2Matrix->el[2][0] * prIn1Matrix->el[0][0];
				prOutMatrix->el[0][1] = prIn2Matrix->el[0][1] * prIn1Matrix->el[1][1];
				prOutMatrix->el[1][1] = prIn2Matrix->el[1][1] * prIn1Matrix->el[1][1];
				prOutMatrix->el[2][1] = prIn2Matrix->el[2][1] * prIn1Matrix->el[1][1];
				prOutMatrix->el[0][2] = prIn2Matrix->el[0][2] * prIn1Matrix->el[2][2];
				prOutMatrix->el[1][2] = prIn2Matrix->el[1][2] * prIn1Matrix->el[2][2];
				prOutMatrix->el[2][2] = prIn2Matrix->el[2][2] * prIn1Matrix->el[2][2];
			}
			else
			{
				/* second rotated, first not scaled or rotated... */
				prOutMatrix->el[0][0] = prIn2Matrix->el[0][0];
				prOutMatrix->el[1][0] = prIn2Matrix->el[1][0];
				prOutMatrix->el[2][0] = prIn2Matrix->el[2][0];
				prOutMatrix->el[0][1] = prIn2Matrix->el[0][1];
				prOutMatrix->el[1][1] = prIn2Matrix->el[1][1];
				prOutMatrix->el[2][1] = prIn2Matrix->el[2][1];
				prOutMatrix->el[0][2] = prIn2Matrix->el[0][2];
				prOutMatrix->el[1][2] = prIn2Matrix->el[1][2];
				prOutMatrix->el[2][2] = prIn2Matrix->el[2][2];
			}
		}
	}
	else
	{
		if (prIn2Matrix->flags & XFORM_SCALE)
		{
			if (prIn1Matrix->flags & XFORM_ROT)
			{
				/* second scaled only, first rotated... */
				prOutMatrix->el[0][0] = prIn2Matrix->el[0][0] * prIn1Matrix->el[0][0];
				prOutMatrix->el[1][0] = prIn2Matrix->el[1][1] * prIn1Matrix->el[1][0];
				prOutMatrix->el[2][0] = prIn2Matrix->el[2][2] * prIn1Matrix->el[2][0];
				prOutMatrix->el[0][1] = prIn2Matrix->el[0][0] * prIn1Matrix->el[0][1];
				prOutMatrix->el[1][1] = prIn2Matrix->el[1][1] * prIn1Matrix->el[1][1];
				prOutMatrix->el[2][1] = prIn2Matrix->el[2][2] * prIn1Matrix->el[2][1];
				prOutMatrix->el[0][2] = prIn2Matrix->el[0][0] * prIn1Matrix->el[0][2];
				prOutMatrix->el[1][2] = prIn2Matrix->el[1][1] * prIn1Matrix->el[1][2];
				prOutMatrix->el[2][2] = prIn2Matrix->el[2][2] * prIn1Matrix->el[2][2];
			}
			else
			{
				if (prIn1Matrix->flags & XFORM_SCALE)
				{
					/* second scaled only, first scaled only... */
					prOutMatrix->el[0][0] = prIn2Matrix->el[0][0] * prIn1Matrix->el[0][0];
					prOutMatrix->el[1][1] = prIn2Matrix->el[1][1] * prIn1Matrix->el[1][1];
					prOutMatrix->el[2][2] = prIn2Matrix->el[2][2] * prIn1Matrix->el[2][2];
					prOutMatrix->el[1][0] = 0.0f;
					prOutMatrix->el[2][0] = 0.0f;
					prOutMatrix->el[0][1] = 0.0f;
					prOutMatrix->el[2][1] = 0.0f;
					prOutMatrix->el[0][2] = 0.0f;
					prOutMatrix->el[1][2] = 0.0f;
				}
				else
				{
					/* second scaled only, first not scaled or rotated... */
					prOutMatrix->el[0][0] = prIn2Matrix->el[0][0];
					prOutMatrix->el[1][1] = prIn2Matrix->el[1][1];
					prOutMatrix->el[2][2] = prIn2Matrix->el[2][2];
					prOutMatrix->el[1][0] = 0.0f;
					prOutMatrix->el[2][0] = 0.0f;
					prOutMatrix->el[0][1] = 0.0f;
					prOutMatrix->el[2][1] = 0.0f;
					prOutMatrix->el[0][2] = 0.0f;
					prOutMatrix->el[1][2] = 0.0f;
				}
			}
		}
		else
		{
			/* second not rotated or scaled... */
			prOutMatrix->el[0][0] = prIn1Matrix->el[0][0];
			prOutMatrix->el[1][0] = prIn1Matrix->el[1][0];
			prOutMatrix->el[2][0] = prIn1Matrix->el[2][0];
			prOutMatrix->el[0][1] = prIn1Matrix->el[0][1];
			prOutMatrix->el[1][1] = prIn1Matrix->el[1][1];
			prOutMatrix->el[2][1] = prIn1Matrix->el[2][1];
			prOutMatrix->el[0][2] = prIn1Matrix->el[0][2];
			prOutMatrix->el[1][2] = prIn1Matrix->el[1][2];
			prOutMatrix->el[2][2] = prIn1Matrix->el[2][2];
		}
	}

	if (prIn1Matrix->flags & XFORM_TRANS)
	{
		if (prIn2Matrix->flags & XFORM_ROT)
		{
			prOutMatrix->el[0][3] = prIn2Matrix->el[0][0] * prIn1Matrix->el[0][3] + prIn2Matrix->el[0][1] * prIn1Matrix->el[1][3] + prIn2Matrix->el[0][2] * prIn1Matrix->el[2][3] + prIn2Matrix->el[0][3];
			prOutMatrix->el[1][3] = prIn2Matrix->el[1][0] * prIn1Matrix->el[0][3] + prIn2Matrix->el[1][1] * prIn1Matrix->el[1][3] + prIn2Matrix->el[1][2] * prIn1Matrix->el[2][3] + prIn2Matrix->el[1][3];
			prOutMatrix->el[2][3] = prIn2Matrix->el[2][0] * prIn1Matrix->el[0][3] + prIn2Matrix->el[2][1] * prIn1Matrix->el[1][3] + prIn2Matrix->el[2][2] * prIn1Matrix->el[2][3] + prIn2Matrix->el[2][3];
		}
		else
		{
			if (prIn2Matrix->flags & XFORM_SCALE)
			{
				prOutMatrix->el[0][3] = prIn2Matrix->el[0][0] * prIn1Matrix->el[0][3] + prIn2Matrix->el[0][3];
				prOutMatrix->el[1][3] = prIn2Matrix->el[1][1] * prIn1Matrix->el[1][3] + prIn2Matrix->el[1][3];
				prOutMatrix->el[2][3] = prIn2Matrix->el[2][2] * prIn1Matrix->el[2][3] + prIn2Matrix->el[2][3];
			}
			else
			{
				prOutMatrix->el[0][3] = prIn1Matrix->el[0][3] + prIn2Matrix->el[0][3];
				prOutMatrix->el[1][3] = prIn1Matrix->el[1][3] + prIn2Matrix->el[1][3];
				prOutMatrix->el[2][3] = prIn1Matrix->el[2][3] + prIn2Matrix->el[2][3];
			}
		}
	}
	else
	{
		prOutMatrix->el[0][3] = prIn2Matrix->el[0][3];
		prOutMatrix->el[1][3] = prIn2Matrix->el[1][3];
		prOutMatrix->el[2][3] = prIn2Matrix->el[2][3];
	}

	*prIn2Matrix = *prOutMatrix;
	return prIn2Matrix;
}

/*--------------------------------------- InvertMatrix ------------------------------------------*/

trMatrix *InvertMatrix(trMatrix *prMatrix)
{
	if (prMatrix->flags == 0)
		return prMatrix;

	if (prMatrix->flags & XFORM_ROT)
	{
		if (prMatrix->flags & XFORM_SCALE)
			Invert3x3(prMatrix);	/* use cramer's rule... */
		else
			TransposeMatrix(prMatrix);

		if (prMatrix->flags & XFORM_TRANS)
		{
			float xtrans = prMatrix->el[0][3];
			float ytrans = prMatrix->el[1][3];
			prMatrix->el[0][3] = - prMatrix->el[0][0] * xtrans - prMatrix->el[0][1] * ytrans - prMatrix->el[0][2] * prMatrix->el[2][3];
			prMatrix->el[1][3] = - prMatrix->el[1][0] * xtrans - prMatrix->el[1][1] * ytrans - prMatrix->el[1][2] * prMatrix->el[2][3];
			prMatrix->el[2][3] = - prMatrix->el[2][0] * xtrans - prMatrix->el[2][1] * ytrans - prMatrix->el[2][2] * prMatrix->el[2][3];
		}
	}
	else
	{
		if (prMatrix->flags & XFORM_SCALE)
		{
			/* invert the scale... */
			prMatrix->el[0][0] = 1.0f / prMatrix->el[0][0];
			prMatrix->el[1][1] = 1.0f / prMatrix->el[1][1];
			prMatrix->el[2][2] = 1.0f / prMatrix->el[2][2];

			if (prMatrix->flags & XFORM_TRANS)
			{
				prMatrix->el[0][3] = - prMatrix->el[0][0] * prMatrix->el[0][3];
				prMatrix->el[1][3] = - prMatrix->el[1][1] * prMatrix->el[1][3];
				prMatrix->el[2][3] = - prMatrix->el[2][2] * prMatrix->el[2][3];
			}
		}
		else
		{
			if (prMatrix->flags & XFORM_TRANS)
			{
				prMatrix->el[0][3] = - prMatrix->el[0][3];
				prMatrix->el[1][3] = - prMatrix->el[1][3];
				prMatrix->el[2][3] = - prMatrix->el[2][3];
			}
		}
	}

	return prMatrix;
}

/*--------------------------------------- Invert3x3 ------------------------------------------*/

static void Invert3x3(trMatrix *prMatrix)
{
	float one_over_det;
	trMatrix rTempMatrix;
	trMatrix *prTempMatrix = &rTempMatrix;

	/* use cramer's rule to invert a 3 by 3 matrix... */

	prTempMatrix->el[0][0] = prMatrix->el[1][1] * prMatrix->el[2][2] - prMatrix->el[2][1] * prMatrix->el[1][2];
	prTempMatrix->el[1][0] = prMatrix->el[0][1] * prMatrix->el[2][2] - prMatrix->el[2][1] * prMatrix->el[0][2];
	prTempMatrix->el[2][0] = prMatrix->el[0][1] * prMatrix->el[1][2] - prMatrix->el[1][1] * prMatrix->el[0][2];
	prTempMatrix->el[0][1] = prMatrix->el[1][0] * prMatrix->el[2][2] - prMatrix->el[2][0] * prMatrix->el[1][2];
	prTempMatrix->el[1][1] = prMatrix->el[0][0] * prMatrix->el[2][2] - prMatrix->el[2][0] * prMatrix->el[0][2];
	prTempMatrix->el[2][1] = prMatrix->el[0][0] * prMatrix->el[1][2] - prMatrix->el[1][0] * prMatrix->el[0][2];
	prTempMatrix->el[0][2] = prMatrix->el[1][0] * prMatrix->el[2][1] - prMatrix->el[2][0] * prMatrix->el[1][1];
	prTempMatrix->el[1][2] = prMatrix->el[0][0] * prMatrix->el[2][1] - prMatrix->el[2][0] * prMatrix->el[0][1];
	prTempMatrix->el[2][2] = prMatrix->el[0][0] * prMatrix->el[1][1] - prMatrix->el[1][0] * prMatrix->el[0][1];

	one_over_det = prMatrix->el[0][0] * prTempMatrix->el[0][0] - prMatrix->el[0][1] * prTempMatrix->el[0][1] + prMatrix->el[0][2] * prTempMatrix->el[0][2];
	one_over_det = 1.0f / one_over_det;

	prMatrix->el[0][0] = prTempMatrix->el[0][0] * one_over_det;
	prMatrix->el[1][0] = - prTempMatrix->el[0][1] * one_over_det;
	prMatrix->el[2][0] = prTempMatrix->el[0][2] * one_over_det;
	prMatrix->el[0][1] = - prTempMatrix->el[1][0] * one_over_det;
	prMatrix->el[1][1] = prTempMatrix->el[1][1] * one_over_det;
	prMatrix->el[2][1] = - prTempMatrix->el[1][2] * one_over_det;
	prMatrix->el[0][2] = prTempMatrix->el[2][0] * one_over_det;
	prMatrix->el[1][2] = - prTempMatrix->el[2][1] * one_over_det;
	prMatrix->el[2][2] = prTempMatrix->el[2][2] * one_over_det;
}

/*--------------------------------------- TransposeMatrix ------------------------------------------*/

static trMatrix *TransposeMatrix(trMatrix *prMatrix)
{
	float xTemp;

	/* does 3 x 3 only */

	xTemp = prMatrix->el[0][1];
	prMatrix->el[0][1] = prMatrix->el[1][0];
	prMatrix->el[1][0] = xTemp;
	xTemp = prMatrix->el[0][2];
	prMatrix->el[0][2] = prMatrix->el[2][0];
	prMatrix->el[2][0] = xTemp;
	xTemp = prMatrix->el[1][2];
	prMatrix->el[1][2] = prMatrix->el[2][1];
	prMatrix->el[2][1] = xTemp;

	return prMatrix;
}

/*--------------------------------------- AddMatrix ------------------------------------------*/

static trMatrix *AddMatrix(trMatrix *prMatrix1, const trMatrix *prMatrix2)
{
	short i, ii;

	/* adds matrix2 to matrix1... */
	/* does 3 x 3 only */

	for (i=0; i<3; i++)
		for (ii=0; ii<3; ii++)
			prMatrix1->el[i][ii] += prMatrix2->el[i][ii];
	return prMatrix1;
}

/*--------------------------------------- SubtractMatrix ------------------------------------------*/

static trMatrix *SubtractMatrix(trMatrix *prMatrix1, const trMatrix *prMatrix2)
{
	short i, ii;

	/* subtracts matrix2 from matrix1... */
	/* does 3 x 3 only */

	for (i=0; i<3; i++)
		for (ii=0; ii<3; ii++)
			prMatrix1->el[i][ii] -= prMatrix2->el[i][ii];
	return prMatrix1;
}

/*--------------------------------------- TransformPos ------------------------------------------*/

trVertex3D *TransformPos(trVertex3D *prPos, const trMatrix *prMatrix)
{
	if (prMatrix->flags == 0) return prPos;

	if (prMatrix->flags & XFORM_ROT)
	{
		float h = prPos->x;
		float v = prPos->y;
		prPos->x = prMatrix->el[0][0] * h + prMatrix->el[0][1] * v + prMatrix->el[0][2] * prPos->z;
		prPos->y = prMatrix->el[1][0] * h + prMatrix->el[1][1] * v + prMatrix->el[1][2] * prPos->z;
		prPos->z = prMatrix->el[2][0] * h + prMatrix->el[2][1] * v + prMatrix->el[2][2] * prPos->z;
	}
	else
	{
		if (prMatrix->flags & XFORM_SCALE)
		{
			prPos->x *= prMatrix->el[0][0];
			prPos->y *= prMatrix->el[1][1];
			prPos->z *= prMatrix->el[2][2];
		}
	}

	if (prMatrix->flags & XFORM_TRANS)
	{
		prPos->x += prMatrix->el[0][3];
		prPos->y += prMatrix->el[1][3];
		prPos->z += prMatrix->el[2][3];
	}

	return prPos;
}

/*--------------------------------------- TransformPos2d ------------------------------------------*/

trVertex3D *TransformPos2d(trVertex3D *prPos, const trMatrix *prMatrix)
{
	if (prMatrix->flags == 0) return prPos;

	if (prMatrix->flags & XFORM_ROT)
	{
		float h = prPos->x;
		prPos->x = prMatrix->el[0][0] * h + prMatrix->el[0][1] * prPos->y + prMatrix->el[0][2] * prPos->z;
		prPos->y = prMatrix->el[1][0] * h + prMatrix->el[1][1] * prPos->y + prMatrix->el[1][2] * prPos->z;
	}
	else
	{
		if (prMatrix->flags & XFORM_SCALE)
		{
			prPos->x *= prMatrix->el[0][0];
			prPos->y *= prMatrix->el[1][1];
		}
	}

	if (prMatrix->flags & XFORM_TRANS)
	{
		prPos->x += prMatrix->el[0][3];
		prPos->y += prMatrix->el[1][3];
	}

	return prPos;
}

/*--------------------------------------- TransformNorm ------------------------------------------*/

trVertex3D *TransformNorm(trVertex3D *prNorm, const trMatrix *prMatrix)
{
	// don't apply translation

	if (prMatrix->flags & XFORM_ROT)
	{
		float h = prNorm->x;
		float v = prNorm->y;
		prNorm->x = prMatrix->el[0][0] * h + prMatrix->el[0][1] * v + prMatrix->el[0][2] * prNorm->z;
		prNorm->y = prMatrix->el[1][0] * h + prMatrix->el[1][1] * v + prMatrix->el[1][2] * prNorm->z;
		prNorm->z = prMatrix->el[2][0] * h + prMatrix->el[2][1] * v + prMatrix->el[2][2] * prNorm->z;
	}
	else
	{
		if (prMatrix->flags & XFORM_SCALE)
		{
			prNorm->x *= prMatrix->el[0][0];
			prNorm->y *= prMatrix->el[1][1];
			prNorm->z *= prMatrix->el[2][2];
		}
	}

	return prNorm;
}

/*--------------------------------------- TransformNorm2d ------------------------------------------*/

trVertex3D *TransformNorm2d(trVertex3D *prNorm, const trMatrix *prMatrix)
{
	// don't apply translation

	if (prMatrix->flags & XFORM_ROT)
	{
		float h = prNorm->x;
		prNorm->x = prMatrix->el[0][0] * h + prMatrix->el[0][1] * prNorm->y + prMatrix->el[0][2] * prNorm->z;
		prNorm->y = prMatrix->el[1][0] * h + prMatrix->el[1][1] * prNorm->y + prMatrix->el[1][2] * prNorm->z;
	}
	else
	{
		if (prMatrix->flags & XFORM_SCALE)
		{
			prNorm->x *= prMatrix->el[0][0];
			prNorm->y *= prMatrix->el[1][1];
		}
	}

	return prNorm;
}

/*--------------------------------------- MatrixFromAxes ------------------------------------------*/

trMatrix *MatrixFromAxes(trMatrix *prMatrix, const trVertex3D *d, const trVertex3D *h, const trVertex3D *pos)
{
	float mag;
	trVertex3D n, n2;
	trMatrix rMatrix;

	n.x = - d->y;
	n.y = d->x;
	n.z = 0.0f;

	mag = get_mag_2d(n);
	if (mag < 0.000000001f)
	{
		// use an arbitrary vector normal to z...
		n.x = 1.0f;
		n.y = 0.0f;
	}
	else
	{
		// normalize vector...
		n.x /= mag;
		n.y /= mag;
	}
	RotationMatrix(prMatrix, &n, mag, d->z);									// rotate @ n till z == d

	if (h)
	{
		RotationMatrix(&rMatrix, n.y, n.x);										// rotate @ z till x == n
		ConcatMatrices(prMatrix, &rMatrix);
		rMatrix = *prMatrix;

		cross(n, *h, n2);												// normal to n and h
		RotationMatrix(prMatrix, d, dot(n2, *d), dot(n, *h));	// rotate @ d till n == h
		ConcatMatrices(prMatrix, &rMatrix);
	}

	// move matrix into position...	
	if (pos) TranslateMatrix(prMatrix, pos);

	return prMatrix;
}

/*--------------------------------------- DifferenceMatrix ------------------------------------------*/

trMatrix *DifferenceMatrix(trMatrix *prMatrix, const trVertex3D *d0, const trVertex3D *h0, const trVertex3D *pos0, const trVertex3D *d1, const trVertex3D *h1, const trVertex3D *pos1)
{
	float mag;
	trVertex3D n1, n2;
	trMatrix rMatrix;

	// three rotations and two translations needed:
	// first, translate from pos0
	// then, rotate @d0 to align h0 and (d0 X d1)
	// then, rotate @(d0 X d1) to align d0 and d1
	// then, rotate @d1 to align (d0 X d1) and h1
	// finally, translate to pos1

	if (!d0 || !d1)
		GetIdentityMatrix(&rMatrix);
	else
	{
		// 2nd rotation...
		cross(*d0, *d1, n1);
		mag = get_mag(n1);
		if (mag > 0.000000001f)
			n1 /= mag;
		else
		{
			n1.x = n1.y = n1.z = 0.0f;
			n2.x = (float)fabs(d0->x);
			n2.y = (float)fabs(d0->y);
			n2.z = (float)fabs(d0->z);

			if (n2.x < n2.y)
				if (n2.x < n2.z)
					n1.x = 1.0f;
				else
					n1.z = 1.0f;
			else
				if (n2.y < n2.z)
					n1.y = 1.0f;
				else
					n1.z = 1.0f;
		}
		RotationMatrix(&rMatrix, &n1, mag, dot(*d0, *d1));
	}

	if (!h0 || !h1)
		*prMatrix = rMatrix;
	else
	{
		// 1st rotation...
		cross(*h0, n1, n2);
		RotationMatrix(prMatrix, d0, dot(n2, *d0), dot(*h0, n1));
		ConcatMatrices(&rMatrix, prMatrix);

		// 3rd rotation...
		cross(n1, *h1, n2);
		RotationMatrix(prMatrix, d1, dot(n2, *d1), dot(n1, *h1));
		ConcatMatrices(prMatrix, &rMatrix);
	}

	if (pos0)
	{
		// 1st translation...
		n2 = - *pos0;
		TranslationMatrix(&rMatrix, &n2);
		ConcatMatrices(prMatrix, &rMatrix);
	}

	if (pos1)
	{
		// 2nd translation...
		TranslateMatrix(prMatrix, pos1);
	}

	return prMatrix;
}

// C3DVertex Class

C3DVertex::C3DVertex()
{
	m_rVertex.x = 0;
	m_rVertex.y = 0;
	m_rVertex.z = 0;
}

C3DVertex::C3DVertex(const trVertex3D &rVert)
{
	Set(rVert);
}

C3DVertex::C3DVertex(float x, float y, float z)
{
	Set(x,y,z);
}

C3DVertex::C3DVertex(float *pos)
{
	Set(pos[0],pos[1],pos[2]);
}

C3DVertex::~C3DVertex()
{

}

float C3DVertex::SetX ( float x )
{
	m_rVertex.x = x;
	return x;
}

float C3DVertex::SetY ( float y )
{
	m_rVertex.y = y;
	return y;
}

float C3DVertex::SetZ ( float z )
{
	m_rVertex.z = z;
	return z;
}

void C3DVertex::Set (const trVertex3D &rVert)
{
	m_rVertex = rVert;
}

void C3DVertex::Set (float x, float y, float z)
{
	m_rVertex.x = x;
	m_rVertex.y = y;
	m_rVertex.z = z;
}

void C3DVertex::Set (float* p)
{
	m_rVertex.x = p[0];
	m_rVertex.y = p[1];
	m_rVertex.z = p[2];
}

float C3DVertex::X (void) const
{
	return m_rVertex.x;
}

float C3DVertex::Y (void) const
{
	return m_rVertex.y;
}

float C3DVertex::Z (void) const
{
	return m_rVertex.z;
}

void C3DVertex::Get ( float* pos )
{
	pos[0] = m_rVertex.x;
	pos[1] = m_rVertex.y;
	pos[2] = m_rVertex.z;
}

// functions
float C3DVertex::Dot (const C3DVertex &p0) const
{
	return dot(m_rVertex,p0);
}

float C3DVertex::Dot2D (const C3DVertex &p0) const
{
	return dot_2d(m_rVertex,p0);
}

void C3DVertex::Cross (const C3DVertex &p0, const C3DVertex &p1)
{
	cross(p0,p1,m_rVertex);
}

float C3DVertex::Cross2D (const C3DVertex &p0) const
{
	return cross_2d(m_rVertex,p0);
}

float C3DVertex::GetMag (void) const 
{
	return get_mag(m_rVertex);
}

float C3DVertex::GetMag2D (void)const 
{
	return get_mag_2d(m_rVertex);
}

float C3DVertex::GetMagSq (void) const 
{
	return m_rVertex.x*m_rVertex.x+m_rVertex.y*m_rVertex.y+m_rVertex.z*m_rVertex.z;
}

void C3DVertex::SetMag (float scaler)
{
	set_mag(m_rVertex,scaler);
}

void C3DVertex::SetMag2D (float scaler)
{
	set_mag_2d(m_rVertex,scaler);
}

bool C3DVertex::Close (const C3DVertex &p0)
{
	return close(m_rVertex,p0);
}

bool C3DVertex::Close2D (const C3DVertex &p0)
{
	return close_2d(m_rVertex,p0);
}

// operators
C3DVertex& C3DVertex::operator = (const C3DVertex &p0)
{
	m_rVertex = p0.m_rVertex;
	return *this;
}

C3DVertex& C3DVertex::operator = (const trVertex3D &p0)
{
	m_rVertex = p0;
	return *this;
}

C3DVertex C3DVertex::operator - (void) const
{
	return -m_rVertex;
}

C3DVertex C3DVertex::operator - (const C3DVertex &p0) const
{
	return m_rVertex-p0;
}

C3DVertex C3DVertex::operator + (const C3DVertex &p0) const
{
	return m_rVertex+p0;
}

C3DVertex C3DVertex::operator * (float scaler) const
{
	return m_rVertex*scaler;
}

C3DVertex C3DVertex::operator / (float scaler) const
{
	return m_rVertex*scaler;
}

void C3DVertex::operator -= (const C3DVertex &p0)
{
	m_rVertex -= p0;
}

void C3DVertex::operator += (const C3DVertex &p0)
{
	m_rVertex += p0;
}

void C3DVertex::operator *= (const C3DVertex &p0)
{
	m_rVertex *= p0;
}

void C3DVertex::operator *= (float scaler)
{
	m_rVertex *= scaler;
}

void C3DVertex::operator /= (const C3DVertex &p0)
{
	m_rVertex /= p0;
}

void C3DVertex::operator /= (float scaler)
{
	m_rVertex /= scaler;
}


// matrix class
CMatrix::CMatrix()
{
	GetIdentity();
	matrix.flags = XFORM_TRANS | XFORM_SCALE | XFORM_ROT;
}

CMatrix::CMatrix(const trMatrix &m)
{
	GetIdentity();
	SetElements((float*)&(m.el[0][0]));
	matrix.flags = m.flags;
}

CMatrix::CMatrix(const CMatrix &m)
{
	GetIdentity();
	Concat(m);
	matrix.flags = m.matrix.flags;
}

CMatrix::~CMatrix()
{
	GetIdentity();
	matrix.flags = 0;
}

float CMatrix::GetElement ( int row, int col )
{
	return matrix.el[row][col];
}

float CMatrix::SetElement ( float val, int row, int col )
{
	matrix.el[row][col] = val;
	return val;
}

float* CMatrix::GetElements ( void )
{
	return &matrix.el[0][0];
}

void CMatrix::GetElements ( float *m )
{
	memcpy(m,GetElements(),sizeof(float)*16);
}

void CMatrix::SetElements ( float *m )
{
	memcpy(&matrix.el[0][0],m,sizeof(float)*16);
}

void CMatrix::Classify( void )
{
	::ClassifyMatrix(&matrix);
}

void CMatrix::Normalize( void )
{
	::NormalizeMatrix(&matrix);
}

void CMatrix::GetIdentity( void )
{
	::GetIdentityMatrix(&matrix);
}

bool CMatrix::Planar ( void )
{
	return ::MatrixPlanar(&matrix,NULL);
}

tePlanes CMatrix::Plane( void )
{
	return ::MatrixPlane(&matrix);
}

bool CMatrix::ScaleNonUniform( void )
{
	return ::MatrixScaleNonUniform(&matrix);
}

bool CMatrix::ScaleNonUniform2d( void )
{
	return ::MatrixScaleNonUniform2d(&matrix);
}

bool CMatrix::operator == ( const CMatrix &m )// equal matrix
{
	return ::EqualMatrix(&matrix, &(m.matrix));
}

void CMatrix::operator += (const CMatrix &m)	// concat
{
	::ConcatMatrices(&matrix, &(m.matrix));
}

void CMatrix::Concat (const CMatrix &m)	// concat
{
	::ConcatMatrices(&matrix, &(m.matrix));
}

bool CMatrix::SamePlane( const CMatrix &m )
{
	return ::SamePlaneMatrix(&matrix, &(m.matrix),NULL);
}

void CMatrix::Invert( void )
{
	::InvertMatrix(&matrix);
}

void CMatrix::Translate( const trVertex3D* pos)
{
	::TranslateMatrix(&matrix,pos);
}

void CMatrix::Scale( const trVertex3D* p)
{
	::ScaleMatrix(&matrix,p);
}

void CMatrix::Rotate2d(float angle, const trVertex3D* p)
{
	::Rotate2dMatrix(&matrix,angle,p);
}

void CMatrix::Mirror(const trVertex3D* p, const trVertex3D* p2)
{
	::MirrorMatrix(&matrix,p,p2);
}

void CMatrix::TranslationMatrix( const trVertex3D* p)
{		
	::TranslationMatrix(&matrix,p);
}

void CMatrix::ScalingMatrix( const trVertex3D* p)
{
	::ScalingMatrix(&matrix,p);
}

void CMatrix::RotationMatrix( const trVertex3D* p, float a, float b)
{
	::RotationMatrix(&matrix,p,a,b);
}

void CMatrix::RotationMatrix( float a, float b)
{
	::RotationMatrix(&matrix,a,b);
}

void CMatrix::ForceDepthMatrix( float depth )
{
	::ForceDepthMatrix(&matrix,depth);
}

C3DVertex& CMatrix::TransformPos ( C3DVertex& point )
{
	::TransformPos(point.GetVertAddress(),&matrix);
	return point;
}

C3DVertex& CMatrix::TransformPos2d ( C3DVertex& point )
{
	::TransformPos2d(point.GetVertAddress(),&matrix);
	return point;
}

C3DVertex& CMatrix::TransformNorm ( C3DVertex& point )
{
	::TransformNorm(point.GetVertAddress(),&matrix);
	return point;
}


C3DVertex& CMatrix::TransformNorm2d ( C3DVertex& point )
{
	::TransformNorm2d(point.GetVertAddress(),&matrix);
	return point;
}

// frustum
/* 
* Copyright (c) 2002 - 2003 Jeffrey Myers
*
* This package is free software;  you can redistribute it and/or
* modify it under the terms of the license found in the file
* named license.txt that should have accompanied this file.
*
* THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
* WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
*/
// implementation for view cull classes

#include "config.h"

#include "bzfgl.h"
#include "ViewCull.h"

// plane class implementation;
ViewPlane::ViewPlane()
{
	m_rNorm.x = m_rNorm.y = m_rNorm.z = m_fODist = 0;
	m_bSet = false;
}

ViewPlane::~ViewPlane()
{
	m_rNorm.x = m_rNorm.y = m_rNorm.z = m_fODist = 0;
}

void ViewPlane::Set ( float fA, float fB, float fC , float fD )
{
	m_bSet = true;
	m_rNorm.x = fA;
	m_rNorm.y = fB;
	m_rNorm.z = fC;
	m_fODist = fD;
	Normalise();
}

void ViewPlane::Normalise ( void )
{
	m_fODist /= get_mag(m_rNorm);
	set_mag(m_rNorm,1.0f);
}

float ViewPlane::PointDist ( float fX, float fY, float fZ )
{
	if (!m_bSet)
		return 0.0f;

	return GetA() * fX + GetB() * fY + GetC() * fZ + GetD();
}

bool ViewPlane::PointBehind ( float fX, float fY, float fZ )
{
	if (!m_bSet)
		return false;

	return PointDist(fX,fY,fZ) <= 0.0f;
}

// view frustum class

BaseFrustum::BaseFrustum()
{
	m_bSet = false;
}

BaseFrustum::~BaseFrustum(){}

// functions to set the frustum
void BaseFrustum::SetPlane ( eFrustumSides side, float a, float b, float c, float d )
{
	m_bSet = true;
	m_aPlanes[side].Set(a,b,c,d);
}

void BaseFrustum::Set ( float fovH, float fovV, C3DVertex &pos, C3DVertex & rot )
{
	m_bSet = false;
	
	// compute the planes
	// figure out some math
}

bool BaseFrustum::PointIn( float fX, float fY, float fZ )
{
	if (!m_bSet)
		return false;

	for(int i = eFrustRight; i < eLastFrustSide; i++ )
	{
		if(m_aPlanes[i].PointBehind(fX,fY,fZ))
			return false;
	}
	return true;	// The point was inside of the frustum (In front of ALL the sides of the frustum)
}

bool BaseFrustum::SphereIn( float fX, float fY, float fZ, float fRad )
{
	if (!m_bSet)
		return false;

	for(int i = eFrustRight; i < eLastFrustSide; i++ )
	{
		// If the center of the sphere is farther away from the plane than the radius
		if( m_aPlanes[i].PointDist(fX,fY,fZ) <= -fRad )
			return false;
	}
	return true;// The sphere was inside of the frustum!
}

bool BaseFrustum::CubeIn( float fX, float fY, float fZ, float fSize )
{
	if (!m_bSet)
		return false;

	return BoxIn(fX,fY,fZ,fSize,fSize,fSize);
}

bool BaseFrustum::BoxIn( float fX, float fY, float fZ ,float fXSize, float fYSize, float fZSize )
{
	if (!m_bSet)
		return false;

	float fXHalfSize = fXSize *0.5f;
	float fYHalfSize = fYSize *0.5f;
	float fZHalfSize = fZSize *0.5f;

	for(int i = eFrustRight; i < eLastFrustSide; i++ )
	{
		if (!m_aPlanes[i].PointBehind(fX-fXHalfSize,fY-fYHalfSize,fZ-fZHalfSize))
			continue;

		if (!m_aPlanes[i].PointBehind(fX+fXHalfSize,fY-fYHalfSize,fZ-fZHalfSize))
			continue;

		if (!m_aPlanes[i].PointBehind(fX+fXHalfSize ,fY + fYHalfSize,fZ-fZHalfSize))
			continue;

		if (!m_aPlanes[i].PointBehind(fX-fXHalfSize ,fY + fYHalfSize,fZ-fZHalfSize))
			continue;

		if (!m_aPlanes[i].PointBehind(fX-fXHalfSize,fY-fYHalfSize,fZ+fZHalfSize))
			continue;

		if (!m_aPlanes[i].PointBehind(fX+fXHalfSize,fY-fYHalfSize,fZ+fZHalfSize))
			continue;

		if (!m_aPlanes[i].PointBehind(fX+fXHalfSize ,fY + fYHalfSize,fZ+fZHalfSize))
			continue;

		if (!m_aPlanes[i].PointBehind(fX-fXHalfSize ,fY + fYHalfSize,fZ+fZHalfSize))
			continue;

		return false;
	}
	return true;
}

bool BaseFrustum::QuadOut ( float *fp1, float *fp2, float *fp3, float *fp4 )
{
	if (!m_bSet)
		return false;

	if (!fp1 || !fp2 || !fp3 || !fp4)
		return false;

	bool	abPoints[4];

	for(int i = eFrustRight; i < eLastFrustSide; i++ )
	{
		abPoints[0] = m_aPlanes[i].PointBehind(fp1[0],fp1[1],fp1[2]);
		abPoints[1] = m_aPlanes[i].PointBehind(fp2[0],fp2[1],fp2[2]);
		abPoints[2] = m_aPlanes[i].PointBehind(fp3[0],fp3[1],fp3[2]);
		abPoints[3] = m_aPlanes[i].PointBehind(fp4[0],fp4[1],fp4[2]);

		if (abPoints[0] && abPoints[1] && abPoints[2] && abPoints[3])
			return true;
	}
	return false;
}



