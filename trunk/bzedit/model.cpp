#ifdef WIN32
#pragma warning( disable : 4786 )
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include <map>
#include "model.h"

DrawItem DrawItem::operator = (const DrawItem &r) {
  this->ID = r.ID;
  this->transparent = r.transparent;
  this->selected = r.selected;
  this->name = r.name;
  this->priority = r.priority;
  return *this;
}

BaseModel::BaseModel() {
  glListID = -1;
}

BaseModel::BaseModel(const BaseModel &r) {
  this->glListID = r.glListID;
  this->texture = r.texture;
}

BaseModel::~BaseModel() {
}

BaseModel BaseModel::operator = (const BaseModel &r) {
  this->glListID = r.glListID;
  this->texture = r.texture;
  return *this;
}

void BaseModel::init(Point3D pos, Point3D scale, float rot) {
  glListID = -1;
}

void BaseModel::render(bool transparent, bool selected, double transparency) {
  if(glListID == -1)
    return;

  if((transparent) && (transparency != 100.0)) {
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
  }
  
  if(selected)
    glColor4f(1, 0.25, 0.25, transparency / 100.0);
  else
    glColor4f(1, 1, 1, transparency / 100.0);

  glEnable(GL_COLOR_MATERIAL);

  glCallList(glListID);

  glColor3f(1, 1, 1);

  if((transparent) && (transparency != 100.0)) {
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_CULL_FACE);
    glColor4f(1, 1, 1, 1);
  }
}

void BaseModel::setItemInfo(char *item, float *data) {
}

unsigned int BaseModel::getTexture() {
  return texture;
}

void BaseModel::setTexture(unsigned int texture) {
  this->texture = texture;
}

void GroundModel::init(Point3D pos, Point3D scale, float rot) {
  if(glListID != -1)
    glDeleteLists(glListID, 1);

  glListID = glGenLists(1);

  glNewList(glListID, GL_COMPILE); {
    float textureScale = 1.0;
    float wallSize = 400.0;

    glDisable(GL_CULL_FACE);

    glBegin(GL_QUADS); {
      glNormal3f(0, 0, 1);
      
      glTexCoord2f(0.0, 0.0);
      glVertex3f(wallSize, -wallSize, 0);

      glTexCoord2f(textureScale, 0.0);
      glVertex3f(-wallSize, -wallSize, 0);

      glTexCoord2f(textureScale, textureScale);
      glVertex3f(-wallSize, wallSize, 0);

      glTexCoord2f(0.0, textureScale);
      glVertex3f(wallSize, wallSize, 0);
    } glEnd();

    glEnable(GL_CULL_FACE);

  } glEndList();
}

void GroundModel::render(bool transparent, bool selected, double transparency) {
  if(glListID == -1)
    return;
  if(selected)
    glColor3f(1, 0.25, 0.25);
  else
    glColor3f(1, 1, 1);

  glEnable(GL_COLOR_MATERIAL);

  glCallList(glListID);

  glColor3f(1, 1, 1);
}

