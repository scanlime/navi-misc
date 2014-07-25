/*
 * bzimporter.c - Importation of the BZFlag text map format
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

#include "bzimporter.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

static GScannerConfig parser_config =
{
  (" \t\r\n"),		/* cset_skip_characters */
  (
  G_CSET_a_2_z
  G_CSET_A_2_Z
  ),			/* cset_identifier_first */
  (
  G_CSET_a_2_z
  G_CSET_A_2_Z
  ),			/* cset_identifier_nth */
  ("#\n"),		/* cpair_comment_single */
  FALSE,		/* case_sensitive */
  FALSE,		/* skip_comment_multi */
  TRUE,			/* skip_comment_single */
  FALSE,		/* scan_comment_multi */
  TRUE,			/* scan_identifier */
  FALSE,		/* scan_identifier_1char */
  FALSE,		/* scan_identifier_NULL */
  TRUE,			/* scan_symbols */
  FALSE,		/* scan_binary */
  FALSE,		/* scan_octal */
  TRUE,			/* scan_float */
  FALSE,		/* scan_hex */
  FALSE,		/* scan_hex_dollar */
  FALSE,		/* scan_string_sq */
  FALSE,		/* scan_string_dq */
  FALSE,		/* numbers_2_int */
  TRUE,			/* int_2_float */
  FALSE,		/* identifier_2_string */
  FALSE,		/* char_2_token */
  TRUE,			/* symbol_2_token */
  FALSE,		/* scope_0_fallback */
};

enum
{
  BZ_TOKEN_INVALID = G_TOKEN_LAST,
  BZ_TOKEN_BOX,
  BZ_TOKEN_PYRAMID,
  BZ_TOKEN_TELEPORTER,
  BZ_TOKEN_LINK,
  BZ_TOKEN_BASE,
  BZ_TOKEN_WEAPON,
  BZ_TOKEN_ZONE,
  BZ_TOKEN_WORLD,
  BZ_TOKEN_POSITION,
  BZ_TOKEN_ROTATION,
  BZ_TOKEN_SIZE,
  BZ_TOKEN_BORDER,
  BZ_TOKEN_FROM,
  BZ_TOKEN_TO,
  BZ_TOKEN_END,
  BZ_TOKEN_LAST
};

enum
{
  BZ_SCOPE_DEFAULT = 0,
  BZ_SCOPE_BOX,
  BZ_SCOPE_PYRAMID,
  BZ_SCOPE_TELEPORTER,
  BZ_SCOPE_LINK,
  BZ_SCOPE_BASE,
  BZ_SCOPE_WEAPON,
  BZ_SCOPE_ZONE,
  BZ_SCOPE_WORLD,
};

static gboolean initialized = FALSE;
static GScanner *scanner;

/* these are the basic object types we need to know about */
static GType box, pyramid, teleporter;
/* GType base, link */

