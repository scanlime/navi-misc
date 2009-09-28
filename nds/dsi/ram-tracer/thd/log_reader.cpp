/*
 * log_reader.cpp -- Encapsulates the details of reading the low-level log file format.
 *                   To add new file formats, this is the only object that should need
 *                   to change at all.
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

#include "log_reader.h"
#include "memtrace_fmt.h"


void
LogReader::Open(const wxChar *path)
{
  fileName.Assign(path);
  fileName.MakeAbsolute();
  file.Open(fileName.GetFullPath());
}

void
LogReader::Close()
{
  file.Close();
}


/*
 * Decode the burst at mt.logOffset, storing the results in mt.
 */

bool
LogReader::Read(MemTransfer &mt)
{
  MemTransfer::OffsetType offset = mt.LogOffset();
  bool haveAddress = false;

  mt.duration = 0;
  mt.byteCount = 0;
  mt.type = mt.ERROR_PROTOCOL;

  /*
   * Keep reading packets until we reach a second ADDR.
   */

  while (true) {
    uint8_t *bytes = file.Get(offset, sizeof(MemPacket));
    if (!bytes) {
      return false;
    }

    MemPacket packet = MemPacket_FromBytes(bytes);
    MemPacketType type = MemPacket_GetType(packet);
    
    if (MemPacket_IsOverflow(packet)) {
      mt.type = mt.ERROR_OVERFLOW;
      return true;
    }
    if (!MemPacket_IsAligned(packet)) {
      mt.type = mt.ERROR_SYNC;
      return true;
    }
    if (!MemPacket_IsChecksumCorrect(packet)) {
      mt.type = mt.ERROR_CHECKSUM;
      return true;
    }

    if (type == MEMPKT_ADDR) {
      if (haveAddress) {
	// This is the first packet in the next transfer. Abort. */
	return true;
      }
      haveAddress = true;
      mt.address = MemPacket_GetPayload(packet) << 1;
    }
    
    // Add up the duration of every packet that's inside the transfer
    mt.duration += MemPacket_GetDuration(packet);
    offset += sizeof(MemPacket);
    
    // Set the transfer direction, make sure it's consistent.

    if (type == MEMPKT_READ) {
      if (mt.byteCount) {
	if (mt.type != mt.READ) {
	  mt.type = mt.ERROR_PROTOCOL;
	  return true;
	}
      } else {
	mt.type = mt.READ;
      }

    } else if (type == MEMPKT_WRITE) {
      if (mt.byteCount) {
	if (mt.type != mt.WRITE) {
	  mt.type = mt.ERROR_PROTOCOL;
	  return true;
	}
      } else {
	mt.type = mt.WRITE;
      }

    } else {
      // The rest of this only applies to data transfer packets
      continue;
    }

    // Decode data transfer packets

    if (mt.byteCount >= mt.MAX_LENGTH) {
      mt.type = mt.ERROR_PROTOCOL;
      return true;
    }      

    bool ub = MemPacket_RW_UpperByte(packet);
    bool lb = MemPacket_RW_LowerByte(packet);
    uint16_t word = MemPacket_RW_Word(packet);

    if (!(ub && lb)) {
      /*
       * This is a single-byte read/write, which we support only in a
       * limited capacity. Single-byte read/write packets must be the
       * only read or write in the transfer.
       */
      if (mt.byteCount) {
	mt.type = mt.ERROR_PROTOCOL;
	return true;
      }

      if (lb) {
	mt.buffer[mt.byteCount++] = word & 0xFF;
      } else {
	mt.address++;
	mt.buffer[mt.byteCount++] = word >> 8;
      }

    } else {
      // Word-wide read/write
      mt.buffer[mt.byteCount++] = word & 0xFF;
      mt.buffer[mt.byteCount++] = word >> 8;
    }
  } 
}


/*
 * Seek to the beginning of the next transfer.
 */
  
bool
LogReader::Next(MemTransfer &mt)
{
  /*
   * We can use the transfer's byteCount to calculate the minimum
   * number of packets that this transfer must have taken up on
   * disk. We'll start looking for the next transfer there.  Usually
   * this estimate will be correct- but if there are any extra packets
   * (like timestamp packets) we'll skip over them.
   */

  MemTransfer::OffsetType offset = mt.LogOffset();
  uint32_t wordCount = (mt.byteCount + 1) >> 1;
  uint32_t minPackets = 1 + wordCount;
  offset += minPackets * sizeof(MemPacket);

  while (true) {
    uint8_t *bytes = file.Get(offset, sizeof(MemPacket));
    if (!bytes) {
      return false;
    }

    MemPacket packet = MemPacket_FromBytes(bytes);

    if (MemPacket_IsAligned(packet)) {
      if (MemPacket_GetType(packet) == MEMPKT_ADDR) {
	mt.Seek(offset);
	return true;
      }
      offset += sizeof(MemPacket);
    } else {
      offset++;
    }
  }
}
    

/*
 * Seek to the beginning of the previous transfer.
 */

bool
LogReader::Prev(MemTransfer &mt)
{
  MemTransfer::OffsetType offset = mt.LogOffset();

  while (true) {
    if (offset < sizeof(MemPacket)) {
      return false;
    }
    offset -= sizeof(MemPacket);

    uint8_t *bytes = file.Get(offset, sizeof(MemPacket));
    if (!bytes) {
      return false;
    }

    MemPacket packet = MemPacket_FromBytes(bytes);

    if (MemPacket_IsAligned(packet)) {
      if (MemPacket_GetType(packet) == MEMPKT_ADDR) {
	mt.Seek(offset);
	return true;
      }
    } else {
      offset += sizeof(MemPacket) - 1;
    }
  }
}


double
LogReader::GetDefaultClockHZ()
{
  return RAM_CLOCK_HZ;
}
