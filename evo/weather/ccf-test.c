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
	g_type_init ();
	EWeatherSource *source = e_weather_source_ccf_new ("DEN");
	e_weather_source_parse (source, ccf);
}
