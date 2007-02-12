/* -*- Mode: C; c-basic-offset: 4; indent-tabs-mode: nil -*-
 *
 * Caterpillar Daemon Protocol
 * Copyright (C) 2007 Micah Dowty <micah@navi.cx>
 *
 */

#ifndef __CATD_PROTOCOL_H__
#define __CATD_PROTOCOL_H__

#include <atomic_ops.h>

/*
 * Lock-free single reader / single writer ring buffer.  The buffer is
 * empty when read_index == write_index.  Note that this means the
 * total number of usable slots is actually array_size-1.
 */
typedef struct {
  AO_t write_index;  /* Next slot to write */
  AO_t read_index;   /* Next slot to read */
  int array_size;    /* Number of slots total */
  off_t offset;
} catd_queue_t;

/*
 * Heap header. This must be present at the beginning of the heap
 * file. It defines the location of all other heap objects.
 */
typedef struct {
  catd_queue_t commands;
} catd_header_t;

/*
 * One command for the daemon to process. These are passed via the
 * 'commands' queue.
 */
typedef struct {

} catd_command_t;


#endif /* __CATD_PROTOCOL_H__ */

