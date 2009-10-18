/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-
 *
 * log_index.h -- Maintains an 'index' database for each log file, and performs
 *                queries using this database.
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

#ifndef __LOG_INDEX_H
#define __LOG_INDEX_H

#include <wx/thread.h>
#include <wx/event.h>
#include <boost/shared_ptr.hpp>
#include <map>
#include <algorithm>

#include "sqlite3x.h"
#include "mem_transfer.h"
#include "log_reader.h"
#include "lru_cache.h"

class LogInstant;
class TransferSummary;

typedef boost::shared_ptr<LogInstant> instantPtr_t;
typedef boost::shared_ptr<TransferSummary> transferPtr_t;


/*
 * An array of values, one per log strata. Each value can hold up to 56
 * bits of data, and is serialized using a variable-length integer encoding.
 */

class LogStrata {
public:
    LogStrata(int numStrata)
        : count(numStrata), values(new uint64_t[numStrata])
    {}

    LogStrata(const LogStrata &copy)
        : count(copy.count), values(new uint64_t[copy.count])
    {
        std::copy(copy.values, copy.values + copy.count, values);
    }

    ~LogStrata()
    {
        delete[] values;
    }

    bool operator ==(const LogStrata &other)
    {
        if (count != other.count)
            return false;
        for (int i = 0; i < count; i++)
            if (values[i] != other.values[i])
                return false;
        return true;
    }

    uint64_t get(int index)
    {
        return values[index];
    }

    void set(int index, uint64_t value)
    {
        values[index] = value;
    }

    void update(int index, uint64_t value, bool reverse=false)
    {
        if (reverse)
            values[index] -= value;
        else
            values[index] += value;
    }

    size_t getPackedLen();
    void pack(uint8_t *buffer);
    void unpack(const uint8_t *buffer, size_t bufferLen);
    void clear();

private:
    int count;
    uint64_t *values;
};


/*
 * A summary of the log's state at a particular instant. LogInstants
 * can be retrieved from a LogIndex, and the LogIndex internally
 * caches them.
 */

class LogInstant {
public:
    LogInstant(int numStrata,
               MemTransfer::ClockType _time = 0,
               MemTransfer::OffsetType _offset = 0,
               bool cleared = false)
        : readTotals(numStrata),
          writeTotals(numStrata),
          zeroTotals(numStrata),
          time(_time),
          offset(_offset)
    {
        if (cleared)
            clear();
    }

    bool operator ==(const LogInstant &other)
    {
        return time == other.time &&
               offset == other.offset &&
               transferId == other.transferId &&
               readTotals == other.readTotals &&
               writeTotals == other.writeTotals &&
               zeroTotals == other.zeroTotals;
    }

    void clear();

    MemTransfer::ClockType time;
    MemTransfer::OffsetType offset;
    MemTransfer::OffsetType transferId;

    void updateTime(MemTransfer::ClockType amount, bool reverse=false)
    {
        if (reverse)
            time -= amount;
        else
            time += amount;
    }

    LogStrata readTotals;
    LogStrata writeTotals;
    LogStrata zeroTotals;
};


/*
 * A summary of a single MemTransfer. These can be retrieved from a
 * LogIndex, and LogIndex caches them. This class is similar to
 * MemTransfer, with a few differences:
 *
 *   - This includes a timestamp, which MemTransfers (and the on-disk
 *     format) does not directly include.
 *
 *   - This does not include the contents of the transfer, just the
 *     metadata.
 */

class TransferSummary {
public:
    typedef MemTransfer::ClockType ClockType;
    typedef MemTransfer::OffsetType OffsetType;
    typedef MemTransfer::LengthType LengthType;
    typedef MemTransfer::TypeEnum TypeEnum;
    typedef MemTransfer::AddressType AddressType;

    // Invalid transfer
    TransferSummary(ClockType _time=-1,
                    OffsetType _offset=-1,
                    OffsetType _id=-1)
        : time(_time),
          type(MemTransfer::ERROR_UNAVAIL),
          address(-1),
          byteCount(0),
          offset(_offset),
          id(_id)
    {}

    /*
     * Note that this timestamp is the _end_ of the transfer, not the
     * beginning. This is consistent with the way LogInstants and the
     * strata cache operate.
     */
    ClockType time;

    TypeEnum type;
    AddressType address;
    LengthType byteCount;
    OffsetType offset;
    OffsetType id;

    const wxString getTypeName() {
        return MemTransfer::getTypeName(type);
    }
};


class LogIndex {
public:
    typedef MemTransfer::ClockType ClockType;
    typedef MemTransfer::OffsetType OffsetType;
    typedef MemTransfer::AddressType AddressType;

    enum State {
        IDLE,
        INDEXING,
        FINISHING,
        COMPLETE,
        ERROR,
    };

    LogIndex();
    ~LogIndex();

    /*
     * Opening/closing a log file automatically starts/stops
     * indexing. The indexer runs in a background thread.
     */
    void Open(LogReader *reader);
    void Close();

