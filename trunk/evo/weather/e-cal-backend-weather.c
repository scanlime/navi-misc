/* Evolution calendar - weather backend
 *
 * Copyright (C) 2000 Ximian, Inc.
 * Copyright (C) 2003 David Trowbridge
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

#include <config.h>
#include <libedata-cal/e-cal-backend-cache.h>
#include <libedata-cal/e-cal-backend-util.h>
#include <libedata-cal/e-cal-backend-sexp.h>
#include <libgnome/gnome-i18n.h>
#include "e-cal-backend-weather.h"
#include "e-weather-source.h"

#define WEATHER_UID_EXT "-weather"



/* Private part of the ECalBackendWeather structure */
struct _ECalBackendWeatherPrivate {
	/* URI to get remote weather data from */
	char *uri;

	/* Local/remote mode */
	CalMode mode;

	/* The file cache */
	ECalBackendCache *cache;

	/* The calendar's default timezone, used for resolving DATE and
	   floating DATE-TIME values. */
	icaltimezone *default_zone;
};



static ECalBackendSyncClass *parent_class;



static ECalComponent*
create_weather (ECalBackendWeather *cbw, WeatherForecast *report)
{
	ECalComponent *cal_comp;
	char *summary;
	const char *name, *uid;
	char *temperature;
	char *snowfall;
	char *conditions;
	char *pop;

	uid = g_strdup_printf ("%s%s", (char *) /* FIXME */ "BLARG!", WEATHER_UID_EXT);
	if (report->high == report->low)
		temperature = g_strdup_printf("%f °F", report->high);
	else
		temperature = g_strdup_printf("%s: %f °F\n%s: %f °F", _("High"), report->high, _("Low"), report->low);
	summary = g_strdup_printf ("%s\n", temperature);
	g_free (temperature);

	//cal_comp = create_component (cbw, uid, /* FIXME */ date, summary);
	g_free (summary);

	return cal_comp;
}

static ECalBackendSyncStatus
e_cal_backend_weather_is_read_only (ECalBackendSync *backend, EDataCal *cal, gboolean *read_only)
{
	*read_only = TRUE;

	return GNOME_Evolution_Calendar_Success;
}

static ECalBackendSyncStatus
e_cal_backend_weather_get_cal_address (ECalBackendSync *backend, EDataCal *cal, char **address)
{
	/* Weather has no particular email addresses associated with it */
	*address = NULL;

	return GNOME_Evolution_Calendar_Success;
}

static ECalBackendSyncStatus
e_cal_backend_weather_get_alarm_email_address (ECalBackendSync *backend, EDataCal *cal, char **address)
{
	/* Weather has no particular email addresses associated with it */
	*address = NULL;

	return GNOME_Evolution_Calendar_Success;
}

static ECalBackendSyncStatus
e_cal_backend_weather_get_ldap_attribute (ECalBackendSync *backend, EDataCal *cal, char **attribute)
{
	*attribute = NULL;

	return GNOME_Evolution_Calendar_Success;
}

static ECalBackendSyncStatus
e_cal_backend_weather_get_static_capabilities (ECalBackendSync *backend, EDataCal *cal, char **capabilities)
{
	*capabilities = NULL;

	return GNOME_Evolution_Calendar_Success;
}

static ECalBackendSyncStatus
e_cal_backend_weather_open (ECalBackendSync *backend, EDataCal *cal, gboolean only_if_exists, const char *username, const char *password)
{
	ECalBackendWeather *cbw;
	ECalBackendWeatherPrivate *priv;

	cbw = E_CAL_BACKEND_WEATHER (backend);
	priv = cbw->priv;

	if (!priv->cache) {
		priv->cache = e_cal_backend_cache_new (e_cal_backend_get_uri (E_CAL_BACKEND (backend)));

		/* FIXME: no need to set it online here when we implement the online/offline stuff correctly */
		priv->mode = CAL_MODE_REMOTE;
		/* FIXME g_idle_add ((GSourceFunc) begin_retrieval_cb, cbw); */
	}

	return GNOME_Evolution_Calendar_Success;
}

static ECalBackendSyncStatus
e_cal_backend_weather_remove (ECalBackendSync *backend, EDataCal *cal)
{
	return GNOME_Evolution_Calendar_PermissionDenied;
}