void WallsModel::init(Point3D pos, Point3D scale, float rot) {
  if(glListID != -1)
    glDeleteLists(glListID, 1);

  glListID = glGenLists(1);

  glNewList(glListID, GL_COMPILE); {
    glDisable(GL_CULL_FACE);

    float wallSize = 400.0;
    float wallZ = 8.0;

    glBegin(GL_TRIANGLE_STRIP); {
      glNormal3f(0, -1, 0);

      glTexCoord2f(0, 0);
      glVertex3f(-wallSize, wallSize, 0);

      glTexCoord2f(0, 1);
      glVertex3f(-wallSize, wallSize, wallZ);

      glTexCoord2f(((wallSize * 2) / wallZ), 0);
      glVertex3f(wallSize, wallSize, 0);

      glTexCoord2f(((wallSize * 2) / wallZ), 1);
      glVertex3f(wallSize, wallSize, wallZ);
    } glEnd();

    glBegin(GL_TRIANGLE_STRIP); {
      glNormal3f(-1, 0, 0);

      glTexCoord2f(((wallSize * 2) / wallZ), 0);
      glVertex3f(wallSize, wallSize, 0);

      glTexCoord2f(((wallSize * 2) / wallZ), 1);
      glVertex3f(wallSize, wallSize, wallZ);

      glTexCoord2f(((wallSize * 2) / wallZ) * 2.0, 0);
      glVertex3f(wallSize, -wallSize, 0);

      glTexCoord2f(((wallSize * 2) / wallZ) * 2.0, 1);
      glVertex3f(wallSize, -wallSize, wallZ);
    } glEnd();

    glBegin(GL_TRIANGLE_STRIP); {
      glNormal3f(0, 1, 0);

      glTexCoord2f(((wallSize * 2) / wallZ) * 2.0, 0);
      glVertex3f(wallSize, -wallSize, 0);

      glTexCoord2f(((wallSize * 2) / wallZ) * 2.0, 1);
      glVertex3f(wallSize, -wallSize, wallZ);

      glTexCoord2f(((wallSize * 2) / wallZ) * 3.0, 0);
      glVertex3f(-wallSize, -wallSize, 0);

      glTexCoord2f(((wallSize * 2) / wallZ) * 3.0, 1);
      glVertex3f(-wallSize, -wallSize, wallZ);
    } glEnd();

    glBegin(GL_TRIANGLE_STRIP); {
      glNormal3f(1, 0, 0);
      
      glTexCoord2f(((wallSize * 2) / wallZ) * 3.0, 0);
      glVertex3f(-wallSize, -wallSize, 0);

      glTexCoord2f(((wallSize * 2) / wallZ) * 3.0, 1);
      glVertex3f(-wallSize, -wallSize, wallZ);

      glTexCoord2f(((wallSize * 2) / wallZ) * 4.0, 0);
      glVertex3f(-wallSize, wallSize, 0);

      glTexCoord2f(((wallSize * 2) / wallZ) * 4.0, 1);
      glVertex3f(-wallSize, wallSize, wallZ);
    } glEnd();

    glEnable(GL_CULL_FACE);

  } glEndList();
}

void WallsModel::render(bool transparent, bool selected, double transparency) {
  if(glListID == -1)
    return;
  if(selected)
    glColor3f(1, 0.25, 0.25);
  else
    glColor3f(1, 1, 1);

  glEnable(GL_COLOR_MATERIAL);

  glCallList(glListID);

  glColor3f(1, 1, 1);
}

void BoxSidesModel::init(Point3D pos, Point3D scale, float rot) {
  if(glListID != -1)
    glDeleteLists(glListID, 1);

  glListID = glGenLists(1);

  float textureUnits = 4.0;

  glNewList(glListID, GL_COMPILE); {
    glPushMatrix();

    glTranslatef(pos.x, pos.y, pos.z);
    glRotatef(rot, 0, 0, 1);

    glBegin(GL_QUADS); {
      // Y+ side
      glNormal3f(0, 1, 0);
      glTexCoord2f(0, 0);
      glVertex3f(-scale.x, scale.y, 0);

      glTexCoord2f(0, scale.z / textureUnits);
      glVertex3f(-scale.x, scale.y, scale.z);

      glTexCoord2f(scale.x / textureUnits * 2, scale.z / textureUnits);
      glVertex3f(scale.x, scale.y, scale.z);

      glTexCoord2f(scale.x / textureUnits * 2, 0);
      glVertex3f(scale.x, scale.y, 0);

      // Y- side
      glNormal3f(0, -1, 0);
      glTexCoord2f(0, 0);
      glVertex3f(scale.x, -scale.y, 0);

      glTexCoord2f(0, scale.z / textureUnits);
      glVertex3f(scale.x, -scale.y, scale.z);

      glTexCoord2f(scale.x / textureUnits * 2, scale.z / textureUnits);
      glVertex3f(-scale.x, -scale.y, scale.z);

      glTexCoord2f(scale.x / textureUnits * 2, 0);
      glVertex3f(-scale.x, -scale.y, 0);

      // X+ side
      glNormal3f(1, 0, 0);
      glTexCoord2f(0, 0);
      glVertex3f(scale.x, scale.y, 0);

      glTexCoord2f(0, scale.z / textureUnits);
      glVertex3f(scale.x, scale.y, scale.z);

      glTexCoord2f(scale.y / textureUnits * 2, scale.z / textureUnits);
      glVertex3f(scale.x, -scale.y, scale.z);

      glTexCoord2f(scale.y / textureUnits * 2, 0);
      glVertex3f(scale.x, -scale.y, 0);

      // X- side
      glNormal3f(-1, 0, 0);
      glTexCoord2f(0, 0);
      glVertex3f(-scale.x, -scale.y, 0);

      glTexCoord2f(0, scale.z / textureUnits);
      glVertex3f(-scale.x, -scale.y, scale.z);

      glTexCoord2f(scale.y / textureUnits * 2, scale.z / textureUnits);
      glVertex3f(-scale.x, scale.y, scale.z);

      glTexCoord2f(scale.y / textureUnits * 2, 0);
      glVertex3f(-scale.x, scale.y, 0);
    } glEnd();

    glPopMatrix();
  } glEndList();
}

