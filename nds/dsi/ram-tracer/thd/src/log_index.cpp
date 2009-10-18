/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-
 *
 * log_index.cpp -- Maintains an 'index' database for each log file, and performs
 *                  queries using this database.
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

#define __STDC_LIMIT_MACROS
#include <stdint.h>
#include <stdio.h>
#include <wx/string.h>
#include <assert.h>
#include "log_index.h"
#include "varint.h"

using namespace sqlite3x;

wxEventType LogIndex::progressEvent = 0;

/*
 * DEBUG: This enables very expensive debugging checks which verify the
 * integrity of the index. Don't enable them unless you suspect something
 * is wrong with the index, since the checks are extremely slow.
 */
#define INDEX_DEBUG  0


LogIndex::LogIndex()
    : progressReceiver(NULL),
      cmd_getInstantForTimestep(NULL),
      cmd_getTransferSummary(NULL),
      lastInstant(GetInstantForTimestep(0)),
      instantCache(INSTANT_CACHE_SIZE, GetInstantForTimestep(0)),
      transferCache(INSTANT_CACHE_SIZE, transferPtr_t(new TransferSummary()))
{
    if (!progressEvent)
        progressEvent = wxNewEventType();

    SetProgress(0.0, IDLE);
}


LogIndex::~LogIndex()
{
    Close();
}


void
LogIndex::Open(LogReader *reader)
{
    /*
     * While we're holding the database lock, open the DB and start
     * indexing if necessary.
     */
    {
        wxCriticalSectionLocker locker(dbLock);

        this->reader = reader;
        logFileSize = std::max<double>(1.0, reader->FileName().GetSize().ToDouble());

        wxFileName indexFile = reader->FileName();
        indexFile.SetExt(wxT("index"));
        wxString indexPath = indexFile.GetFullPath();

        db.open(indexPath.fn_str());
        InitDB();

        /*
         * Is this index complete and up-to-date?
         * If not, throw it away and start over.
         *
         * TODO: In the future we may want to store indexing progress, so we
         *       can stop indexing partway through and resume later.
         */
        if (CheckFinished()) {
            SetProgress(1.0, COMPLETE);
        } else {
            db.close();
            DeleteCommands();

            wxRemoveFile(indexPath);
            db.open(indexPath.fn_str());

            InitDB();
            StartIndexing();
        }
    }

    /*
     * If the index is complete, store some important state from the
     * database. If we're still indexing, this state will be stored
     * periodically by the indexer.
     */
    if (GetState() != INDEXING) {
        lastInstant = GetInstantForTimestep(INT64_MAX);
    }
}


void
LogIndex::DeleteCommands()
{
    // Assumes dbLock is already locked.

    if (cmd_getInstantForTimestep) {
        delete cmd_getInstantForTimestep;
        cmd_getInstantForTimestep = NULL;
    }

    if (cmd_getTransferSummary) {
        delete cmd_getTransferSummary;
        cmd_getTransferSummary = NULL;
    }
}


void
LogIndex::Close()
{
    wxCriticalSectionLocker locker(dbLock);
    db.close();
    DeleteCommands();
    reader = NULL;
}


void
LogIndex::InitDB()
{
    // Assumes dbLock is already locked.

    // Our index database can be regenerated at any time, so trade reliability for speed.
    db.executenonquery("PRAGMA journal_mode = OFF");
    db.executenonquery("PRAGMA synchronous = OFF");
    db.executenonquery("PRAGMA legacy_file_format = OFF");
    db.executenonquery("PRAGMA cache_size = 10000");

    // Stores state for Finish()/checkinished().
    db.executenonquery("CREATE TABLE IF NOT EXISTS logInfo ("
                       "name, mtime, timestepSize, blockSize, stratumSize)");

    /*
     * The strata- thick layers of coarse but quick spatial stats.
     * Every single timeslice in the log has a row in this table. The
     * counters for each stratum are stored as a packed list of varints
     * in a BLOB.
     */

    db.executenonquery("CREATE TABLE IF NOT EXISTS strata ("
                       "time INTEGER PRIMARY KEY ASC,"
                       "offset,"
                       "transferId,"
                       "readTotals,"
                       "writeTotals,"
                       "zeroTotals"
                       ")");

    // Snapshots of modified blocks at each timeslice
    db.executenonquery("CREATE TABLE IF NOT EXISTS wblocks ("
                       "time,"
                       "block,"
                       "firstOffset,"
                       "lastOffset,"
                       "data"
                       ")");
}


