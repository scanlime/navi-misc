/* Evolution calendar - weather backend
 *
 * Copyright (C) 2000 Ximian, Inc.
 * Copyright (C) 2004 David Trowbridge
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
#include <string.h>
#include "e-cal-backend-weather.h"
#include "e-weather-source.h"

#define WEATHER_UID_EXT "-weather"

static gboolean reload_cb (ECalBackendWeather *cbw);
static gboolean begin_retrieval_cb (ECalBackendWeather *cbw);
static ECalComponent* create_weather (ECalBackendWeather *cbw, WeatherForecast *report);

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
	GHashTable *zones;

	/* Reload */
	guint reload_timeout_id;
	guint source_changed_id;
	guint is_loading : 1;

	/* Flags */
	gboolean opened;

	/* Weather source */
	EWeatherSource *source;
};

static ECalBackendSyncClass *parent_class;

static gboolean
reload_cb (ECalBackendWeather *cbw)
{
	ECalBackendWeatherPrivate *priv;

	priv = cbw->priv;

	if (priv->is_loading)
		return TRUE;

	priv->reload_timeout_id = 0;
	priv->opened = TRUE;
	begin_retrieval_cb (cbw);
	return FALSE;
}

static void
source_changed (ESource *source, ECalBackendWeather *cbw)
{
	g_print ("source changed!\n");
}

static void
maybe_start_reload_timeout (ECalBackendWeather *cbw)
{
	ECalBackendWeatherPrivate *priv;
	ESource *source;
	const gchar *refresh_str;

	priv = cbw->priv;

	if (priv->reload_timeout_id)
		return;

	source = e_cal_backend_get_source (E_CAL_BACKEND (cbw));
	if (!source) {
		g_warning ("Could not get source for ECalBackendWeather reload.");
		return;
	}

	if (priv->source_changed_id == 0)
		priv->source_changed_id = g_signal_connect (G_OBJECT (source),
		                                            "changed",
							    G_CALLBACK (source_changed),
							    cbw);

	refresh_str = e_source_get_property (source, "refresh");

	/* By default, reload every 4 hours. At least for CCF, the forecasts only come out
	 * twice a day, and chances are while the NWS and similar organizations have some
	 * serious bandwidth, they would appreciate it if we didn't hammer their servers
	 */
	priv->reload_timeout_id = g_timeout_add ((refresh_str ? atoi (refresh_str) : 240) * 60000,
	    					 (GSourceFunc) reload_cb, cbw);

}

static void
finished_retrieval_cb (GList *forecasts, ECalBackendWeather *cbw)
{
	ECalBackendWeatherPrivate *priv;
	ECalComponent *comp;
	icalcomponent *icomp;
	GList *l;

	priv = cbw->priv;

	if (forecasts == NULL) {
		e_cal_backend_notify_error (E_CAL_BACKEND (cbw), _("Could not retrieve weather data"));
		return;
	}

	/* update cache */
	l = e_cal_backend_cache_get_components (priv->cache);
	for (; l != NULL; l = g_list_next (l)) {
		icomp = e_cal_component_get_icalcomponent (E_CAL_COMPONENT (l->data));
		e_cal_backend_notify_object_removed (E_CAL_BACKEND (cbw),
			icalcomponent_get_uid (icomp),
			icalcomponent_as_ical_string (icomp),
			NULL);
		g_object_unref (G_OBJECT (l->data));
	}
	g_list_free (l);
	e_file_cache_clean (E_FILE_CACHE (priv->cache));

	for (l = forecasts; l != NULL; l = g_list_next (l)) {
		comp = create_weather (cbw, l->data);
		e_cal_backend_cache_put_component (priv->cache, comp);
		icomp = e_cal_component_get_icalcomponent (comp);
		e_cal_backend_notify_object_created (E_CAL_BACKEND (cbw), icalcomponent_as_ical_string (icomp));
	}

	priv->is_loading = FALSE;
}

