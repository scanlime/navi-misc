/* -*- Mode: C; c-basic-offset: 4 -*-
 *
 * Public header for SBT86. Defines common data types for binary
 * translated programs, and the SBTProcess class.
 *
 * SBTProcess is a binary-translated "process", a sandboxed virtual
 * machine with an isolated address space. SBTProcess itself is an
 * abstract base class. SBT86 generates code for subclasses of
 * SBTProcess.
 *
 * Copyright (c) 2009 Micah Dowty <micah@navi.cx>
 *
 *    Permission is hereby granted, free of charge, to any person
 *    obtaining a copy of this software and associated documentation
 *    files (the "Software"), to deal in the Software without
 *    restriction, including without limitation the rights to use,
 *    copy, modify, merge, publish, distribute, sublicense, and/or sell
 *    copies of the Software, and to permit persons to whom the
 *    Software is furnished to do so, subject to the following
 *    conditions:
 *
 *    The above copyright notice and this permission notice shall be
 *    included in all copies or substantial portions of the Software.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *    OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef _SBT86_H_
#define _SBT86_H_

#include <stdint.h>
#include <setjmp.h>
#include <nds/arm9/sassert.h>


/*
 * Forward declarations
 */

class SBTHardware;
class SBTRegs;
class SBTStack;
class SBTProcess;
class SBTSegmentCache;


/*
 * SBTHaltCode --
 *
 *    Enumeration with halt() values we use with SBTProcess.
 */

enum SBTHaltCode {
    SBTHALT_MASK_CODE      = 0xFFFF0000,
    SBTHALT_MASK_ARG       = 0x0000FFFF,

    SBTHALT_DOS_EXIT       = (1 << 16),    // OR'ed with DOS exit code
    SBTHALT_FRAME_DRAWN    = (2 << 16),    // We just rendered a frame
    SBTHALT_LOAD_ROOM_ID   = (3 << 16),    // Loading room ID from AL
    SBTHALT_KEYBOARD_POLL  = (4 << 16),    // About to poll key state
};


/*
 * SBTAddressId --
 *
 *    These are addresses which can be determined statically by an
 *    SBT86 script, then looked up at runtime via SBTProcess.
 */

enum SBTAddressId {
    SBTADDR_WORLD_DATA,
    SBTADDR_CIRCUIT_DATA,
    SBTADDR_ROBOT_DATA,
};


/*
 * SBTHardware --
 *
 *    Abstract base class for a virtual hardware implementation which
 *    backs an SBTProcess. Calls to the process's SBTHardware
 *    implementation are generated by SBT86 itself, and this is a good
 *    place for SBT86 hooks to call into.
 */

class SBTHardware
{
 public:
    virtual uint8_t in(uint16_t port, uint32_t timestamp) = 0;
    virtual void out(uint16_t port, uint8_t value, uint32_t timestamp) = 0;

    virtual SBTRegs interrupt10(SBTProcess *proc, SBTRegs reg) = 0;
    virtual SBTRegs interrupt16(SBTProcess *proc, SBTRegs reg) = 0;
    virtual SBTRegs interrupt21(SBTProcess *proc, SBTRegs reg) = 0;

    virtual void drawScreen(SBTProcess *proc, uint8_t *framebuffer) = 0;
};


/*
 * SBTRegs --
 *
 *    Register state for the virtual 8086 processor.
 */

class SBTRegs
{
 public:
    union {
        uint16_t ax;
        struct {
            uint8_t al, ah;
        };
    };
    union {
        uint16_t bx;
        struct {
            uint8_t bl, bh;
        };
    };
    union {
        uint16_t cx;
        struct {
            uint8_t cl, ch;
        };
    };
    union {
        uint16_t dx;
        struct {
            uint8_t dl, dh;
        };
    };
    uint16_t si, di;
    uint16_t cs, ds, es, ss;
    uint16_t bp, sp;

    /*
     * We cheat enormously on implementing 8086 flags: Instead of
     * calculating the flags for every ALU instruction, we store a
     * 32-bit version of that instruction's result.  All flag tests
     * are rewritten in terms of this result word. Anything that
     * explicitly sets flags does so by tweaking this result word in
     * such a way as to change the flag value we would calculate.
     *
     * To avoid having to store the word width separately, all 8-bit
     * results are left-shifted by 8.
     */
    uint32_t uresult;
    int32_t sresult;

    /*
     * Inline accessors for manually setting/inspecting flags.
     */

    inline bool getZF() {
        return (uresult & 0xFFFF) == 0;
    }
    inline bool getSF() {
        return (uresult & 0x8000) != 0;
    }
    inline bool getOF() {
        return (((sresult >> 1) ^ (sresult)) & 0x8000) != 0;
    }
    inline bool getCF() {
        return (uresult & 0x10000) != 0;
    }

