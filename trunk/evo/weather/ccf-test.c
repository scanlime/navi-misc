#include "e-weather-source-ccf.h"

const char *ccf =
    "000\n" \
    "FPUS45 KBOU 160630\n" \
    "CCFBOU\n" \
    " \n" \
    "DEN UU 084/053 087/053 090 99000\n" \
    "    UBBBB 053/088 052/075 048/064 041/063 0000001222\n" \
    "BOU UU 084/053 087/053 090 99000\n" \
    "    UBBBB 053/088 052/075 048/064 041/063 0000001222\n";

int
main ()
{
	GList *list;
	GList *data;
	g_type_init ();
	EWeatherSource *source = e_weather_source_ccf_new ("DEN");
	list = e_weather_source_parse (source, ccf);

	data = list;
	do
	{
		WeatherForecast *f = data->data;
		g_print ("%s:\n\t%f/%f\n\t%d\n%d%%\n\n", ctime (&f->curtime), f->high, f->low, f->conditions, f->pop);
	} while (data = g_list_next (data));
}
