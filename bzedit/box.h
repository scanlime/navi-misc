#ifndef __BOX_H__
#define __BOX_H__

#include "object.h"

class Box : public Object {
  public:
    Box();
    ~Box();

    virtual bool	Init();
    virtual void	DefaultData();
    virtual void	Render(int name = -1);
    static std::string	GetNewName();

    virtual bool	setItemInfo(std::string name, float data);
    virtual bool	getItemInfo(std::string name, float &data);

    virtual GtkWidget	*getPositionDialog();
    virtual GtkWidget	*getRotationDialog();
    virtual GtkWidget	*getScaleDialog();

  protected:
    float px, py, pz;
    float sx, sy, sz;
    float angle;

    static int number;
    int texture, wallmodel, roofmodel;
};

#endif
