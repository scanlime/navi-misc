#include "ground.h"

Ground::Ground(std::string name) : Object(name)
{
  pixbuf = gdk_pixbuf_new_from_file("data/groundicon.png", NULL);
}

bool Ground::Init()
{
  if(!(texman && modman))
    return false;

  texture = texman->GetID("data/ground");
  model = modman->newModel("ground");
  BaseModel *mod = modman->getModel(model);
  Point3D p;
  p.x = p.y = p.z = 0;
  mod->setTexture(texture);
  mod->init(p, p);
  return true;
}

void Ground::Render(int name)
{
  if(modman)
    modman->addDrawItem(model, false, false, name, FIRST);
}
