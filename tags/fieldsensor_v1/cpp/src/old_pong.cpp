#include <FieldSensor.h>
#include <math.h>
#include <Vector.h>

#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <vector>

using namespace std;

class game {
public:
  game(void) {
    computerX = 0.5;
    reset();
  } 

  void reset(void) {
    float randAngle = annie::random() * 6.28319;

    ballPosition.set(0.5, 0.5);
    ballVelocity.set(cos(randAngle), sin(randAngle));
    ballVelocity *= 0.5;
  }

  void draw(float dt) {
    /* Get player input
     * - X axis controls paddle
     * - ignore the Y axis
     * - Z axis senses whether the player is near the paddle- it will fade out when unused.
     */
    Vector3 sensorReading = s.getPosition();
    if (sensorReading[2] > Zthreshold) {
      playerAlpha -= paddleFadeSpeed * dt;
      if (playerAlpha < 0) {
	playerAlpha = 0;
	playerX = -1;
      }
    }
    else {
      float newPosition = sensorReading[0] * (1-paddleWidth) + paddleWidth/2;
      playerVelocity = (newPosition - playerX) / dt;
      playerX = newPosition;
      playerAlpha += paddleFadeSpeed * dt;
      if (playerAlpha > 1)
	playerAlpha = 1;
    }

    /* Computer player */
    if (computerX < ballPosition[0] - paddleWidth/2)
      computerVelocity = maxComputerSpeed;
    else if (computerX > ballPosition[0] + paddleWidth/2)
      computerVelocity = -maxComputerSpeed;
    else
      computerVelocity = 0;
    computerX += computerVelocity + dt;
    if (computerX < paddleWidth/2)
      computerX = paddleWidth/2;
    if (computerX > 1-paddleWidth/2)
      computerX = 1-paddleWidth/2;

    /* Draw everything */
    glColor4f(1,1,1,0.5);
    drawPlayfield();
    glColor4f(1,1,0,playerAlpha);
    drawPaddle(playerX,0);
    glColor4f(0,1,0,1);
    drawPaddle(computerX,1);
    glColor4f(1,1,1,1);
    drawRectangle(ballPosition[0] - ballWidth/2, ballPosition[1] - ballWidth/2, 
		  ballPosition[0] + ballWidth/2, ballPosition[1] + ballWidth/2);

    /* Ball movement */
    ballPosition += ballVelocity * dt;

    /* Ball bouncing off the playfield edges */
    if (ballPosition[0] <= 0 && ballPosition[1] >= 0 && ballPosition[1] <= 1) {
      ballVelocity[0] = -ballVelocity[0];
      ballPosition[0] = 0;
    }
    if (ballPosition[0] >= 1 && ballPosition[1] >= 0 && ballPosition[1] <= 1) {
      ballVelocity[0] = -ballVelocity[0];
      ballPosition[0] = 1;
    }

    /* Ball bouncing off the paddles */
    if (ballPosition[0] >= playerX-paddleWidth/2 && ballPosition[0] <= playerX+paddleWidth/2 &&
	ballPosition[1] <= paddleHeight/2 && ballPosition[1] >= -paddleHeight/2) {
      ballVelocity[1] = -ballVelocity[1];
      ballVelocity[0] += playerVelocity;
    }
    if (ballPosition[0] >= computerX-paddleWidth/2 && ballPosition[0] <= computerX+paddleWidth/2 &&
	ballPosition[1] >= 1-paddleHeight/2 && ballPosition[1] <= 1+paddleHeight/2) {
      ballVelocity[1] = -ballVelocity[1];
      ballVelocity[0] += computerVelocity;
    }

    /* Reset after it leaves the playfield */
    if (ballPosition[1] <= -1.5)
      reset();
    if (ballPosition[1] >= 2.5)
      reset();
  }    

private:
  void drawPlayfield(void) {
    dottedLine(0,0,0,1,borderWidth);
    dottedLine(1,0,1,1,borderWidth);
  }

  void drawPaddle(float x, float y) {
    drawRectangle(x-paddleWidth/2, y-paddleHeight/2,
		  x+paddleWidth/2, y+paddleHeight/2);
  }

  void drawRectangle(float x1, float y1, float x2, float y2) {
    glBegin(GL_QUADS);
    glVertex2f(x1,y1);
    glVertex2f(x2,y1);
    glVertex2f(x2,y2);
    glVertex2f(x1,y2);
    glEnd();
  }

  void dottedLine(float x1, float y1, float x2, float y2, float width) {
    for (;;) {
      drawRectangle(x1-width/2,y1-width/2,x1+width,y1+width);
      if (x1<x2)
	x1 += width*3;
      else if (y1<y2)
	y1 += width*3;
      else break;
    }
  }
	
  const static float borderWidth = 0.007;
  const static float paddleWidth = 0.2;
  const static float ballWidth = 0.01;
  const static float paddleHeight = 0.01;
  const static float Zthreshold = 0.99;
  const static float paddleFadeSpeed = 1.0;
  const static float maxComputerSpeed = 0.008;

  FieldSensor s;
  float playerX, playerAlpha, computerX, computerVelocity, playerVelocity;
  Vector2 ballPosition;
  Vector2 ballVelocity;
};
  

int main(void) {
  SDL_Init(SDL_INIT_VIDEO);
  SDL_Surface *surface = SDL_SetVideoMode(1024,768,0,SDL_OPENGL | SDL_FULLSCREEN);

  glEnable(GL_BLEND);
  glEnable(GL_LINE_SMOOTH);
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

  game g;
  Uint32 oldTicks = SDL_GetTicks();

  while (1) {
    SDL_Event event;
    while (SDL_PollEvent(&event))
      switch (event.type) {

      case SDL_QUIT:
        SDL_Quit();
	return 0;

      case SDL_KEYDOWN:
        SDL_Quit();
      	return 0;

      }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(-0.5,-0.3,-1.1);
    glRotatef(-45,1,0,0);

    Uint32 newTicks = SDL_GetTicks();
    g.draw((newTicks - oldTicks) / 1000.0);
    oldTicks = newTicks;

    SDL_GL_SwapBuffers();
  }

  return 0;
}
