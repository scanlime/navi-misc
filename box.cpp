#include "box.h"

int Box::number = 0;

Box::Box() : Object(Box::GetNewName())
{
  pixbuf = gdk_pixbuf_new_from_file("data/boxicon.png", NULL);
  wallmodel = roofmodel = -1;
  DefaultData();
}

Box::~Box()
{
  number--;
}

bool Box::Init()
{
  if(!(texman && modman))
    return false;

  BaseModel *mod;
  Point3D p; p.x = px; p.y = py; p.z = pz;
  Point3D s; s.x = sx; s.y = sy; s.z = sz;

  texture = texman->GetID("data/boxwall");
  if(wallmodel < 0)
    wallmodel = modman->newModel("boxsides");
  mod = modman->getModel(wallmodel);
  mod->setTexture(texture);
  mod->init(p, s, angle);

  texture = texman->GetID("data/roof");
  if(roofmodel < 0)
    roofmodel = modman->newModel("boxtops");
  mod = modman->getModel(roofmodel);
  mod->setTexture(texture);
  mod->init(p, s, angle);

  return true;
}

void Box::DefaultData()
{
  Object::DefaultData();
  px = py = pz = 0;
  sx = sy = 30; sz = 9.42;
  angle = 0;
}

void Box::Render(int name)
{
  if(modman) {
    modman->addDrawItem(wallmodel, false, false, name);
    modman->addDrawItem(roofmodel, false, false, name);
  }
}

std::string Box::GetNewName()
{
  number++;
  char name[30];
  sprintf(name, "box-%d", number);
  return std::string(name);
}

bool Box::setItemInfo(std::string name, float data)
{
  if(name == "px") {
    px = data;
    return true;
  } else if(name == "py") {
    py = data;
    return true;
  } else if(name == "pz") {
    pz = data;
    return true;
  } else if(name == "sx") {
    sx = data;
    return true;
  } else if(name == "sy") {
    sy = data;
    return true;
  } else if(name == "sz") {
    sz = data;
    return true;
  } else if(name == "rot") {
    angle = data;
    return true;
  }
  return Object::setItemInfo(name, data);
}

bool Box::getItemInfo(std::string name, float &data)
{
  if(name == "px") {
    data = px;
    return true;
  } else if(name == "py") {
    data = py;
    return true;
  } else if(name == "pz") {
    data = pz;
    return true;
  } else if(name == "sx") {
    data = sx;
    return true;
  } else if(name == "sy") {
    data = sy;
    return true;
  } else if(name == "sz") {
    data = sz;
    return true;
  } else if(name == "rot") {
    data = angle;
    return true;
  }
  return Object::getItemInfo(name, data);
}

GtkWidget *Box::getPositionDialog()
{
  GtkWidget *frame;
  GtkWidget *table;
  GtkWidget *x, *y, *z;
  GtkWidget *xpos, *ypos, *zpos;

  frame = gtk_frame_new("Position");
  table = gtk_table_new(3, 2, FALSE);
  x = gtk_label_new("X");
  y = gtk_label_new("Y");
  z = gtk_label_new("Z");
  xpos = gtk_spin_button_new_with_range(-400, 400, 0.1);
  ypos = gtk_spin_button_new_with_range(-400, 400, 0.1);
  zpos = gtk_spin_button_new_with_range(0, 10000, 0.1);

  gtk_table_attach_defaults(GTK_TABLE(table), x, 0, 1, 0, 1);
  gtk_table_attach_defaults(GTK_TABLE(table), y, 0, 1, 1, 2);
  gtk_table_attach_defaults(GTK_TABLE(table), z, 0, 1, 2, 3);
  gtk_table_attach_defaults(GTK_TABLE(table), xpos, 1, 2, 0, 1);
  gtk_table_attach_defaults(GTK_TABLE(table), ypos, 1, 2, 1, 2);
  gtk_table_attach_defaults(GTK_TABLE(table), zpos, 1, 2, 2, 3);

  gtk_container_add(GTK_CONTAINER(frame), table);
  gtk_widget_set_size_request(GTK_WIDGET(frame), 150, -1);
  gtk_widget_show_all(frame);
  return frame;
}

GtkWidget *Box::getRotationDialog()
{
  GtkWidget *frame;
  GtkWidget *rot;

  frame = gtk_frame_new("Rotation");
  rot = gtk_spin_button_new_with_range(0, 360, 0.1);
  gtk_container_add(GTK_CONTAINER(frame), rot);
  gtk_widget_set_size_request(GTK_WIDGET(frame), 150, -1);
  gtk_widget_show_all(frame);
  return frame;
}

GtkWidget *Box::getScaleDialog()
{
  GtkWidget *frame;
  GtkWidget *table;
  GtkWidget *x, *y, *z;
  GtkWidget *xpos, *ypos, *zpos;

  frame = gtk_frame_new("Scale");
  table = gtk_table_new(3, 2, FALSE);
  x = gtk_label_new("X");
  y = gtk_label_new("Y");
  z = gtk_label_new("Z");
  xpos = gtk_spin_button_new_with_range(0, 10000, 0.1);
  ypos = gtk_spin_button_new_with_range(0, 10000, 0.1);
  zpos = gtk_spin_button_new_with_range(0, 10000, 0.1);

  gtk_table_attach_defaults(GTK_TABLE(table), x, 0, 1, 0, 1);
  gtk_table_attach_defaults(GTK_TABLE(table), y, 0, 1, 1, 2);
  gtk_table_attach_defaults(GTK_TABLE(table), z, 0, 1, 2, 3);
  gtk_table_attach_defaults(GTK_TABLE(table), xpos, 1, 2, 0, 1);
  gtk_table_attach_defaults(GTK_TABLE(table), ypos, 1, 2, 1, 2);
  gtk_table_attach_defaults(GTK_TABLE(table), zpos, 1, 2, 2, 3);

  gtk_container_add(GTK_CONTAINER(frame), table);
  gtk_widget_set_size_request(GTK_WIDGET(frame), 150, -1);
  gtk_widget_show_all(frame);
  return frame;
}
