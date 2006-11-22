#include <nds.h>
#include <nds/arm9/exceptions.h>
#include <stdio.h>
#include <stdarg.h>
#include <loader-exit.h>
#include "debug.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_INTERNAL_DEBUG_H

#ifdef FT_DEBUG_LEVEL_TRACE
int ft_trace_levels[trace_count];
#endif

void Panic(void)
{
    iprintf("\nPanic! Press any key for loader");

    REG_IME = 0;
    do {
	scanKeys();
    } while (keysHeld());
    do {
	scanKeys();
    } while (!keysHeld());

    Loader_Exit9();
    iprintf("\nNo loader found! Halting.\n");
    while (1);
}
 
void FT_Message(const char*  fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    viprintf(fmt, ap);
    va_end(ap);
}

void FT_Panic(const char*  fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    viprintf(fmt, ap);
    va_end(ap);

    Panic();
}

void ft_debug_init(void)
{
#ifdef FT_DEBUG_LEVEL_TRACE
    int i;
    for (i=0; i<trace_count; i++) {
	ft_trace_levels[i] = 10;
    }
#endif
}

void exception_handler() {
    int i;
    u32 currentMode = getCPSR() & 0x1f;
    u32 thumbState = ((*(u32*)0x027FFD90) & 0x20);    
    u32 savedPC = *(u32*)0x027FFD98;
    u32 exceptionAddress;

    static const char *registerNames[] = {
	"r0","r1","r2","r3","r4","r5","r6","r7",
	"r8","r9","r10","r11","r12","sp","lr","pc",
    };

    exceptionRegisters[15] = savedPC;

    if (currentMode == 0x17) {
	/* Data abort- actual faulting instruction was 8 bytes earlier */
	exceptionAddress = savedPC - 8;
    } else {
	/*
	 * XXX: Assuming invalid instruction error?
	 * Place the fault at the previous instruction.
	 */
	exceptionAddress = savedPC - (thumbState ? 2 : 4);
    }

    iprintf("\nException %02x @ %08x (%s)\n",
	    currentMode, exceptionAddress,
	    thumbState ? "Thumb" : "ARM");

    for (i = 0; i < 8; i++) {
	iprintf(" %-03s %08x ", registerNames[i], exceptionRegisters[i]);
	iprintf(" %-03s %08x \n", registerNames[i+8], exceptionRegisters[i+8]);
    }

    Panic();
}