void BoxTopsModel::init(Point3D pos, Point3D scale, float rot) {
  if(glListID != -1)
    glDeleteLists(glListID, 1);
  
  glListID = glGenLists(1);

  float textureUnits = 4.0;

  glNewList(glListID, GL_COMPILE); {
    glPushMatrix();

    glTranslatef(pos.x, pos.y, pos.z);
    glRotatef(rot, 0, 0, 1);

    glBegin(GL_QUADS); {
      // z+ side
      glNormal3f(0, 0, 1);
      glTexCoord2f(scale.x / textureUnits * 2, 0);
      glVertex3f(scale.x, -scale.y, scale.z);

      glTexCoord2f(scale.x / textureUnits * 2, scale.y / textureUnits * 2);
      glVertex3f(scale.x, scale.y, scale.z);

      glTexCoord2f(0, scale.y / textureUnits * 2);
      glVertex3f(-scale.x, scale.y, scale.z);

      glTexCoord2f(0, 0);
      glVertex3f(-scale.x, -scale.y, scale.z);

      // z- side
      glNormal3f(0, 0, -1);
      glTexCoord2f(0, 0);
      glVertex3f(-scale.x, -scale.y, 0);

      glTexCoord2f(0, scale.y / textureUnits * 2);
      glVertex3f(-scale.x, scale.y, 0);

      glTexCoord2f(scale.x / textureUnits * 2, scale.y / textureUnits * 2);
      glVertex3f(scale.x, scale.y, 0);

      glTexCoord2f(scale.x / textureUnits * 2, scale.y / textureUnits * 2);
      glVertex3f(scale.x, -scale.y, 0);
    } glEnd();

    glPopMatrix();
  } glEndList();
}

