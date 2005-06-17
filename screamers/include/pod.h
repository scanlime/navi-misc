/* screamers
* Copyright (c) 2005 ScreamersGroup
*
* This package is free software;  you can redistribute it and/or
* modify it under the terms of the license found in the file
* named COPYING that should have accompanied this file.
*
* THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
* WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
*/

#ifndef _POD_H_
#define _POD_H_

class Pod
{
public:
	float position[3];
	float orientation[4];	// wow, a quaternion!

	float shields;		// [0,1]
};

#endif
