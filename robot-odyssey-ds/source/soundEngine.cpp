/* -*- Mode: C++; c-basic-offset: 4 -*-
 *
 * Sound engine for Robot Odyssey DS. This is a special-purpose PC
 * speaker emulator which runs mainly on the ARM7. It is designed to
 * handle the PC speaker mode Robot Odyssey uses, in which the speaker
 * output is used as a 1-bit DAC.
 *
 * The PC speaker emulation is performed in software, and we stream
 * the results to the DS's sound hardware via a single looping sound
 * channel. We double-buffer this sound channel in two halves. While
 * the first half is playing, we're filling the second half, and vice
 * versa.
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

#include <nds.h>
#include <string.h>
#include "soundEngine.h"

uint8_t SoundEngine::buffer[SoundEngine::BUFFER_SIZE];
int SoundEngine::bufferIndex;
bool SoundEngine::streaming;
uint8_t SoundEngine::edgeState;
uint32_t SoundEngine::currentTime;


void SoundEngine::init(int timerChannel)
{
    soundOn();
    streaming = false;
    edgeState = 0;
    initStreaming(timerChannel);
}

void SoundEngine::soundOn()
{
    int pmReg;

    powerOn((PM_Bits) POWER_SOUND);

    pmReg = readPowerManagement(PM_CONTROL_REG);
    pmReg &= ~PM_SOUND_MUTE;
    pmReg |= PM_SOUND_AMP;
    writePowerManagement(PM_CONTROL_REG, pmReg);

    REG_SOUNDCNT = SOUND_ENABLE;
    REG_MASTER_VOLUME = 127;
}

void SoundEngine::initStreaming(int timerChannel)
{
    memset(buffer, 0, sizeof buffer);
    bufferIndex = 0;

    /*
     * Next timer tick will populate the first half of the buffer
     * (bufferIndex=0) but by the time we get there, the audio
     * hardware will already be started on the second half.  We want
     * the sound hardware and Timer 0 to stay synchronized, so that
     * they're always operating on opposite halves of 'buffer'.
     */
    timerStart(timerChannel,
               ClockDivider_256,
               timerFreqToTicks_256(SAMPLE_RATE / SAMPLES_PER_FRAME),
               timerCallback);

    /*
     * Start playing a circular sound buffer that holds 2 frames.
     */
    SCHANNEL_SOURCE(CHANNEL_PCSPEAKER)       = (uint32_t) &buffer[0];
    SCHANNEL_REPEAT_POINT(CHANNEL_PCSPEAKER) = 0;
    SCHANNEL_LENGTH(CHANNEL_PCSPEAKER)       = BUFFER_SIZE / sizeof(uint32_t);
    SCHANNEL_TIMER(CHANNEL_PCSPEAKER)        = SOUND_FREQ(SAMPLE_RATE);
    SCHANNEL_CR(CHANNEL_PCSPEAKER)           = SCHANNEL_ENABLE |
                                               SOUND_VOL(127) |
                                               SOUND_PAN(64) |
                                               SOUND_REPEAT |
                                               SOUND_FORMAT_8BIT;
}

void SoundEngine::timerCallback()
{
    /*
     * Perform per-buffer state updates, then fill the next frame
     * (1/2 of 'buffer') with audio samples.
     */

    uint8_t *bufPtr = buffer + bufferIndex;
    bufferIndex ^= SAMPLES_PER_FRAME;

    if (streaming) {
        /*
         * We're in the middle of an audio stream.  Generate samples
         * based on the values in our FIFO, which are timestamps at
         * which a PC speaker edge transition (0->1 or 1->0) occurred.
         */

        int remaining;

        for (remaining = SAMPLES_PER_FRAME; remaining; remaining--, bufPtr++) {
            uint16_t head = getIPC()->head;
            uint16_t tail = getIPC()->tail;

            if (head == tail) {
                /*
                 * Stream ended. Fill the rest of the buffer with silence.
                 */
                streaming = false;
                memset(bufPtr, edgeState, remaining);
                break;
            }

            /*
             * Slurp up any events from the timestamp buffer which have
             * elapsed by now, and adjust the current speaker state
             * accordingly.
             */

            while (head != tail &&
                   ((int32_t)(getIPC()->fifo[tail] - currentTime)) <= 0) {

                edgeState ^= 0x80;
                getIPC()->tail = tail = (tail + 1) & (FIFO_SIZE - 1);
                head = getIPC()->head;
            }

            currentTime += CLOCKS_PER_SAMPLE;
            *bufPtr = edgeState;
        }

    } else {
        /*
         * No stream is currently in progress. Output silence.  If we
         * see data in the FIFO, start a stream on the _next_ timer
         * tick, to give the FIFO some opportunity to pre-buffer.
         */

        if (getIPC()->head != getIPC()->tail) {
            currentTime = getIPC()->fifo[getIPC()->tail];
            streaming = true;
        }

        memset(bufPtr, edgeState, SAMPLES_PER_FRAME);
    }
}