void PyramidModel::init(Point3D pos, Point3D scale, float rot) {
  if(glListID != -1)
    glDeleteLists(glListID, 1);

  glListID = glGenLists(1);

  float textureUnits = 4.0;

  glNewList(glListID, GL_COMPILE); {
    glPushMatrix();

    glTranslatef(pos.x, pos.y, pos.z);
    glRotatef(rot, 0, 0, 1);

    glBegin(GL_QUADS); {
      // z- side
      glNormal3f(0, 0, -1);
      glTexCoord2f(0, 0);
      glVertex3f(-scale.x, -scale.y, 0);

      glTexCoord2f(0, scale.y / textureUnits * 2);
      glVertex3f(-scale.x, scale.y, 0);

      glTexCoord2f(scale.x / textureUnits * 2, scale.y / textureUnits * 2);
      glVertex3f(scale.x, scale.y, 0);

      glTexCoord2f(scale.x / textureUnits * 2, 0);
      glVertex3f(scale.x, -scale.y, 0);
    } glEnd();

    Point3D vec1, vec2, norm;
    glBegin(GL_TRIANGLES); {
      // x+ side
      vec1.x = scale.x; vec1.y = -scale.y; vec1.z = scale.z;
      vec2.x = scale.x * 2; vec2.y = 0; vec2.z = 0;
      cross(vec1, vec2, norm);

      glNormal3f(norm.x, norm.y, norm.z);
      glTexCoord2f(0, 0);
      glVertex3f(-scale.x, scale.y, 0);

      glTexCoord2f(scale.x / textureUnits, scale.z / textureUnits);
      glVertex3f(0, 0, scale.z);

      glTexCoord2f(scale.x / textureUnits * 2, 0);
      glVertex3f(scale.x, scale.y, 0);

      // y+ side
      vec1.x = scale.x; vec1.y = -scale.y; vec1.z = scale.z;
      vec2.x = 0; vec2.y = scale.y * 2; vec2.z = 0;
      cross(vec1, vec2, norm);

      glNormal3f(norm.x, norm.y, norm.z);
      glTexCoord2f(0, 0);
      glVertex3f(scale.x, scale.y, 0);

      glTexCoord2f(scale.y / textureUnits, scale.z / textureUnits);
      glVertex3f(0, 0, scale.z);

      glTexCoord2f(scale.y / textureUnits * 2, 0);
      glVertex3f(scale.x, -scale.y, 0);

      // x- side
      vec1.x = -scale.x; vec1.y = scale.y; vec1.z = scale.z;
      vec2.x = -scale.x * 2; vec2.y = 0; vec2.z = 0;
      cross(vec1, vec2, norm);

      glNormal3f(norm.x, norm.y, norm.z);
      glTexCoord2f(0, 0);
      glVertex3f(scale.x, -scale.y, 0);

      glTexCoord2f(scale.x / textureUnits, scale.z / textureUnits);
      glVertex3f(0, 0, scale.z);

      glTexCoord2f(scale.x / textureUnits * 2, 0);
      glVertex3f(-scale.x, -scale.y, 0);

      // y- side
      vec1.x = -scale.x; vec1.y = scale.y; vec1.z = scale.z;
      vec2.x = 0; vec2.y = scale.y * 2; vec2.z = 0;
      cross(vec1, vec2, norm);

      glNormal3f(norm.x, norm.y, norm.z);
      glTexCoord2f(0, 0);
      glVertex3f(-scale.x, -scale.y, 0);

      glTexCoord2f(scale.y / textureUnits, scale.z / textureUnits);
      glVertex3f(0, 0, scale.z);

      glTexCoord2f(scale.y / textureUnits * 2, 0);
      glVertex3f(-scale.x, scale.y, 0);
    } glEnd();

    glPopMatrix();
  } glEndList();
}

void TPortFieldModel::init(Point3D pos, Point3D scale, float rot) {
  if(glListID != -1)
    glDeleteLists(glListID, 1);

  glListID = glGenLists(1);

  glNewList(glListID, GL_COMPILE); {
    glPushMatrix();

    glTranslatef(pos.x, pos.y, pos.z);
    glRotatef(rot, 0, 0, 1);

    glDisable(GL_TEXTURE_2D);
    
    glBegin(GL_QUADS); {
      // x positive side
      glNormal3f(1, 0, 0);
      glVertex3f(scale.x / 2.0, scale.y, 0);
      glVertex3f(scale.x / 2.0, scale.y, scale.z);
      glVertex3f(scale.x / 2.0, -scale.y, scale.z);
      glVertex3f(scale.x / 2.0, -scale.y, 0);

      // x negative side
      glNormal3f(-1, 0, 0);
      glVertex3f(-scale.x / 2.0, -scale.y, 0);
      glVertex3f(-scale.x / 2.0, -scale.y, scale.z);
      glVertex3f(-scale.x / 2.0, scale.y, scale.z);
      glVertex3f(-scale.x / 2.0, scale.y, 0);

    } glEnd();
    
    glEnable(GL_TEXTURE_2D);
    
    glPopMatrix();
  } glEndList();
}

