#include "map.h"

Map::Map(std::string name) : Object(name)
{
  pixbuf = gdk_pixbuf_new_from_file("data/mapicon.png", NULL);
}

bool Map::Init()
{
  size = 400;
  return true;
}

void Map::Render(int name)
{
}

bool Map::setItemInfo(std::string name, float data)
{
  if(name == "size") {
    size = data;
    return true;
  }
  return Object::setItemInfo(name, data);
}

bool Map::getItemInfo(std::string name, float &data)
{
  if(name == "size") {
    data = size;
    return true;
  }
  return Object::getItemInfo(name, data);
}