/*
 * Perform all of the final steps for completing the index, and mark
 * it as complete.
 */
void
LogIndex::Finish()
{
    SetProgress(1.0, FINISHING);

    wxCriticalSectionLocker locker(dbLock);
    sqlite3_transaction transaction(db);

    // Used for GetTransferSummary()
    db.executenonquery("CREATE INDEX IF NOT EXISTS transferIdIdx "
                       "on strata (transferId)");

    db.executenonquery("CREATE INDEX IF NOT EXISTS wblockIdx1 "
                       "on wblocks (time)");
    db.executenonquery("CREATE UNIQUE INDEX IF NOT EXISTS wblockIdx2 "
                       "on wblocks (block, time)");

    db.executenonquery("ANALYZE");

    wxFileName indexFile = reader->FileName();
    sqlite3_command cmd(db, "INSERT INTO logInfo VALUES(?,?,?,?,?)");

    cmd.bind(1, indexFile.GetName().fn_str());
    cmd.bind(2, (sqlite3x::int64_t) indexFile.GetModificationTime().GetTicks());
    cmd.bind(3, TIMESTEP_SIZE);
    cmd.bind(4, BLOCK_SIZE);
    cmd.bind(5, STRATUM_SIZE);

    cmd.executenonquery();

    transaction.commit();

    // Reset all cached commands
    DeleteCommands();

    SetProgress(1.0, COMPLETE);
}


/*
 * Check whether an existing index is correct and finished.
 */

bool
LogIndex::CheckFinished()
{
    // Assumes dbLock is already locked.

    wxFileName indexFile = reader->FileName();
    sqlite3_command cmd(db, "SELECT * FROM logInfo");
    sqlite3_cursor reader = cmd.executecursor();

    if (!reader.step()) {
        // No loginfo data
        return false;
    }

    wxString name(reader.getstring(0).c_str(), wxConvUTF8);
    sqlite3x::int64_t mtime = reader.getint64(1);
    int timestepSize = reader.getint(2);
    int blockSize = reader.getint(3);
    int stratumSize = reader.getint(4);

    if (name == indexFile.GetName() &&
        mtime == indexFile.GetModificationTime().GetTicks() &&
        timestepSize == TIMESTEP_SIZE &&
        blockSize == BLOCK_SIZE &&
        stratumSize == STRATUM_SIZE) {
        return true;
    } else {
        return false;
    }
}


void
LogIndex::StartIndexing()
{
    SetProgress(0.0, INDEXING);
    indexer = new IndexerThread(this);
    indexer->Create();
    indexer->Run();
}


void
LogIndex::SetProgressReceiver(wxEvtHandler *handler)
{
    progressReceiver = handler;
    SetProgress(progress, state);
}


void
LogIndex::SetProgress(double _progress, State _state)
{
    state = _state;
    progress = _progress;

    if (progressReceiver) {
        wxCommandEvent event(progressEvent);
        progressReceiver->AddPendingEvent(event);
    }
}


