/* X-Chat
 * Copyright (C) 1998 Peter Zelezny.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */
#define _FILE_OFFSET_BITS 64 /* allow selection of large files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <gtk/gtk.h>
#include "../common/cfgfiles.h"
#include "gtkutil.h"
#include "pixmaps.h"

/* gtkutil.c, just some gtk wrappers */

extern void path_part (char *file, char *path, int pathlen);

struct file_req
{
	GtkWidget *dialog;
	void *userdata;
	filereqcallback callback;
	int write;
};

static char last_dir[256] = "";


GtkWidget *
gtkutil_simpledialog (char *msg)
{
	GtkWidget *dialog;

	dialog = gtk_message_dialog_new (NULL, 0, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "%s", msg);
	g_signal_connect (G_OBJECT (dialog), "response", G_CALLBACK (gtk_widget_destroy), 0);
	gtk_window_set_resizable (GTK_WINDOW (dialog), FALSE);
	gtk_window_set_position (GTK_WINDOW (dialog), GTK_WIN_POS_MOUSE);
	gtk_widget_show (dialog);

	return dialog;
}

static void
gtkutil_file_req_destroy (GtkWidget * wid, struct file_req *freq)
{
	freq->callback (freq->userdata, NULL);
	free (freq);
}

static int
gtkutil_check_file (char *file, struct file_req *freq)
{
	struct stat st;
	int axs = FALSE;

	path_part (file, last_dir, sizeof (last_dir));

	if (stat (file, &st) != -1)
	{
		if (S_ISDIR (st.st_mode))
		{
			char *tmp = malloc (strlen (file) + 2);

			strcpy (tmp, file);

			if (tmp[strlen(tmp)-1] != '/')
				strcat (tmp, "/");
#if GTK_CHECK_VERSION(2,4,0)
			gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (freq->dialog), tmp);
#else
			gtk_file_selection_set_filename (GTK_FILE_SELECTION (freq->dialog), tmp);
#endif
			free (tmp);
			return 0;
		}
	}
	if (freq->write)
	{
		if (access (last_dir, W_OK) == 0)
			axs = TRUE;
	} else
	{
		if (stat (file, &st) != -1)
		{
			if (!S_ISDIR (st.st_mode))
				axs = TRUE;
		}
	}

	if (axs)
	{
		char *utf8_file;
		/* convert to UTF8. It might be converted back to locale by
			server.c's g_convert */
		utf8_file = g_filename_to_utf8 (file, -1, NULL, NULL, NULL);
		freq->callback (freq->userdata, utf8_file);
		g_free (utf8_file);
	} else
	{
		if (freq->write)
			gtkutil_simpledialog (_("Cannot write to that file."));
		else
			gtkutil_simpledialog (_("Cannot read that file."));
	}

	return 1;
}

static void
gtkutil_file_req_done (GtkWidget * wid, struct file_req *freq)
{
#if GTK_CHECK_VERSION(2,4,0)
	int kill = 0;
	GSList *files, *cur;
	GtkFileChooser *fs = GTK_FILE_CHOOSER (freq->dialog);

	if (gtk_file_chooser_get_select_multiple (fs))
	{
		files = cur = gtk_file_chooser_get_filenames (fs);
		while (cur)
		{
			kill |= gtkutil_check_file (cur->data, freq);
			g_free (cur->data);
			cur = cur->next;
		}
		if (files)
			g_slist_free (files);
	} else
	{
		kill = gtkutil_check_file (gtk_file_chooser_get_filename (fs), freq);
	}
#else
	int i = 0, kill = 0;
	gchar **files;
	GtkFileSelection *fs = GTK_FILE_SELECTION (freq->dialog);

	if (gtk_file_selection_get_select_multiple (fs))
	{
		files = gtk_file_selection_get_selections (fs);
		if (files)
		{
			while (files[i])
			{
				kill |= gtkutil_check_file (files[i], freq);
				i++;
			}
			g_strfreev (files);
		}
	} else
	{
		kill |= gtkutil_check_file ((char *)gtk_file_selection_get_filename (fs), freq);
	}
#endif

	if (kill)	/* this should call the "destroy" cb, where we free(freq) */
		gtk_widget_destroy (freq->dialog);
}

