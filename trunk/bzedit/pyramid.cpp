#include "pyramid.h"

int Pyramid::number = 0;

Pyramid::Pyramid() : Object(Pyramid::GetNewName())
{
  pixbuf = gdk_pixbuf_new_from_file("data/pyricon.png", NULL);
  model = -1;
  DefaultData();
}

Pyramid::~Pyramid()
{
  number--;
}

bool Pyramid::Init()
{
  if(!(texman && modman))
    return false;

  BaseModel *mod;
  Point3D p; p.x = px; p.y = py; p.z = pz;
  Point3D s; s.x = sx; s.y = sy; s.z = sx;

  texture = texman->GetID("data/pyrwall");
  if(model < 0)
    model = modman->newModel("pyramid");
  mod = modman->getModel(model);
  mod->setTexture(texture);
  mod->init(p, s, angle);

  return true;
}

void Pyramid::DefaultData()
{
  Object::DefaultData();
  px = py = pz = 0;
  sx = sy = 8.2; sz = 10.25;
  angle = 0;
  model = -1;
}

void Pyramid::Render(bool transparent, bool selected, int name)
{
  if(modman) {
    modman->addDrawItem(model, transparent, selected, name);
  }
}

std::string Pyramid::GetNewName()
{
  number++;
  char name[30];
  sprintf(name, "pyramid-%d", number);
  return std::string(name);
}

bool Pyramid::setItemInfo(std::string name, float data)
{
  return Object::setItemInfo(name, data);
}

bool Pyramid::getItemInfo(std::string name, float &data)
{
  return Object::getItemInfo(name, data);
}
