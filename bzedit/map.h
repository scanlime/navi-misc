#ifndef __MAP_H__
#define __MAP_H__

#include "object.h"

class Map : public Object {
  public:
    Map(std::string name);

    virtual bool	Init();
    virtual void	Render(int name = -1);

    virtual bool	setItemInfo(std::string name, float data);
    virtual bool	getItemInfo(std::string name, float &data);

  protected:
    float size;
};

#endif
