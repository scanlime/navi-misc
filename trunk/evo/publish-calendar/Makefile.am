INCLUDES =				\
	-I$(top_srcdir)			\
	$(EVOLUTION_CALENDAR_CFLAGS)

@EVO_PLUGIN_RULE@

plugin_DATA = org-gnome-publish-calendar.eplug
plugin_LTLIBRARIES = liborg-gnome-publish-calendar.la

liborg_gnome_publish_calendar_la_SOURCES = \
	publish-calendar.c

liborg_gnome_publish_calendar_la_LDFLAGS = -module -avoid-version

EXTRA_DIST = org-gnome-publish-calendar.eplug.in
