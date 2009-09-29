/*
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

#include <wx/string.h>
#include <assert.h>
#include "log_index.h"

using namespace sqlite3x;

wxEventType LogIndex::progressEvent = 0;


LogIndex::LogIndex()
  : progressReceiver(NULL),
    duration(0)
{
  if (!progressEvent)
    progressEvent = wxNewEventType();

  SetProgress(0.0, IDLE);
}

void
LogIndex::Open(LogReader *reader)
{
  wxCriticalSectionLocker locker(dbLock);

  this->reader = reader;
  logFileSize = std::max<double>(1.0, reader->FileName().GetSize().ToDouble());
  numBlocks = (reader->MemSize() + BLOCK_MASK) >> BLOCK_SHIFT;

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
  if (checkFinished()) {
    SetProgress(1.0, COMPLETE);
  } else {
    db.close();
    wxRemoveFile(indexPath);
    db.open(indexPath.fn_str());
    InitDB();
    StartIndexing();
  }
}


void
LogIndex::Close()
{
  db.close();
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
		     "name, mtime, timestepSize, blockSize, duration)");

  // Information about the end of each timeslice, for each block that was read
  db.executenonquery("CREATE TABLE IF NOT EXISTS rblocks ("
 		     "time,"
		     "block,"
		     "firstReadOffset,"
		     "lastReadOffset,"
		     "readTotal"
		     ")");

  // Information about the end of each timeslice, for each block that was written
  db.executenonquery("CREATE TABLE IF NOT EXISTS wblocks ("
 		     "time,"
		     "block,"
		     "firstWriteOffset,"
		     "lastWriteOffset,"
		     "writeTotal,"
		     "zeroTotal,"
		     "data"
		     ")");
}


/*
 * Mark the database as complete, and stamp it with
 * the name and modification time of the raw log.
 *
 * Also saves the log's duration, for loading later by
 * checkFinished().
 */

void
LogIndex::Finish()
{
  // Assumes dbLock is already locked.

  wxFileName indexFile = reader->FileName();
  sqlite3_command cmd(db, "INSERT INTO logInfo VALUES(?,?,?,?,?)");

  cmd.bind(1, indexFile.GetName().fn_str());
  cmd.bind(2, (sqlite3x::int64_t) indexFile.GetModificationTime().GetTicks());
  cmd.bind(3, TIMESTEP_SIZE);
  cmd.bind(4, BLOCK_SIZE);
  cmd.bind(5, (sqlite3x::int64_t) duration);

  cmd.executenonquery();
}


/*
 * Check whether an existing index is correct and finished.
 * If so, return true and load the log's final duration. If not, returns false.
 */

