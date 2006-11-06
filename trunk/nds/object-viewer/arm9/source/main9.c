/* -*- mode: c; c-basic-offset: 4; -*- */

#include <nds.h>
#include <nds/arm9/console.h>
#include <stdio.h>
#include <loader-exit.h>

#include "display_list_bin.h"

uint32 frames = 0;

void fps_timer_irq(void)
{
    	iprintf("%d FPS    \x1b[60D", frames);
	frames = 0;
}

int main(void)
{
    float x_angle = 0, y_angle = 0;
    float x_speed = 0, y_speed = 0;
    float distance = 1;

    powerON(POWER_ALL);
    irqInit();
    irqEnable(IRQ_VBLANK);
    videoSetMode(MODE_0_3D);
    consoleDemoInit();

    /* Set up a timer to count FPS */
    TIMER3_DATA = TIMER_FREQ_1024(1);
    TIMER3_CR = TIMER_DIV_1024 | TIMER_IRQ_REQ;
    irqSet(IRQ_TIMER3, fps_timer_irq);
    irqEnable(IRQ_TIMER3);

    glViewPort(0, 0, SCREEN_WIDTH-1, SCREEN_HEIGHT-1);
    glClearColor(0, 0, 0);
    glClearDepth(GL_MAX_DEPTH);

    iprintf("Hello World!\n\n");

    glReset();
    gluPerspective(35, SCREEN_WIDTH / (float) SCREEN_HEIGHT, 0.1, 40.0);
    gluLookAt(0.0, 0.0, 1.0,   /* Eye */
	      0.0, 0.0, 0.0,   /* Look at */
	      0.0, 1.0, 0.0);  /* Up */

    glMaterialf(GL_AMBIENT, RGB15(8,8,8));
    glMaterialf(GL_DIFFUSE, RGB15(16,16,16));
    glMaterialf(GL_SPECULAR, BIT(15) | RGB15(8,8,8));
    glMaterialf(GL_EMISSION, RGB15(5,5,5));
    glMaterialShinyness();
    glColor(RGB15(31, 31, 31));

    glLight(0, RGB15(20, 15, 15),
	    floattov10(-0.9),
	    floattov10( 0.9),
	    floattov10( 0.2));

    glLight(1, RGB15(15, 15, 20),
	    floattov10( 0.4),
	    floattov10( 0.9),
	    floattov10( 0.2));

    glPolyFmt(POLY_ALPHA(31) | POLY_CULL_BACK |
	      POLY_FORMAT_LIGHT0 | POLY_FORMAT_LIGHT1);

    while (1) {
	scanKeys();
	if (keysHeld() & KEY_START)  Loader_Exit9();
	if (keysHeld() & KEY_UP)     x_speed += 0.05;
	if (keysHeld() & KEY_DOWN)   x_speed -= 0.05;
	if (keysHeld() & KEY_LEFT)   y_speed += 0.05;
	if (keysHeld() & KEY_RIGHT)  y_speed -= 0.05;
	if (keysHeld() & KEY_R)      distance += 0.05;
	if (keysHeld() & KEY_L)      distance -= 0.05;

	glPushMatrix();

	glTranslate3f32(floattof32( 0.0),
			floattof32( 0.0),
			floattof32(-distance));

	glRotateX(x_angle += x_speed);
	glRotateY(y_angle += y_speed);

	glCallList(display_list_bin);

	glPopMatrix(1);

	glFlush();
	frames++;
	swiWaitForVBlank();
    }

    return 0;
}
