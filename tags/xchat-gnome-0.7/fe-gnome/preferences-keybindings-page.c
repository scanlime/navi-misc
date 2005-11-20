/* This program was written with lots of love under the GPL by Jonathan
 * Blandford <jrb@gnome.org>
 *
 * It was subsequently heartlessly stolen from the control-center keybindings
 * configger and impetuously pounded into xchat-gnome by Dan Kuester
 * (dkuester@colorado.edu).
 */

#include <config.h>

#include <libintl.h>
#include <string.h>
#include <gtk/gtk.h>
#include <gconf/gconf-client.h>
#include <gdk/gdkx.h>
#include <glade/glade.h>
#include <X11/Xatom.h>

#include "eggcellrendererkeys.h"
#include "gui.h"

#define MAX_ELEMENTS_BEFORE_SCROLLING 10

#if defined(__powerpc__) && defined (__linux__)
#define USE_FBLEVEL
#else
#undef USE_FBLEVEL
#endif

typedef struct
{
	const char *name;
	gint data;
} KeyListEntry;

const KeyListEntry navigation_key_list[] =
{
	{ "/apps/xchat/keybindings/GoPreviousNetwork", 0 },
	{ "/apps/xchat/keybindings/GoNextNetwork", 0 },
	{ "/apps/xchat/keybindings/GoPreviousDiscussion", 0 },
	{ "/apps/xchat/keybindings/GoNextDiscussion", 0 },
	{ NULL }
};
const KeyListEntry server_key_list[] =
{
	{ "/apps/xchat/keybindings/DiscussionClose", 0 },
	{ NULL }
};

enum
{
	DESCRIPTION_COLUMN,
	KEYENTRY_COLUMN,
	N_COLUMNS
};

typedef struct
{
	char *gconf_key;
	guint keyval;
	guint keycode;
	EggVirtualModifierType mask;
	gboolean editable;
	GtkTreeModel *model;
	guint gconf_cnxn;
	char *description;
} KeyEntry;

static void  reload_key_entries ();
static char* binding_name       (guint                   keyval,
				 guint			 keycode,
                                 EggVirtualModifierType  mask,
                                 gboolean                translate);

static GtkTreeModel*
get_real_model (GtkTreeView *tree_view)
{
	GtkTreeModel *model;
	GtkTreeModel *submodel;

	model	= gtk_tree_view_get_model (tree_view);

	if (model)
		submodel = gtk_tree_model_sort_get_model (GTK_TREE_MODEL_SORT (model));
	else
		submodel = NULL;

	return submodel;
}

static char*
binding_name (guint                   keyval,
	      guint		      keycode,
              EggVirtualModifierType  mask,
              gboolean                translate)
{
	if (keyval != 0 || keycode != 0)
		return egg_virtual_accelerator_name (keyval, keycode, mask);
	else
		return translate ? g_strdup (_("Disabled")) : g_strdup ("disabled");
}

static gboolean
binding_from_string (const char             *str,
                     guint                  *accelerator_key,
		     guint		    *keycode,
                     EggVirtualModifierType *accelerator_mods)
{
	g_return_val_if_fail (accelerator_key != NULL, FALSE);

	if (str == NULL || (str && strcmp (str, "disabled") == 0)) {
		*accelerator_key = 0;
		*keycode = 0;
		*accelerator_mods = 0;
		return TRUE;
	}

	egg_accelerator_parse_virtual (str, accelerator_key, keycode, accelerator_mods);

	if (*accelerator_key == 0)
		return FALSE;
	else
		return TRUE;
}

