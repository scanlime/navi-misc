#include "tvbuggy.h"

ScheduledProgram::ScheduledProgram()
	: Program(), repeat(false)
{
	for (int i = 0; i < 7; i++)
		days[i] = false;
}

ScheduledProgram::~ScheduledProgram()
{
}

void
ScheduledProgram::setOnDay(DayOfWeek day, bool on)
{
	days[day] = on;
}

void
ScheduledProgram::setRepeatingSchedule(bool repeat)
{
	this->repeat = repeat;
}

bool
ScheduledProgram::isOnDay(DayOfWeek day) const
{
	return days[day];
}

bool
ScheduledProgram::isRepeatingSchedule() const
{
	return repeat;
}

