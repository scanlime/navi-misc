#include <annie.h>
#include <FieldSensor.h>
#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>

int main(void) {
  FieldSensor s;
  annie::TwoLayerNetwork net("data/current.net");
  double packet[8];
  annie::VECTOR position;

  float colors[][4] = {
    {1.0,0.8,0.6,0.9},
    {1.0,0.7,0.6,0.9},
    {1.0,0.6,0.6,0.9},
    {1.0,0.5,0.6,0.9},
    {0.6,0.8,1.0,0.9},
    {0.6,0.7,1.0,0.9},
    {0.6,0.6,1.0,0.9},
    {0.6,0.5,1.0,0.9},
  };

  SDL_Init(SDL_INIT_VIDEO);
  SDL_Surface *surface = SDL_SetVideoMode(1024,768,0,SDL_OPENGL);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glShadeModel(GL_SMOOTH);
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glClearDepth(1.0);
  glDepthFunc(GL_LEQUAL);

  glViewport(0, 0, surface->w, surface->h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45, 1.0*surface->w/surface->h, 0.1, 100.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  while (1) {
    SDL_Event event;
    while (SDL_PollEvent(&event))
      switch (event.type) {

      case SDL_QUIT:
	return 0;

      }

    s.readPacket(packet);
    position = net.getOutput(packet);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(0.0,0.0,-4.0);

    /* Raw values */
    glPushMatrix();
    glTranslatef(-2.0,-1.6,0);
    for (int i=0;i<8;i++) {
      float height = packet[i] * 20;
      glColor4fv(colors[i]);
      glBegin(GL_QUADS);
      glVertex3f(-0.05,0,0);
      glVertex3f(-0.05,height,0);
      glVertex3f( 0.05,height,0);
      glVertex3f( 0.05,0,0);
      glEnd();
      glTranslatef(0.2,0,0);
    }
    glPopMatrix();

    /* Neural network output values */
    glPushMatrix();
    glTranslatef(1.6,-1.6,0);
    glScalef(1,1,0.25);
    glColor4f(1,1,0.5,1);
    for (int i=0;i<3;i++) {
      float height = position[i];
      glBegin(GL_QUADS);
      glVertex3f(-0.05,0,0);
      glVertex3f(-0.05,height,0);
      glVertex3f( 0.05,height,0);
      glVertex3f( 0.05,0,0);
      glEnd();
      glBegin(GL_LINES);
      for (float y=0;y<1.1;y+=0.1) {
	glVertex3f(-0.08,y,0);
	glVertex3f(-0.1,y,0);
      }
      glEnd();
      glTranslatef(0.2,0,0);
    }
    glPopMatrix();

    /* 3D cube graph */

    glPushMatrix();
    glScalef(1.6,-1.6,1.6);
    glRotatef(60,1,0,0);
    glTranslatef(-0.5,-0.2,0.3);
    glScalef(1,1,0.6);
    
    /* Base rectangle */
    glColor4f(0.5,0.5,0.5,1);
    glBegin(GL_LINE_LOOP);
    glVertex3f(0,0,0);
    glVertex3f(1,0,0);
    glVertex3f(1,1,0);
    glVertex3f(0,1,0);
    glEnd();
    
    /* Position indicator cube */
    glColor4f(0.9,0.9,1,1);
    glPushMatrix();
    glTranslatef(position[0],position[1],position[2]+0.01);
    glScalef(0.01,0.01,0.01);
    glBegin(GL_QUADS);
    glVertex3f(-1,-1,-1);
    glVertex3f( 1,-1,-1);
    glVertex3f( 1, 1,-1);
    glVertex3f(-1, 1,-1);
    glVertex3f(-1,-1, 1);
    glVertex3f( 1,-1, 1);
    glVertex3f( 1, 1, 1);
    glVertex3f(-1, 1, 1);
    glVertex3f(-1, 1,-1);
    glVertex3f( 1, 1,-1);
    glVertex3f( 1, 1, 1);
    glVertex3f(-1, 1, 1);
    glVertex3f(-1,-1,-1);
    glVertex3f( 1,-1,-1);
    glVertex3f( 1,-1, 1);
    glVertex3f(-1,-1, 1);
    glVertex3f(-1,-1,-1);
    glVertex3f(-1, 1,-1);
    glVertex3f(-1, 1, 1);
    glVertex3f(-1,-1, 1);
    glVertex3f( 1,-1,-1);
    glVertex3f( 1, 1,-1);
    glVertex3f( 1, 1, 1);
    glVertex3f( 1,-1, 1);
    glEnd();
    glPopMatrix();

    /* Shadows of the position cube on each axis plane */
    glColor4f(0.5,0.5,0.8,1);
    glPushMatrix();
    glTranslatef(position[0],position[1],0);
    glScalef(0.01,0.01,0.01);
    glBegin(GL_LINE_LOOP);
    glVertex3f(-1,-1,0);
    glVertex3f( 1,-1,0);
    glVertex3f( 1, 1,0);
    glVertex3f(-1, 1,0);
    glEnd();
    glPopMatrix();
    glPushMatrix();
    glTranslatef(position[0],0,position[2]);
    glScalef(0.01,0.01,0.01);
    glBegin(GL_LINE_LOOP);
    glVertex3f(-1,0,-1);
    glVertex3f( 1,0,-1);
    glVertex3f( 1,0, 1);
    glVertex3f(-1,0, 1);
    glEnd();
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0,position[1],position[2]);
    glScalef(0.01,0.01,0.01);
    glBegin(GL_LINE_LOOP);
    glVertex3f(0,-1,-1);
    glVertex3f(0, 1,-1);
    glVertex3f(0, 1, 1);
    glVertex3f(0,-1, 1);
    glEnd();
    glPopMatrix();

    glPopMatrix();

    SDL_GL_SwapBuffers();
  }

  return 0;
}