static void
accel_set_func (GtkTreeViewColumn *tree_column,
                GtkCellRenderer   *cell,
                GtkTreeModel      *model,
                GtkTreeIter       *iter,
                gpointer           data)
{
	KeyEntry *key_entry;

	gtk_tree_model_get (model, iter, KEYENTRY_COLUMN, &key_entry, -1);

	if (key_entry == NULL)
		g_object_set (G_OBJECT (cell),
			      "visible", FALSE,
			      NULL);
	else if (! key_entry->editable)
		g_object_set (G_OBJECT (cell),
			      "visible", TRUE,
			      "editable", FALSE,
			      "accel_key", key_entry->keyval,
			      "accel_mask", key_entry->mask,
			      "keycode", key_entry->keycode,
			      "style", PANGO_STYLE_ITALIC,
			      NULL);
	else
		g_object_set (G_OBJECT (cell),
			      "visible", TRUE,
			      "editable", TRUE,
			      "accel_key", key_entry->keyval,
			      "accel_mask", key_entry->mask,
			      "keycode", key_entry->keycode,
			      "style", PANGO_STYLE_NORMAL,
			      NULL);
}

static gboolean
keybinding_key_changed_foreach (GtkTreeModel *model,
				GtkTreePath  *path,
				GtkTreeIter  *iter,
				gpointer      user_data)
{
	KeyEntry *key_entry;
	KeyEntry *tmp_key_entry;

	key_entry = (KeyEntry *)user_data;
	gtk_tree_model_get (key_entry->model, iter, KEYENTRY_COLUMN, &tmp_key_entry, -1);

	if (key_entry == tmp_key_entry) {
		gtk_tree_model_row_changed (key_entry->model, path, iter);
		return TRUE;
	}
	return FALSE;
}

static void
keybinding_key_changed (GConfClient *client,
			guint        cnxn_id,
			GConfEntry  *entry,
			gpointer     user_data)
{
	KeyEntry *key_entry;
	const gchar *key_value;

	key_entry = (KeyEntry *)user_data;
	key_value = gconf_value_get_string (entry->value);

	binding_from_string (key_value, &key_entry->keyval, &key_entry->keycode, &key_entry->mask);
	key_entry->editable = gconf_entry_get_is_writable (entry);

	/* update the model */
	gtk_tree_model_foreach (key_entry->model, keybinding_key_changed_foreach, key_entry);
}

static int
keyentry_sort_func (GtkTreeModel *model,
                    GtkTreeIter  *a,
                    GtkTreeIter  *b,
                    gpointer      user_data)
{
	KeyEntry *key_entry_a;
	KeyEntry *key_entry_b;
	char *name_a;
	char *name_b;
	int retval;

	/* mmmmm, super-slow. */

	key_entry_a = NULL;
	gtk_tree_model_get (model, a,
			    KEYENTRY_COLUMN, &key_entry_a,
			    -1);

	key_entry_b = NULL;
	gtk_tree_model_get (model, b,
			    KEYENTRY_COLUMN, &key_entry_b,
			    -1);


	if (key_entry_a != NULL)
		name_a = binding_name (key_entry_a->keyval,
				       key_entry_a->keycode,
				       key_entry_a->mask,
				       TRUE);
	else
		name_a = NULL;

	if (key_entry_b != NULL)
		name_b = binding_name (key_entry_b->keyval,
				       key_entry_b->keycode,
				       key_entry_b->mask,
				       TRUE);
	else
		name_b = NULL;

	if (name_a && name_b)
		retval = g_utf8_collate (name_a, name_b);
	else if (name_a)
		retval = 1;
	else if (name_b)
		retval = -1;
	else
		retval = 0;

	g_free (name_a);
	g_free (name_b);

	return retval;
}

