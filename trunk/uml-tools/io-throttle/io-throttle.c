/*
 * This is a simple way to limit the disk I/O usage
 * of a hungry but relatively unimportant process.
 * Its only argument is the token limit, as a number
 * between 0 and 1.
 *
 * Example usage:
 *   io-throttle 0.75 < huge.log | analyze-log -
 *
 * -- Micah Dowty <micah@navi.cx>
 */

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>

/* Given a key name, extract a numeric value from a string
 * of key=value pairs.
 */
static int extract_key(const char* pairs, const char* key)
{
    /* Point 'p' immediately after the key string */
    const char* p = strstr(pairs, key);
    if (!p)
	return 0;
    p += strlen(key);

    /* Skip the '=' delimiter */
    if (*p != '=')
	return 0;
    p++;

    /* The rest is a number hopefully */
    return atoi(p);
}

/* Read /proc/io_status and return a floating point number
 * between 0 and 1 indicating how many tokens we have left.
 */
static float get_token_usage()
{
    char buffer[256];
    int fd;
    int retval;

    /* Read all of io_status into a small buffer */
    fd = open("/proc/io_status", O_RDONLY);
    if (fd < 0) {
	perror("open");
	exit(1);
    }
    retval = read(fd, buffer, sizeof(buffer)-1);
    buffer[retval] = '\0';
    close(fd);

    return extract_key(buffer, "io_tokens") /
	(float)extract_key(buffer, "token_max");
}

int main(int argc, char** argv)
{
    float limit;
    char buffer[4096];
    ssize_t retval;

    if (argc != 2) {
	printf("usage: %s limit\n", argv[0]);
	return 1;
    }
    limit = atof(argv[1]);

    while (1) {
	while (get_token_usage() < limit)
	    usleep(100000);

	retval = read(STDIN_FILENO, buffer, sizeof(buffer));
	if (retval < 0) {
	    perror("read");
	    return 1;
	}
	if (retval == 0)
	    break;
	retval = write(STDOUT_FILENO, buffer, retval);
	if (retval <= 0) {
	    perror("write");
	    return 1;
	}
    }

    return 0;
}
