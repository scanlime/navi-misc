#include <string.h>
#include "tvbuggy.h"

typedef struct
{
	Configuration *config;

	bool inPaths;
	bool inListings;
	bool inWatchList;
	bool inSchedule;
	bool inUpcoming;

	char *buffer;
	int   bufferLen;

} ConfigParserState;

static void
cfgStartDocument(void *_state)
{
	ConfigParserState *state = (ConfigParserState *)_state;

	state->inPaths     = false;
	state->inListings  = false;
	state->inWatchList = false;
	state->inSchedule  = false;
	state->inUpcoming  = false;

	state->buffer     = NULL;
	state->bufferLen  = 0;
}

static void
cfgEndDocument(void *_state)
{
	ConfigParserState *state = (ConfigParserState *)_state;

	if (state->buffer != NULL)
	{
		delete state->buffer;
		state->buffer = NULL;
	}
}

static void
cfgStartElement(void *_state, const xmlChar *_name, const xmlChar **_attrs)
{
	ConfigParserState *state = (ConfigParserState *)_state;
	const char *name   = (const char *)_name;
	const char **attrs = (const char **)_attrs;

	if (!strcmp(name, "paths"))
	{
		state->inPaths = true;
	}
	else if (!strcmp(name, "listings"))
	{
		state->inListings = true;
	}
	else if (!strcmp(name, "watchlist"))
	{
		state->inWatchList = true;
	}
	else if (!strcmp(name, "schedule"))
	{
		state->inSchedule = true;
	}
	else if (!strcmp(name, "upcoming"))
	{
		state->inUpcoming = true;
	}
}

static void
cfgEndElement(void *_state, const xmlChar *_name)
{
	ConfigParserState *state = (ConfigParserState *)_state;
	const char *name = (const char *)_name;

	if (!strcmp(name, "paths"))
	{
		state->inPaths = false;
	}
	else if (!strcmp(name, "listings"))
	{
		state->inListings = false;

		state->config->setListingsFile(state->buffer);
	}
	else if (!strcmp(name, "watchlist"))
	{
		state->inWatchList = false;

		state->config->setWatchListFile(state->buffer);
	}
	else if (!strcmp(name, "schedule"))
	{
		state->inSchedule = false;

		state->config->setScheduleFile(state->buffer);
	}
	else if (!strcmp(name, "upcoming"))
	{
		state->inUpcoming = false;

		state->config->setUpcomingFile(state->buffer);
	}

	if (state->buffer != NULL)
	{
		delete state->buffer;

		state->buffer    = NULL;
		state->bufferLen = 0;
	}
}

static void
cfgCharacters(void *_state, const xmlChar *_chars, int len)
{
	ConfigParserState *state = (ConfigParserState *)_state;
	const char *chars = (const char *)_chars;
	char *newBuffer;
	int   newLen;

	newLen    = state->bufferLen + len;
	newBuffer = new char[newLen + 1];

	if (state->buffer == NULL)
	{
		strncpy(newBuffer, chars, len);
	}
	else
	{
		strncpy(newBuffer, state->buffer, state->bufferLen + 1);
		strncat(newBuffer, chars, len);
	}

	newBuffer[newLen] = '\0';

	if (state->buffer != NULL)
		delete state->buffer;

	state->buffer    = newBuffer;
	state->bufferLen = newLen;
}

static void
cfgWarning(void *state, const char *msg, ...)
{
	va_list args;

	fprintf(stderr, "Config parser warning: ");
	va_start(args, msg);
	vfprintf(stderr, msg, args);
	va_end(args);
}

static void
cfgError(void *state, const char *msg, ...)
{
	va_list args;

	fprintf(stderr, "Config parser error: ");
	va_start(args, msg);
	vfprintf(stderr, msg, args);
	va_end(args);
}

static void
cfgFatalError(void *state, const char *msg, ...)
{
	va_list args;

	fprintf(stderr, "Config parser fatal error: ");
	va_start(args, msg);
	vfprintf(stderr, msg, args);
	va_end(args);
}
 
static xmlSAXHandler configHandler =
{
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	cfgStartDocument,
	cfgEndDocument,
	cfgStartElement,
	cfgEndElement,
	NULL,
	cfgCharacters,
	NULL,
	NULL,
	NULL,
	cfgWarning,
	cfgError,
	cfgFatalError
};


/**************************************************************************
 * Configuration class
 **************************************************************************/
Configuration::Configuration()
	: listingsFile(NULL), scheduleFile(NULL), watchListFile(NULL),
	  upcomingFile(NULL)
{
}

Configuration::~Configuration()
{
	if (listingsFile  != NULL) delete listingsFile;
	if (scheduleFile  != NULL) delete scheduleFile;
	if (watchListFile != NULL) delete watchListFile;
	if (upcomingFile  != NULL) delete upcomingFile;
}

void
Configuration::load()
{
	xmlParserCtxtPtr  ctxt;
	ConfigParserState state;

	ctxt = xmlCreateFileParserCtxt("/home/chipx86/.tvbuggy/config.xml");

	if (!ctxt)
		return;

	ctxt->sax = &configHandler;
	ctxt->userData = &state;

	state.config = this;

	xmlParseDocument(ctxt);

	if (!ctxt->wellFormed)
		fprintf(stderr, "Config file not well formed!\n");

	ctxt->sax = NULL;
	xmlFreeParserCtxt(ctxt);
}

void
Configuration::save()
{
}

void
Configuration::setListingsFile(const char *file)
{
	listingsFile = strdup(file);
}

void
Configuration::setScheduleFile(const char *file)
{
	scheduleFile = strdup(file);
}

void
Configuration::setWatchListFile(const char *file)
{
	watchListFile = strdup(file);
}

void
Configuration::setUpcomingFile(const char *file)
{
	upcomingFile = strdup(file);
}

const char *
Configuration::getListingsFile() const
{
	return listingsFile;
}

const char *
Configuration::getScheduleFile() const
{
	return scheduleFile;
}

const char *
Configuration::getWatchListFile() const
{
	return watchListFile;
}

const char *
Configuration::getUpcomingFile() const
{
	return upcomingFile;
}