void TPortFieldModel::render(bool transparent, bool selected, double transparency) {
  if(glListID == -1)
    return;

  if((transparent) && (transparency != 100.0)) {
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
  }

  if(selected)
    glColor4f(0.4, 0.05, 0.05, transparency / 100.0);
  else
    glColor4f(0.25, 0.25, 0.25, transparency / 100.0);

  glEnable(GL_COLOR_MATERIAL);

  glCallList(glListID);

  if((transparent) && (transparency != 100.0)) {
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_CULL_FACE);
    glColor4f(1, 1, 1, 1);
  }
}

void TPortFrameModel::init(Point3D pos, Point3D scale, float rot) {
  if(glListID != -1)
    glDeleteLists(glListID, 1);

  glListID = glGenLists(1);

  glNewList(glListID, GL_COMPILE); {
    glPushMatrix();

    glTranslatef(pos.x, pos.y, pos.z);
    glRotatef(rot, 0, 0, 1);

    float textureSize = 1.0;
    float shift = 0.0;

    glBegin(GL_TRIANGLE_STRIP); {
      glNormal3f(-1, 0, 0);
      glTexCoord2f(0, 0);
      glVertex3f(-border / 2, scale.y + border, 0);

      glTexCoord2f(border / textureSize, 0);
      glVertex3f(-border / 2, scale.y, 0);

      glTexCoord2f(0, (scale.z + border) / textureSize);
      glVertex3f(-border / 2, scale.y + border, scale.z + border);

      glTexCoord2f(border / textureSize, scale.z / textureSize);
      glVertex3f(-border / 2, scale.y, scale.z);

      glTexCoord2f(((2 * border) + ( 2 * scale.y)) / textureSize, (scale.z + border) / textureSize);
      glVertex3f(-border / 2, -scale.y - border, scale.z + border);

      glTexCoord2f(((border) + (2 * scale.y)) / textureSize, (scale.z) / textureSize);
      glVertex3f(-border / 2, -scale.y, scale.z);

      glTexCoord2f(((2 * border) + (2 * scale.y)) / textureSize, 0);
      glVertex3f(-border / 2, -scale.y - border, 0);

      glTexCoord2f(((border) + (2 * scale.y)) / textureSize, 0);
      glVertex3f(-border / 2, -scale.y, 0);
    } glEnd();

    glFrontFace(GL_CW);
    float shift2 = shift;

    glBegin(GL_TRIANGLE_STRIP); {
      glNormal3f(1, 0, 0);
      glTexCoord2f(0, shift2);
      glVertex3f(border / 2, scale.y + border, 0);

      glTexCoord2f(border / textureSize, shift2);
      glVertex3f(border / 2, scale.y, 0);

      glTexCoord2f(0, (scale.z + border) / textureSize + shift2);
      glVertex3f(border / 2, scale.y + border, scale.z + border);

      glTexCoord2f(border / textureSize, scale.z / textureSize + shift2);
      glVertex3f(border / 2, scale.y, scale.z);

      glTexCoord2f(((2 * border) + (2 * scale.y)) / textureSize, (scale.z + border) / textureSize + shift2);
      glVertex3f(border / 2, -scale.y - border, scale.z + border);

      glTexCoord2f(((border) + (2 * scale.y)) / textureSize, scale.z / textureSize + shift2);
      glVertex3f(border / 2, -scale.y, scale.z);

      glTexCoord2f(((2 * border) + (2 * scale.y)) / textureSize, shift2);
      glVertex3f(border / 2, -scale.y - border, 0);

      glTexCoord2f(((border) + (2 * scale.y)) / textureSize, shift2);
      glVertex3f(border / 2, -scale.y, 0);
    } glEnd();

    glFrontFace(GL_CCW);

    glBegin(GL_QUADS); {
      float topShift = 0.40;
      // top
      glNormal3f(topShift, 0, 1);
      glTexCoord2f(topShift, 0.0001);
      glVertex3f(border / 2, -scale.y - border, scale.z + border);

      glTexCoord2f(((2 * border) + (2 * scale.y)) / textureSize + topShift, 0.0001);
      glVertex3f(border / 2, scale.y + border, scale.z + border);

      glTexCoord2f(((2 * border) + (2 * scale.y)) / textureSize + topShift, 0);
      glVertex3f(-border / 2, scale.y + border, scale.z + border);

      glTexCoord2f(topShift, 0);
      glVertex3f(-border / 2, -scale.y - border, scale.z + border);

      // underside of top
      glNormal3f(0, 0, -1);
      glTexCoord2f(topShift, 0);
      glVertex3f(-border / 2, -scale.y - border, scale.z);

      glTexCoord2f(((2 * border) + (2 * scale.y)) / textureSize + topShift, 0);
      glVertex3f(-border / 2, scale.y + border, scale.z);

      glTexCoord2f(((2 * border) + (2 * scale.y)) / textureSize + topShift, 0.0001);
      glVertex3f(border / 2, scale.y + border, scale.z);

      glTexCoord2f(topShift, 0.0001);
      glVertex3f(border / 2, -scale.y - border, scale.z);

      // y plus outside
      glNormal3f(0, 1, 0);
      glTexCoord2f(border / textureSize, shift);
      glVertex3f(-border / 2, scale.y + border, 0);

      glTexCoord2f(border / textureSize, (scale.z + border) / textureSize + shift);
      glVertex3f(-border / 2, scale.y + border, scale.z + border);

      glTexCoord2f(0, (scale.z + border) / textureSize + shift);
      glVertex3f(border / 2, scale.y + border, scale.z + border);

      glTexCoord2f(0, shift);
      glVertex3f(border / 2, scale.y + border, 0);

      // y plus inside
      glNormal3f(0, -1, 0);
      glTexCoord2f(0, 0);
      glVertex3f(border / 2, scale.y, 0);

      glTexCoord2f(0, scale.z / textureSize);
      glVertex3f(border / 2, scale.y, scale.z);

      glTexCoord2f(border / textureSize, scale.z / textureSize);
      glVertex3f(-border / 2, scale.y, scale.z);

      glTexCoord2f(border / textureSize, 0);
      glVertex3f(-border / 2, scale.y, 0);

      // y minus outside
      glNormal3f(0, 1, 0);
      glTexCoord2f(0, shift);
      glVertex3f(border / 2, -scale.y - border, 0);

      glTexCoord2f(0, (scale.z + border) / textureSize + shift);
      glVertex3f(border / 2, -scale.y - border, scale.z + border);

      glTexCoord2f(border / textureSize, (scale.z + border) / textureSize + shift);
      glVertex3f(-border / 2, -scale.y - border, scale.z + border);

      glTexCoord2f(border / textureSize, shift);
      glVertex3f(-border / 2, -scale.y - border, 0);

      // y minus inside
      glNormal3f(0, -1, 0);
      glTexCoord2f(border / textureSize, 0);
      glVertex3f(-border / 2, -scale.y, 0);

      glTexCoord2f(border / textureSize, scale.z / textureSize);
      glVertex3f(-border / 2, -scale.y, scale.z);

      glTexCoord2f(0, scale.z / textureSize);
      glVertex3f(border / 2, -scale.y, scale.z);

      glTexCoord2f(0, 0);
      glVertex3f(border / 2, -scale.y, 0);

      // y plus leg bottom
      glNormal3f(0, 0, -1);
      glTexCoord2f(border / textureSize, border / textureSize);
      glVertex3f(border / 2, scale.y + border, 0);

      glTexCoord2f(0, border / textureSize);
      glVertex3f(border / 2, scale.y, 0);

      glTexCoord2f(0, 0);
      glVertex3f(-border / 2, scale.y, 0);

      glTexCoord2f(border / textureSize, 0);
      glVertex3f(-border / 2, scale.y + border, 0);

      // y minus leg bottom
      glNormal3f(0, 0, -1);
      glTexCoord2f(border / textureSize, border / textureSize);
      glVertex3f(-border / 2, -scale.y - border, 0);

      glTexCoord2f(border / textureSize, 0);
      glVertex3f(-border / 2, -scale.y, 0);

      glTexCoord2f(0, 0);
      glVertex3f(border / 2, -scale.y, 0);

      glTexCoord2f(0, border / textureSize);
      glVertex3f(border / 2, -scale.y - border, 0);
    } glEnd();

    glPopMatrix();
  } glEndList();
}

