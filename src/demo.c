/* -*- mode: c; c-basic-offset: 4; -*- */

/*  wget http://navi.cx/releases/therm-dataset-1.bz2
 *  bunzip2 therm-dataset-1.bz2
 */

#include <config.h>
#include <glib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "pagestorage.h"
#include "bptree.h"


gint compare_int_func(gconstpointer a, gconstpointer b, gpointer user_data)
{
    return (*(const int*)a) - (*(const int*)b);
}

gboolean foreach_func(int* key, gdouble* value, gpointer user_data)
{
    printf("%d = %f\n", *key, *value);
    return FALSE;
}

int main(int argc, char** argv)
{
    RtgPageStorage* storage;
    RtgBPTree* tree;
    int key;
    gdouble value;

    storage = rtg_page_storage_mapped_new("foo.rtg", 0);

    tree = rtg_bptree_new(storage, "My Tree", sizeof(key), sizeof(value),
			  compare_int_func, NULL);

    key = time(NULL);
    value = 486;
    rtg_bptree_insert(tree, &key, &value, NULL);

    /* Show the tree's contents */
    rtg_bptree_foreach(tree, (RtgBPTreeCallback) foreach_func, NULL);

    rtg_bptree_close(tree);
    rtg_page_storage_close(storage);
    return 0;
}

/* The End */