static ECalBackendSyncStatus
e_cal_backend_weather_discard_alarm (ECalBackendSync *backend, EDataCal *cal, const char *uid, const char *auid)
{
	return GNOME_Evolution_Calendar_Success;
}

static ECalBackendSyncStatus
e_cal_backend_weather_receive_objects (ECalBackendSync *backend, EDataCal *cal, const char *calobj)
{
	return GNOME_Evolution_Calendar_PermissionDenied;
}

static ECalBackendSyncStatus
e_cal_backend_weather_get_default_object (ECalBackendSync *backend, EDataCal *cal, char **object)
{
	return GNOME_Evolution_Calendar_UnsupportedMethod;
}

static ECalBackendSyncStatus
e_cal_backend_weather_get_object (ECalBackendSync *backend, EDataCal *cal, const char *uid, const char *rid, char **object)
{
	ECalBackendWeather *cbw = E_CAL_BACKEND_WEATHER (backend);
	ECalBackendWeatherPrivate *priv = cbw->priv;

	if (!uid)
		return GNOME_Evolution_Calendar_ObjectNotFound;

	/* FIXME */

	return GNOME_Evolution_Calendar_ObjectNotFound;
}

static ECalBackendSyncStatus
e_cal_backend_weather_get_object_list (ECalBackendSync *backend, EDataCal *cal, const char *sexp_string, GList **objects)
{
	ECalBackendWeather *cbw = E_CAL_BACKEND_WEATHER (backend);
	ECalBackendWeatherPrivate *priv = cbw->priv;
	ECalBackendSExp *sexp = e_cal_backend_sexp_new (sexp_string);

	if (!sexp)
		return GNOME_Evolution_Calendar_InvalidQuery;

	/* FIXME */
	return GNOME_Evolution_Calendar_Success;
}

static ECalBackendSyncStatus
e_cal_backend_weather_get_timezone (ECalBackendSync *backend, EDataCal *cal, const char *tzid, char **object)
{
	ECalBackendWeather *cbw;
	ECalBackendWeatherPrivate *priv;
	icaltimezone *zone;
	icalcomponent *icalcomp;

	cbw = E_CAL_BACKEND_WEATHER (backend);
	priv = cbw->priv;

	g_return_val_if_fail (tzid != NULL, GNOME_Evolution_Calendar_ObjectNotFound);

	zone = e_cal_backend_internal_get_timezone (E_CAL_BACKEND (backend), tzid);
	if (!zone)
		return GNOME_Evolution_Calendar_ObjectNotFound;

	icalcomp = icaltimezone_get_component (zone);
	if (!icalcomp)
		return GNOME_Evolution_Calendar_InvalidObject;

	*object = g_strdup (icalcomponent_as_ical_string (icalcomp));

	return GNOME_Evolution_Calendar_Success;
}

/* FIXME e_cal_backend_weather_add_timezone */

static ECalBackendSyncStatus
e_cal_backend_weather_set_default_timezone (ECalBackendSync *backend, EDataCal *cal, const char *tzid)
{
	ECalBackendWeather *cbw;
	ECalBackendWeatherPrivate *priv;

	cbw = E_CAL_BACKEND_WEATHER (backend);
	priv = cbw->priv;

	priv->default_zone = e_cal_backend_internal_get_timezone (E_CAL_BACKEND (backend), tzid);
	if (!priv->default_zone) {
		priv->default_zone = icaltimezone_get_utc_timezone ();

		return GNOME_Evolution_Calendar_ObjectNotFound;
	}

	return GNOME_Evolution_Calendar_Success;
}

static ECalBackendSyncStatus
e_cal_backend_weather_get_free_busy (ECalBackendSync *backend, EDataCal *cal, GList *users, time_t start, time_t end, GList **freebusy)
{
	/* Weather doesn't count as busy time */
	icalcomponent *vfb = icalcomponent_new_vfreebusy ();
	icaltimezone *utc_zone = icaltimezone_get_utc_timezone ();
	char *calobj;

	icalcomponent_set_dtstart (vfb, icaltime_from_timet_with_zone (start, FALSE, utc_zone));
	icalcomponent_set_dtend (vfb, icaltime_from_timet_with_zone (end, FALSE, utc_zone));

	calobj = icalcomponent_as_ical_string (vfb);
	*freebusy = g_list_append (NULL, g_strdup (calobj));
	icalcomponent_free (vfb);

	return GNOME_Evolution_Calendar_Success;
}

