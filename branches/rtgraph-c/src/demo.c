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
  printf("Created an empty temporary page storage\n");

  foo = rtg_page_storage_alloc(storage);
  printf("Allocated page %d\n", foo);

  strcpy(rtg_page_storage_lookup(storage, foo), "Boing");
  printf("Filled page with stuff\n");

  rtg_page_storage_add_name(storage, "foo", foo);
  printf("Stored a named pointer to the page\n");

  return 0;
}

/* The End */