void
LogIndex::StoreInstant(LogInstant &instant)
{
    /*
     * Store a LogInstant to the strata index.
     * The caller must have already locked the database and started a transaction.
     */

    sqlite3_command cmd(db, "INSERT INTO strata VALUES(?,?,?,?,?,?)");

    cmd.bind(1, (sqlite3x::int64_t) instant.time);
    cmd.bind(2, (sqlite3x::int64_t) instant.offset);
    cmd.bind(3, (sqlite3x::int64_t) instant.transferId);

    uint8_t buffer[GetNumStrata() * 8];   // Worst-case packed size

    instant.readTotals.pack(buffer);
    cmd.bind(4, buffer, instant.readTotals.getPackedLen());

    instant.writeTotals.pack(buffer);
    cmd.bind(5, buffer, instant.writeTotals.getPackedLen());

    instant.zeroTotals.pack(buffer);
    cmd.bind(6, buffer, instant.zeroTotals.getPackedLen());

    cmd.executenonquery();
}


void
LogIndex::AdvanceInstant(LogInstant &instant, MemTransfer &mt, bool reverse)
{
    /*
     * Advance a LogInstant forward or backward by processing one memory transfer.
     */

    if (INDEX_DEBUG)
        printf("Advancing: inst(time=%lld offset=%lld) rev=%d "
               "mt(type=%s, dur=%d, offset=%lld)\n",
               instant.time, instant.offset,
               reverse,
               (const char *)mt.getTypeName().mb_str(),
               mt.duration, mt.offset);

    instant.updateTime(mt.duration, reverse);
    instant.offset = mt.offset;
    instant.transferId = mt.id;

    if (mt.byteCount) {
        AlignedIterator<STRATUM_SHIFT> iter(mt);

        do {
            switch (mt.type) {

            case MemTransfer::READ: {
                instant.readTotals.update(iter.blockId, iter.len, reverse);
                break;
            }

            case MemTransfer::WRITE: {
                MemTransfer::LengthType numZeroes = 0;

                for (MemTransfer::LengthType i = 0; i < iter.len; i++) {
                    uint8_t byte = mt.buffer[i + iter.mtOffset];
                    if (!byte)
                        numZeroes++;
                }

                instant.writeTotals.update(iter.blockId, iter.len, reverse);
                instant.zeroTotals.update(iter.blockId, numZeroes, reverse);
                break;
            }

            default:
                break;
            }
        } while (iter.next());
    }
}


