#ifndef _PROGRAMLIST_H_
#define _PROGRAMLIST_H_

#include "LinkedList.h"

class ProgramList : private LinkedList
{
	public:
		ProgramList() : LinkedList() {}
		virtual ~ProgramList() {}

		void addProgram(Program *program) { add(program); }
		
		Program *getPrograms() const { return (Program *)getNodes(); }
};

#endif /* _PROGRAMLIST_H_ */