    inline void setZF() {
        uresult &= ~0xFFFF;
    }
    inline void clearZF() {
        uresult |= 1;
    }
    inline void setOF() {
        sresult = 0x8000;
    }
    inline void clearOF() {
        sresult = 0;
    }
    inline void setCF() {
        uresult |= 0x10000;
    }
    inline void clearCF() {
        uresult &= 0xFFFF;
    }

    inline uint32_t saveCF() {
        return uresult & 0x10000;
    }
    inline void restoreCF(uint32_t saved) {
        uresult = (uresult & 0xFFFF) | saved;
    }
};


/*
 * SBTStack --
 *
 *    The virtual stack used by a translated SBT86 process. This
 *    cheats significantly, and the emulated stack is actually not
 *    part of the normal address space. The stack is also strongly
 *    typed, in order to catch translated code which breaks our
 *    assumptions.
 */

class SBTStack
{
 public:
    inline void init() {
        top = 0;
        retVerification = 0xBEEF;
    }

    inline void pushw(uint16_t word) {
        sassert(top < STACK_SIZE, "SBT86 stack overflow");
        words[top] = word;
        tags[top] = STACK_TAG_WORD;
        top++;
    }

    inline void pushf(SBTRegs reg) {
        sassert(top < STACK_SIZE, "SBT86 stack overflow");
        flags[top].uresult = reg.uresult;
        flags[top].sresult = reg.sresult;
        tags[top] = STACK_TAG_FLAGS;
        top++;
    }

    inline void pushret() {
        sassert(top < STACK_SIZE, "SBT86 stack overflow");
        tags[top] = STACK_TAG_RETADDR;
        top++;
    }

    inline uint16_t popw() {
        top--;
        sassert(tags[top] == STACK_TAG_WORD, "SBT86 stack tag mismatch");
        return words[top];
    }

    inline SBTRegs popf(SBTRegs reg) {
        top--;
        sassert(tags[top] == STACK_TAG_FLAGS, "SBT86 stack tag mismatch");
        reg.uresult = flags[top].uresult;
        reg.sresult = flags[top].sresult;
        return reg;
    }

    inline void popret() {
        top--;
        sassert(tags[top] == STACK_TAG_RETADDR, "SBT86 stack tag mismatch");
    }

    /*
     * These are special stack accessors for use in hook routines.
     * Some routines in Robot Odyssey save the return value off
     * the stack, manipulate the caller's stack, then restore
     * the return value.
     *
     * preSaveRet() should be called before the return value is
     * saved at the beginning of such a function. It converts the
     * top of the stack from a RETADDR to a WORD, and stores a
     * verification value in that word.
     *
     * postRestoreRet() should be called after the return value is
     * restored. It verifies the value saved by preSaveRet, and
     * converts the top of stack back to a RETADDR.
     */

    inline void preSaveRet() {
        sassert(tags[top - 1] == STACK_TAG_RETADDR, "SBT86 stack tag mismatch");
        words[top - 1] = retVerification;
        tags[top - 1] = STACK_TAG_WORD;
    }

    inline void postRestoreRet() {
        sassert(tags[top - 1] == STACK_TAG_WORD, "SBT86 stack tag mismatch");
        sassert(words[top - 1] == retVerification, "SBT86 stack retaddr mismatch");
        tags[top - 1] = STACK_TAG_RETADDR;
        retVerification++;
    }

 private:
    enum Tag {
        STACK_TAG_INVALID = 0,
        STACK_TAG_WORD,
        STACK_TAG_FLAGS,
        STACK_TAG_RETADDR,
    };

    static const uint32_t STACK_SIZE = 512;
    uint32_t top;
    uint32_t retVerification;

    Tag tags[STACK_SIZE];
    uint32_t words[STACK_SIZE];
    struct {
        uint32_t uresult;
        int32_t sresult;
    } flags[STACK_SIZE];
};


/*
 * SBTProcess --
 *
 *    An abstract base class to contain the state for one translated process.
 */

class SBTProcess
{
 public:
    /*
     * Prepare an instance of this process to execute. This zeroes
     * memory, and resets the program counter. The process will begin
     * executing from the beginning on the next run().
     *
     * (This function does not start the process, only prepares it.)
     *
     * Must be called once before the first run(), but you may also
     * call this function at any time afterwards.
     */
    void exec(const char *cmdLine = "");

    /*
     * The SBTHardware instance used by this process to emulate
     * software interrupts and I/O operations.
     *
     * Must be set once before the first run(), but you may also set
     * it before any subsequent run().
     */
    SBTHardware *hardware;

    /*
     * Run this process until the next halt(). Returns the code that
     * was passed to halt(). To contine right where we left off, call
     * run() again.
     */
    int run(void);

    /*
     * Exit from the current run() invocation, with the supplied
     * return code.  This function must be called while the process is
     * running, so it may only be invoked by SBT86 hooks or
     * SBTHardware implementations.
     *
     * While the process is halted, it may be copied in order to fork
     * the process.
     */
    void halt(int code);