static void
clear_old_model (GtkWidget *tree_view)
{
	GtkTreeModel *model;
	GtkTreeModel *sort_model;
	GtkTreeIter iter;
	KeyEntry *key_entry;
	gboolean valid;
	GConfClient *client;

	client = gconf_client_get_default ();
	model = get_real_model (GTK_TREE_VIEW (tree_view));

	if (model != NULL)
    {
		g_object_ref (model);

		for (valid = gtk_tree_model_get_iter_first (model, &iter);
		     valid;
		     valid = gtk_tree_model_iter_next (model, &iter)) {
			gtk_tree_model_get (model, &iter, KEYENTRY_COLUMN, &key_entry, -1);
			if (key_entry != NULL) {
				gconf_client_notify_remove (client, key_entry->gconf_cnxn);
				g_free (key_entry->gconf_key);
				g_free (key_entry->description);
				g_free (key_entry);
			}
		}
		g_object_unref (model);
	}

	model = (GtkTreeModel *) gtk_tree_store_new (N_COLUMNS, G_TYPE_STRING, G_TYPE_POINTER);

	sort_model = gtk_tree_model_sort_new_with_model (model);

	/* N_COLUMNS is just a place to stick the extra sort function */
	gtk_tree_sortable_set_sort_func (GTK_TREE_SORTABLE (sort_model),
					 N_COLUMNS,
					 keyentry_sort_func,
					 NULL, NULL);

	gtk_tree_view_set_model (GTK_TREE_VIEW (tree_view), sort_model);

	g_object_unref (G_OBJECT (model));
	g_object_unref (G_OBJECT (sort_model));

	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (glade_xml_get_widget (gui.xml, "actions_swindow")),
					GTK_POLICY_NEVER, GTK_POLICY_NEVER);
	gtk_widget_set_usize (glade_xml_get_widget (gui.xml,"actions_swindow"), -1, -1);
}

static gboolean
count_rows_foreach (GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer data)
{
	gint *rows = data;

	(*rows)++;

	return FALSE;
}

static void
append_keys_to_tree (const gchar        *title,
		     const KeyListEntry *keys_list)
{
	GConfClient *client;
	GtkTreeIter parent_iter;
	GtkTreeModel *model;
	gint i, j;

	client = gconf_client_get_default ();
	model = get_real_model (GTK_TREE_VIEW (glade_xml_get_widget (gui.xml, "shortcut_treeview")));

	i = 0;
	gtk_tree_model_foreach (model, count_rows_foreach, &i);

	gtk_tree_store_append (GTK_TREE_STORE (model), &parent_iter, NULL);
	gtk_tree_store_set (GTK_TREE_STORE (model), &parent_iter, DESCRIPTION_COLUMN, title, -1);

	for (j = 0; keys_list[j].name != NULL; j++) {
		GConfEntry *entry;
		GConfSchema *schema = NULL;
		KeyEntry *key_entry;
		GError *error = NULL;
		GtkTreeIter iter;
		const gchar *key_string;
		gchar *key_value;

		key_string = keys_list[j].name;

		entry = gconf_client_get_entry (client,
						key_string,
						NULL,
						TRUE,
						&error);
		if (error || entry == NULL) {
			/* We don't actually want to popup a dialog - just skip this one */
			if (error)
				g_error_free (error);
			continue;
		}

		if (gconf_entry_get_schema_name (entry))
			schema = gconf_client_get_schema (client, gconf_entry_get_schema_name (entry), &error);

		if (error || schema == NULL) {
			/* We don't actually want to popup a dialog - just skip this one */
			if (error)
				g_error_free (error);
			continue;
		}

		key_value = gconf_client_get_string (client, key_string, &error);

		key_entry = g_new0 (KeyEntry, 1);
		key_entry->gconf_key = g_strdup (key_string);
		key_entry->editable = gconf_entry_get_is_writable (entry);
		key_entry->model = model;
		gconf_client_add_dir (client, key_string, GCONF_CLIENT_PRELOAD_ONELEVEL, NULL);
		key_entry->gconf_cnxn = gconf_client_notify_add (client,
								 key_string,
								 (GConfClientNotifyFunc) &keybinding_key_changed,
								 key_entry, NULL, NULL);
		binding_from_string (key_value, &key_entry->keyval, &key_entry->keycode, &key_entry->mask);
		g_free (key_value);
		key_entry->description = g_strdup (gconf_schema_get_short_desc (schema));

		if (i == MAX_ELEMENTS_BEFORE_SCROLLING) {
			GtkRequisition rectangle;
			gtk_widget_ensure_style (glade_xml_get_widget (gui.xml, "shortcut_treeview"));
			gtk_widget_size_request (glade_xml_get_widget (gui.xml, "shortcut_treeview"), &rectangle);
			gtk_widget_set_size_request (glade_xml_get_widget (gui.xml, "shortcut_treeview"), -1, rectangle.height);
			gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (glade_xml_get_widget (gui.xml, "actions_swindow")),
							GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
		}
		i++;
		gtk_tree_store_append (GTK_TREE_STORE (model), &iter, &parent_iter);
		if (gconf_schema_get_short_desc (schema))
			gtk_tree_store_set (GTK_TREE_STORE (model), &iter,
					    DESCRIPTION_COLUMN,
					    key_entry->description,
					    KEYENTRY_COLUMN, key_entry,
					    -1);
		else
			gtk_tree_store_set (GTK_TREE_STORE (model), &iter,
					    DESCRIPTION_COLUMN, _("<Unknown Action>"),
					    KEYENTRY_COLUMN, key_entry,
					    -1);
		gtk_tree_view_expand_all (GTK_TREE_VIEW (glade_xml_get_widget (gui.xml, "shortcut_treeview")));
		gconf_entry_free (entry);
		gconf_schema_free (schema);
	}

	if (i == 0)
		gtk_widget_hide (glade_xml_get_widget (gui.xml, "shortcuts_vbox"));
	else
		gtk_widget_show (glade_xml_get_widget (gui.xml, "shortcuts_vbox"));
}

