#ifndef _TVBUGGY_H_
#define _TVBUGGY_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#ifdef WITH_DMALLOC
# include <dmalloc.h>
#endif

#ifdef WITH_LEAKBUG
# include <leakbug.h>
# define LB_REGISTER(ptr, dataSize) \
#     lbRegister((ptr), (dataSize), __FILE__, __LINE__, LEAKBUG_DEBUG_LEVEL)
# define LB_REGISTER_ARRAY(ptr, dataSize, numElements) \
#     lbRegisterArray((void **)(ptr), (dataSize), (numElements), \
#                     __FILE__, __LINE__, LEAKBUG_DEBUG_LEVEL)
#else
# define LB_REGISTER(ptr, dataSize)
# define LB_REGISTER_ARRAY(ptr, dataSize, numElements)
#endif

/* For strptime() */
#define _XOPEN_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#include <libxml/parser.h>
#include <libxml/parserInternals.h>

#include "Configuration.h"
#include "Channel.h"
#include "HashTable.h"
#include "Program.h"
#include "ProgramList.h"
#include "Schedule.h"
#include "ScheduledProgram.h"
#include "Upcoming.h"
#include "utils.h"
#include "WatchList.h"

#endif /* _TVBUGGY_H_ */
