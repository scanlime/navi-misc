INCLUDES =					\
	$(EVOLUTION_CFLAGS)			\
	$(SOURCE_SEL_CFLAGS)

plugin_LTLIBRARIES = libcomnovell-calendar-http.la

calendar_http_in_files = com-novell-calendar-http.eplug.in

plugin_DATA = $(calendar_http_in_files:.eplug.in=.eplug)
@EVO_PLUGIN_RULE@

libcomnovell_calendar_http_la_SOURCES	=	\
	calendar-http.c

libcomnovell_calendar_http_la_LIBADD	=	\
	$(EVOLUTION_LIBS)			\
	$(SOURCE_SEL_LIBS)