static void
reload_key_entries ()
{
	clear_old_model (glade_xml_get_widget (gui.xml, "shortcut_treeview"));

	append_keys_to_tree (_("Navigation"), navigation_key_list);
	append_keys_to_tree (_("Server"), server_key_list);
}

static void
key_entry_controlling_key_changed (GConfClient *client,
				   guint        cnxn_id,
				   GConfEntry  *entry,
				   gpointer     user_data)
{
	reload_key_entries (user_data);
}

static gboolean
cb_check_for_uniqueness (GtkTreeModel *model,
			 GtkTreePath  *path,
			 GtkTreeIter  *iter,
			 KeyEntry *new_key)
{
	KeyEntry *element;

	gtk_tree_model_get (new_key->model, iter, KEYENTRY_COLUMN, &element, -1);

	/* no conflict for : blanks, different modifiers, or ourselves */
	if (element == NULL || new_key->mask != element->mask || !strcmp (new_key->gconf_key, element->gconf_key))
		return FALSE;

	if (new_key->keyval != 0) {
		if (new_key->keyval != element->keyval)
			return FALSE;
	} else if (element->keyval != 0 || new_key->keycode != element->keycode) {
		return FALSE;
	}

	new_key->editable = FALSE;
	new_key->gconf_key = element->gconf_key;
	new_key->description = element->description;
	return TRUE;
}