static void
initialize_scanner()
{
  scanner = g_scanner_new (&parser_config);

  g_scanner_scope_add_symbol (scanner, BZ_SCOPE_DEFAULT, "box",        GINT_TO_POINTER (BZ_TOKEN_BOX));
  g_scanner_scope_add_symbol (scanner, BZ_SCOPE_DEFAULT, "pyramid",    GINT_TO_POINTER (BZ_TOKEN_PYRAMID));
  g_scanner_scope_add_symbol (scanner, BZ_SCOPE_DEFAULT, "teleporter", GINT_TO_POINTER (BZ_TOKEN_TELEPORTER));
  g_scanner_scope_add_symbol (scanner, BZ_SCOPE_DEFAULT, "link",       GINT_TO_POINTER (BZ_TOKEN_LINK));
  g_scanner_scope_add_symbol (scanner, BZ_SCOPE_DEFAULT, "base",       GINT_TO_POINTER (BZ_TOKEN_BASE));
  g_scanner_scope_add_symbol (scanner, BZ_SCOPE_DEFAULT, "weapon",     GINT_TO_POINTER (BZ_TOKEN_WEAPON));
  g_scanner_scope_add_symbol (scanner, BZ_SCOPE_DEFAULT, "zone",       GINT_TO_POINTER (BZ_TOKEN_ZONE));
  g_scanner_scope_add_symbol (scanner, BZ_SCOPE_DEFAULT, "world",      GINT_TO_POINTER (BZ_TOKEN_WORLD));

  g_scanner_scope_add_symbol (scanner, BZ_SCOPE_BOX, "position", GINT_TO_POINTER (BZ_TOKEN_POSITION));
  g_scanner_scope_add_symbol (scanner, BZ_SCOPE_BOX, "rotation", GINT_TO_POINTER (BZ_TOKEN_ROTATION));
  g_scanner_scope_add_symbol (scanner, BZ_SCOPE_BOX, "size",     GINT_TO_POINTER (BZ_TOKEN_SIZE));
  g_scanner_scope_add_symbol (scanner, BZ_SCOPE_BOX, "end",      GINT_TO_POINTER (BZ_TOKEN_END));

  g_scanner_scope_add_symbol (scanner, BZ_SCOPE_PYRAMID, "position", GINT_TO_POINTER (BZ_TOKEN_POSITION));
  g_scanner_scope_add_symbol (scanner, BZ_SCOPE_PYRAMID, "rotation", GINT_TO_POINTER (BZ_TOKEN_ROTATION));
  g_scanner_scope_add_symbol (scanner, BZ_SCOPE_PYRAMID, "size",     GINT_TO_POINTER (BZ_TOKEN_SIZE));
  g_scanner_scope_add_symbol (scanner, BZ_SCOPE_PYRAMID, "end",      GINT_TO_POINTER (BZ_TOKEN_END));

  g_scanner_scope_add_symbol (scanner, BZ_SCOPE_TELEPORTER, "position", GINT_TO_POINTER (BZ_TOKEN_POSITION));
  g_scanner_scope_add_symbol (scanner, BZ_SCOPE_TELEPORTER, "rotation", GINT_TO_POINTER (BZ_TOKEN_ROTATION));
  g_scanner_scope_add_symbol (scanner, BZ_SCOPE_TELEPORTER, "size",     GINT_TO_POINTER (BZ_TOKEN_SIZE));
  g_scanner_scope_add_symbol (scanner, BZ_SCOPE_TELEPORTER, "border",   GINT_TO_POINTER (BZ_TOKEN_BORDER));
  g_scanner_scope_add_symbol (scanner, BZ_SCOPE_TELEPORTER, "end",      GINT_TO_POINTER (BZ_TOKEN_END));

  g_scanner_scope_add_symbol (scanner, BZ_SCOPE_LINK, "from", GINT_TO_POINTER (BZ_TOKEN_FROM));
  g_scanner_scope_add_symbol (scanner, BZ_SCOPE_LINK, "to",   GINT_TO_POINTER (BZ_TOKEN_TO));
  g_scanner_scope_add_symbol (scanner, BZ_SCOPE_LINK, "end",  GINT_TO_POINTER (BZ_TOKEN_END));

  g_scanner_scope_add_symbol (scanner, BZ_SCOPE_BASE, "end", GINT_TO_POINTER (BZ_TOKEN_END));

  g_scanner_scope_add_symbol (scanner, BZ_SCOPE_WEAPON, "end", GINT_TO_POINTER (BZ_TOKEN_END));

  g_scanner_scope_add_symbol (scanner, BZ_SCOPE_ZONE, "end", GINT_TO_POINTER (BZ_TOKEN_END));

  g_scanner_scope_add_symbol (scanner, BZ_SCOPE_WORLD, "end", GINT_TO_POINTER (BZ_TOKEN_END));

  box        = g_type_from_name ("Box");
  pyramid    = g_type_from_name ("Pyramid");
  teleporter = g_type_from_name ("Teleporter");
}