static gboolean
begin_retrieval_cb (ECalBackendWeather *cbw)
{
	ECalBackendWeatherPrivate *priv = cbw->priv;

	if (priv->mode != CAL_MODE_REMOTE)
		return TRUE;

	maybe_start_reload_timeout (cbw);

	if (priv->source == NULL)
		priv->source = e_weather_source_new (e_cal_backend_get_uri (E_CAL_BACKEND (cbw)));

	if (priv->is_loading)
		return FALSE;

	priv->is_loading = TRUE;

	e_weather_source_parse (priv->source, (EWeatherSourceFinished) finished_retrieval_cb, cbw);

	return FALSE;
}

static const char*
getConditions (WeatherForecast *report)
{
	switch (report->conditions) {
		case WEATHER_FAIR:			return _("Fair");
		case WEATHER_SNOW_SHOWERS:		return _("Snow showers");
		case WEATHER_SNOW:			return _("Snow");
		case WEATHER_PARTLY_CLOUDY:		return _("Partly cloudy");
		case WEATHER_SMOKE:			return _("Smoke");
		case WEATHER_THUNDERSTORMS:		return _("Thunderstorms");
		case WEATHER_CLOUDY:			return _("Cloudy");
		case WEATHER_DRIZZLE:			return _("Drizzle");
		case WEATHER_SUNNY:			return _("Sunny");
		case WEATHER_DUST:			return _("Dust");
		case WEATHER_CLEAR:			return _("Clear");
		case WEATHER_MOSTLY_CLOUDY:		return _("Mostly cloudy");
		case WEATHER_WINDY:			return _("Windy");
		case WEATHER_RAIN_SHOWERS:		return _("Rain showers");
		case WEATHER_FOGGY:			return _("Foggy");
		case WEATHER_RAIN_OR_SNOW_MIXED:	return _("Rain/snow mixed");
		case WEATHER_SLEET:			return _("Sleet");
		case WEATHER_VERY_HOT_OR_HOT_HUMID:	return _("Very hot/humid");
		case WEATHER_BLIZZARD:			return _("Blizzard");
		case WEATHER_FREEZING_RAIN:		return _("Freezing rain");
		case WEATHER_HAZE:			return _("Haze");
		case WEATHER_BLOWING_SNOW:		return _("Blowing snow");
		case WEATHER_FREEZING_DRIZZLE:		return _("Freezing drizzle");
		case WEATHER_VERY_COLD_WIND_CHILL:	return _("Very cold/wind chill");
		case WEATHER_RAIN:			return _("Rain");
		default:				return NULL;
	}
}

static const char*
getCategory (WeatherForecast *report)
{
	/* Right now this is based on which icons we have available */
	switch (report->conditions) {
		case WEATHER_FAIR:			return _("Weather: Sunny");
		case WEATHER_SNOW_SHOWERS:		return _("Weather: Snow");
		case WEATHER_SNOW:			return _("Weather: Snow");
		case WEATHER_PARTLY_CLOUDY:		return _("Weather: Partly Cloudy");
		case WEATHER_SMOKE:			return _("Weather: Fog");
		case WEATHER_THUNDERSTORMS:		return _("Weather: Thunderstorms");
		case WEATHER_CLOUDY:			return _("Weather: Cloudy");
		case WEATHER_DRIZZLE:			return _("Weather: Rain");
		case WEATHER_SUNNY:			return _("Weather: Sunny");
		case WEATHER_DUST:			return _("Weather: Fog");
		case WEATHER_CLEAR:			return _("Weather: Sunny");
		case WEATHER_MOSTLY_CLOUDY:		return _("Weather: Cloudy");
		case WEATHER_WINDY:			return _("");
		case WEATHER_RAIN_SHOWERS:		return _("Weather: Rain");
		case WEATHER_FOGGY:			return _("Weather: Fog");
		case WEATHER_RAIN_OR_SNOW_MIXED:	return _("Weather: Rain");
		case WEATHER_SLEET:			return _("Weather: Rain");
		case WEATHER_VERY_HOT_OR_HOT_HUMID:	return _("Weather: Sunny");
		case WEATHER_BLIZZARD:			return _("Weather: Snow");
		case WEATHER_FREEZING_RAIN:		return _("Weather: Rain");
		case WEATHER_HAZE:			return _("Weather: Fog");
		case WEATHER_BLOWING_SNOW:		return _("Weather: Snow");
		case WEATHER_FREEZING_DRIZZLE:		return _("Weather: Rain");
		case WEATHER_VERY_COLD_WIND_CHILL:	return _("");
		case WEATHER_RAIN:			return _("Weather: Rain");
		default:				return NULL;
	}
}

