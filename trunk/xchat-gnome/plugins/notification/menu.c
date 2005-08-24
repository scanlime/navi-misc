/*
 * menu.c - Context menu for the notification plugin.
 *
 * Copyright (C) 2005 W. Evan Sheehan
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

static void channel_menu_init       (ChannelMenu* menu);
static void channel_menu_class_init (ChannelMenuClass* klass);
static void channel_menu_dispose    (GObject* object);
static void channel_menu_finalize   (GObject* object);

GType
channel_menu_get_type (void)
{
	static GType channel_menu_type = 0;

	/* Register the type only once. */
	if (!channel_menu_type) {
		static const GTypeInfo channel_menu_info = {
			sizeof (ChannelMenuClass),
			NULL, /* base init. */
			NULL, /* base finalize. */
			(GClassInitFunc) channel_menu_class_init,
			NULL, /* class finalize. */
			NULL, /* class data. */
			sizeof (ChannelMenu),
			0,    /* n_preallocs. */
			(GInstanceInitFunc) channel_menu_init,
		};

		/* Register the type. */
		channel_menu_type = g_type_register_static (GTK_TYPE_MENU, "ChannelMenu", &channel_menu_info, 0);
	}

	/* Return the type. */
	return channel_menu_type;
}

static void
channel_menu_init (ChannelMenu* menu)
{
	menu->channels = g_hash_table_new (g_str_hash, g_str_equal);
}

static void
channel_menu_class_init (ChannelMenuClass* klass)
{
	GObjectClass* object_class = (GObjectClass*) klass;

	object_class->dispose = channel_menu_dispose;
	object_class->finalize = channel_menu_finalize;
}

static gboolean
destroy_channels (gchar* channel, ChannelMenuItem* item, gpointer data)
{
	g_free (channel);
	g_object_unref (item);
	return TRUE;
}

static void
channel_menu_dispose (GObject* object)
{
	/* Remove all the channels from the hash table. */
	ChannelMenu* menu = (ChannelMenu*) object;
	g_hash_table_foreach_remove (menu->channels, (GHRFunc) destroy_channels, NULL);
}

static void
channel_menu_finalize (GObject* object)
{
	ChannelMenu* menu = (ChannelMenu*) object;
	g_hash_table_destroy (menu->channels);
}

void
channel_menu_item_add (gchar* name)
{
}

void
channel_menu_remove (gchar* name)
{
}

void
channel_menu_update (gchar* name, gint status)
{
}


void
channel_menu_item_set_status (gint status)
{
}