#if GTK_CHECK_VERSION(2,4,0)

static void
gtkutil_file_req_response (GtkWidget *dialog, gint res, struct file_req *freq)
{
	switch (res)
	{
	case GTK_RESPONSE_ACCEPT:
		gtkutil_file_req_done (dialog, freq);
		break;

	case GTK_RESPONSE_CANCEL:
		/* this should call the "destroy" cb, where we free(freq) */
		gtk_widget_destroy (freq->dialog);
	}
}

#else

static void
gtkutil_file_req_cancel (GtkWidget * wid, struct file_req *freq)
{
	gtk_widget_destroy (freq->dialog);
}

#endif
 
void
gtkutil_file_req (char *title, void *callback, void *userdata, char *filter, int flags)
{
	struct file_req *freq;
	GtkWidget *dialog;

#if GTK_CHECK_VERSION(2,4,0)
	/* == GTK 2.4 uses the new GtkFileChooser, which is much nicer! */
	if (flags & FRF_WRITE)
		dialog = gtk_file_chooser_dialog_new (title, NULL, GTK_FILE_CHOOSER_ACTION_SAVE, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT, NULL);
	else
		dialog = gtk_file_chooser_dialog_new (title, NULL, GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, NULL);
	if (flags & FRF_MULTIPLE)
		gtk_file_chooser_set_select_multiple (GTK_FILE_CHOOSER (dialog), TRUE);
	if (last_dir[0])
		gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (dialog), last_dir);
	if (flags & FRF_ADDFOLDER)
		gtk_file_chooser_add_shortcut_folder (GTK_FILE_CHOOSER (dialog), (char *) get_xdir_fs (), NULL);

	freq = malloc (sizeof (struct file_req));
	freq->dialog = dialog;
	freq->write = (flags & FRF_WRITE);
	freq->callback = callback;
	freq->userdata = userdata;

	g_signal_connect (G_OBJECT (dialog), "response", G_CALLBACK (gtkutil_file_req_response), freq);

#else
	dialog = gtk_file_selection_new (title);
	if (flags & FRF_MULTIPLE)
		gtk_file_selection_set_select_multiple (GTK_FILE_SELECTION (dialog), TRUE);
	if (last_dir[0])
		gtk_file_selection_set_filename (GTK_FILE_SELECTION (dialog), last_dir);

	freq = malloc (sizeof (struct file_req));
	freq->dialog = dialog;
	freq->write = (flags & FRF_WRITE);
	freq->callback = callback;
	freq->userdata = userdata;

	g_signal_connect (G_OBJECT (GTK_FILE_SELECTION (dialog)->cancel_button), "clicked", G_CALLBACK (gtkutil_file_req_cancel), (gpointer) freq);
	g_signal_connect (G_OBJECT (GTK_FILE_SELECTION (dialog)->ok_button), "clicked", G_CALLBACK (gtkutil_file_req_done), (gpointer) freq);
#endif

	g_signal_connect (G_OBJECT (dialog), "destroy", G_CALLBACK (gtkutil_file_req_destroy), (gpointer) freq);
	gtk_widget_show (dialog);
}

void
gtkutil_destroy (GtkWidget * igad, GtkWidget * dgad)
{
	gtk_widget_destroy (dgad);
}

static void
gtkutil_get_str_response (GtkDialog *dialog, gint arg1, gpointer entry)
{
	void (*callback) (int cancel, char *text, void *user_data);
	char *text;
	void *user_data;

	text = (char *) gtk_entry_get_text (GTK_ENTRY (entry));
	callback = g_object_get_data (G_OBJECT (dialog), "cb");
	user_data = g_object_get_data (G_OBJECT (dialog), "ud");

	switch (arg1)
	{
	case GTK_RESPONSE_REJECT:
		callback (TRUE, text, user_data);
		gtk_widget_destroy (GTK_WIDGET (dialog));
		break;
	case GTK_RESPONSE_ACCEPT:
		callback (FALSE, text, user_data);
		gtk_widget_destroy (GTK_WIDGET (dialog));
		break;
	}
}

