/*
 * plugins.h - Load all of the plugins in the plugins/ directory
 *
 * BZEdit
 * Copyright (C) 2004 David Trowbridge
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#include "plugins.h"

void
load_plugins (void)
{
  GDir *dir;
  const gchar *file;
  GModule *plugin;

  if (!g_module_supported())
    return;

  dir = g_dir_open ("src/plugins", 0, NULL);
  if (dir == NULL)
    return;

  while (file = g_dir_read_name (dir))
  {
    gchar *full = g_strdup_printf("src/plugins/%s", file);
    plugin = g_module_open (full, 0);
    if (plugin != NULL)
      g_print("loaded plugin '%s'\n", file);
    /*
    else
      g_print("%s\n", g_module_error ());
    */
    g_free(full);
  }
  g_print ("\n");

  g_dir_close (dir);
}

GList*
find_type_leaves (GType base)
{
  GType *children, *t;
  guint n, i, c;
  GList *ret = NULL;

  children = g_type_children (base, &n);
  for (i = 0; i < n; i++)
  {
    t = g_type_children (children[i], &c);
    if (c != 0)
      ret = g_list_concat (ret, find_type_leaves (children[i]));
    else {
      ret = g_list_append (ret, GUINT_TO_POINTER (children[i]));
    }
    g_free (t);
  }
  g_free (children);

  return ret;
}

GList*
find_type_children (GType base)
{
  GType *children, *t;
  guint n, i, c;
  GList *ret = NULL;

  children = g_type_children (base, &n);
  for (i = 0; i < n; i++)
  {
    t = g_type_children (children[i], &c);
    ret = g_list_append (ret, GUINT_TO_POINTER (children[i]));
    if (c != 0)
      ret = g_list_concat (ret, find_type_children (children[i]));
    g_free (t);
  }
  g_free (children);

  return ret;
}
