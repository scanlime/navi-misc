#include <gmodule.h>
#include "pyramid.h"

const gchar*
g_module_check_init (GModule *module)
{
  static GType pyramid;

  /* permanently insert ourself */
  g_module_make_resident (module);

  /* and register our types */
  pyramid = PYRAMID_TYPE;

  return NULL;
}