    /*
     * Return a pointer to an emulated memory segment. Only 64K of
     * memory past the returned pointer is guaranteed to be valid.  If
     * the address is out of range, returns NULL.
     */
    uint8_t *memSeg(uint16_t seg);

    /*
     * Memory access utilities.
     */

    uint8_t peek8(uint16_t seg, uint16_t off);
    void poke8(uint16_t seg, uint16_t off, uint8_t value);
    int16_t peek16(uint16_t seg, uint16_t off);
    void poke16(uint16_t seg, uint16_t off, uint16_t value);

    /*
     * Register state. May be manipulated by anyone, but only when the
     * process is halted. Running processes use cached register state.
     */
    SBTRegs reg;

    /*
     * Emulated stack for this process.
     */
    SBTStack stack;

    /*
     * Error handler for failed dynamic branches.
     */
    void failedDynamicBranch(uint16_t cs, uint16_t ip, uint32_t value);

    /*
     * Look up an address which was calculated statically by SBT86.
     */
    virtual uint16_t getAddress(SBTAddressId id) = 0;

 protected:
    /*
     * Virtual functions generated by SBT86
     */
    virtual void loadCache() = 0;
    virtual void saveCache() = 0;
    virtual uint8_t *getData() = 0;
    virtual uint32_t getDataLen() = 0;
    virtual uint16_t getRelocSegment() = 0;
    virtual uint16_t getEntryCS() = 0;
    virtual uintptr_t getEntryPtr() = 0;

 private:
    /*
     * Real mode memory for this process.
     * This shouldn't be accessed directly, use memSeg().
     */
    static const uint32_t MEM_SIZE = 128 * 1024;
    uint8_t mem[MEM_SIZE];

    /*
     * The highest normal segment we can support.  Any segments over
     * MAX_SEGMENT get remapped to MAX_SEGMENT. This serves two
     * purposes:
     *
     *   1. It makes reads from the BIOS harmless.
     *   2. It puts the CGA framebuffer somewhere useful.
     */
    static const uint32_t MAX_SEGMENT = (MEM_SIZE - 0x10000) >> 4;

    /*
     * Jump buffers for run() and halt(). jmpRun is how we return to run(),
     * and jmpHalt is how we resume execution after a halt().
     */
    jmp_buf jmpRun, jmpHalt;

    /*
     * Native stack for this process. This is the stack we're executing
     * native code off of, and it contains the physical return addresses
     * for each translated procedure call. This is also the stack where we
     * run SBTHardware implementations.
     */
    static const uint32_t NATIVE_STACK_WORDS = 4 * 1024;
    uint32_t nativeStack[NATIVE_STACK_WORDS];
};


/*
 * SBTSegmentCache --
 *
 *    Cached segment pointers. Translated indirects use these pointers
 *    to avoid segment lookups on every memory access.
 */

class SBTSegmentCache
{
 public:
    uint8_t *cs;
    uint8_t *ds;
    uint8_t *es;
    uint8_t *ss;

    /*
     * Functions to load each cached segment.
     */
    inline void loadCS(SBTProcess *process, SBTRegs reg) {
        cs = process->memSeg(reg.cs);
    }
    inline void loadDS(SBTProcess *process, SBTRegs reg) {
        ds = process->memSeg(reg.ds);
    }
    inline void loadES(SBTProcess *process, SBTRegs reg) {
        es = process->memSeg(reg.es);
    }
    inline void loadSS(SBTProcess *process, SBTRegs reg) {
        ss = process->memSeg(reg.ss);
    }

    inline void load(SBTProcess *process, SBTRegs reg) {
        loadCS(process, reg);
        loadDS(process, reg);
        loadES(process, reg);
        loadSS(process, reg);
    }

    /*
     * Static utilities for 16-bit reads and writes. We have to split
     * this up into two 8-bit reads/writes, to avoid the ARM's
     * alignment constraints.
     */
    static inline uint16_t read16(uint8_t *ptr) {
        return ptr[0] | (ptr[1] << 8);
    }
    static inline void write16(uint8_t *ptr, uint16_t x) {
        ptr[0] = (uint8_t) x;
        ptr[1] = x >> 8;
    }
};


/*
 * SBT_DECL_PROCESS --
 *
 *    A macro to declare a process subclass created by SBT86.
 *    This is basically the replacement for a header files for
 *    these classes.
 */

#define SBT_DECL_PROCESS(name)                          \
    class name : public SBTProcess                      \
    {                                                   \
    protected:                                          \
        virtual void loadCache();                       \
        virtual void saveCache();                       \
        virtual uint8_t *getData();                     \
        virtual uint32_t getDataLen();                  \
        virtual uint16_t getRelocSegment();             \
        virtual uint16_t getEntryCS();                  \
        virtual uintptr_t getEntryPtr();                \
    public:                                             \
        virtual uint16_t getAddress(SBTAddressId id);   \
    }


#endif // _SBT86_H_