static float
ctof (float c)
{
	return ((c * 9.0f / 5.0f) + 32.0f);
}

static float
cmtoin (float cm)
{
	return cm / 2.54f;
}

static ECalComponent*
create_weather (ECalBackendWeather *cbw, WeatherForecast *report)
{
	ECalComponent *cal_comp;
	ECalComponentText comp_summary;
	icalcomponent *ical_comp;
	struct icaltimetype itt;
	ECalComponentDateTime dt;
	const char *uid;
	GSList *text_list = NULL;
	ECalComponentText *description;
	char *pop, *snow;
	ESource *source;
	gboolean fahrenheit;
	gboolean inches;
	const char *format;

	g_return_val_if_fail (E_IS_CAL_BACKEND_WEATHER (cbw), NULL);

	source = e_cal_backend_get_source (E_CAL_BACKEND (cbw));
	format = e_source_get_property (source, "temperature");
	if (format == NULL) {
		fahrenheit = FALSE;
	} else {
		fahrenheit = (strcmp (format, "fahrenheit") == 0);
	}
	format = e_source_get_property (source, "snowfall");
	if (format == NULL) {
		inches = FALSE;
	} else {
		inches = (strcmp (format, "inches") == 0);
	}

	/* create the component and event object */
	ical_comp = icalcomponent_new (ICAL_VEVENT_COMPONENT);
	cal_comp = e_cal_component_new ();
	e_cal_component_set_icalcomponent (cal_comp, ical_comp);

	/* set uid */
	uid = e_cal_component_gen_uid ();
	e_cal_component_set_uid (cal_comp, uid);

	/* Set all-day event's date from forecast data */
	itt = icaltime_from_timet (report->date, 1);
	dt.value = &itt;
	dt.tzid = NULL;
	e_cal_component_set_dtstart (cal_comp, &dt);

	itt = icaltime_from_timet (report->date, 1);
	icaltime_adjust (&itt, 1, 0, 0, 0);
	dt.value = &itt;
	dt.tzid = NULL;
	/* We have to add 1 day to DTEND, as it is not inclusive. */
	e_cal_component_set_dtend (cal_comp, &dt);

	/* The summary is the high or high/low temperatures */
	if (report->high == report->low)
	{
		if (fahrenheit)
			comp_summary.value = g_strdup_printf (_("%.1f°F"), ctof (report->high));
		else
			comp_summary.value = g_strdup_printf (_("%.1f°C"), report->high);
	}
	else
	{
		if (fahrenheit)
			comp_summary.value = g_strdup_printf (_("%.1f/%.1f°F"), ctof (report->high), ctof (report->low));
		else
			comp_summary.value = g_strdup_printf (_("%.1f/%.1f°C"), report->high, report->low);
	}
	comp_summary.altrep = NULL;
	e_cal_component_set_summary (cal_comp, &comp_summary);

	if (report->pop != 0)
		pop = g_strdup_printf (_("%d%% chance of precipitation\n"), report->pop);
	else
		pop = "";
	if (report->snowhigh == 0)
		snow = "";
	else if (report->snowhigh == report->snowlow) {
		if (inches)
			snow = g_strdup_printf (_("%.1fin snow\n"), cmtoin(report->snowhigh));
		else
			snow = g_strdup_printf (_("%.1fcm snow\n"), report->snowhigh);
	} else {
		if (inches)
			snow = g_strdup_printf (_("%.1f-%.1fin snow\n"), cmtoin(report->snowlow), cmtoin(report->snowhigh));
		else
			snow = g_strdup_printf (_("%.1f-%.1fcm snow\n"), report->snowlow, report->snowhigh);
	}
	description = g_new0 (ECalComponentText, 1);
	description->value = g_strdup_printf ("%s\n%s%s", getConditions (report), pop, snow);
	description->altrep = "";
	text_list = g_slist_append (text_list, description);
	e_cal_component_set_description_list (cal_comp, text_list);

	/* Set category and visibility */
	e_cal_component_set_categories (cal_comp, getCategory (report));
	e_cal_component_set_classification (cal_comp, E_CAL_COMPONENT_CLASS_PRIVATE);

	/* Weather is shown as free time */
	e_cal_component_set_transparency (cal_comp, E_CAL_COMPONENT_TRANSP_TRANSPARENT);

	e_cal_component_commit_sequence (cal_comp);

	g_free (pop);
	g_free (snow);

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

		if (!priv->cache) {
			e_cal_backend_notify_error (E_CAL_BACKEND (cbw), _("Could not create cache file"));
			return GNOME_Evolution_Calendar_OtherError;
		}

		if (priv->mode == CAL_MODE_LOCAL)
			return GNOME_Evolution_Calendar_Success;

		g_idle_add ((GSourceFunc) begin_retrieval_cb, cbw);
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
	ECalComponent *comp;

	g_return_val_if_fail (uid != NULL, GNOME_Evolution_Calendar_ObjectNotFound);
	g_return_val_if_fail (priv->cache != NULL, GNOME_Evolution_Calendar_ObjectNotFound);

	comp = e_cal_backend_cache_get_component (priv->cache, uid, rid);
	g_return_val_if_fail (comp != NULL, GNOME_Evolution_Calendar_ObjectNotFound);

	*object = e_cal_component_get_as_string (comp);
	g_free (comp);

	return GNOME_Evolution_Calendar_ObjectNotFound;
}