    /*
     * Anyone can ask about the current state of our index.  You can
     * also request that we send a wxWidgets event (with the ID returned
     * by GetProgressEvent()) by calling SetProgressReceiver.
     */
    State GetState() { return state; }
    double GetProgress() { return progress; }
    wxEventType GetProgressEvent() { return progressEvent; }
    void SetProgressReceiver(wxEvtHandler *handler);

    /*
     * We keep a running total of the log's duration during
     * indexing, and after indexing is complete this is an
     * accurate representation of the log's total length.
     */

    ClockType GetDuration() {
        return lastInstant->time;
    }
    OffsetType GetNumTransfers() {
        return lastInstant->transferId;
    }

    /*
     * Information about the (fixed) geometry of the log index.
     */
    int GetBlockSize() const {
        return BLOCK_SIZE;
    }
    int GetNumBlocks() const {
        return (reader->MemSize() + BLOCK_MASK) >> BLOCK_SHIFT;
    }
    int GetNumStrata() const {
        return (reader->MemSize() + STRATUM_MASK) >> STRATUM_SHIFT;
    }
    AddressType GetStratumFirstAddress(int s) const {
        return s << STRATUM_SHIFT;
    }
    AddressType GetStratumLastAddress(int s) const {
        return (s << STRATUM_SHIFT) | STRATUM_MASK;
    }

    /*
     * Get a summary of the state of the log at a particular instant.
     * An instant is identified by a particular clock cycle.
     *
     * It's possible to look up the log state for any given clock cycle,
     * but in most applications it isn't important to find exactly a
     * particular cycle. So, the lookup can often be 'fuzzy'. We'll
     * return an instant that's no farther than 'distance' from the
     * specified time.
     */
    instantPtr_t GetInstant(ClockType time, ClockType distance = 0);

    /*
     * Get a summary of a particular memory transfer. This includes
     * information about the transfer's type, offset, timestamp,
     * address, and length. It does not include the transfer's data
     * contents.
     *
     * If 'id' can't be found or it represents an invalid transfer,
     * the error will be reported via 'type' in the returned transfer.
     * Both successful and unsuccessful lookups are cached.
     */
    transferPtr_t GetTransferSummary(OffsetType id);

private:
    /*
     * Definitions:
     *
     *   timestep -- A unit of time progress. Timestep boundaries are where we
     *               write a snapshot of the current log state to the index.
     *               Timesteps are spaced apart according to a maximum log length
     *               between them, specified in bytes.
     *
     *   stratum -- A very coarse spatial division, for calculating quick metrics
     *              used to navigate the log file.
     *
     *   block -- A fine spatial unit used for storing logged data in manageable
     *            chunks. Any data that changes during a timestep is indexed with
     *            block granularity.
     */

    static const int INSTANT_CACHE_SIZE = 1 << 15;

    static const int TIMESTEP_SIZE = 256 * 1024;     // Timestep duration, in bytes

    static const int BLOCK_SHIFT = 9;                // 512 bytes
    static const int BLOCK_SIZE = 1 << BLOCK_SHIFT;
    static const int BLOCK_MASK = BLOCK_SIZE - 1;

    static const int STRATUM_SHIFT = 14;             // 16 kB (1024 strata per 16MB)
    static const int STRATUM_SIZE = 1 << STRATUM_SHIFT;
    static const int STRATUM_MASK = STRATUM_SIZE - 1;

    void DeleteCommands();
    void InitDB();
    void Finish();
    bool CheckFinished();
    void SetProgress(double progress, State state);
    void StartIndexing();
    void StoreInstant(LogInstant &instant);
    void AdvanceInstant(LogInstant &instant, MemTransfer &mt, bool reverse = false);
    instantPtr_t GetInstantForTimestep(ClockType upperBound);
    instantPtr_t GetInstantFromStartingPoint(instantPtr_t start, ClockType time,
                                             ClockType distance = 0);

    class IndexerThread : public wxThread {
    public:
        IndexerThread(LogIndex *_index) : index(_index) {}
        virtual ExitCode Entry();

    private:
        LogIndex *index;
    };

    // Always acquire locks in the order listed.
    wxCriticalSection dataLock;  // Local data: reader, all caches, lastInstant
    wxCriticalSection dbLock;    // Protects the database and cmd_*

    sqlite3x::sqlite3_connection db;
    sqlite3x::sqlite3_command *cmd_getInstantForTimestep;
    sqlite3x::sqlite3_command *cmd_getTransferSummary;

    LogReader *reader;
    IndexerThread *indexer;
    double logFileSize;

    FuzzyCache<ClockType, instantPtr_t> instantCache;
    FuzzyCache<OffsetType, transferPtr_t> transferCache;
    instantPtr_t lastInstant;

    State state;
    double progress;
    static wxEventType progressEvent;
    wxEvtHandler *progressReceiver;
};


#endif /* __LOG_INDEX_H */
