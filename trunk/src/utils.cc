#include "tvbuggy.h"

time_t
parseTime(const char *time)
{
	struct tm tm;

	strptime(time, "%Y%m%d00000", &tm);

	return mktime(&tm);
}
