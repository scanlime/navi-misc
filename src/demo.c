#include <config.h>
#include <gtk/gtk.h>
#include <string.h>
#include "pagestorage.h"

int main(int argc, char** argv)
{
  RtgPageStorage* storage;
  RtgPageAddress foo;

  gtk_init(&argc, &argv);

  storage = rtg_page_storage_temp_new(0);

  foo = rtg_page_storage_alloc(storage);
  strcpy(rtg_page_storage_lookup(storage, foo), "Boing");

  rtg_page_storage_add_name(storage, "foo", foo);

  return 0;
}

/* The End */