void TPortFrameModel::setItemInfo(char *item, float *data) {
  if(strcasecmp(item, "border") == 0)
    border = *data;
}

void BaseTopsModel::init(Point3D pos, Point3D scale, float rot) {
  if(glListID != -1)
    glDeleteLists(glListID, 1);

  glListID = glGenLists(1);

//  float textureUnits = 4.0;

  glNewList(glListID, GL_COMPILE); {
    glPushMatrix();

    glTranslatef(pos.x, pos.y, pos.z);
    glRotatef(rot, 0, 0, 1);

    glDisable(GL_TEXTURE_2D);

    glBegin(GL_QUADS); {
      float topZ = 0.0;
      if(pos.z > 0.0)
	topZ = 1.0;

      // z-positive side
      glNormal3f(0, 0, 1);
      glVertex3f(scale.x, -scale.y, topZ);
      glVertex3f(scale.x, scale.y, topZ);
      glVertex3f(-scale.x, scale.y, topZ);
      glVertex3f(-scale.x, -scale.y, topZ);

      // z-negative side
      if(pos.z > 0.0) {
        glNormal3f(0, 0, -1);
        glVertex3f(-scale.x, -scale.y, 0);
        glVertex3f(-scale.x, scale.y, 0);
        glVertex3f(scale.x, scale.y, 0);
        glVertex3f(scale.x, -scale.y, 0);
      }
    } glEnd();

    glPopMatrix();
  } glEndList();
}

