#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include "TextureMngr.h"
#include "3dtypes.h"

typedef enum {
  FIRST = 0,
  HIGH,
  NORMAL,
  ALWAYS,
  TRANSNOZ,
  TRANSPARENT,
  LAST
} DrawPriority;

class DrawItem {
  public:
    DrawItem operator = (const DrawItem &r);

    int ID;
    bool transparent;
    bool selected;
    int name;
    DrawPriority priority;
};

class BaseModel {
  public:
    BaseModel();
    BaseModel(const BaseModel &r);
    virtual ~BaseModel();
    BaseModel operator = (const BaseModel &r);

    virtual void init(Point3D pos, Point3D scale, float frot = 0);
    virtual void render(bool transparent = false, bool selected = false, double transparency = 100.0);
    virtual void setItemInfo(char *item, float *data);

    unsigned int getTexture();
    void setTexture(unsigned int texture);

  protected:
    int glListID;
    unsigned int texture;
};

class GroundModel: public BaseModel {
  public:
    virtual void init(Point3D pos, Point3D scale, float frot = 0);
    virtual void render(bool transparent = false, bool selected = false, double transparency = 100.0);
};

class WallsModel : public BaseModel {
  public:
    virtual void init(Point3D pos, Point3D scale, float frot = 0);
    virtual void render(bool transparent = false, bool selected = false, double transparency = 100.0);
};

class BoxSidesModel : public BaseModel {
  public:
    virtual void init(Point3D pos, Point3D scale, float frot = 0);
};

class BoxTopsModel : public BaseModel {
  public:
    virtual void init(Point3D pos, Point3D scale, float frot = 0);
};

class PyramidModel : public BaseModel {
  public:
    virtual void init(Point3D pos, Point3D scale, float frot = 0);
};

class TPortFieldModel : public BaseModel {
  public:
    virtual void init(Point3D pos, Point3D scale, float frot = 0);
    virtual void render(bool transparent = false, bool selected = false, double transparency = 100.0);
};

class TPortFrameModel : public BaseModel {
  public:
    virtual void init(Point3D pos, Point3D scale, float frot = 0);
    virtual void setItemInfo(char *item, float *data);

  protected:
    float border;
};

class BaseTopsModel : public BaseModel {
  public:
    virtual void init(Point3D pos, Point3D scale, float frot = 0);
    virtual void render(bool transparent = false, bool selected = false, double transparency = 100.0);
    virtual void setItemInfo(char *item, float *data);

  protected:
    int color;
};

class LinkModel : public BaseModel {
  public:
    LinkModel();

    virtual void render(bool transparent = false, bool selected = false, double transparency = 100.0);
    virtual void setItemInfo(char *item, float *data);

  protected:
    void *world;
    int from;
    int to;
};

typedef std::vector<DrawItem> DrawList;
typedef std::vector<DrawItem>::iterator DrawListIterator;
typedef std::vector<BaseModel *> ModelList;
typedef std::vector<BaseModel *>::iterator ModelListIterator;

class ModelManager {
  public:
    ModelManager();
    ModelManager(CTextureManager *texman);
    ~ModelManager();

    void setTextureManager(CTextureManager *texman);
    void init();
    void clear();

    int newModel(char *type);
    void killModel(int item);

    BaseModel *getModel(unsigned int item);

    void addDrawItem(unsigned int item, bool transparent = false, bool selected = false, int name = -1, DrawPriority priority = NORMAL);
    void clearDrawList();
    void renderList(double transparency = 100.0);

  private:
    DrawList drawList;
    ModelList modelList;
    CTextureManager *texman;
};

#endif
