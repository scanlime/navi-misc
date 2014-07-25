/*
 * Skybox.cpp - Simple skybox using ScriptableObject and PGTexture
 *
 * Copyright (C) 2002-2004 Micah Dowty and David Trowbridge
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

#include <GL/glu.h>
#include "Skybox.h"

Skybox::Skybox(JetCOW *cow, Sint32 id, const char *type) : SceneNode(cow,id,type) {
  memset(&lists, 0, sizeof(lists));
  memset(&sides, 0, sizeof(sides));
}

void Skybox::makeLists(void) {
  /* We need this so the display lists are created from the same
   * thread as draw()... some broken implementations of OpenGL seem to need this.
   */

  float size = 2000.0f;

  if (!lists[0]) {
    sides[0] = new PGTexture("skyboxes/malrav11/left",  true);
    lists[0] = glGenLists(1);
    glNewList(lists[0], GL_COMPILE); {
      glBegin(GL_QUADS); {
	glTexCoord2f(0, 1); glVertex3f(size - 2, -size, -size);
	glTexCoord2f(0, 0); glVertex3f(size - 2,  size, -size);
	glTexCoord2f(1, 0); glVertex3f(size - 2,  size,  size);
	glTexCoord2f(1, 1); glVertex3f(size - 2, -size,  size);
      } glEnd();
    } glEndList();
  }

  if (!lists[1]) {
    sides[1] = new PGTexture("skyboxes/malrav11/right", true);
    lists[1] = glGenLists(1);
    glNewList(lists[1], GL_COMPILE); {
      glBegin(GL_QUADS); {
	glTexCoord2f(0, 1); glVertex3f(-size + 2, -size,  size);
	glTexCoord2f(0, 0); glVertex3f(-size + 2,  size,  size);
	glTexCoord2f(1, 0); glVertex3f(-size + 2,  size, -size);
	glTexCoord2f(1, 1); glVertex3f(-size + 2, -size, -size);
      } glEnd();
    } glEndList();
  }

  if (!lists[2]) {
    sides[2] = new PGTexture("skyboxes/malrav11/front", true);
    lists[2] = glGenLists(1);
    glNewList(lists[2], GL_COMPILE); {
      glBegin(GL_QUADS); {
	glTexCoord2f(0, 0); glVertex3f(-size,  size, -size + 2);
	glTexCoord2f(1, 0); glVertex3f( size,  size, -size + 2);
	glTexCoord2f(1, 1); glVertex3f( size, -size, -size + 2);
	glTexCoord2f(0, 1); glVertex3f(-size, -size, -size + 2);
      } glEnd();
    } glEndList();
  }

  if (!lists[3]) {
    sides[3] = new PGTexture("skyboxes/malrav11/back",  true);
    lists[3] = glGenLists(1);
    glNewList(lists[3], GL_COMPILE); {
      glBegin(GL_QUADS); {
	glTexCoord2f(1, 1); glVertex3f(-size, -size,  size - 2);
	glTexCoord2f(0, 1); glVertex3f( size, -size,  size - 2);
	glTexCoord2f(0, 0); glVertex3f( size,  size,  size - 2);
	glTexCoord2f(1, 0); glVertex3f(-size,  size,  size - 2);
      } glEnd();
    } glEndList();
  }

  if (!lists[4]) {
    sides[4] = new PGTexture("skyboxes/malrav11/top",   true);
    lists[4] = glGenLists(1);
    glNewList(lists[4], GL_COMPILE); {
      glBegin(GL_QUADS); {
	glTexCoord2f(0, 1); glVertex3f(-size, size - 2,  size);
	glTexCoord2f(0, 0); glVertex3f( size, size - 2,  size);
	glTexCoord2f(1, 0); glVertex3f( size, size - 2, -size);
	glTexCoord2f(1, 1); glVertex3f(-size, size - 2, -size);
      } glEnd();
    } glEndList();
  }

  if (!lists[5]) {
    sides[5] = new PGTexture("skyboxes/malrav11/bottom",true);
    lists[5] = glGenLists(1);
    glNewList(lists[5], GL_COMPILE); {
      glBegin(GL_QUADS); {
	glTexCoord2f(1, 0); glVertex3f(-size, -size + 2, -size);
	glTexCoord2f(1, 1); glVertex3f( size, -size + 2, -size);
	glTexCoord2f(0, 1); glVertex3f( size, -size + 2,  size);
	glTexCoord2f(0, 0); glVertex3f(-size, -size + 2,  size);
      } glEnd();
    } glEndList();
  }
}

Skybox::~Skybox() {
  for (int i=0;i<6;i++) {
    if (sides[i])
      delete sides[i];
    if (lists[i])
      glDeleteLists(lists[i], 1);
  }
}

void Skybox::draw() {
  glEnable(GL_TEXTURE_2D);
  glDisable(GL_BLEND);
  glDisable(GL_LIGHTING);

  glColor3f(1, 1, 1);
  
  makeLists();
  sides[0]->bind(); glCallList(lists[0]);
  sides[1]->bind(); glCallList(lists[1]);
  sides[2]->bind(); glCallList(lists[2]);
  sides[3]->bind(); glCallList(lists[3]);
  sides[4]->bind(); glCallList(lists[4]);
  sides[5]->bind(); glCallList(lists[5]);

  glDisable(GL_TEXTURE_2D);
  glEnable(GL_LIGHTING);
}

/* The End */