void BaseTopsModel::render(bool transparent, bool selected, double transparency) {
  if(glListID == -1)
    return;

  float Color[4] = {0, 0, 0, 1};

  if((transparent) && (transparency != 100.0)) {
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
    Color[3] = (transparency / 100.0);
  }

  switch(color) {
    case 1: Color[0] = 1; break;
    case 2: Color[1] = 1; break;
    case 3: Color[2] = 1; break;
    case 4: Color[0] = Color[2] = 1; break;
    default: Color[0] = Color[1] = Color[2] = 0.75; break;
  }

  if(!selected) {
    Color[0] *= 0.75;
    Color[1] *= 0.75;
    Color[2] *= 0.75;
  }

  glColor4fv(Color);
  glEnable(GL_COLOR_MATERIAL);
  glCallList(glListID);

  if((transparent) && (transparency != 100.0)) {
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_CULL_FACE);
    glColor4f(1, 1, 1, 1);
  }
}

void BaseTopsModel::setItemInfo(char *item, float *data) {
  if(strcasecmp(item, "color") == 0)
    color = (int) *data;
}

LinkModel::LinkModel() {
  world = NULL;
  from = -2;
  to = -2;
}

void LinkModel::render(bool transparent, bool selected, double transparency) {
  if(!world)
    return;
  // FIXME
}

void LinkModel::setItemInfo(char *item, float *data) {
  if(strcasecmp(item, "world") == 0)
    world = (void *) data;
  else if(strcasecmp(item, "from") == 0)
    from = *(int *) data;
  else if(strcasecmp(item, "to") == 0)
    to = *(int *) data;
}

ModelManager::ModelManager() {
  drawList.clear();
  modelList.clear();
  texman = NULL;
}

ModelManager::ModelManager(CTextureManager *texman) {
  drawList.clear();
  modelList.clear();
  this->texman = texman;
}

ModelManager::~ModelManager() {
  clear();
  drawList.clear();
}

void ModelManager::setTextureManager(CTextureManager *texman) {
  this->texman = texman;
}

void ModelManager::init() {
}

