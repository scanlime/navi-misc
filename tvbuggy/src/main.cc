#include "tvbuggy.h"

Configuration *config;

int
main(int argc, char **argv)
{
	Schedule *schedule;
	WatchList *watchlist;

	printf("Loading configuration.\n");
	config = new Configuration();
	config->load();

	printf("Loading watch list.\n");

	printf("Loading schedule.\n");
	schedule = new Schedule();

	/* Clean up. */
	delete config;
	delete schedule;

	return 0;
}
