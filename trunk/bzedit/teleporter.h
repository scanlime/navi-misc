#ifndef __TELEPORTER_H__
#define __TELEPORTER_H__

#include "object.h"

class Teleporter : public Object {
  public:
    Teleporter();
    ~Teleporter();

    virtual bool	Init();
    virtual void	DefaultData();
    virtual void	Render(bool transparent = false, bool selected = false, int name = -1);
    static std::string	GetNewName();

    virtual bool	setItemInfo(std::string name, float data);
    virtual bool	getItemInfo(std::string name, float &data);

  protected:
    float px, py, pz;
    float sy, sz;
    float angle;
    float border;

    static int number;
    int texture, fieldmodel, framemodel;
};

#endif