static void
gtkutil_str_enter (GtkWidget *entry, GtkWidget *dialog)
{
	gtk_dialog_response (GTK_DIALOG (dialog), GTK_RESPONSE_ACCEPT);
}

void
fe_get_str (char *msg, char *def, void *callback, void *userdata)
{
	GtkWidget *dialog;
	GtkWidget *entry;
	GtkWidget *hbox;
	GtkWidget *label;

	dialog = gtk_dialog_new_with_buttons (msg, NULL, 0, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, NULL);
	gtk_box_set_homogeneous (GTK_BOX (GTK_DIALOG (dialog)->vbox), TRUE);
	gtk_window_set_position (GTK_WINDOW (dialog), GTK_WIN_POS_MOUSE);
	hbox = gtk_hbox_new (TRUE, 0);

	g_object_set_data (G_OBJECT (dialog), "cb", callback);
	g_object_set_data (G_OBJECT (dialog), "ud", userdata);

	entry = gtk_entry_new ();
	g_signal_connect (G_OBJECT (entry), "activate", G_CALLBACK (gtkutil_str_enter), dialog);
	gtk_entry_set_text (GTK_ENTRY (entry), def);
	gtk_box_pack_end (GTK_BOX (hbox), entry, 0, 0, 0);

	label = gtk_label_new (msg);
	gtk_box_pack_end (GTK_BOX (hbox), label, 0, 0, 0);

	g_signal_connect (G_OBJECT (dialog), "response", G_CALLBACK (gtkutil_get_str_response), entry);

	gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dialog)->vbox), hbox);

	gtk_widget_show_all (dialog);
}

static void
gtkutil_get_number_response (GtkDialog *dialog, gint arg1, gpointer spin)
{
	void (*callback) (int cancel, int value, void *user_data);
	int num;
	void *user_data;

	num = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (spin));
	callback = g_object_get_data (G_OBJECT (dialog), "cb");
	user_data = g_object_get_data (G_OBJECT (dialog), "ud");

	switch (arg1)
	{
	case GTK_RESPONSE_REJECT:
		callback (TRUE, num, user_data);
		gtk_widget_destroy (GTK_WIDGET (dialog));
		break;
	case GTK_RESPONSE_ACCEPT:
		callback (FALSE, num, user_data);
		gtk_widget_destroy (GTK_WIDGET (dialog));
		break;
	}
}

void
fe_get_int (char *msg, int def, void *callback, void *userdata)
{
	GtkWidget *dialog;
	GtkWidget *spin;
	GtkWidget *hbox;
	GtkWidget *label;
	GtkAdjustment *adj;

	dialog = gtk_dialog_new_with_buttons (msg, NULL, 0, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, NULL);
	gtk_box_set_homogeneous (GTK_BOX (GTK_DIALOG (dialog)->vbox), TRUE);
	gtk_window_set_position (GTK_WINDOW (dialog), GTK_WIN_POS_MOUSE);
	hbox = gtk_hbox_new (TRUE, 0);

	g_object_set_data (G_OBJECT (dialog), "cb", callback);
	g_object_set_data (G_OBJECT (dialog), "ud", userdata);

	spin = gtk_spin_button_new (NULL, 1, 0);
	adj = gtk_spin_button_get_adjustment ((GtkSpinButton*)spin);
	adj->lower = 0;
	adj->upper = 1024;
	adj->step_increment = 1;
	gtk_adjustment_changed (adj);
	gtk_spin_button_set_value ((GtkSpinButton*)spin, def);
	gtk_box_pack_end (GTK_BOX (hbox), spin, 0, 0, 0);

	label = gtk_label_new (msg);
	gtk_box_pack_end (GTK_BOX (hbox), label, 0, 0, 0);

	g_signal_connect (G_OBJECT (dialog), "response", G_CALLBACK (gtkutil_get_number_response), spin);

	gtk_container_add (GTK_CONTAINER (GTK_DIALOG (dialog)->vbox), hbox);

	gtk_widget_show_all (dialog);
}

