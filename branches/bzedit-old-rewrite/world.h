#ifndef __WORLD_H__
#define __WORLD_H__

#include "object.h"
#include "TextureMngr.h"
#include "model.h"
#include <map>
#include <string>

class World : public std::map<std::string, Object *>
{
  public:
    World();
    World(const World &r);
    virtual ~World();

    virtual void	Init();

    virtual bool	Load(std::string filename);
    virtual bool	Append(std::string filename);
    virtual bool	Write(std::string filename);

    virtual void	Render();
    virtual void	RenderTargets();

    void		SetTextureManager(CTextureManager *man);
    void		SetModelManager(ModelManager *man);

  protected:
    CTextureManager	*texman;
    ModelManager	*modman;
};

#endif
