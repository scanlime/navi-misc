#ifndef _PROGRAM_H_
#define _PROGRAM_H_

#include <time.h>

#include "Channel.h"
#include "Node.h"

class Channel;
class Node;

class Program : public Node
{
	public:
		Program();
		virtual ~Program();

		void setChannel(Channel *channel);
		void setChannel(int number);

		void setTitle(const char *title);
		void setSubTitle(const char *subtitle);
		void setDescription(const char *description);
		void setDate(const char *date);

		void setStartTime(time_t startTime);
		void setEndTime(time_t endTime);
		
		void setStartTime(const char *startTime);
		void setEndTime(const char *endTime);
		
		Channel *getChannel() const;

		const char *getTitle() const;
		const char *getSubTitle() const;
		const char *getDescription() const;
		const char *getDate() const;

		time_t getStartTime() const;
		time_t getEndTime() const;

	private:
		Channel *channel;

		char *title;
		char *subtitle;
		char *description;
		char *date;
		
		time_t startTime;
		time_t endTime;
};

#endif /* _PROGRAM_H_ */