wxThread::ExitCode
LogIndex::IndexerThread::Entry()
{
    bool aborted = false;
    bool running = true;

    // Beginning of this timestep, end of previous timestep
    MemTransfer::OffsetType prevOffset = 0;
    ClockType prevTime = 0;

    /* Data about the current log instant */
    LogInstant instant(index->GetNumStrata());
    instant.clear();

    /* State of each block */
    struct BlockState {
        uint64_t firstWriteOffset;
        uint64_t lastWriteOffset;
        bool wDirty;
        uint8_t data[BLOCK_SIZE];
    };

    BlockState *blocks = new BlockState[index->GetNumBlocks()];
    memset(blocks, 0, sizeof blocks[0] * index->GetNumBlocks());

    /*
     * Index the log. Read transactions, keeping track of the state of
     * each memory block. When we cross a timestep boundary, flush any
     * modified blocks to the database.
     *
     * The database is protected by a lock, but for the LogReader it's
     * more efficient to just clone the log reader, giving us a
     * separate buffer just for this thread.
     */

    LogReader reader(*index->reader);
    MemTransfer mt(prevOffset);

    /*
     * Periodically we should release our locks, commit the transaction,
     * and inform the UI of our progress. We determine how often to do
     * this by looking at the wallclock time between every two log
     * timesteps.
     */

    const int updateHZ = 15;
    const int maxMillisecPerUpdate = 1000 / updateHZ;
    wxDateTime lastUpdateTime = wxDateTime::UNow();

    /*
     * Loop over timesteps. We end transactions and unlock the dbLock
     * between timesteps.
     */
    while (running) {
        bool eof;

        wxCriticalSectionLocker locker(index->dbLock);
        sqlite3_transaction transaction(index->db);
        sqlite3_command wblockInsert(index->db, "INSERT INTO wblocks VALUES(?,?,?,?,?)");
        wxDateTime now;

        // Loop over timesteps between one progress update
        do {

            // Loop over memory transfers
            do {
                eof = !reader.Read(mt);

                if (eof) {
                    running = false;
                } else {

                    if (mt.type == MemTransfer::WRITE) {
                        AlignedIterator<BLOCK_SHIFT> iter(mt);
                        do {
                            BlockState *block = &blocks[iter.blockId];

                            block->lastWriteOffset = mt.offset;
                            if (!block->wDirty) {
                                block->firstWriteOffset = block->lastWriteOffset;
                                block->wDirty = true;
                            }

                            for (MemTransfer::LengthType i = 0; i < iter.len; i++) {
                                uint8_t byte = mt.buffer[i + iter.mtOffset];
                                block->data[i + iter.blockOffset] = byte;
                            }
                        } while (iter.next());
                    }

                    index->AdvanceInstant(instant, mt);
                    reader.Next(mt);

                    if (INDEX_DEBUG)
                        printf("Indexing at: %lld/%lld\n", instant.time, instant.offset);
                }

                // Loop until end of timestep, or end of file.
            } while (!eof && mt.offset < prevOffset + TIMESTEP_SIZE);

            // Flush all blocks that have been touched.

            for (MemTransfer::AddressType blockId = 0;
                 blockId < index->GetNumBlocks();
                 blockId++) {
                BlockState *block = &blocks[blockId];

                if (block->wDirty) {
                    wblockInsert.bind(1, (sqlite3x::int64_t) instant.time);
                    wblockInsert.bind(2, (sqlite3x::int64_t) blockId);
                    wblockInsert.bind(3, (sqlite3x::int64_t) block->firstWriteOffset);
                    wblockInsert.bind(4, (sqlite3x::int64_t) block->lastWriteOffset);
                    wblockInsert.bind(5, block->data, sizeof block->data);
                    wblockInsert.executenonquery();
                    block->wDirty = false;
                }
            }

            // Store a LogInstant for this timestep

            if (instant.time != prevTime)
                index->StoreInstant(instant);
            prevTime = instant.time;
            prevOffset = instant.offset;

            // Are we finished with this group of timesteps?
            now = wxDateTime::UNow();
        } while ((now - lastUpdateTime).GetMilliseconds() < maxMillisecPerUpdate);
        lastUpdateTime = now;

        // Finished a group of timesteps
        transaction.commit();

        /*
         * Periodic actions: Report progress, check for abort.
         */

        index->lastInstant = instantPtr_t(new LogInstant(instant));
        index->SetProgress(instant.offset / index->logFileSize, INDEXING);

        if (TestDestroy()) {
            aborted = true;
            running = false;
        }
    }

    /*
     * Clean up
     */

    delete[] blocks;

    if (aborted) {
        index->SetProgress(instant.offset / index->logFileSize, ERROR);
    } else {
        index->Finish();
    }

    reader.Close();
}


boost::shared_ptr<LogInstant>
LogIndex::GetInstant(ClockType time, ClockType distance)
{
    wxCriticalSectionLocker dataLocker(dataLock);

    time = std::min<ClockType>(time, GetDuration());

    instantPtr_t inst = instantCache.findClosest(time);
    ClockType dist = instantCache.distance(inst->time, time);

    /*
     * First try: Is there already a good instant in the cache?
     */

    if (dist <= distance) {
        return inst;
    }

    /*
     * No. See what the closest one is from the current timestep.
     * If this is closer that the cached one, we'll use it instead.
     */

    instantPtr_t dbInst = GetInstantForTimestep(time);
    instantCache.store(dbInst->time, dbInst);

    ClockType dbInstDist = instantCache.distance(dbInst->time, time);
    if (dbInstDist < dist) {
        inst = dbInst;
    }

    /*
     * Still no luck. Iterate forward/backward.
     */

    inst = LogIndex::GetInstantFromStartingPoint(inst, time, distance);
    instantCache.store(inst->time, inst);

    // DEBUG: Verify against another starting point
    if (INDEX_DEBUG) {
        instantPtr_t first = GetInstantForTimestep(0);
        instantPtr_t check = GetInstantFromStartingPoint(first, dbInst->time);
        printf("Checking %lld/%lld/%lld against %lld/%lld/%lld\n",
               dbInst->time, dbInst->offset, dbInst->transferId,
               check->time, check->offset, check->transferId);
        assert(*check == *dbInst);
    }
    if (INDEX_DEBUG) {
        instantPtr_t first = GetInstantForTimestep(0);
        instantPtr_t check = GetInstantFromStartingPoint(first, inst->time);
        assert(*check == *inst);
    }

    return inst;
}


