#ifndef __WALLS_H__
#define __WALLS_H__

#include "object.h"

class Walls : public Object {
  public:
    Walls(std::string name);

    virtual bool	Init();
    virtual void	Render(int name = -1);

  protected:
    int			texture;
    int			model;
};

#endif