static void
accel_edited_callback (GtkCellRendererText   *cell,
                       const char            *path_string,
                       guint                  keyval,
                       EggVirtualModifierType mask,
		       guint		      keycode,
                       gpointer               data)
{
	GConfClient *client;
	GtkTreeView *view = (GtkTreeView *)data;
	GtkTreeModel *model;
	GtkTreePath *path = gtk_tree_path_new_from_string (path_string);
	GtkTreeIter iter;
	KeyEntry *key_entry, tmp_key;
	GError *err = NULL;
	char *str;

	model = gtk_tree_view_get_model (view);
	gtk_tree_model_get_iter (model, &iter, path);
	gtk_tree_path_free (path);
	gtk_tree_model_get (model, &iter, KEYENTRY_COLUMN, &key_entry, -1);

	/* sanity check */
	if (key_entry == NULL)
		return;

	model = get_real_model (view);
	tmp_key.model  = model;
	tmp_key.keyval = keyval;
	tmp_key.keycode = keycode;
	tmp_key.mask   = mask;
	tmp_key.gconf_key = key_entry->gconf_key;
	tmp_key.description = NULL;
	tmp_key.editable = TRUE; /* kludge to stuff in a return flag */

	if (keyval != 0 || keycode != 0) /* any number of keys can be disabled */
		gtk_tree_model_foreach (model, (GtkTreeModelForeachFunc) cb_check_for_uniqueness, &tmp_key);

	/* flag to see if the new accelerator was in use by something */
	if (!tmp_key.editable) {
		GtkWidget *dialog;
		char *name;

		name = egg_virtual_accelerator_name (keyval, keycode, mask);

		dialog = gtk_message_dialog_new (GTK_WINDOW (gtk_widget_get_toplevel (GTK_WIDGET (view))),
						 GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL,
						 GTK_MESSAGE_WARNING,
						 GTK_BUTTONS_CANCEL,
						 _("The shortcut \"%s\" is already used for:\n \"%s\"\n"),
						 name,
						 tmp_key.description ?
						 tmp_key.description : tmp_key.gconf_key);
		g_free (name);
		gtk_dialog_run (GTK_DIALOG (dialog));
		gtk_widget_destroy (dialog);

		/* set it back to its previous value. */
		egg_cell_renderer_keys_set_accelerator (EGG_CELL_RENDERER_KEYS (cell),
							key_entry->keyval,
							key_entry->keycode,
							key_entry->mask);
		return;
	}

	str = binding_name (keyval, keycode, mask, FALSE);

	client = gconf_client_get_default();
	gconf_client_set_string (client, key_entry->gconf_key, str, &err);
	g_free (str);
	g_object_unref (G_OBJECT (client));

	if (err != NULL) {
		GtkWidget *dialog;

		dialog = gtk_message_dialog_new (GTK_WINDOW (gtk_widget_get_toplevel (GTK_WIDGET (view))),
						 GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL,
						 GTK_MESSAGE_WARNING,
						 GTK_BUTTONS_OK,
						 _("Error setting new accelerator in configuration database: %s\n"),
						 err->message);
		gtk_dialog_run (GTK_DIALOG (dialog));

		gtk_widget_destroy (dialog);
		g_error_free (err);
		key_entry->editable = FALSE;
	}
}

static void
accel_cleared_callback (GtkCellRendererText *cell,
			const char          *path_string,
			gpointer             data)
{
	GConfClient *client;
	GtkTreeView *view = (GtkTreeView *) data;
	GtkTreePath *path = gtk_tree_path_new_from_string (path_string);
	KeyEntry *key_entry;
	GtkTreeIter iter;
	GError *err = NULL;
	GtkTreeModel *model;

	model = gtk_tree_view_get_model (view);
	gtk_tree_model_get_iter (model, &iter, path);
	gtk_tree_path_free (path);
	gtk_tree_model_get (model, &iter, KEYENTRY_COLUMN, &key_entry, -1);

	/* sanity check */
	if (key_entry == NULL)
		return;

	/* Unset the key */
	client = gconf_client_get_default();
	gconf_client_set_string (client, key_entry->gconf_key, "disabled", &err);
	g_object_unref (G_OBJECT (client));

	if (err != NULL) {
		GtkWidget *dialog;

		dialog = gtk_message_dialog_new (GTK_WINDOW (gtk_widget_get_toplevel (GTK_WIDGET (view))),
						 GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL,
						 GTK_MESSAGE_WARNING,
						 GTK_BUTTONS_OK,
						 _("Error unsetting accelerator in configuration database: %s\n"),
						 err->message);
		gtk_dialog_run (GTK_DIALOG (dialog));

		gtk_widget_destroy (dialog);
		g_error_free (err);
		key_entry->editable = FALSE;
	}
}


typedef struct
{
	GtkTreeView *tree_view;
	GtkTreePath *path;
} IdleData;

