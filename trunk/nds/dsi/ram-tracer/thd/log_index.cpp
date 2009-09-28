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
  // Our index database can be regenerated at any time, so trade reliability for speed.
  db.executenonquery("PRAGMA journal_mode = OFF");
  db.executenonquery("PRAGMA cache_size = 10000");

  // Stores state for Finish()/checkinished().
  db.executenonquery("CREATE TABLE IF NOT EXISTS logInfo ("
		     "name, mtime, timestepSize, blockSize, duration)");
}


/*
 * Initialize block tables- heavier than InitDB(), this only needs
 * to happen when we actually start indexing.
 */

void
LogIndex::InitBlockTables()
{
  int i;
  sqlite3_transaction transaction(db);

  /*
   * For each spatial block, we have two separate index tables:
   * a read index and a write index.
   *
   * The read index has a row for all timesteps in which the block is
   * read. The row contains:
   *
   *   - Number of elapsed clock ticks at the end of the timestep
   *   - A file offset for the beginning and end of the transfers
   *     that affect this block during the indicated timestep
   *
   *   - Cumulative read byte count
   *
   * The write index has a row for all timesteps in which the block
   * was modified. Each row contains:
   *
   *   - Number of elapsed clock ticks at the end of the timestep
   *   - A file offset for the beginning and end of the transfers
   *     that affect this block during the indicated timestep
   *
   *   - Cumulative write byte count
   *   - Cumulative count of the number of zero bytes
   *
   *   - Block contents at the end of the timestep
   */

  for (i = 0; i < numBlocks; i++) {
    db.executenonquery("CREATE TABLE " + BlockTableName('r', i) +
		       "("
		         "time INTEGER PRIMARY KEY ASC,"
		         "offsetBegin,"
		         "offsetEnd,"
		         "readTotal"
		       ")");

    db.executenonquery("CREATE TABLE " + BlockTableName('w', i) +
		       "("
  		         "time INTEGER PRIMARY KEY ASC,"
 		         "offsetBegin,"
		         "offsetEnd,"
		         "writeTotal,"
		         "zeroTotal,"
		         "blockData BLOB"
		       ")");
  }

  transaction.commit();
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


/*
 * Retrieve the SQL table name for a particular block.
 */

std::string
LogIndex::BlockTableName(char type, int blockNum)
{
  char buf[16];
  sprintf(buf, "b%c%x", type, blockNum);
  return std::string(buf);
}


wxThread::ExitCode
LogIndex::IndexerThread::Entry()
{
  MemTransfer mt;
  bool aborted = false;

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
    sqlite3_command *rCmd;
    sqlite3_command *wCmd;

  };

  BlockState *blocks = new BlockState[index->numBlocks];
  memset(blocks, 0, sizeof blocks[0] * index->numBlocks);

  index->InitBlockTables();
  sqlite3_transaction transaction(index->db);

  /*
   * Initialize parsed SQL queries for inserting into each block's
   * read/write tables.
   */

  for (MemTransfer::AddressType blockId = 0; blockId < index->numBlocks; blockId++) {
    BlockState *block = &blocks[blockId];
    block->rCmd = new sqlite3_command(index->db, "INSERT INTO "
				      + BlockTableName('r', blockId)
				      + " VALUES(?,?,?,?)");
    block->wCmd = new sqlite3_command(index->db, "INSERT INTO "
				      + BlockTableName('w', blockId)
				      + " VALUES(?,?,?,?,?,?)");
  }

  /*
   * Index the log. Read transactions, keeping track of the state of
   * each memory block. When we cross a timestep boundary, flush any
   * modified blocks to the database.
   */

  LogReader *reader = index->reader;
  mt.Seek(tsBegin);

  while (true) {
    bool eof = !reader->Read(mt);

    if (!eof) {
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

    if (eof || mt.LogOffset() >= tsBegin + TIMESTEP_SIZE) {
      /*
       * End of timestep, or end of file.
       * Flush all blocks that have been touched.
       */

      for (MemTransfer::AddressType blockId = 0; blockId < index->numBlocks; blockId++) {
	BlockState *block = &blocks[blockId];

	if (block->rDirty) {
	  block->rCmd->bind(1, (sqlite3x::int64_t) clock);
	  block->rCmd->bind(2, (sqlite3x::int64_t) block->firstReadOffset);
	  block->rCmd->bind(3, (sqlite3x::int64_t) block->lastReadOffset);
	  block->rCmd->bind(4, (sqlite3x::int64_t) block->readTotal);
	  block->rCmd->executenonquery();
	}

	if (block->wDirty) {
	  block->wCmd->bind(1, (sqlite3x::int64_t) clock);
	  block->wCmd->bind(2, (sqlite3x::int64_t) block->firstReadOffset);
	  block->wCmd->bind(3, (sqlite3x::int64_t) block->lastReadOffset);
	  block->wCmd->bind(4, (sqlite3x::int64_t) block->writeTotal);
	  block->wCmd->bind(5, (sqlite3x::int64_t) block->zeroTotal);
	  block->wCmd->bind(6, block->data, sizeof block->data);
	  block->wCmd->executenonquery();
	}

	block->rDirty = false;
	block->wDirty = false;
      }

      tsBegin = mt.LogOffset();

      /*
       * Periodic actions: Report progress, check for abort.
       */

      index->duration = clock;
      index->SetProgress(tsBegin / index->logFileSize, INDEXING);

      if (TestDestroy()) {
	aborted = true;
	break;
      }
    }

    if (eof) {
      break;
    }
  }

  /*
   * Clean up
   */

  for (MemTransfer::AddressType blockId = 0; blockId < index->numBlocks; blockId++) {
    BlockState *block = &blocks[blockId];
    delete block->rCmd;
    delete block->wCmd;
  }
  delete[] blocks;
  transaction.commit();

  if (aborted) {
    index->SetProgress(tsBegin / index->logFileSize, ERROR);
  } else {
    index->Finish();
    index->SetProgress(1.0, COMPLETE);
  }
}
