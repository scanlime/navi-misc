#ifndef __BASE_H__
#define __BASE_H__

#include "object.h"

class Base : public Object {
  public:
    Base();
    ~Base();

    virtual bool	Init();
    virtual void	DefaultData();
    virtual void	Render(bool transparent = false, bool selected = false, int name = -1);
    static std::string	GetNewName();

    virtual bool	setItemInfo(std::string name, float data);
    virtual bool	getItemInfo(std::string name, float &data);

  protected:
    float px, py, pz;
    float sx, sy;
    float angle;
    int color;

    static int number;
    int topsmodel, sidesmodel, texture;
};

#endif