static gboolean
real_start_editing_cb (IdleData *idle_data)
{
	gtk_widget_grab_focus (GTK_WIDGET (idle_data->tree_view));
	gtk_tree_view_set_cursor (idle_data->tree_view,
				  idle_data->path,
				  gtk_tree_view_get_column (idle_data->tree_view, 1),
				  TRUE);

	gtk_tree_path_free (idle_data->path);
	g_free (idle_data);
	return FALSE;
}

static gboolean
start_editing_cb (GtkTreeView    *tree_view,
		  GdkEventButton *event)
{
	GtkTreePath *path;

	if (event->window != gtk_tree_view_get_bin_window (tree_view))
		return FALSE;

	if (gtk_tree_view_get_path_at_pos (tree_view, (gint) event->x, (gint) event->y, &path, NULL, NULL, NULL)) {
		IdleData *idle_data;

		if (gtk_tree_path_get_depth (path) == 1) {
			gtk_tree_path_free (path);
			return FALSE;
		}

		idle_data = g_new (IdleData, 1);
		idle_data->tree_view = tree_view;
		idle_data->path = path;
		g_signal_stop_emission_by_name (G_OBJECT (tree_view), "button_press_event");
		g_idle_add ((GSourceFunc) real_start_editing_cb, idle_data);
	}
	return TRUE;
}

void
initialize_preferences_keybindings_page ()
{
	GConfClient *client;
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	GtkWidget *widget;


	client = gconf_client_get_default ();

	g_signal_connect (GTK_TREE_VIEW (glade_xml_get_widget (gui.xml, "shortcut_treeview")),
			  "button_press_event",
			  G_CALLBACK (start_editing_cb), gui.xml),

	column = gtk_tree_view_column_new_with_attributes (_("Action"),
							   gtk_cell_renderer_text_new (),
							   "text", DESCRIPTION_COLUMN,
							   NULL);
	gtk_tree_view_column_set_resizable (column, FALSE);

	gtk_tree_view_append_column (GTK_TREE_VIEW (glade_xml_get_widget (gui.xml, "shortcut_treeview")), column);
	gtk_tree_view_column_set_sort_column_id (column, DESCRIPTION_COLUMN);

	renderer = (GtkCellRenderer *) g_object_new (EGG_TYPE_CELL_RENDERER_KEYS,
						     "editable", TRUE,
						     "accel_mode", EGG_CELL_RENDERER_KEYS_MODE_X,
						     NULL);

	g_signal_connect (G_OBJECT (renderer),
			  "accel_edited",
			  G_CALLBACK (accel_edited_callback),
			  glade_xml_get_widget (gui.xml, "shortcut_treeview"));

	g_signal_connect (G_OBJECT (renderer),
			  "accel_cleared",
			  G_CALLBACK (accel_cleared_callback),
			  glade_xml_get_widget (gui.xml, "shortcut_treeview"));

	column = gtk_tree_view_column_new_with_attributes (_("Shortcut"), renderer, NULL);
	gtk_tree_view_column_set_cell_data_func (column, renderer, accel_set_func, NULL, NULL);
	gtk_tree_view_column_set_resizable (column, FALSE);

	gtk_tree_view_append_column (GTK_TREE_VIEW (glade_xml_get_widget (gui.xml, "shortcut_treeview")), column);
	/* N_COLUMNS is just a place to stick the extra sort function */
	gtk_tree_view_column_set_sort_column_id (column, N_COLUMNS);

	gconf_client_add_dir (client, "/apps/gnome_keybinding_properties", GCONF_CLIENT_PRELOAD_ONELEVEL, NULL);
	gconf_client_add_dir (client, "/apps/metacity/general", GCONF_CLIENT_PRELOAD_ONELEVEL, NULL);
	gconf_client_notify_add (client,
				 "/apps/metacity/general/num_workspaces",
				 (GConfClientNotifyFunc) &key_entry_controlling_key_changed,
				 gui.xml, NULL, NULL);

	/* set up the dialog */
	reload_key_entries (gui.xml);

	widget = glade_xml_get_widget (gui.xml, "shortcut_treeview");
	gtk_widget_show (widget);

	g_object_unref (client);
}

