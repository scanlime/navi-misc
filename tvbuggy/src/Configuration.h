#ifndef _CONFIG_H_
#define _CONFIG_H_

class Configuration
{
	public:
		Configuration();
		~Configuration();

		void load();
		void save();

		void setListingsFile(const char *file);
		void setScheduleFile(const char *file);
		void setWatchListFile(const char *file);
		void setUpcomingFile(const char *file);

		const char *getListingsFile()  const;
		const char *getScheduleFile()  const;
		const char *getWatchListFile() const;
		const char *getUpcomingFile()  const;

	private:
		char *listingsFile;
		char *scheduleFile;
		char *watchListFile;
		char *upcomingFile;
};

#endif /* _CONFIG_H_ */

