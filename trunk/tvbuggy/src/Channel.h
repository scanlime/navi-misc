#ifndef _CHANNEL_H_
#define _CHANNEL_H_

#include "Program.h"
#include "Node.h"

class Program;
class ProgramList;
class Node;

class Channel : public Node
{
	public:
		Channel(int num);
		~Channel();

		void addProgram(Program *program);

	private:
		int number;

		char *id;
		char *name;
		char *iconUrl;

		ProgramList *programs;
};

Channel *getChannel(int num);

#endif /* _CHANNEL_H_ */