GtkWidget *
gtkutil_button (GtkWidget *box, char *stock, char *tip, void *callback,
				 void *userdata, char *labeltext)
{
	GtkWidget *wid, *img, *label, *bbox;

	wid = gtk_button_new ();
	g_signal_connect (G_OBJECT (wid), "clicked", G_CALLBACK (callback), userdata);

	bbox = gtk_hbox_new (0, 0);
	gtk_container_add (GTK_CONTAINER (wid), bbox);
	gtk_widget_show (bbox);

	if (stock)
	{
		img = gtk_image_new_from_stock (stock, GTK_ICON_SIZE_MENU);
		gtk_widget_set_usize (img, 8, 6);
		gtk_container_add (GTK_CONTAINER (bbox), img);
		gtk_widget_show (img);
	}

	if (labeltext)
	{
		label = gtk_label_new_with_mnemonic (labeltext);
		gtk_container_add (GTK_CONTAINER (bbox), label);
		gtk_widget_show (label);
		gtk_container_add (GTK_CONTAINER (box), wid);
	} else
	{
		gtk_box_pack_start (GTK_BOX (box), wid, 0, 0, 0);
	}

	gtk_widget_show (wid);
	if (tip)
		add_tip (wid, tip);

	return wid;
}

void
gtkutil_label_new (char *text, GtkWidget * box)
{
	GtkWidget *label = gtk_label_new (text);
	gtk_container_add (GTK_CONTAINER (box), label);
	gtk_widget_show (label);
}

GtkWidget *
gtkutil_entry_new (int max, GtkWidget * box, void *callback, gpointer userdata)
{
	GtkWidget *entry = gtk_entry_new_with_max_length (max);
	gtk_container_add (GTK_CONTAINER (box), entry);
	if (callback)
		g_signal_connect (G_OBJECT (entry), "changed", G_CALLBACK (callback), userdata);
	gtk_widget_show (entry);
	return entry;
}

void
add_tip (GtkWidget * wid, char *text)
{
	GtkTooltips *tip = gtk_tooltips_new ();
	gtk_tooltips_set_tip (tip, wid, text, 0);
}

void
show_and_unfocus (GtkWidget * wid)
{
	GTK_WIDGET_UNSET_FLAGS (wid, GTK_CAN_FOCUS);
	gtk_widget_show (wid);
}

/* pass NULL as selection to paste to both clipboard & X11 text */
void
gtkutil_copy_to_clipboard (GtkWidget *widget, GdkAtom selection, const gchar *str)
{
	GtkWidget *win;
	GtkClipboard *clip, *clip2;

	win = gtk_widget_get_toplevel (GTK_WIDGET (widget));
	if (GTK_WIDGET_TOPLEVEL (win))
	{
		glong len = g_utf8_strlen (str, -1);

		if (selection)
		{
#if (GTK_MAJOR_VERSION == 2) && (GTK_MINOR_VERSION == 0)
			gtk_clipboard_set_text (gtk_clipboard_get (selection), str, len);
		} else
		{
			clip = gtk_clipboard_get (GDK_SELECTION_PRIMARY);
			clip2 = gtk_clipboard_get (GDK_SELECTION_CLIPBOARD);
#else
			clip = gtk_widget_get_clipboard (win, selection);
			gtk_clipboard_set_text (clip, str, len);
		} else
		{
			/* copy to both primary X selection and clipboard */
			clip = gtk_widget_get_clipboard (win, GDK_SELECTION_PRIMARY);
			clip2 = gtk_widget_get_clipboard (win, GDK_SELECTION_CLIPBOARD);
#endif
			gtk_clipboard_set_text (clip, str, len);
			gtk_clipboard_set_text (clip2, str, len);
		}
	}
}