static ECalBackendSyncStatus
e_cal_backend_weather_get_changes (ECalBackendSync *backend, EDataCal *cal, const char *change_id, GList **adds, GList **modifies, GList **deletes)
{
	return GNOME_Evolution_Calendar_Success;
}

static gboolean
e_cal_backend_weather_is_loaded (ECalBackend *backend)
{
	ECalBackendWeather *cbw;
	ECalBackendWeatherPrivate *priv;

	cbw = E_CAL_BACKEND_WEATHER (backend);
	priv = cbw->priv;

	if (!priv->cache)
		return FALSE;

	return TRUE;
}

static void e_cal_backend_weather_start_query (ECalBackend *backend, EDataCalView *query)
{
	ECalBackendWeather *cbw;
	ECalBackendWeatherPrivate *priv;
	ECalBackendSExp *sexp;

	cbw = E_CAL_BACKEND_WEATHER (backend);
	priv = cbw->priv;

	g_print (G_STRLOC ": Starting query (%s)", e_data_cal_view_get_text (query));

	if (!priv->cache) {
		e_data_cal_view_notify_done (query, GNOME_Evolution_Calendar_NoSuchCal);
		return;
	}

	sexp = e_data_cal_view_get_object_sexp (query);
	if (!sexp) {
		e_data_cal_view_notify_done (query, GNOME_Evolution_Calendar_InvalidQuery);
		return;
	}

	/* FIXME */
}

static CalMode
e_cal_backend_weather_get_mode (ECalBackend *backend)
{
	ECalBackendWeather *cbw;
	ECalBackendWeatherPrivate *priv;

	cbw = E_CAL_BACKEND_WEATHER (backend);
	priv = cbw->priv;

	return priv->mode;
}

static void
e_cal_backend_weather_set_mode (ECalBackend *backend, CalMode mode)
{
	ECalBackendWeather *cbw;
	ECalBackendWeatherPrivate *priv;
	GNOME_Evolution_Calendar_CalMode set_mode;

	cbw = E_CAL_BACKEND_WEATHER (backend);
	priv = cbw->priv;

	switch (mode) {
		case CAL_MODE_LOCAL:
		case CAL_MODE_REMOTE:
			priv->mode = mode;
			set_mode = cal_mode_to_corba (mode);
			break;
		case CAL_MODE_ANY:
			priv->mode = CAL_MODE_REMOTE;
			set_mode = GNOME_Evolution_Calendar_MODE_REMOTE;
			break;
		default:
			set_mode = GNOME_Evolution_Calendar_MODE_ANY;
			break;
	}

	if (set_mode == GNOME_Evolution_Calendar_MODE_ANY)
		e_cal_backend_notify_mode (backend, GNOME_Evolution_Calendar_CalListener_MODE_NOT_SUPPORTED, cal_mode_to_corba (priv->mode));
	else
		e_cal_backend_notify_mode (backend, GNOME_Evolution_Calendar_CalListener_MODE_SET, set_mode);
}

static icaltimezone *
e_cal_backend_weather_internal_get_default_timezone (ECalBackend *backend)
{
	ECalBackendWeather *cbw = E_CAL_BACKEND_WEATHER (backend);

	return cbw->priv->default_zone;
}

static icaltimezone *
e_cal_backend_weather_internal_get_timezone (ECalBackend *backend, const char *tzid)
{
	ECalBackendWeather *cbw = E_CAL_BACKEND_WEATHER (backend);

	return cbw->priv->default_zone;
}



