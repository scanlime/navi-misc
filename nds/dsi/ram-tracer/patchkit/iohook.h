/*
 * iohook.h - Functions for using I/O hooks.
 *
 * Copyright (C) 2009 Micah Dowty
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef __IOHOOK_H
#define __IOHOOK_H

#include "iohook_defs.h"


/*
 * Low-level I/O: Read or write variable length blocks.
 *
 * IOHook_Send() returns a cookie that can be passed to IOHook_Recv().
 * IOHook_Recv() returns the number of bytes actually read.
 *
 * Both functions always read/write using 32-bit operations,
 * and they always read or write in units of 28 bytes. This
 * is especially important for IOHook_Recv(). You'll need to
 * reserve extra buffer space if your data is not a multiple
 * of 28 bytes.
 */

void IOHook_Init(void);
uint32_t IOHook_Send(uint8_t service, const uint32_t *data, uint32_t len);
uint32_t IOHook_Recv(uint32_t cookie, uint32_t *data, uint32_t len);

/*
 * Buffered version of IOHook_Send, for string data.
 *
 * XXX: It's a bit wasteful to have string literals in the binary just so we can
 *      send copies of them back to the PC. If there was an easy way to keep string
 *      literals in a separate section, we could just send a pointer which the host
 *      could look up from our ELF file.
 */
uint32_t IOHook_SendStr(uint8_t service, const char *str);

/*
 * Higher-level I/O
 */

static inline void
IOHook_LogStr(const char *str)
{
   IOHook_SendStr(IOH_SVC_LOG_STR, str);
}

static inline void
IOHook_LogHex(const uint32_t *data, uint32_t len)
{
   IOHook_Send(IOH_SVC_LOG_HEX, data, len);
}

static inline void
IOHook_Quit(const char *str)
{
   IOHook_SendStr(IOH_SVC_QUIT, str);
   while (1);
}

static inline void
IOHook_FOpenW(const char *str)
{
   IOHook_SendStr(IOH_SVC_FOPEN_W, str);
}

static inline void
IOHook_FOpenR(const char *str)
{
   IOHook_SendStr(IOH_SVC_FOPEN_R, str);
}

static inline void
IOHook_FSeek(uint32_t offset)
{
   IOHook_Send(IOH_SVC_FSEEK, &offset, sizeof offset);
}

static inline void
IOHook_FWrite(const uint32_t *data, uint32_t len)
{
   IOHook_Send(IOH_SVC_FWRITE, data, len);
}

/*
 * Read data from file, using multiple packets if necessary.
 * May write up to IOH_DATA_LEN bytes past the end of the
 * buffer.
 */
void IOHook_FRead(uint32_t *data, uint32_t len);


#endif /* __IOHOOK_H */
