#include "object.h"

Object::Object(std::string name)
{
  objectname = name;
  DefaultData();
}

Object::Object(const Object &o)
{
  Init();
  (*this) = o;
}

Object::~Object()
{
  // just stubbed
}

void Object::CopyFrom(Object &o)
{
  texman = o.texman;
  modman = o.modman;
}

void Object::SetTextureManager(CTextureManager *man)
{
  texman = man;
}

void Object::SetModelManager(ModelManager *man)
{
  modman = man;
}

void Object::DefaultData()
{
  texman = NULL;
  modman = NULL;
}

void Object::Render(int name)
{
  // just stubbed
}

void Object::Write(std::ostream &stream)
{
  // just stubbed
}

void Object::Write(char *data)
{
  // just stubbed
}

bool Object::Read(char *data)
{
  return false;
}

bool Object::Init()
{
  return false;
}

GdkPixbuf *Object::GetIcon()
{
  return pixbuf;
}

std::string Object::GetName()
{
  return objectname;
}

bool Object::setItemInfo(std::string name, int data)
{
  return false;
}

bool Object::setItemInfo(std::string name, float data)
{
  return false;
}

bool Object::setItemInfo(std::string name, char *data)
{
  return false;
}

bool Object::getItemInfo(std::string name, int &data)
{
  return false;
}

bool Object::getItemInfo(std::string name, float &data)
{
  return false;
}

bool Object::getItemInfo(std::string name, char *data)
{
  return false;
}

bool Object::setItemInfo(std::string name, int type, ...)
{
  return false;
}

GtkWidget *Object::getPositionDialog()
{
  return NULL;
}

GtkWidget *Object::getRotationDialog()
{
  return NULL;
}

GtkWidget *Object::getScaleDialog()
{
  return NULL;
}

GtkWidget *Object::getObjectSpecificDialog()
{
  return NULL;
}
