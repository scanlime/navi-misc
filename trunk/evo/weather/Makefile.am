INCLUDES =						\
	-DG_LOG_DOMAIN=\"libecalbackendweather\"	\
	-I$(top_srcdir)					\
	-I$(top_builddir)				\
	-I$(top_srcdir)/calendar			\
	-I$(top_builddir)/calendar			\
	-I$(top_srcdir)/calendar/libical/src		\
	-I$(top_builddir)/calendar/libical/src		\
	-I$(top_srcdir)/calendar/libical/src/libical	\
	-I$(top_builddir)/calendar/libical/src/libical	\
	-I$(top_srcdir)/addressbook			\
	-I$(top_builddir)/addressbook			\
	$(EVOLUTION_CALENDAR_CFLAGS)			\
	$(SOUP_CFLAGS)

noinst_LTLIBRARIES = libecalbackendweather.la

libecalbackendweather_la_SOURCES =	\
	e-cal-backend-weather.c		\
	e-cal-backend-weather.h		\
	e-weather-source.c		\
	e-weather-source.h		\
	e-weather-source-ccf.c		\
	e-weather-source-ccf.h

libecalbackendweather_la_LIBADD =	\
	$(top_builddir)/calendar/libedata-cal/libedata-cal.la	\
	$(SOUP_LIBS)

