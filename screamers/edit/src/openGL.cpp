
#include "openGL.h"

void glSetTextureScale ( float du, float dv )
{
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glScalef(du,dv,1);
	glMatrixMode(GL_MODELVIEW);
}

void glResetTextureScale ( void )
{
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
}

void glDrawSolidBox ( float size[3] )
{
	float realSize[3];
	realSize[0] = size[0]*0.5f;
	realSize[1] = size[1]*0.5f;
	realSize[2] = size[2]*0.5f;

	glPushMatrix();
	glScalef(realSize[0],realSize[1],realSize[2]);

//	glSetTextureScale(uvScale[0],uvScale[1]);
	glBegin(GL_QUADS);
		// draw the top
		glNormal3f(0,0,1);
		glVertex3f(-1.0f,1.0f,1.0f);
		glVertex3f(-1.0f,-1.0f,1.0f);
		glVertex3f(1.0f,-1.0f,1.0f);
		glVertex3f(1.0f,1.0f,1.0f);

		// draw the bottom
		glNormal3f(0,0,-1);
		glVertex3f(1.0f,1.0f,-1.0f);
		glVertex3f(1.0f,-1.0f,-1.0f);
		glVertex3f(-1.0f,-1.0f,-1.0f);
		glVertex3f(-1.0f,1.0f,-1.0f);

		// Y- side
		glNormal3f(0,-1,0);
		glVertex3f(-1.0f,-1.0f,-1.0f);
		glVertex3f(1.0f,-1.0f,-1.0f);
		glVertex3f(1.0f,-1.0f,1.0f);
		glVertex3f(-1.0f,-1.0f,1.0f);

		// y+ side
		glNormal3f(0,1,0);
		glVertex3f(-1.0f,1.0f,1.0f);
		glVertex3f(1.0f,1.0f,1.0f);
		glVertex3f(1.0f,1.0f,-1.0f);
		glVertex3f(-1.0f,1.0f,-1.0f);

		// x- Side
		glNormal3f(-1,0,0);
		glVertex3f(-1,1,-1);
		glVertex3f(-1,-1,-1);
		glVertex3f(-1,-1,1);
		glVertex3f(-1,1,1);

		// x- Side
		glNormal3f(1,0,0);
		glVertex3f(1,1,1);
		glVertex3f(1,-1,1);
		glVertex3f(1,-1,-1);
		glVertex3f(1,1,-1);
	glEnd();

	glPopMatrix();
}

void glDrawOutlineBox ( float size[3], float lineWidth )
{
	float realSize[3];
	realSize[0] = size[0]*0.5f;
	realSize[1] = size[1]*0.5f;
	realSize[2] = size[2]*0.5f;

	glLineWidth(lineWidth);
	glPushMatrix();
	glScalef(realSize[0],realSize[1],realSize[2]);

	//	glSetTextureScale(uvScale[0],uvScale[1]);
	glPolygonMode (GL_FRONT, GL_LINE);
	glBegin(GL_QUADS);
	// draw the top
	glNormal3f(0,0,1);
	glVertex3f(-1.0f,1.0f,1.0f);
	glVertex3f(-1.0f,-1.0f,1.0f);
	glVertex3f(1.0f,-1.0f,1.0f);
	glVertex3f(1.0f,1.0f,1.0f);

	// draw the bottom
	glNormal3f(0,0,-1);
	glVertex3f(1.0f,1.0f,-1.0f);
	glVertex3f(1.0f,-1.0f,-1.0f);
	glVertex3f(-1.0f,-1.0f,-1.0f);
	glVertex3f(-1.0f,1.0f,-1.0f);

	// Y- side
	glNormal3f(0,-1,0);
	glVertex3f(-1.0f,-1.0f,-1.0f);
	glVertex3f(1.0f,-1.0f,-1.0f);
	glVertex3f(1.0f,-1.0f,1.0f);
	glVertex3f(-1.0f,-1.0f,1.0f);

	// y+ side
	glNormal3f(0,1,0);
	glVertex3f(-1.0f,1.0f,1.0f);
	glVertex3f(1.0f,1.0f,1.0f);
	glVertex3f(1.0f,1.0f,-1.0f);
	glVertex3f(-1.0f,1.0f,-1.0f);

	// x- Side
	glNormal3f(-1,0,0);
	glVertex3f(-1,1,-1);
	glVertex3f(-1,-1,-1);
	glVertex3f(-1,-1,1);
	glVertex3f(-1,1,1);

	// x- Side
	glNormal3f(1,0,0);
	glVertex3f(1,1,1);
	glVertex3f(1,-1,1);
	glVertex3f(1,-1,-1);
	glVertex3f(1,1,-1);
	glEnd();

	glPopMatrix();

	glLineWidth(1);
	glPolygonMode (GL_FRONT, GL_FILL);
}

void glDrawSolidBox ( float x, float y, float z )
{
	float t[3];
	t[0] = x;
	t[1] = y;
	t[2] = z;
	glDrawSolidBox(t);
}

void glSetPolygonOffSet ( float offset )
{
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(offset,1);
}

void glResetPolygonOffset ( void )
{
	glPolygonOffset(0,0);
	glDisable(GL_POLYGON_OFFSET_LINE);
}

void glCapedCylinder ( float topRadius, float bottomRadius, int segments, float height )
{
	glPushMatrix();
	glTranslatef(0,0,-height*0.5f);

	gluCylinder(gluNewQuadric(),bottomRadius,topRadius,height,segments,1);

	glTranslatef(0,0,height);
	gluDisk(gluNewQuadric(),0,topRadius,segments,1);

	glTranslatef(0,0,-height);
	//glFrontFace(GL_CW);

	glRotatef(180,1,0,0);
	gluDisk(gluNewQuadric(),0,bottomRadius,segments,1);

	glFrontFace(GL_CCW);

	glPopMatrix();
}

void glCylinder ( float topRadius, float bottomRadius, int segments, float height )
{
	glPushMatrix();
	glTranslatef(0,0,-height*0.5f);

	gluCylinder(gluNewQuadric(),bottomRadius,topRadius,height,segments,1);

	glPopMatrix();
}

void glSphere ( float radius, int segments )
{
	glPushMatrix();
	//glTranslatef(0,0,-radius*0.5f);

	gluSphere(gluNewQuadric(),radius,segments,segments);

	glPopMatrix();
}

