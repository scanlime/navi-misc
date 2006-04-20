/* xscreensaver.c
 *
 * Copyright (C) 2005 Isak Savo <isak.savo@gmail.com>
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
#include <config.h>
#include <string.h>
#include "autoaway.h"
#include "xscreensaver.h"

/* Use xprop to find out if screensaver is on or off */
static gboolean
get_screensaver_running_xprop (void)
{
	gchar *cmd = "xprop -f _SCREENSAVER_STATUS 32ac -root _SCREENSAVER_STATUS";
	gchar *stdout = NULL, *ptr;
	gboolean rv = FALSE;

	if (g_spawn_command_line_sync (cmd, &stdout, NULL, NULL, NULL)) {
		g_strchomp (stdout);
		ptr = strstr (stdout, " = ");
		if (ptr != NULL) {
			ptr += 3;
			if ((strncmp (ptr, "BLANK", 5) == 0 || strncmp (ptr, "LOCK", 4) == 0))
				rv = TRUE;
		}
	}
	trace ("xprop_screensaver, returning %s", rv ? "TRUE" : "FALSE");
	g_free (stdout);
	return rv;
}

/* Use xscreensaver-command to find out if screensaver is on or off */
static gboolean
get_screensaver_running_xs_cmd (void)
{
	gchar *cmd = "xscreensaver-command --time";
	gchar *stdout = NULL, *ptr;
	gboolean rv = FALSE;

	if (g_spawn_command_line_sync (cmd, &stdout, NULL, NULL, NULL)) {
		ptr = strstr (stdout, " screen ");
		if (ptr != NULL) {
			ptr += 8;
			if ((strncmp (ptr, "blanked", 7) == 0 || strncmp (ptr, "locked", 6) == 0))
				rv = TRUE;
		}
	}
	g_free (stdout);

	trace ("xscmd_screensaver, returning %s", rv ? "TRUE" : "FALSE");

	return rv;
}

/***********************************
 * PUBLIC
 ***********************************/
gboolean
get_xss_has_ipc (void)
{
	if (g_find_program_in_path ("xprop") ||
	    g_find_program_in_path ("xscreensaver-command"))
	    return TRUE;

	return FALSE;
}

gboolean
get_xss_screensaver_active (void)
{
	if (g_find_program_in_path ("xprop"))
		return get_screensaver_running_xprop();
	else if (g_find_program_in_path ("xscreensaver-command"))
		return get_screensaver_running_xs_cmd();
	return FALSE;
}
