/* 3dScreamers
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
#ifndef _FCULL_H_
#define _FCULL_H_

#include "3dTypes.h"
#include "GeoUtils.h"
#include <stdio.h>

class ViewFrustumGL : public BaseFrustum
{
public:
	ViewFrustumGL();
	virtual ~ViewFrustumGL();
	void Get();/// load frustum from GL
};

#endif //_FCULL_H_
