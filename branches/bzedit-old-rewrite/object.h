#ifndef __OBJECT_H__
#define __OBJECT_H__

#include "TextureMngr.h"
#include "model.h"
#include <iostream>
#include <memory>
#include <string>
#include <gnome.h>

class Object
{
  public:
    Object(std::string name);
    Object(const Object &o);
    virtual ~Object();

    virtual void	CopyFrom(Object &o);

    void		SetTextureManager(CTextureManager *man);
    void		SetModelManager(ModelManager *man);

    virtual void	DefaultData();

    virtual void	Render(int name = -1);
    virtual void	Write(std::ostream &stream);
    virtual void	Write(char *data);
    virtual bool	Read(char *data);
    virtual bool	Init();

    virtual GdkPixbuf	*GetIcon();
    std::string		GetName();

    virtual bool	setItemInfo(std::string name, int data);
    virtual bool	setItemInfo(std::string name, float data);
    virtual bool	setItemInfo(std::string name, char *data);
    virtual bool	getItemInfo(std::string name, int &data);
    virtual bool	getItemInfo(std::string name, float &data);
    virtual bool	getItemInfo(std::string name, char *data);

    virtual bool	setItemInfo(std::string name, int type, ...);

    // yuck! i promised myself that i would never ever ever interweave interface
    // with backend ever again. however, here it makes sense. *sob*
    virtual GtkWidget	*getPositionDialog();
    virtual GtkWidget	*getRotationDialog();
    virtual GtkWidget	*getScaleDialog();
    virtual GtkWidget	*getObjectSpecificDialog();

  protected:
    CTextureManager	*texman;
    ModelManager	*modman;
    GdkPixbuf		*pixbuf;

    std::string		objectname;
};

#endif
