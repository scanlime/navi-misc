#ifndef __PYRAMID_H__
#define __PYRAMID_H__

#include "object.h"

class Pyramid : public Object {
  public:
    Pyramid();
    ~Pyramid();

    virtual bool	Init();
    virtual void	DefaultData();
    virtual void	Render(bool transparent = false, bool selected = false, int name = -1);
    static std::string	GetNewName();

    virtual bool	setItemInfo(std::string name, float data);
    virtual bool	getItemInfo(std::string name, float &data);

  protected:
    float px, py, pz;
    float sx, sy, sz;
    float angle;

    static int number;
    int texture, model;
};

#endif