static ECalBackendSyncStatus
e_cal_backend_weather_get_object_list (ECalBackendSync *backend, EDataCal *cal, const char *sexp_string, GList **objects)
{
	ECalBackendWeather *cbw = E_CAL_BACKEND_WEATHER (backend);
	ECalBackendWeatherPrivate *priv = cbw->priv;
	ECalBackendSExp *sexp = e_cal_backend_sexp_new (sexp_string);
	GList *components, *l;

	if (!sexp)
		return GNOME_Evolution_Calendar_InvalidQuery;

	*objects = NULL;
	components = e_cal_backend_cache_get_components (priv->cache);
	for (l = components; l != NULL; l = g_list_next (l)) {
		if (e_cal_backend_sexp_match_comp (sexp, E_CAL_COMPONENT (l->data), E_CAL_BACKEND (backend)))
			*objects = g_list_append (*objects, e_cal_component_get_as_string (l->data));
	}

	g_list_foreach (components, (GFunc) g_object_unref, NULL);
	g_list_free (components);
	g_object_unref (sexp);

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

static ECalBackendSyncStatus
e_cal_backend_weather_add_timezone (ECalBackendSync *backend, EDataCal *cal, const char *tzobj)
{
	ECalBackendWeather *cbw;
	ECalBackendWeatherPrivate *priv;
	icalcomponent *tz_comp;
	icaltimezone *zone;
	char *tzid;

	cbw = (ECalBackendWeather*) backend;

	g_return_val_if_fail (E_IS_CAL_BACKEND_WEATHER (cbw), GNOME_Evolution_Calendar_OtherError);
	g_return_val_if_fail (tzobj != NULL, GNOME_Evolution_Calendar_OtherError);

	priv = cbw->priv;

	tz_comp = icalparser_parse_string (tzobj);
	g_return_val_if_fail (tz_comp != NULL, GNOME_Evolution_Calendar_InvalidObject);

	if (icalcomponent_isa (tz_comp) != ICAL_VTIMEZONE_COMPONENT)
		return GNOME_Evolution_Calendar_InvalidObject;

	zone = icaltimezone_new ();
	icaltimezone_set_component (zone, tz_comp);
	tzid = icaltimezone_get_tzid (zone);

	if (g_hash_table_lookup (priv->zones, tzid)) {
		icaltimezone_free (zone, TRUE);
		return GNOME_Evolution_Calendar_Success;
	}

	g_hash_table_insert (priv->zones, g_strdup (tzid), zone);
	return GNOME_Evolution_Calendar_Success;
}

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
	GList *components, *l, *objects;

	cbw = E_CAL_BACKEND_WEATHER (backend);
	priv = cbw->priv;

	if (!priv->cache) {
		e_data_cal_view_notify_done (query, GNOME_Evolution_Calendar_NoSuchCal);
		return;
	}

	sexp = e_data_cal_view_get_object_sexp (query);
	if (!sexp) {
		e_data_cal_view_notify_done (query, GNOME_Evolution_Calendar_InvalidQuery);
		return;
	}

	objects = NULL;
	components = e_cal_backend_cache_get_components (priv->cache);
	for (l = components; l != NULL; l = g_list_next (l)) {
		if (e_cal_backend_sexp_match_comp (sexp, E_CAL_COMPONENT (l->data), backend))
			objects = g_list_append (objects, e_cal_component_get_as_string (l->data));
	}

	if (objects)
		e_data_cal_view_notify_objects_added (query, (const GList *) objects);

	g_list_foreach (components, (GFunc) g_object_unref, NULL);
	g_list_free (components);
	g_list_foreach (objects, (GFunc) g_free, NULL);
	g_list_free (objects);
	g_object_unref (sexp);

	e_data_cal_view_notify_done (query, GNOME_Evolution_Calendar_Success);
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
	gboolean loaded;

	cbw = E_CAL_BACKEND_WEATHER (backend);
	priv = cbw->priv;

	loaded = e_cal_backend_weather_is_loaded (backend);

	switch (mode) {
		case CAL_MODE_LOCAL:
		case CAL_MODE_REMOTE:
			priv->mode = mode;
			set_mode = cal_mode_to_corba (mode);
			if (loaded && priv->reload_timeout_id) {
				g_source_remove (priv->reload_timeout_id);
				priv->reload_timeout_id = 0;
			}
			break;
		case CAL_MODE_ANY:
			priv->mode = mode;
			set_mode = cal_mode_to_corba (mode);
			if (loaded)
				g_idle_add ((GSourceFunc) begin_retrieval_cb, backend);
			break;
		default:
			set_mode = GNOME_Evolution_Calendar_MODE_ANY;
			break;
	}

	if (loaded) {
		if (set_mode == GNOME_Evolution_Calendar_MODE_ANY)
			e_cal_backend_notify_mode (backend,
						   GNOME_Evolution_Calendar_CalListener_MODE_NOT_SUPPORTED,
						   cal_mode_to_corba (priv->mode));
		else
			e_cal_backend_notify_mode (backend,
						   GNOME_Evolution_Calendar_CalListener_MODE_SET,
						   set_mode);
	}
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

static void
free_zone (gpointer data)
{
	icaltimezone_free (data, TRUE);
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

	g_hash_table_destroy (priv->zones);

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

	priv->reload_timeout_id = 0;
	priv->source_changed_id = 0;
	priv->opened = FALSE;
	priv->source = NULL;
	priv->cache = NULL;

	priv->zones = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, free_zone);
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
	sync_class->get_object_sync = e_cal_backend_weather_get_object;
	sync_class->get_object_list_sync = e_cal_backend_weather_get_object_list;
	sync_class->get_timezone_sync = e_cal_backend_weather_get_timezone;
	sync_class->add_timezone_sync = e_cal_backend_weather_add_timezone;
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
