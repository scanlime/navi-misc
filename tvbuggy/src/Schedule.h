#ifndef _SCHEDULE_H_
#define _SCHEDULE_H_

class ProgramList;

class Schedule : public ProgramList
{
	public:
		Schedule();
		virtual ~Schedule();

		void load();
		void save();
};

#endif /* _SCHEDULE_H_ */
