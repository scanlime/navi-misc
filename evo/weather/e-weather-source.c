/* Evolution calendar - weather backend source class
 *
 * Copyright (C) 2005 Novell, Inc (www.novell.com)
 *
 * Authors: David Trowbridge <trowbrds@cs.colorado.edu>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of version 2 of the GNU General Public
 * License as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 */

#include "e-weather-source.h"
#include "e-weather-source-ccf.h"

void
e_weather_source_parse (EWeatherSource *source, EWeatherSourceFinished done, gpointer data)
{
	EWeatherSourceClass *class;
	g_return_if_fail (source != NULL);
	class = (EWeatherSourceClass*) G_OBJECT_GET_CLASS (source);
	return class->parse (source, done, data);
}

static void
e_weather_source_class_init (EWeatherSourceClass *class)
{
	/* nothing to do here */
}

static void
e_weather_source_init (EWeatherSource *source)
{
	/* nothing to do here */
}

GType
e_weather_source_get_type (void)
{
	static GType e_weather_source_type = 0;

	if (!e_weather_source_type) {
		static GTypeInfo info = {
			sizeof (EWeatherSourceClass),
			(GBaseInitFunc) NULL,
			(GBaseFinalizeFunc) NULL,
			(GClassInitFunc) e_weather_source_class_init,
			NULL, NULL,
			sizeof (EWeatherSource),
			0,
			(GInstanceInitFunc) e_weather_source_init
		};
		e_weather_source_type = g_type_register_static (G_TYPE_OBJECT, "EWeatherSource", &info, 0);
	}

	return e_weather_source_type;
}

EWeatherSource*	e_weather_source_new (const char *uri)
{
	char *base = uri + 10; /* skip weather:// */
	if (strncmp (base, "ccf/", 4) == 0)
		return e_weather_source_ccf_new (base);
	return NULL;
}
