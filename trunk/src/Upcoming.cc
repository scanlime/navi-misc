#include "tvbuggy.h"

UpComing::UpComing()
	: LinkedList()
{
}

UpComing::~UpComing()
{
}

void
UpComing::load()
{
}

void
UpComing::save()
{
}

void
UpComing::addTimeSlot(TimeSlot *timeSlot)
{
	add(timeSlot);
}

TimeSlot *
UpComing::getTimeSlots() const
{
	return (TimeSlot *)getNodes();
}

