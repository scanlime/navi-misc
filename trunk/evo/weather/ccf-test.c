#include <libgnomevfs/gnome-vfs.h>
#include "e-weather-source-ccf.h"

gboolean done;

void finished (GList *list, gpointer d)
{
	GList *data;

	if (list == NULL)
	{
		g_print ("Error!\n");
		done = TRUE;
		return;
	}
	data = list;
	do
	{
		WeatherForecast *f = data->data;
		g_print ("%s\tTemp:\t%f/%f\n\tCond:\t%d\n\tPOP:\t%d%%\n\n", ctime (&f->date), f->high, f->low, f->conditions, f->pop);
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
	e_weather_source_parse (source, finished, NULL);
	while (!done)
		g_main_context_iteration (NULL, TRUE);
	gnome_vfs_shutdown ();
}