/* Finalize handler for the weather backend */
static void
e_cal_backend_weather_finalize (GObject *object)
{
	ECalBackendWeather *cbw;
	ECalBackendWeatherPrivate *priv;

	g_return_if_fail (object != NULL);
	g_return_if_fail (E_IS_CAL_BACKEND_WEATHER (object));

	cbw = (ECalBackendWeather *) object;
	priv = cbw->priv;

	if (priv->cache) {
		g_object_unref (priv->cache);
		priv->cache = NULL;
	}

	g_free (priv);
	cbw->priv = NULL;

	if (G_OBJECT_CLASS (parent_class)->finalize)
		(* G_OBJECT_CLASS (parent_class)->finalize) (object);
}

/* Object initialization function for the weather backend */
static void
e_cal_backend_weather_init (ECalBackendWeather *cbw, ECalBackendWeatherClass *class)
{
	ECalBackendWeatherPrivate *priv;

	priv = g_new0 (ECalBackendWeatherPrivate, 1);

	cbw->priv = priv;
}

/* Class initialization function for the weather backend */
static void
e_cal_backend_weather_class_init (ECalBackendWeatherClass *class)
{
	GObjectClass *object_class;
	ECalBackendClass *backend_class;
	ECalBackendSyncClass *sync_class;

	object_class = (GObjectClass *) class;
	backend_class = (ECalBackendClass *) class;
	sync_class = (ECalBackendSyncClass *) class;

	parent_class = (ECalBackendSyncClass *) g_type_class_peek_parent (class);

	object_class->finalize = e_cal_backend_weather_finalize;

	sync_class->is_read_only_sync = e_cal_backend_weather_is_read_only;
	sync_class->get_cal_address_sync = e_cal_backend_weather_get_cal_address;
	sync_class->get_alarm_email_address_sync = e_cal_backend_weather_get_alarm_email_address;
	sync_class->get_ldap_attribute_sync = e_cal_backend_weather_get_ldap_attribute;
	sync_class->get_static_capabilities_sync = e_cal_backend_weather_get_static_capabilities;
	sync_class->open_sync = e_cal_backend_weather_open;
	sync_class->remove_sync = e_cal_backend_weather_remove;
	sync_class->discard_alarm_sync = e_cal_backend_weather_discard_alarm;
	sync_class->receive_objects_sync = e_cal_backend_weather_receive_objects;
	sync_class->get_default_object_sync = e_cal_backend_weather_get_default_object;
//	sync_class->get_object_sync = e_cal_backend_weather_get_object;
//	sync_class->get_object_list_sync = e_cal_backend_weather_get_object_list;
	sync_class->get_timezone_sync = e_cal_backend_weather_get_timezone;
//	sync_class->add_timezone_sync = e_cal_backend_weather_add_timezone;
	sync_class->set_default_timezone_sync = e_cal_backend_weather_set_default_timezone;
	sync_class->get_freebusy_sync = e_cal_backend_weather_get_free_busy;
	sync_class->get_changes_sync = e_cal_backend_weather_get_changes;
	backend_class->is_loaded = e_cal_backend_weather_is_loaded;
	backend_class->start_query = e_cal_backend_weather_start_query;
	backend_class->get_mode = e_cal_backend_weather_get_mode;
	backend_class->set_mode = e_cal_backend_weather_set_mode;

	backend_class->internal_get_default_timezone = e_cal_backend_weather_internal_get_default_timezone;
	backend_class->internal_get_timezone = e_cal_backend_weather_internal_get_timezone;
}


/**
 * e_cal_backend_weather_get_type:
 * @void: 
 *
 * Registers the #ECalBackendWeather class if necessary, and returns
 * the type ID associated to it.
 *
 * Return value: The type ID of the #ECalBackendWeather class.
 **/
GType
e_cal_backend_weather_get_type (void)
{
	static GType e_cal_backend_weather_type = 0;

	if (!e_cal_backend_weather_type)
	{
		static GTypeInfo info = {
			sizeof (ECalBackendWeatherClass),
			(GBaseInitFunc) NULL,
			(GBaseFinalizeFunc) NULL,
			(GClassInitFunc) e_cal_backend_weather_class_init,
			NULL, NULL,
			sizeof (ECalBackendWeather),
			0,
			(GInstanceInitFunc) e_cal_backend_weather_init
		};
		e_cal_backend_weather_type = g_type_register_static (E_TYPE_CAL_BACKEND_SYNC, "ECalBackendWeather", &info, 0);
	}

	return e_cal_backend_weather_type;
}
