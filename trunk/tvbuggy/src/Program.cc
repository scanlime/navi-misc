#include <string.h>
#include "tvbuggy.h"

Program::Program()
	: Node(), channel(NULL), title(NULL), subtitle(NULL), description(NULL),
	  date(NULL), startTime(), endTime()
{
}

Program::~Program()
{
	delete title;
	delete subtitle;
	delete description;
	delete date;
}

void
Program::setChannel(Channel *channel)
{
	this->channel = channel;
}

void
Program::setChannel(int number)
{
	this->channel = new Channel(number);
}

void
Program::setTitle(const char *title)
{
	this->title = strdup(title);
}

void
Program::setSubTitle(const char *subtitle)
{
	this->subtitle = strdup(subtitle);
}

void
Program::setDescription(const char *description)
{
	this->description = strdup(description);
}

void
Program::setDate(const char *date)
{
	this->date = strdup(date);
}

void
Program::setStartTime(time_t startTime)
{
	this->startTime = startTime;
}

void
Program::setStartTime(const char *startTime)
{
	this->startTime = parseTime(startTime);
}

void
Program::setEndTime(const char *endTime)
{
	this->endTime = parseTime(endTime);
}

void
Program::setEndTime(time_t endTime)
{
	this->endTime = endTime;
}
		
Channel *
Program::getChannel() const
{
	return channel;
}

const char *
Program::getTitle() const
{
	return title;
}

const char *
Program::getSubTitle() const
{
	return subtitle;
}

const char *
Program::getDescription() const
{
	return description;
}

const char *
Program::getDate() const
{
	return date;
}

time_t
Program::getStartTime() const
{
	return startTime;
}

time_t
Program::getEndTime() const
{
	return endTime;
}

