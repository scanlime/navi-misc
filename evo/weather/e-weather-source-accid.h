/* Evolution calendar - weather backend source class for parsing
 * 	ACCID-based data from msnbc.com (a-la mythweather)
 *
 * Copyright (C) 2005 Novell, Inc (www.novell.com)
 *
 * Authors: David Trowbridge <trowbrds@cs.colorado.edu>
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

#ifndef E_WEATHER_SOURCE_ACCID_H
#define E_WEATHER_SOURCE_ACCID_H

#include <libsoup/soup-session-async.h>
#include <libsoup/soup-uri.h>
#include "e-weather-source.h"

G_BEGIN_DECLS

#define E_TYPE_WEATHER_SOURCE_ACCID            (e_weather_source_accid_get_type ())
#define E_WEATHER_SOURCE_ACCID(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), E_TYPE_WEATHER_SOURCE_ACCID, EWeatherSourceACCID))
#define E_WEATHER_SOURCE_ACCID_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), E_TYPE_WEATHER_SOURCE_ACCID, EWeatherSourceACCID))
#define E_IS_WEATHER_SOURCE_ACCID(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), E_TYPE_WEATHER_SOURCE_ACCID))
#define E_IS_WEATHER_SOURCE_ACCID_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), E_TYPE_WEATHER_SOURCE_ACCID))

typedef struct _EWeatherSourceACCID EWeatherSourceACCID;
typedef struct _EWeatherSourceACCIDClass EWeatherSourceACCIDClass;

struct _EWeatherSourceACCID {
	EWeatherSource parent;

	char *station;
	EWeatherSourceFinished done;
	SoupSession *soup_session;
	gpointer finished_data;
};

struct _EWeatherSourceACCIDClass {
	EWeatherSourceClass parent_class;
};

EWeatherSource* e_weather_source_accid_new (const char *uri);
GType           e_weather_source_accid_get_type (void);

G_END_DECLS

#endif
