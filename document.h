#ifndef __DOCUMENT_H__
#define __DOCUMENT_H__

#include <string>
#include "world.h"
#include "TextureMngr.h"
#include "model.h"

class Document {
  public:
    Document();

    void init();

    void newf();
    bool openf(std::string);
    bool save();
    bool save(std::string);
    void exit();
    void undo();
    void redo();
    void cut();
    void copy();
    void paste();
    void convert_to_deg();
    void convert_to_rad();

    void addbox();
    void addpyr();
    void addtel();
    void addlnk();
    void addbas();

    World		world;
    bool		open;
    bool		unmodified;
    std::string		file;
    CTextureManager	texman;
    ModelManager	modman;
    GtkTreeModel	*model;
    GtkTreeStore	*store;

  protected:
    GdkPixbuf *visibility;
    void buildlist();
};

#endif
