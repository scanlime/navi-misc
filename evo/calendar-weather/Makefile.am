INCLUDES =				\
	$(EVOLUTION_CFLAGS)		\
	$(EVOLUTION_CALENDAR_CFLAGS)	\
	$(SOURCE_SEL_CFLAGS)		\
	-DWEATHER_DATADIR=\""$(weatherdatadir)"\"

@EVO_PLUGIN_RULE@

plugin_DATA = org-gnome-calendar-weather.eplug
plugin_LTLIBRARIES = liborg-gnome-calendar-weather.la

liborg_gnome_calendar_weather_la_SOURCES = calendar-weather.c
liborg_gnome_calendar_weather_la_LDFLAGS = -module -avoid-version

EXTRA_DIST = org-gnome-calendar-weather.eplug.in