void ModelManager::clear() {
  ModelListIterator it = modelList.begin();
  while(it != modelList.end()) {
    if(*it)
      delete (*it);
    it++;
  }
  modelList.clear();
}

int ModelManager::newModel(char *type) {
  BaseModel *model = NULL;

  if(strcasecmp(type, "ground") == 0)
    model = (BaseModel *) new GroundModel;
  else if(strcasecmp(type, "walls") == 0)
    model = (BaseModel *) new WallsModel;
  else if(strcasecmp(type, "boxsides") == 0)
    model = (BaseModel *) new BoxSidesModel;
  else if(strcasecmp(type, "boxtops") == 0)
    model = (BaseModel *) new BoxTopsModel;
  else if(strcasecmp(type, "pyramid") == 0)
    model = (BaseModel *) new PyramidModel;
  else if(strcasecmp(type, "tportfield") == 0)
    model = (BaseModel *) new TPortFieldModel;
  else if(strcasecmp(type, "tportframe") == 0)
    model = (BaseModel *) new TPortFrameModel;
  else if(strcasecmp(type, "basetops") == 0)
    model = (BaseModel *) new BaseTopsModel;
  else if(strcasecmp(type, "link") == 0)
    model = (BaseModel *) new LinkModel;

  if(!model)
    return -1;

  int id = modelList.size();
  modelList.push_back(model);
  return id;
}

void ModelManager::killModel(int item) {
  clearDrawList();
}

BaseModel *ModelManager::getModel(unsigned int item) {
  if(item >= modelList.size())
    return NULL;
  return modelList[item];
}

void ModelManager::addDrawItem(unsigned int item, bool transparent, bool selected, int name, DrawPriority priority) {
  if(item >= modelList.size())
    return;

  DrawItem drawItem;
  drawItem.selected = selected;
  drawItem.transparent = transparent;
  drawItem.ID = item;
  drawItem.name = name;
  drawItem.priority = priority;

  drawList.push_back(drawItem);
}

void ModelManager::clearDrawList() {
  drawList.clear();
}

void ModelManager::renderList(double transparency) {
  DrawListIterator modelit = drawList.begin();
  DrawItem item;
  BaseModel *model;

  std::map<int, DrawList> textureList;
  while(modelit != drawList.end()) {
    item = (*modelit);
    model = getModel(item.ID);
    textureList[model->getTexture()].push_back(item);
    modelit++;
  }
  glEnable(GL_TEXTURE_2D);

  if(transparency == 100.0) {
    std::map<int, DrawList>::iterator textureit = textureList.begin();
    for(int i = 0; i < LAST; i++) {
      textureit = textureList.begin();
      if(i == ALWAYS) {
        // for funky transparent stuff
        glDepthMask(false);
        glDepthFunc(GL_LEQUAL);
        glPolygonOffset(-4, -4);
        glEnable(GL_POLYGON_OFFSET_FILL);
      }
      if(i == TRANSNOZ)
        glDepthMask(false);
      if(i == LAST)
        glDepthFunc(GL_LEQUAL);
      while(textureit != textureList.end()) {
        modelit = (*textureit).second.begin();
        texman->Bind((*textureit).first);
        while(modelit != (*textureit).second.end()) {
	  item = (*modelit);
	  model = getModel(item.ID);

	  if(model && (i == item.priority)) {
	    if(item.name != -1)
	      glLoadName(item.name + 1);
	    model->render(item.transparent, item.selected);
	  }
	  modelit++;
        }
        textureit++;
      }
      if((i == ALWAYS) || (i == TRANSNOZ) || (i == LAST)) {
        glDepthMask(true);
        glDisable(GL_POLYGON_OFFSET_FILL);
        glDepthFunc(GL_LESS);
      }
    }
  } else {
    modelit = drawList.begin();
    while(modelit != drawList.end()) {
      item = (*modelit);
      model = getModel(item.ID);
      texman->Bind(model->getTexture());
      model->render(item.transparent, item.selected, transparency);
      modelit++;
    }
  }
}
