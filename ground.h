#ifndef __GROUND_H__
#define __GROUND_H__

#include "object.h"

class Ground : public Object {
  public:
    Ground(std::string name);

    virtual bool	Init();
    virtual void	Render(int name = -1);

  protected:
    int			texture;
    int			model;
};

#endif
