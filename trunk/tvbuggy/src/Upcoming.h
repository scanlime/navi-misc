#ifndef _TVBUGGY_UPCOMING_H_
#define _TVBUGGY_UPCOMING_H_

#include "tvbuggy.h"

class TimeSlot : public Node, private LinkedList
{
	public:
		TimeSlot() : Node() {}
		virtual ~TimeSlot() {}

		void addProgram(Program *program) { add(program);      }
		void setTime(time_t time)         { this->time = time; }

		Program *getPrograms() const      { return (Program *)getNodes(); }
		time_t getTime()                  { return time;                  }

	private:
		time_t time;
};

class UpComing : private LinkedList
{
	public:
		UpComing();
		virtual ~UpComing();

		void load();
		void save();

		void addTimeSlot(TimeSlot *timeSlot);

		TimeSlot *getTimeSlots() const;
};

#endif /* _TVBUGGY_UPCOMING_H_ */