instantPtr_t
LogIndex::GetInstantFromStartingPoint(instantPtr_t start, ClockType time,
                                      ClockType distance)
{
    /*
     * Using 'start' as the starting point for iteration, find the
     * first LogInstant which is at or below 'time', or any LogInstant
     * that is within 'distance' from 'time'.
     */

    ClockType dist = instantCache.distance(start->time, time);

    // Already close enough?
    if (dist <= distance) {
        return start;
    }

    /*
     * If we're still not close enough, we need to make a new
     * LogInstant by advancing the existing one forward or backward
     * through the original log file.
     *
     * A LogInstant is inclusive of the data in the MemTransfer at the
     * instant's saved offset. So if we're iterating forward, we need
     * to call Next() first. If we're iterating backward, we need to
     * call Prev() second.
     */

    instantPtr_t newInst(new LogInstant(*start));
    MemTransfer mt(newInst->offset, newInst->transferId);
    const char *indexErrFmt = "INDEX: %s error while %s (clock: %lld -> %lld)\n";

    if (time < newInst->time) {
        /*
         * The time threshold is before the current instant, and it
         * isn't already within the fuzz window provided by
         * 'distance'.
         *
         * This means we *must* iterate backwards until we enter the
         * fuzz window and/or cross over 'time'.
         */

        ClockType target = time + distance;

        do {
            if (!reader->Read(mt)) {
                fprintf(stderr, indexErrFmt, "Read", "reverse-iterating",
                        newInst->time, target);
                return newInst;
            }
            if (!reader->Prev(mt)) {
                // Reached the beginning of the log
                return GetInstantForTimestep(0);
            }

            // Reverse advance
            AdvanceInstant(*newInst, mt, true);

        } while (newInst->time > target);

    } else {
        /*
         * The time threshold is after the current instant, and it
         * isn't already in the fuzz window. We need to iterate
         * forward, stopping either:
         *
         *    1. After entering the fuzz region
         *    2. Just *before* crossing over 'time'.
         *
         * So we'll step forward, but if we realize that we just went
         * too far, we might go back a step.
         */

        ClockType target = time - distance;

        do {
            if (!reader->Next(mt)) {
                // Reached the end of the log
                break;
            }
            if (!reader->Read(mt)) {
                fprintf(stderr, indexErrFmt, "Read", "advancing",
                        newInst->time, target);
                return newInst;
            }

            AdvanceInstant(*newInst, mt);

            if (INDEX_DEBUG)
                printf("Forward: %lld -> %lld\n", newInst->time, target);

        } while (newInst->time < target);

        if (newInst->time > time) {
            /*
             * Went too far. Back up a step.
             */

            if (!reader->Prev(mt)) {
                fprintf(stderr, indexErrFmt, "Seek", "backing up",
                        newInst->time, target);
                return newInst;
            }

            // Reverse advance
            AdvanceInstant(*newInst, mt, true);

            assert(newInst->time <= time);

            if (INDEX_DEBUG)
                printf("Backed up: %lld -> %lld\n", newInst->time, target);
        }
    }

    return newInst;
}


