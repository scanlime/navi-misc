#include <config.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include "pagestorage.h"
#include "bptree.h"


gint compare_int_func(gconstpointer _a, gconstpointer _b, gpointer)
{
  int a = *(int*) _a;
  int b = *(int*) _b;
  if (a<b)
    return -1;
  else if (a>b)
    return 1;
  else
    return 0;
}

int main(int argc, char** argv)
{
  RtgPageStorage* storage;
  RtgBPTree* tree;
  int key;
  gdouble value;

  gtk_init(&argc, &argv);

  storage = rtg_page_storage_mapped_new("foo.rtg", 0);

  tree = rtg_bptree_new(storage, "My Tree", sizeof(key), sizeof(value),
			compare_int_func, NULL);

  foo = rtg_page_storage_find_name("MyFooWidget Plus!"
  foo = rtg_page_storage_alloc(storage);
  printf("Allocated page %d\n", foo);

  strcpy(rtg_page_storage_lookup(storage, foo), "Boing");
  printf("Filled page with stuff\n");

  rtg_page_storage_add_name(storage, "foo", foo);
  printf("Stored a named pointer to the page\n");

  return 0;
}

/* The End */
