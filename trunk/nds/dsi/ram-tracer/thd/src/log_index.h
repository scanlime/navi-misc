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

class LogInstant;

typedef boost::shared_ptr<LogInstant> instantPtr_t;


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
               MemTransfer::OffsetType _offset = 0)
        : readTotals(numStrata),
          writeTotals(numStrata),
          zeroTotals(numStrata),
          time(_time),
          offset(_offset)
    {}

    bool operator ==(const LogInstant &other)
    {
        return time == other.time &&
               offset == other.offset &&
               readTotals == other.readTotals &&
               writeTotals == other.writeTotals &&
               zeroTotals == other.zeroTotals;
    }

    void clear();

    MemTransfer::ClockType time;
    MemTransfer::OffsetType offset;

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
 * A utility class that manages a cache of LogInstants.  The cached
 * instants are sorted by time, and they can be queried to find nearby
 * instants rather than just exact matches.
 */

class InstantCache {
public:
    InstantCache(int numStrata)
        : defaultInstant(new LogInstant(numStrata))
    {
        defaultInstant->clear();
    }

    typedef MemTransfer::ClockType key_t;
    typedef std::map<key_t, instantPtr_t> map_t;

    static key_t distance(key_t a, key_t b)
    {
        if (a >= b)
            return a - b;
        else
            return b - a;
    }

    instantPtr_t findClosest(key_t k)
    {
        map_t::iterator below = cacheMap.upper_bound(k);
        map_t::iterator above = cacheMap.lower_bound(k);
        bool belowExists = below != cacheMap.end();
        bool aboveExists = above != cacheMap.end();

        if (belowExists && (!aboveExists || (k - below->first <= above->first - k))) {
            return below->second;
        }

        if (aboveExists && (!belowExists || (k - below->first >= above->first - k))) {
            return above->second;
        }

        // Map is empty. Return a blank instant.
        return defaultInstant;
    }

    void store(instantPtr_t v)
    {
        cacheMap.insert(map_t::value_type(v->time, v));
        count++;
    }

    void vacuum(void)
    {
        // XXX: Cull old entries from the cache, to get it below the max size
    }

private:
    static const int INSTANT_CACHE_SIZE = 4096;

    instantPtr_t defaultInstant;
    int count;
    map_t cacheMap;
};


class LogIndex {
public:
    typedef MemTransfer::ClockType ClockType;

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
     * accurate representation of the log's total length in
     * clock ticks.
     */
    ClockType GetDuration() { return duration; }

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
    instantPtr_t GetInstant(ClockType time, ClockType distance=0);

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

    static const int TIMESTEP_SIZE = 256 * 1024;     // Timestep duration, in bytes

    static const int BLOCK_SHIFT = 9;                // 512 bytes
    static const int BLOCK_SIZE = 1 << BLOCK_SHIFT;
    static const int BLOCK_MASK = BLOCK_SIZE - 1;

    static const int STRATUM_SHIFT = 16;             // 64 kB (256 strata per 16MB)
    static const int STRATUM_SIZE = 1 << STRATUM_SHIFT;
    static const int STRATUM_MASK = STRATUM_SIZE - 1;

    void InitDB();
    void Finish();
    bool CheckFinished();
    void SetProgress(double progress, State state);
    void StartIndexing();
    void StoreInstant(LogInstant &instant);
    void AdvanceInstant(LogInstant &instant, MemTransfer &mt, bool reverse = false);
    instantPtr_t GetInstantForTimestep(ClockType upperBound);

    class IndexerThread : public wxThread {
    public:
        IndexerThread(LogIndex *_index) : index(_index) {}
        virtual ExitCode Entry();

    private:
        LogIndex *index;
    };

    LogReader *reader;
    wxCriticalSection dbLock;
    sqlite3x::sqlite3_connection db;
    IndexerThread *indexer;
    double logFileSize;
    ClockType duration;

    InstantCache instantCache;

    sqlite3x::sqlite3_command *cmd_getInstantForTimestep;

    State state;
    double progress;
    static wxEventType progressEvent;
    wxEvtHandler *progressReceiver;
};


#endif /* __LOG_INDEX_H */
