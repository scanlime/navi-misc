#include <libgnomevfs/gnome-vfs.h>
#include "e-weather-source-ccf.h"

gboolean done;

void finished (GList *list)
{
	GList *data;
	data = list;
	do
	{
		WeatherForecast *f = data->data;
		g_print ("%s:\n\t%f/%f\n\t%d\n%d%%\n\n", ctime (&f->curtime), f->high, f->low, f->conditions, f->pop);
	} while (data = g_list_next (data));
	done = TRUE;
}

int
main ()
{
	g_type_init ();
	gnome_vfs_init ();
	EWeatherSource *source = e_weather_source_ccf_new ("weather:ccf/BOU/DEN");
	done = FALSE;
	e_weather_source_parse (source, finished);
	while (!done)
		g_main_context_iteration (NULL, TRUE);
	gnome_vfs_shutdown ();
}
