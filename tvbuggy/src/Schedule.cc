#include "tvbuggy.h"

extern Configuration *config;

typedef struct
{
	Schedule *schedule;

	ScheduledProgram *program;

	bool inProgram;
	bool inDays;

} ScheduleParserState;

static void
schedStartDocument(void *ctx)
{
	ScheduleParserState *state = (ScheduleParserState *)ctx;

	state->inProgram = false;
	state->inDays    = false;
}

static void
schedEndDocument(void *ctx)
{
}

static void
schedStartElement(void *ctx, const xmlChar *_name, const xmlChar **_atts)
{
	ScheduleParserState *state = (ScheduleParserState *)ctx;
	const char *name  = (const char *)_name;
	const char **atts = (const char **)_atts;

	if (!strcmp(name, "program"))
	{
		state->inProgram = true;
		
		state->program = new ScheduledProgram();

		if (atts != NULL)
		{
			int i;

			for (i = 0; atts[i] != NULL; i += 2)
			{
				const char *att = atts[i];
				const char *value = atts[i + 1];

				if (!strcmp(att, "repeat"))
				{
					if (!strcmp(value, "true"))
						state->program->setRepeatingSchedule(true);
				}
				else if (!strcmp(att, "channel"))
				{
					state->program->setChannel(atoi(value));
				}
				else if (!strcmp(att, "start"))
				{
					state->program->setStartTime(value);
				}
				else if (!strcmp(att, "end"))
				{
					state->program->setEndTime(value);
				}
			}
		}
	}
	else if (!strcmp(name, "days"))
	{
		state->inDays = true;
	}
	else if (!strcmp(name, "sunday"))
	{
		state->program->setOnDay(Sunday, true);
	}
	else if (!strcmp(name, "monday"))
	{
		state->program->setOnDay(Monday, true);
	}
	else if (!strcmp(name, "tuesday"))
	{
		state->program->setOnDay(Tuesday, true);
	}
	else if (!strcmp(name, "wednesday"))
	{
		state->program->setOnDay(Wednesday, true);
	}
	else if (!strcmp(name, "thursday"))
	{
		state->program->setOnDay(Thursday, true);
	}
	else if (!strcmp(name, "friday"))
	{
		state->program->setOnDay(Friday, true);
	}
	else if (!strcmp(name, "saturday"))
	{
		state->program->setOnDay(Saturday, true);
	}
}

static void
schedEndElement(void *ctx, const xmlChar *_name)
{
	ScheduleParserState *state = (ScheduleParserState *)ctx;
	const char *name = (const char *)_name;

	if (!strcmp(name, "program"))
	{
		state->inProgram = false;

		state->schedule->addProgram(state->program);

		state->program = NULL;
	}
	else if (!strcmp(name, "days"))
	{
		state->inDays = false;
	}
}

#if 0
static void
schedCharacters(void *ctx, const xmlChar *chars, int len)
{
	ScheduleParserState *state = (ScheduleParserState *)ctx;
	char *newBuffer;
	int   newLen;

	newLen    = state->bufferLen + len;
	newBuffer = (char *)malloc(newLen + 1);

	strncpy(newBuffer, state->buffer, state->bufferLen);
	strncat(newBuffer, chars, len);
	newBuffer[newLen] = '\0';

	if (state->buffer != NULL)
		free(state->buffer);

	state->buffer    = newBuffer;
	state->bufferLen = newLen;
}
#endif

static void
schedWarning(void *ctx, const char *msg, ...)
{
	va_list args;

	fprintf(stderr, "Schedule parser warning: ");
	va_start(args, msg);
	vfprintf(stderr, msg, args);
	va_end(args);
}

static void
schedError(void *ctx, const char *msg, ...)
{
	va_list args;

	fprintf(stderr, "Schedule parser error: ");
	va_start(args, msg);
	vfprintf(stderr, msg, args);
	va_end(args);
}

static void
schedFatalError(void *ctx, const char *msg, ...)
{
	va_list args;

	fprintf(stderr, "Schedule parser fatal error: ");
	va_start(args, msg);
	vfprintf(stderr, msg, args);
	va_end(args);
}
 
static xmlSAXHandler scheduleHandler =
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
	schedStartDocument,
	schedEndDocument,
	schedStartElement,
	schedEndElement,
	NULL,
	NULL, /*schedCharacters, */
	NULL,
	NULL,
	NULL,
	schedWarning,
	schedError,
	schedFatalError
};


/**************************************************************************
 * Schedule class
 **************************************************************************/
Schedule::Schedule()
	: ProgramList()
{
}

Schedule::~Schedule()
{
}

void
Schedule::load()
{
	ScheduleParserState state;
	xmlParserCtxtPtr ctxt;
	const char *scheduleFile;
	Schedule *schedule;

	scheduleFile = config->getScheduleFile();

	ctxt = xmlCreateFileParserCtxt(scheduleFile);

	if (ctxt == NULL)
		return;

	state.schedule = this;

	ctxt->sax = &scheduleHandler;
	ctxt->userData = &state;

	xmlParseDocument(ctxt);

	if (!ctxt->wellFormed)
		fprintf(stderr, "Schedule file not well formed!\n");

	ctxt->sax = NULL;
	xmlFreeParserCtxt(ctxt);
}

void
Schedule::save()
{
}