static gboolean
parse_double (SceneObject *object, const gchar *property)
{
  GTokenType token;
  GTokenValue value;
  gboolean negative = FALSE;
  gdouble v;

  token = g_scanner_get_next_token (scanner);
  if (token == G_TOKEN_CHAR)
  {
    value = g_scanner_cur_value (scanner);
    if (value.v_char == '-')
      negative = TRUE;
    else
    {
      g_scanner_unexp_token (scanner, G_TOKEN_FLOAT, NULL, NULL, NULL, NULL, TRUE);
      return FALSE;
    }
    token = g_scanner_get_next_token (scanner);
  }
  if (token != G_TOKEN_FLOAT)
  {
    g_scanner_unexp_token (scanner, G_TOKEN_FLOAT, NULL, NULL, NULL, NULL, TRUE);
    return FALSE;
  }
  value = g_scanner_cur_value (scanner);
  v = value.v_float;
  if (negative)
    v *= -1.0;
  if (property != NULL)
    g_object_set (G_OBJECT (object), property, v, NULL);
  return TRUE;
}

static gboolean
parse_box (Editor *editor)
{
  GTokenType token;
  SceneObject *b = SCENE_OBJECT (g_object_new (box, NULL));

  g_scanner_set_scope (scanner, BZ_SCOPE_BOX);

  do
  {
    token = g_scanner_get_next_token (scanner);
    switch (token)
    {
      case G_TOKEN_EOF:
        g_scanner_error (scanner, "%s: unexpected EOF at line %d\n", scanner->input_name, g_scanner_cur_line (scanner));
	goto fail;
      case BZ_TOKEN_POSITION:
        if (!parse_double (b, "x")) goto fail;
        if (!parse_double (b, "y")) goto fail;
        if (!parse_double (b, "z")) goto fail;
	break;
      case BZ_TOKEN_ROTATION:
        if (!parse_double (b, "rotation")) goto fail;
	break;
      case BZ_TOKEN_SIZE:
        if (!parse_double (b, "width")) goto fail;
        if (!parse_double (b, "depth")) goto fail;
        if (!parse_double (b, "height")) goto fail;
	break;
      case BZ_TOKEN_END:
        break;
      default:
        g_scanner_unexp_token (scanner, G_TOKEN_SYMBOL, NULL, NULL, NULL, NULL, TRUE);
	goto fail;
    };
  } while (token != BZ_TOKEN_END);

  scene_add (editor->scene, b);
  g_signal_connect (G_OBJECT (b), "selected", G_CALLBACK (editor_selected), (gpointer) editor);
  scene_object_select (b);
  return TRUE;

  fail:
  g_object_unref (b);
  return FALSE;
}

static gboolean
parse_pyramid (Editor *editor)
{
  GTokenType token;
  SceneObject *p = SCENE_OBJECT (g_object_new (pyramid, NULL));

  g_scanner_set_scope (scanner, BZ_SCOPE_PYRAMID);

  do
  {
    token = g_scanner_get_next_token (scanner);
    switch (token)
    {
      case G_TOKEN_EOF:
        g_scanner_error (scanner, "%s: unexpected EOF at line %d\n", scanner->input_name, g_scanner_cur_line (scanner));
	goto fail;
      case BZ_TOKEN_POSITION:
        if (!parse_double (p, "x")) goto fail;
        if (!parse_double (p, "y")) goto fail;
        if (!parse_double (p, "z")) goto fail;
	break;
      case BZ_TOKEN_ROTATION:
        if (!parse_double (p, "rotation")) goto fail;
	break;
      case BZ_TOKEN_SIZE:
        if (!parse_double (p, "width")) goto fail;
        if (!parse_double (p, "depth")) goto fail;
        if (!parse_double (p, "height")) goto fail;
	break;
      case BZ_TOKEN_END:
        break;
      default:
        g_scanner_unexp_token (scanner, token, NULL, NULL, NULL, NULL, TRUE);
	goto fail;
    }
  } while (token != BZ_TOKEN_END);

  scene_add (editor->scene, p);
  g_signal_connect (G_OBJECT (p), "selected", G_CALLBACK (editor_selected), (gpointer) editor);
  scene_object_select (p);
  return TRUE;

  fail:
  g_object_unref (p);
  return FALSE;
}