bool
LogIndex::checkFinished()
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
  sqlite3x::int64_t savedDuration = reader.getint(4);

  if (name == indexFile.GetName() &&
      mtime == indexFile.GetModificationTime().GetTicks() &&
      timestepSize == TIMESTEP_SIZE &&
      blockSize == BLOCK_SIZE) {
    duration = (MemTransfer::ClockType) savedDuration;
  } else {
    duration = 0;
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


wxThread::ExitCode
LogIndex::IndexerThread::Entry()
{
  MemTransfer mt;
  bool aborted = false;
  bool running = true;

  /* Current timestep */
  MemTransfer::OffsetType tsBegin = 0;

  /* Current timestamp, in clock cycles */
  MemTransfer::ClockType clock = 0;

  /* State of each block */
  struct BlockState {
    uint64_t firstReadOffset;
    uint64_t firstWriteOffset;
    uint64_t lastReadOffset;
    uint64_t lastWriteOffset;
    uint64_t readTotal;
    uint64_t writeTotal;
    uint64_t zeroTotal;
    bool rDirty, wDirty;
    uint8_t data[BLOCK_SIZE];
  };

  BlockState *blocks = new BlockState[index->numBlocks];
  memset(blocks, 0, sizeof blocks[0] * index->numBlocks);

  /*
   * Index the log. Read transactions, keeping track of the state of
   * each memory block. When we cross a timestep boundary, flush any
   * modified blocks to the database.
   */

  LogReader *reader = index->reader;
  mt.Seek(tsBegin);

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
    sqlite3_command rblockInsert(index->db, "INSERT INTO rblocks VALUES(?,?,?,?,?)");
    sqlite3_command wblockInsert(index->db, "INSERT INTO wblocks VALUES(?,?,?,?,?,?,?)");
    wxDateTime now;

    // Loop over timesteps between one progress update
    do {
    
      // Loop over memory transfers
      do {
	eof = !reader->Read(mt);
      
	if (eof) {
	  running = false;
	} else {
	  /*
	   * Iterate over each block that this transfer touches.  It is
	   * uncommon for a transfer to touch more than one block, but this
	   * needs to be generic.
	   */

	  if (mt.byteCount > 0) {
	    // Range of blocks that this transfer touches
	    MemTransfer::AddressType addrFirst = mt.address;
	    MemTransfer::AddressType addrLast = mt.address + mt.byteCount - 1;
	    MemTransfer::AddressType blockFirst = addrFirst >> BLOCK_SHIFT;
	    MemTransfer::AddressType blockLast = addrLast >> BLOCK_SHIFT;

	    // Range of bytes that intersects the first block
	    MemTransfer::LengthType mtOffset = 0;
	    MemTransfer::LengthType blockOffset = addrFirst & BLOCK_MASK;

	    MemTransfer::AddressType blockId = blockFirst;
	    while (true) {
	      BlockState *block = &blocks[blockId];
	      MemTransfer::LengthType len = std::min<MemTransfer::LengthType>
		(BLOCK_SIZE - blockOffset, mt.byteCount - mtOffset);
     
	      switch (mt.type) {

	      case MemTransfer::READ:
		block->lastReadOffset = mt.LogOffset();
		block->readTotal += len;

		if (!block->rDirty) {
		  block->firstReadOffset = block->lastReadOffset;
		  block->rDirty = true;
		}
		break;

	      case MemTransfer::WRITE:
		block->lastWriteOffset = mt.LogOffset();
		block->writeTotal += len;

		if (!block->wDirty) {
		  block->firstWriteOffset = block->lastWriteOffset;
		  block->wDirty = true;
		}

		for (MemTransfer::LengthType i = 0; i < len; i++) {
		  uint8_t byte = mt.buffer[i + mtOffset];
		  block->data[i + blockOffset] = byte;
		  if (!byte)
		    block->zeroTotal++;
		}
		break;

	      default:
		break;
	      }

	      // Next block. (Uncommon case)
	      if (blockId == blockLast) {
		break;
	      }
	      blockId++;
	      mtOffset += len;
	      blockOffset = 0;
	    }
	  }

	  clock += mt.duration;
	  reader->Next(mt);
	}

	// Loop until end of timestep, or end of file.
      } while (!eof && mt.LogOffset() < tsBegin + TIMESTEP_SIZE);

      // Flush all blocks that have been touched.

      for (MemTransfer::AddressType blockId = 0; blockId < index->numBlocks; blockId++) {
	BlockState *block = &blocks[blockId];

	if (block->rDirty) {
	  rblockInsert.bind(1, (sqlite3x::int64_t) clock);
	  rblockInsert.bind(2, (sqlite3x::int64_t) blockId);
	  rblockInsert.bind(3, (sqlite3x::int64_t) block->firstReadOffset);
	  rblockInsert.bind(4, (sqlite3x::int64_t) block->lastReadOffset);
	  rblockInsert.bind(5, (sqlite3x::int64_t) block->readTotal);
	  rblockInsert.executenonquery();
	}

	if (block->wDirty) {
	  wblockInsert.bind(1, (sqlite3x::int64_t) clock);
	  wblockInsert.bind(2, (sqlite3x::int64_t) blockId);
	  wblockInsert.bind(3, (sqlite3x::int64_t) block->firstWriteOffset);
	  wblockInsert.bind(4, (sqlite3x::int64_t) block->lastWriteOffset);
	  wblockInsert.bind(5, (sqlite3x::int64_t) block->writeTotal);
	  wblockInsert.bind(6, (sqlite3x::int64_t) block->zeroTotal);
	  wblockInsert.bind(7, block->data, sizeof block->data);
	  wblockInsert.executenonquery();
	}

	block->rDirty = false;
	block->wDirty = false;
      }

      tsBegin = mt.LogOffset();

      // Are we finished with this group of timestamps?
      now = wxDateTime::UNow();
    } while ((now - lastUpdateTime).GetMilliseconds() < maxMillisecPerUpdate);
    lastUpdateTime = now;

    // Finished a group of timesteps
    transaction.commit();

    /*
     * Periodic actions: Report progress, check for abort.
     */

    index->duration = clock;
    index->SetProgress(tsBegin / index->logFileSize, INDEXING);

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
    index->SetProgress(tsBegin / index->logFileSize, ERROR);
  } else {

    index->SetProgress(1.0, FINISHING);

    index->db.executenonquery("CREATE INDEX IF NOT EXISTS rblockIdx1 "
			      "on rblocks (time)");
    index->db.executenonquery("CREATE UNIQUE INDEX IF NOT EXISTS rblockIdx2 "
			      "on rblocks (block, time)");
    
    index->db.executenonquery("CREATE INDEX IF NOT EXISTS wblockIdx1 "
			      "on wblocks (time)");
    index->db.executenonquery("CREATE UNIQUE INDEX IF NOT EXISTS wblockIdx2 "
			      "on wblocks (block, time)");

    index->db.executenonquery("ANALYZE");

    index->Finish();
    index->SetProgress(1.0, COMPLETE);
  }
}


boost::shared_ptr<LogInstant>
LogIndex::GetInstant(ClockType time, ClockType distance)
{
  /* XXX: BOGUS. */

  wxCriticalSectionLocker locker(dbLock);
  boost::shared_ptr<LogInstant> instant(new LogInstant(time, numBlocks));

  sqlite3_command cmd(db, "SELECT readTotal FROM rblocks WHERE"
		      " block == ? AND time < ? ORDER BY time DESC LIMIT 1");

  cmd.bind(2, (sqlite3x::int64_t) time);

  for (int block = 0; block < 50; block++) {
    cmd.bind(1, block);

    sqlite3_cursor reader = cmd.executecursor();
    if (reader.step()) {
      instant->blockReadTotals[block] = reader.getint64(0);
    }
  }

  return instant;
}


LogInstant::LogInstant(LogIndex::ClockType _time, int numBlocks)
  : time(_time)
{
  blockReadTotals = new uint64_t[numBlocks];
  blockWriteTotals = new uint64_t[numBlocks];
  blockZeroTotals = new uint64_t[numBlocks];
}


LogInstant::~LogInstant()
{
  delete blockReadTotals;
  delete blockWriteTotals;
  delete blockZeroTotals;
}
