#include "base.h"

int Base::number = 0;

Base::Base() : Object(Base::GetNewName())
{
  pixbuf = gdk_pixbuf_new_from_file("data/basicon.png", NULL);
  topsmodel = sidesmodel = -1;
  DefaultData();
}

Base::~Base()
{
  number--;
}

bool Base::Init()
{
  if(!(texman && modman))
    return false;

  BaseModel *mod;
  Point3D p; p.x = px; p.y = py; p.z = pz;
  Point3D s; s.x = sx; s.y = sy; s.z = 0;

  texture = texman->GetID("data/boxwall");
  if(topsmodel == -1)
    topsmodel = modman->newModel("basetops");
  mod = modman->getModel(topsmodel);
  mod->setTexture(0xffffffff);
  float temp = color;
  mod->setItemInfo("color", &temp);
  mod->init(p, s, angle);

  if(sidesmodel < 0) {
    if(p.z > 0) {
      sidesmodel = modman->newModel("boxsides");
    } else {
      if(p.z == 0) {
	modman->killModel(sidesmodel);
	sidesmodel = -1;
      }
    }
  }

  if(sidesmodel != -1) {
    mod = modman->getModel(sidesmodel);
    if(!mod)
      return false;
    s.z = 1.0;
    mod->setTexture(texture);
    mod->init(p, s, angle);
  }
  return true;
}

void Base::DefaultData()
{
  px = py = 0; pz = 10;
  sx = sy = 30;
  color = 1;

  topsmodel = sidesmodel = -1;
}

void Base::Render(bool transparent, bool selected, int name)
{
  if(modman) {
    modman->addDrawItem(topsmodel, transparent, selected, name, ALWAYS);
    if(sidesmodel != -1)
      modman->addDrawItem(sidesmodel, transparent, selected, name, HIGH);
  }
}

std::string Base::GetNewName()
{
  number++;
  char name[30];
  sprintf(name, "base-%d", number);
  return std::string(name);
}

bool Base::setItemInfo(std::string name, float data)
{
  return Object::setItemInfo(name, data);
}

bool Base::getItemInfo(std::string name, float &data)
{
  return Object::getItemInfo(name, data);
}