instantPtr_t
LogIndex::GetInstantForTimestep(ClockType upperBound)
{
    /*
     * This is a low-level function to get a LogInstant from the
     * database of timesteps, without regard to transfer playback or
     * the instant cache.
     *
     * This function, unlike most of the others in LogIndex, is legal
     * to call before a log has been opened. If no log exists, we'll
     * end up returning an all-zero instant.
     */

    instantPtr_t instant(new LogInstant(GetNumStrata()));
    wxCriticalSectionLocker locker(dbLock);

    if (db.db()) {
        sqlite3_command *cmd = cmd_getInstantForTimestep;

        if (!cmd) {
            cmd = cmd_getInstantForTimestep =
                new sqlite3_command(db, "SELECT * FROM strata WHERE "
                                    "time <= ? ORDER BY time DESC LIMIT 1");
        }

        cmd->bind(1, (sqlite3x::int64_t) upperBound);
        sqlite3_cursor crsr = cmd->executecursor();

        if (crsr.step()) {
            int size;
            const void *blob;

            instant->time = crsr.getint64(0);
            instant->offset = crsr.getint64(1);
            instant->transferId = crsr.getint64(2);

            blob = crsr.getblob(3, size);
            instant->readTotals.unpack((const uint8_t *)blob, size);

            blob = crsr.getblob(4, size);
            instant->writeTotals.unpack((const uint8_t *)blob, size);

            blob = crsr.getblob(5, size);
            instant->zeroTotals.unpack((const uint8_t *)blob, size);

            return instant;
        }
    }

    /*
     * Construct the first valid instant: Since instants are
     * inclusive of the transfer at their 'offset', this would
     * just be an instant which includes only the transfer at
     * offset zero.
     */

    MemTransfer mt;
    instant->clear();
    if (reader)
        reader->Read(mt);
    AdvanceInstant(*instant, mt);
    return instant;
}


