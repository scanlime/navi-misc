/* -*- Mode: C; c-basic-offset: 4; indent-tabs-mode: nil -*-
 *
 * Caterpillar Daemon - A super-lightweight persistent connection aggregator/proxy
 * Copyright (C) 2007 Micah Dowty <micah@navi.cx>
 *
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include "protocol.h"

/* Performance parameters */
#define EPOLL_SIZE      512
#define NUM_EVENTS      64
#define SERVER_BACKLOG  2

static struct {
    char basepath[PATH_MAX];    
    char scratch_buffer[8192];
    AO_t exit_flag;

    int command_fd;
    int event_fd;

    void *heap_base;
    off_t heap_size;
    catd_header_t *header;

    int epoll_fd;
} catd;

/* Look up a dynamically sized heap object. Returns NULL on failure. */
#define HOBJ_SIZED(offset, type, size)  ((type *) ( ((offset) + (size) <= catd.heap_size) ? (catd.heap_base + (offset)) : NULL ))

/* Look up a statically sized heap object */
#define HOBJ(offset, type)  HOBJ_SIZED(offset, type, sizeof(type))


/************************************************************************/
/************************************************** Utilities ***********/
/************************************************************************/

/* Fatal error: only for use when absolutely necessary! */
static void
panic_msg(const char *message)
{
    printf("Panic: %s\n", message);
    exit(1);
}

static void
panic_errno(const char *message)
{
    perror(message);
    exit(1);
}

static const char *
catd_static_path(const char *extension)
{
    static char buffer[PATH_MAX];
    if (snprintf(buffer, PATH_MAX, "%s.%s", catd.basepath, extension) >= PATH_MAX) {
	panic_msg("Path too long");
    }
    return buffer;
}

static int
set_nonblock(int fd)
{
    int retval = fcntl(fd, F_GETFL);
    if (retval < 0) {
	return -1;
    }
    return fcntl(fd, F_SETFL, retval | O_NONBLOCK);
}

static void
catd_sighandler_exit(int signal)
{
    AO_store(&catd.exit_flag, 1);
}


/************************************************************************/
/************************************************** Initialization ******/
/************************************************************************/

static void
catd_init_path(int argc, char **argv)
{
    char cwd[PATH_MAX];
    if (!getcwd(cwd, sizeof cwd)) {
        panic_errno("getting current directory");
    }

    if (argc != 2) {
        fprintf(stderr, "usage: %s <basepath>\n", argv[0]);
        exit(1);
    }

    /* Save an absolute path: when we daemonize, we change directories to '/' */
    if (snprintf(catd.basepath, sizeof catd.basepath, "%s/%s", cwd, argv[1]) >= sizeof catd.basepath) {
	panic_msg("Path too long");
    }
}

static void
catd_init_heap(void)
{
    int heap_fd;
    struct stat st;

    heap_fd = open(catd_static_path("heap"), O_RDWR);
    if (heap_fd < 0) {
	panic_errno("open heap");
    }	

    if (fstat(heap_fd, &st)) {
	panic_errno("stat heap");
    }
    catd.heap_size = st.st_size;

    catd.heap_base = mmap(NULL, catd.heap_size, PROT_READ | PROT_WRITE,
			  MAP_SHARED, heap_fd, 0);
    if (catd.heap_base == MAP_FAILED) {
	panic_errno("mmap heap");
    }
    
    /* Close the heap file, but keep the memory mapped */
    close(heap_fd);

    /* Store a direct pointer to the heap header */
    catd.header = HOBJ(0, catd_header_t);
    if (!catd.header) {
        panic_msg("heap is too small");
    }
}

static void
catd_init_poll(void)
{
    catd.epoll_fd = epoll_create(EPOLL_SIZE);
    if (catd.epoll_fd < 0) {
	panic_msg("epoll_create");
    }
}

static void
catd_init_pipes(void)
{
    struct epoll_event ev;

    /*
     * The command and event pipes are used to send idempotent wakeups
     * to and from catd. The data passed through each pipe is ignored-
     * they are just used as a doorbell for commands stored in the heap.
     *
     * This architecture frees us from worrying about blocking while
     * writing to each pipe: if the pipe buffer is full, there's already
     * a wakeup pending anyway.
     */ 

    catd.command_fd = open(catd_static_path("command"), O_RDONLY | O_NONBLOCK);
    if (catd.command_fd < 0) {
	panic_errno("open command pipe");
    }

    /* Must open the event pipe O_RDWR rather than O_WRONLY, so we
     * don't block waiting for a reader.
     */
    catd.event_fd = open(catd_static_path("event"), O_RDWR | O_NONBLOCK);
    if (catd.event_fd < 0) {
	panic_errno("open event pipe");
    }

    /* Begin listening for commands */
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = catd.command_fd;
    if (epoll_ctl(catd.epoll_fd, EPOLL_CTL_ADD, catd.command_fd, &ev) < 0) {
        panic_errno("listening on command pipe");
    }
}

