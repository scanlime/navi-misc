#include "walls.h"

Walls::Walls(std::string name) : Object(name)
{
  pixbuf = gdk_pixbuf_new_from_file("data/wallsicon.png", NULL);
}

bool Walls::Init()
{
  if(!(texman && modman))
    return false;

  texture = texman->GetID("data/wall");
  model = modman->newModel("walls");
  Point3D p;
  BaseModel *mod = modman->getModel(model);
  p.x = p.y = p.z = 0;
  mod->setTexture(texture);
  mod->init(p, p);
  return true;
}

void Walls::Render(int name)
{
  if(modman)
    modman->addDrawItem(model, false, false, name, FIRST);
}
