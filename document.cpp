#include "document.h"
#include "globals.h"
#include "ground.h"
#include "walls.h"
#include "map.h"
#include "box.h"
#include "pyramid.h"
#include "teleporter.h"
#include "base.h"

Document::Document()
{
  open = true;
  unmodified = true;
  file = "";

  texman.LoadAll(DATADIR);

  world.SetTextureManager(&texman);
  modman.setTextureManager(&texman);
  world.SetModelManager(&modman);

  store = gtk_tree_store_new(3, G_TYPE_STRING, GDK_TYPE_PIXBUF, GDK_TYPE_PIXBUF);
  model = GTK_TREE_MODEL(store);
}

void Document::init()
{
  visibility = gdk_pixbuf_new_from_file("data/eye.png", NULL);
  world["0"] = new Map("Map");
  world["0:0"] = new Ground("Ground");
  world["0:1"] = new Walls("Walls");

  world["0"]->SetTextureManager(&texman);
  world["0"]->SetModelManager(&modman);
  world["0"]->Init();
  world["0:0"]->SetTextureManager(&texman);
  world["0:0"]->SetModelManager(&modman);
  world["0:0"]->Init();
  world["0:1"]->SetTextureManager(&texman);
  world["0:1"]->SetModelManager(&modman);
  world["0:1"]->Init();

  buildlist();
}

void Document::newf()
{
}

bool Document::openf(std::string)
{
  return true;
}

bool Document::save()
{
  return true;
}

bool Document::save(std::string)
{
  return true;
}

void Document::exit()
{
}

void Document::undo()
{
}

void Document::redo()
{
}

void Document::cut()
{
}

void Document::copy()
{
}

void Document::paste()
{
}

void Document::convert_to_deg()
{
}

void Document::convert_to_rad()
{
}

void Document::addbox()
{
  GtkTreeIter iter;
  GtkTreePath *path;
  Object *o = new Box;

  o->SetTextureManager(&texman);
  o->SetModelManager(&modman);
  o->Init();
  gtk_tree_store_append(store, &iter, NULL);
  gtk_tree_store_set(store, &iter, 0, o->GetName().c_str(), 1, o->GetIcon(), 2, visibility, -1);
  path = gtk_tree_model_get_path(model, &iter);
  world[gtk_tree_path_to_string(path)] = o;
  gtk_tree_path_free(path);
}

void Document::addpyr()
{
  GtkTreeIter iter;
  GtkTreePath *path;
  Object *o = new Pyramid;

  o->SetTextureManager(&texman);
  o->SetModelManager(&modman);
  o->Init();
  gtk_tree_store_append(store, &iter, NULL);
  gtk_tree_store_set(store, &iter, 0, o->GetName().c_str(), 1, o->GetIcon(), 2, visibility, -1);
  path = gtk_tree_model_get_path(model, &iter);
  world[gtk_tree_path_to_string(path)] = o;
  gtk_tree_path_free(path);
}

void Document::addtel()
{
  GtkTreeIter iter;
  GtkTreePath *path;
  Object *o = new Teleporter;

  o->SetTextureManager(&texman);
  o->SetModelManager(&modman);
  o->Init();
  gtk_tree_store_append(store, &iter, NULL);
  gtk_tree_store_set(store, &iter, 0, o->GetName().c_str(), 1, o->GetIcon(), 2, visibility, -1);
  path = gtk_tree_model_get_path(model, &iter);
  world[gtk_tree_path_to_string(path)] = o;
  gtk_tree_path_free(path);
}

void Document::addlnk()
{
}

void Document::addbas()
{
  GtkTreeIter iter;
  GtkTreePath *path;
  Object *o = new Base;

  o->SetTextureManager(&texman);
  o->SetModelManager(&modman);
  o->Init();
  gtk_tree_store_append(store, &iter, NULL);
  gtk_tree_store_set(store, &iter, 0, o->GetName().c_str(), 1, o->GetIcon(), 2, visibility, -1);
  path = gtk_tree_model_get_path(model, &iter);
  world[gtk_tree_path_to_string(path)] = o;
  gtk_tree_path_free(path);
}

void Document::buildlist()
{
  GtkTreePath *path;
  GtkTreeIter iter, iter2;

  World::iterator it = world.begin();
  while(it != world.end()) {
    char *ptr = strdup(it->first.c_str());
    if(strrchr(ptr, ':') == NULL) {
      // root-level node
      gtk_tree_store_append(store, &iter, NULL);
      gtk_tree_store_set(store, &iter, 0, it->second->GetName().c_str(), 1,  it->second->GetIcon(), 2, visibility, -1);
    } else {
      *strrchr(ptr, ':') = '\0';
      path = gtk_tree_path_new_from_string(ptr);
      gtk_tree_model_get_iter(model, &iter, path);
      gtk_tree_store_append(store, &iter2, &iter);
      gtk_tree_path_free(path);
      gtk_tree_store_set(store, &iter2, 0, it->second->GetName().c_str(), 1,  it->second->GetIcon(), 2, visibility, -1);
    }
    free(ptr);
    it++;
  }
}
