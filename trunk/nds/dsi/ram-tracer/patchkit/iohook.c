/*
 * iohook.c - Functions for using I/O hooks.
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

#include "iohook.h"

static uint8_t sequence;

#define MIN(a,b)  ((a) > (b) ? (b) : (a))


void
IOHook_Init(void)
{
   // We don't have real initialized data yet, so this is mandatory.
   sequence = 0;
}


uint32_t
IOHook_Send(uint8_t service, const uint32_t *data, uint32_t len)
{
   uint32_t cookie;

   while (len) {
      uint32_t dataLen, footer;

      cookie = (service << IOH_SVC_SHIFT) | (sequence << IOH_SEQ_SHIFT);
      dataLen = MIN(IOH_DATA_LEN, len);
      footer = cookie | (dataLen << IOH_LEN_SHIFT);
      sequence++;

      /*
       * Use inline assembly to checksum and copy the data.
       * We want to write out the result packet in one memory
       * burst, so use an stm instruction to write the whole
       * packet in one go.
       */
      asm volatile ("ldm %0, {r2-r8} \n"          // Load data

                    // Checksum
                    "add r1, r2, r3 \n"           // Add 32-bit words
                    "add r1, r1, r4 \n"
                    "add r1, r1, r5 \n"
                    "add r1, r1, r6 \n"
                    "add r1, r1, r7 \n"
                    "add r1, r1, r8 \n"
                    "add r12, r1, r1, LSL#8 \n"   // Add 8-bit bytes
                    "add r12, r12, r1, LSL#16 \n"
                    "add r12, r12, r1, LSL#24 \n"
                    "lsr r12, r12, #24 \n"        // Shift checksum

                    "orr r12, r12, %1 \n"         // OR in rest of footer
                    "stm %2, {r2-r8,r12} \n"      // Send packet

                    :: "r" (data),
                       "r" (footer),
                       "r" (IOH_ADDR)
                    : "memory", "r1", "r2", "r3", "r4", "r5",
                      "r6", "r7", "r8", "r12");

      len -= dataLen;
      data = (uint32_t*) (dataLen + (uint8_t*)data);
   }

   return cookie;
}


uint32_t
IOHook_Recv(uint32_t cookie, uint32_t *data, uint32_t len)
{
   len = MIN(IOH_DATA_LEN, len);

   asm volatile("0:"
                "ldm %1, {r2-r8,r12} \n"      // Read patch buffer
                "and r1, r12, %2 \n"          // Check SVC/SEQ
                "cmp r1, %3 \n"
                "bne 0b \n"                   // Poll for correct SVC and SEQ
                "stm %4, {r2-r8} \n"          // Store data

                // Checksum
                "add r1, r2, r3 \n"           // Add 32-bit words
                "add r1, r1, r4 \n"
                "add r1, r1, r5 \n"
                "add r1, r1, r6 \n"
                "add r1, r1, r7 \n"
                "add r1, r1, r8 \n"
                "add r2, r1, r1, LSL#8 \n"    // Add 8-bit bytes
                "add r2, r2, r1, LSL#16 \n"
                "add r2, r2, r1, LSL#24 \n"
                "lsr r2, r2, #24 \n"          // Shift checksum

                "and r1, r12, #0xff \n"       // Mask off received check byte
                "cmp r1, r2 \n"               // Is checksum valid?
                "1: \n"                       //   Get stuck on checksum errors
                "bne 1b \n"

                "mov %0, r12, LSR#8 \n"       // Shift and return packet len

                : "=r" (len)
                : "r" (IOH_ADDR),
                  "r" (IOH_SVC_MASK | IOH_SEQ_MASK),
                  "r" (cookie),
                  "r" (data)
                : "memory", "r1", "r2", "r3", "r4", "r5",
                  "r6", "r7", "r8", "r12");

   return len & 0xFF;
}
