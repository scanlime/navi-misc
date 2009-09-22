/*
 * Trivial inlines for memory operations.
 * --Micah Dowty
 */

#include <stdint.h>

static inline
memset32(void *dest, uint32_t value, uint32_t bytes)
{
   uint32_t *dest32 = dest;
   while (bytes -= sizeof *dest32)
      *(dest32++) = value;
}

static inline
memcpy32(void *dest, const void *src, uint32_t bytes)
{
   const uint32_t *src32 = src;
   uint32_t *dest32 = dest;
   while (bytes -= sizeof *dest32)
      *(dest32++) = *(src32++);
}
