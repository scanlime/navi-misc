#ifndef _SCHEDULEDPROGRAM_H_
#define _SCHEDULEDPROGRAM_H_

#include "utils.h"

class Program;

class ScheduledProgram : public Program
{
	public:
		ScheduledProgram();
		~ScheduledProgram();

		void setOnDay(DayOfWeek day, bool on);
		void setRepeatingSchedule(bool repeat);

		bool isOnDay(DayOfWeek day) const;

		bool isRepeatingSchedule() const;

	private:
		bool days[7];
		bool repeat;
};

#endif /* _SCHEDULEDPROGRAM_H_ */