static gboolean
parse_teleporter (Editor *editor)
{
  GTokenType token;
  SceneObject *t = SCENE_OBJECT (g_object_new (teleporter, NULL));

  g_scanner_set_scope (scanner, BZ_SCOPE_TELEPORTER);

  do
  {
    token = g_scanner_get_next_token (scanner);
    switch (token)
    {
      case G_TOKEN_EOF:
        g_scanner_error (scanner, "%s: unexpected EOF at line %d\n", scanner->input_name, g_scanner_cur_line (scanner));
	goto fail;
      case BZ_TOKEN_POSITION:
        if (!parse_double (t, "x")) goto fail;
        if (!parse_double (t, "y")) goto fail;
        if (!parse_double (t, "z")) goto fail;
	break;
      case BZ_TOKEN_ROTATION:
        if (!parse_double (t, "rotation")) goto fail;
	break;
      case BZ_TOKEN_SIZE:
	/* no X size */
	parse_double (t, NULL);
        if (!parse_double (t, "width")) goto fail;
        if (!parse_double (t, "height")) goto fail;
	break;
      case BZ_TOKEN_BORDER:
        if (!parse_double (t, "border")) goto fail;
	break;
      case BZ_TOKEN_END:
        break;
      default:
        g_scanner_unexp_token (scanner, token, NULL, NULL, NULL, NULL, TRUE);
	goto fail;
    }
  } while (token != BZ_TOKEN_END);

  scene_add (editor->scene, t);
  g_signal_connect (G_OBJECT (t), "selected", G_CALLBACK (editor_selected), (gpointer) editor);
  scene_object_select (t);
  return TRUE;

  fail:
  g_object_unref (t);
  return FALSE;
}

static gboolean
parse_link (Editor *editor)
{
  GTokenType token;

  g_scanner_set_scope (scanner, BZ_SCOPE_LINK);

  do
  {
    token = g_scanner_get_next_token (scanner);
    switch (token)
    {
      case G_TOKEN_EOF:
        g_scanner_error (scanner, "%s: unexpected EOF at line %d\n", scanner->input_name, g_scanner_cur_line (scanner));
	goto fail;

      case BZ_TOKEN_FROM:
      case BZ_TOKEN_TO:
        token = g_scanner_get_next_token (scanner);
	break;

      case BZ_TOKEN_END:
        break;

      default:
        g_scanner_unexp_token (scanner, token, NULL, NULL, NULL, NULL, TRUE);
	goto fail;
    }
  } while (token != BZ_TOKEN_END);

  return TRUE;

  fail:
  return FALSE;
}

gboolean
import_bz (gchar *filename, Editor *editor)
{
  gint fd;
  GTokenType token;

  if (!initialized)
    initialize_scanner ();

  /* bleh, fd. yick */
  fd = open (filename, O_RDONLY);

  g_scanner_input_file (scanner, fd);
  scanner->input_name = g_strdup (filename);

  scene_freeze (editor->scene);

  do
  {
    g_scanner_set_scope (scanner, BZ_SCOPE_DEFAULT);

    token = g_scanner_get_next_token (scanner);

    switch (token)
    {
      case BZ_TOKEN_BOX:
        if (!parse_box (editor)) goto fail;
	break;
      case BZ_TOKEN_PYRAMID:
        if (!parse_pyramid (editor)) goto fail;
	break;
      case BZ_TOKEN_TELEPORTER:
        if (!parse_teleporter (editor)) goto fail;
	break;
      case BZ_TOKEN_LINK:
        if (!parse_link (editor)) goto fail;
	break;
      case G_TOKEN_EOF:
        break;
      default:
        g_scanner_unexp_token (scanner, G_TOKEN_SYMBOL, NULL, "keyword", NULL, NULL, TRUE);
	goto fail;
    }
  } while (token != G_TOKEN_EOF);
  g_free ((gchar*) scanner->input_name);
  scene_thaw (editor->scene);
  close (fd);
  return TRUE;

  fail:
  g_free ((gchar*) scanner->input_name);
  scene_thaw (editor->scene);
  close (fd);
  return FALSE;
}
