#include "world.h"
#include <GL/gl.h>

World::World()
{
  texman = NULL;
  modman = NULL;
}

World::World(const World &r)
{
  this->texman = r.texman;
  this->modman = r.modman;
}

World::~World()
{
}

void World::Init()
{
}

bool World::Load(std::string filename)
{
  return true;
}

bool World::Append(std::string filename)
{
  return true;
}

bool World::Write(std::string filename)
{
  return true;
}

void World::Render()
{
  modman->clearDrawList();
  iterator it = begin();
  while(it != end()) {
    it->second->Render();
    it++;
  }
  modman->renderList();
}

void World::RenderTargets()
{
  int name = 0;
  modman->clearDrawList();

  iterator it = begin();
  while(it != end()) {
    glLoadName(name);
//    it->second->Render(name);
    it++; name++;
  }
  modman->renderList();
}

void World::SetTextureManager(CTextureManager *man)
{
  texman = man;
}

void World::SetModelManager(ModelManager *man)
{
  modman = man;
}
