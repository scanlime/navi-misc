#include "teleporter.h"

int Teleporter::number = 0;

Teleporter::Teleporter() : Object(Teleporter::GetNewName())
{
  pixbuf = gdk_pixbuf_new_from_file("data/telicon.png", NULL);
  fieldmodel = framemodel = -1;
  DefaultData();
}

Teleporter::~Teleporter()
{
  number--;
}

bool Teleporter::Init()
{
  if(!(texman && modman))
    return false;

  BaseModel *mod;
  Point3D p; p.x = px; p.y = py; p.z = pz;
  Point3D s; s.x = 0; s.y = sy; s.z = sz;

  texture = texman->GetID("data/caution");

  if(fieldmodel < 0)
    fieldmodel = modman->newModel("tportfield");
  mod = modman->getModel(fieldmodel);
  mod->setTexture(0xffffffff);
  mod->init(p, s, angle);

  if(framemodel < 0)
    framemodel = modman->newModel("tportframe");
  mod = modman->getModel(framemodel);
  mod->setItemInfo("border", &border);
  mod->setTexture(texture);
  mod->init(p, s, angle);

  return true;
}

void Teleporter::DefaultData()
{
  Object::DefaultData();
  px = py = pz = 0;
  sy = 4.48; sz = 20.16;
  angle = 0;
  border = 1;
  fieldmodel = framemodel = -1;
}

void Teleporter::Render(bool transparent, bool selected, int name)
{
  if(modman) {
    modman->addDrawItem(fieldmodel, transparent, selected, name, TRANSPARENT);
    modman->addDrawItem(framemodel, transparent, selected, name);
  }
}

std::string Teleporter::GetNewName()
{
  number++;
  char name[30];
  sprintf(name, "teleporter-%d", number);
  return std::string(name);
}

bool Teleporter::setItemInfo(std::string name, float data)
{
  return Object::setItemInfo(name, data);
}

bool Teleporter::getItemInfo(std::string name, float &data)
{
  return Object::getItemInfo(name, data);
}
