#include "tvbuggy.h"

Channel::Channel(int num)
	: Node(), number(num), id(NULL), name(NULL), iconUrl(NULL)
{
	programs = new ProgramList();
}

Channel::~Channel()
{
	delete id;
	delete name;
	delete iconUrl;
	delete programs;
}

void
Channel::addProgram(Program *program)
{
	programs->addProgram(program);
}


Channel *
getChannel(int num)
{
	Channel *channel;

	channel = new Channel(num);

	return channel;
}

