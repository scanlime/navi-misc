INCLUDES =						\
	-DG_LOG_DOMAIN=\"libecalbackendweather\"	\
	-DWEATHER_DATADIR=\""$(weatherdatadir)"\"	\
	$(EDS_BACKEND_CFLAGS)

extension_LTLIBRARIES = libecalbackendweather.la

libecalbackendweather_la_SOURCES =	\
	e-cal-backend-weather-factory.c	\
	e-cal-backend-weather-factory.h	\
	e-cal-backend-weather.c		\
	e-cal-backend-weather.h		\
	e-weather-source.c		\
	e-weather-source.h		\
	e-weather-source-accid.c	\
	e-weather-source-accid.h	\
	e-weather-source-ccf.c		\
	e-weather-source-ccf.h

libecalbackendweather_la_LIBADD =	\
	$(EDS_BACKEND_LIBS)		\
	$(SOUP_LIBS)

libecalbackendweather_la_LDFLAGS =	\
	-avoid-version