transferPtr_t
LogIndex::GetTransferSummary(OffsetType id)
{
    wxCriticalSectionLocker dataLocker(dataLock);
    transferPtr_t tp = transferCache.findClosest(id);

    if (tp->id == id) {
        // Found it in the cache
        return tp;
    }

    /*
     * The closest one in the cache isn't a match, but how close was
     * it? If we're farther away than the index's timestep
     * granularity, we'll be better off (on average) using the strata
     * table as a starting point.
     *
     * We don't know the file offest of 'id' yet, so we can't make an
     * exact calculation. But we can estimate it using the average
     * number of transfers per byte from lastInstant.
     */

    bool withinTimestep;

    if (tp->type == MemTransfer::ERROR_UNAVAIL) {
        withinTimestep = false;
    } else {
        OffsetType idDistance = id > tp->id ? id - tp->id : tp->id - id;
        OffsetType fileDistance = ((idDistance * (uint64_t)lastInstant->offset)
                                   / lastInstant->transferId);
        withinTimestep = fileDistance <= TIMESTEP_SIZE;
    }

    if (!db.db()) {
        // No database yet.
        return tp;
    }

    if (!withinTimestep) {
        /*
         * Not close enough. Get a new starting point from the strata table.
         * This doesn't read the entire MemTransfer, just the file offset,
         * timestamp, and ID.
         *
         * Note that LogInstants are inclusive of the transfer that
         * its offset and ID point to, so the instant timestamp is the
         * _end_ of the transfer. This is consistent with the
         * semantics we've chosen for TransferSummary.
         */

        bool success;

        {
            wxCriticalSectionLocker locker(dbLock);
            sqlite3_command *cmd = cmd_getTransferSummary;

            if (!cmd) {
                cmd = cmd_getTransferSummary =
                    new sqlite3_command(db, "SELECT time, offset, transferId FROM strata"
                                        " WHERE transferId <= ? ORDER BY transferId"
                                        " DESC LIMIT 1");
            }

            cmd->bind(1, (sqlite3x::int64_t) id);
            sqlite3_cursor crsr = cmd->executecursor();
            success = crsr.step();

            if (success) {
                tp = transferPtr_t(new TransferSummary(crsr.getint64(0),
                                                       crsr.getint64(1),
                                                       crsr.getint64(2)));
            }
        }

        if (!success) {
            /*
             * Earlier than all existing transfers.  Use
             * GetInstantForTimestep() to read the very first
             * MemTransfer from disk, so we know what the timestamp is
             * at the end of this transfer. We must do this after
             * releasing the dbLock.
             */

            instantPtr_t inst = GetInstantForTimestep(0);
            tp = transferPtr_t(new TransferSummary(inst->time,
                                                   inst->offset,
                                                   inst->transferId));
        }
    }

    /*
     * We now have a starting point for iteration, but it isn't
     * necessarily a complete TransferSummary, so it can't be stored
     * in the cache yet.
     *
     * Iterate forward or backward until we reach the transfer we're
     * searching for, then cache and return it.
     */

    MemTransfer mt(tp->offset, tp->id);
    const char *indexErrFmt = "INDEX: %s error while %s (ID: %lld -> %lld)\n";

    if (id < mt.id) {
        /*
         * Seek backwards
         */

        while (1) {
            if (!reader->Read(mt)) {
                fprintf(stderr, indexErrFmt, "Read", "reverse-iterating", mt.id, id);
                break;
            }
            
            // Exit only after Read()'ing the transfer we're interested in
            if (id == mt.id) {
                break;
            }

            if (!reader->Prev(mt)) {
                fprintf(stderr, indexErrFmt, "Seek", "reverse-iterating", mt.id, id);
                break;
            }

            // Back up the clock, so we're pointing to the end of 'mt'.
            tp->time -= mt.duration;
        }

    } else {
        /*
         * Seek forward
         */

        while (id != mt.id) {
            if (!reader->Next(mt)) {
                fprintf(stderr, indexErrFmt, "Seek", "advancing", mt.id, id);
                break;
            }
            if (!reader->Read(mt)) {
                fprintf(stderr, indexErrFmt, "Read", "advancing", mt.id, id);
                break;
            }

            // Advance the clock to the end of 'mt'
            tp->time += mt.duration;
        } 
    }

    /*
     * Now both 'mt' and 'tp' are pointing to the correct transfer,
     * and we've read the transfer details from disk. Fill in the
     * rest of the transfer info, and cache the results.
     */

    tp->type = mt.type;
    tp->address = mt.address;
    tp->byteCount = mt.byteCount;
    tp->offset = mt.offset;
    tp->id = mt.id;

    transferCache.store(tp->id, tp);
    return tp;
}


size_t
LogStrata::getPackedLen()
{
    size_t len = 0;
    for (int i = 0; i < count; i++) {
        len += varint::len(values[i]);
    }
    return len;
}


void
LogStrata::pack(uint8_t *buffer)
{
    uint8_t *p = buffer;

    for (int i = 0; i < count; i++) {
        varint::write(values[i], p);
        p += varint::len(values[i]);
    }

    // DEBUG: Verify pack/unpack
    if (INDEX_DEBUG) {
        const uint8_t *q = buffer;
        const uint8_t *fence = buffer + getPackedLen();
        for (int i = 0; i < count; i++) {
            assert(values[i] == varint::read(q, fence));
        }
    }
}


void
LogStrata::unpack(const uint8_t *buffer, size_t bufferLen)
{
    const uint8_t *fence = buffer + bufferLen;
    for (int i = 0; i < count; i++) {
        values[i] = varint::read(buffer, fence);
    }
}


void
LogStrata::clear()
{
    memset(values, 0, sizeof values[0] * count);
}


void
LogInstant::clear()
{
    time = 0;
    offset = 0;
    transferId = 0;
    readTotals.clear();
    writeTotals.clear();
    zeroTotals.clear();
}