static void
catd_init_daemon(void)
{
    FILE *pidfile = fopen(catd_static_path("pid"), "w");
    if (!pidfile) {
	panic_errno("open pidfile");
    }

    /*
    if (daemon(0, 0) < 0) {
	panic_errno("daemonizing");
    }
    */

    signal(SIGINT, catd_sighandler_exit);
    signal(SIGTERM, catd_sighandler_exit);
    signal(SIGPIPE, SIG_IGN);

    fprintf(pidfile, "%d\n", getpid());
    fclose(pidfile);
}

static void
catd_cleanup_daemon(void)
{
    unlink(catd_static_path("pid"));
}

/* int catd_listen(const char *path) */
/* { */
/*     struct sockaddr_un addr; */

/*     catd.listen_fd = socket(PF_UNIX, SOCK_STREAM, 0); */
/*     if (catd.listen_fd < 0) { */
/* 	return catd.listen_fd; */
/*     } */

/*     memset(&addr, 0, sizeof(addr)); */
/*     server_addr.sun_family = PF_UNIX; */
/*     if (strlen(path) >= sizeof(server_addr.sun_path)) { */
/*     strcpy(server_addr.sun_path, path, ); */

/*     if (bind(server_fd, (struct sockaddr*) &server_addr, */
/* 	     sizeof(server_addr))) { */
/* 	perror("bind"); */
/*         return 1; */
/*     } */

/*     if (listen(server_fd, SERVER_BACKLOG)) { */
/* 	perror("listen"); */
/* 	return 1; */
/*     } */

/*     if (set_nonblock(server_fd)) { */
/* 	perror("set_nonblock"); */
/* 	return 1; */
/*     } */

/*     new_event.events = EPOLLET | EPOLLIN; */
/*     new_event.data.fd = server_fd; */
/*     if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &new_event)) { */
/* 	perror("epoll_ctl"); */
/* 	return 1; */
/*     } */
/* } */


/************************************************************************/
/************************************************** Command Processing **/
/************************************************************************/

static void
catd_process_command(catd_command_t *command)
{

}

static void
catd_process_commands(void)
{
    catd_queue_t *queue = &catd.header->commands;
    int write_index = AO_load(&queue->write_index);
    int read_index = AO_load(&queue->read_index);

    if (!queue->array_size) {
        return;
    }
    
    while (read_index != write_index) {
        catd_command_t *command = HOBJ(
            queue->offset + read_index * sizeof(catd_command_t), catd_command_t);
        if (command) {
            catd_process_command(command);
        }
        read_index = (read_index + 1) % queue->array_size;
    }

    AO_store(&queue->read_index, read_index);
}


/************************************************************************/
/************************************************** Main Loop ***********/
/************************************************************************/

static void 
catd_poll_loop(void)
{
    int nfds, i;
    struct epoll_event events[NUM_EVENTS];

    while (!AO_load(&catd.exit_flag)) {
        nfds = epoll_wait(catd.epoll_fd, events, NUM_EVENTS, -1);
        if (nfds < 0) {
            switch (errno) {
            case EINVAL:
            case EINTR:
                continue;
            default:
                panic_errno("epoll_wait");
            }
        }

        for (i=0; i<nfds; i++) { 

            if (events[i].data.fd == catd.command_fd) {
                /*
                 * This is the command doorbell. Disarm the doorbell
                 * first, so that if the doorbell is poked again during
                 * command processing we'll check the queue again.
                 */
                while (read(catd.command_fd, catd.scratch_buffer, sizeof catd.scratch_buffer) > 0);
                catd_process_commands();
            }

        }
    }
}

int
main(int argc, char **argv)
{
    catd_init_path(argc, argv);
    catd_init_heap();
    catd_init_poll();
    catd_init_pipes();
    catd_init_daemon();

    catd_poll_loop();

    catd_cleanup_daemon();

    return 0;
}
