#ifndef _TVBUGGY_UTILS_H_
#define _TVBUGGY_UTILS_H_

typedef enum
{
	Sunday    = 0,
	Monday    = 1,
	Tuesday   = 2,
	Wednesday = 3,
	Thursday  = 4,
	Friday    = 5,
	Saturday  = 6

} DayOfWeek;

time_t parseTime(const char *time);

#endif /* _TVBUGGY_UTILS_H_ */
