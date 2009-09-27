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


void
LogIndex::Open(LogReader *reader)
{
  this->reader = reader;

  wxFileName indexFile = reader->FileName();
  indexFile.SetExt(wxT("index"));
  wxString indexPath = indexFile.GetFullPath();

  numBlocks = (reader->MemSize() + BLOCK_MASK) >> BLOCK_SHIFT;

  db.open(indexPath.fn_str());
  InitDB();

  /*
   * Is this index complete and up-to-date?
   * If not, throw it away and start over.
   *
   * TODO: In the future we may want to store indexing progress, so we
   *       can stop indexing partway through and resume later.
   */
  if (!isFinished()) {
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
  db.executenonquery("PRAGMA journal_mode=memory");

  // Stores state for Finish()/isFinished().
  db.executenonquery("CREATE TABLE IF NOT EXISTS logInfo ("
		     "name, mtime, timestepSize, blockSize)");
}


/*
 * Initialize block tables- heavier than InitDB(), this only needs
 * to happen when we actually start indexing.
 */

void
LogIndex::InitBlockTables()
{
  int i;

  db.executenonquery("BEGIN TRANSACTION");

  /*
   * For each spatial block, we have two separate index tables:
   * a read index and a write index.
   *
   * The read index has a row for all timesteps in which the block is
   * read. The row contains:
   *
   *   - The first timestamp in this timestep
   *   - A file offset for the beginning and end of this timestep
   *   - Cumulative read byte count
   *
   * The write index has a row for all timesteps in which the block
   * was modified. Each row contains:
   *
   *   - The first timestamp in this timestep
   *   - A file offset for the beginning and end of this timestep
   *   - Cumulative write byte count
   *   - Cumulative count of the number of zero bytes
   *   - Block contents at the beginning of the timestep
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

  db.executenonquery("COMMIT TRANSACTION");
}


/*
 * Mark the database as complete, and stamp it with
 * the name and modification time of the raw log.
 */

void
LogIndex::Finish()
{
  wxFileName indexFile = reader->FileName();
  sqlite3_command cmd(db, "INSERT INTO logInfo VALUES(?,?)");

  cmd.bind(1, indexFile.GetName().fn_str());
  cmd.bind(2, (sqlite3x::int64_t) indexFile.GetModificationTime().GetTicks());
  cmd.bind(3, TIMESTEP_SIZE);
  cmd.bind(4, BLOCK_SIZE);

  cmd.executenonquery();
}


bool
LogIndex::isFinished()
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

  return (name == indexFile.GetName() &&
	  mtime == indexFile.GetModificationTime().GetTicks() &&
	  timestepSize == TIMESTEP_SIZE &&
	  blockSize == BLOCK_SIZE);
}


void
LogIndex::StartIndexing()
{
  InitBlockTables();

  indexer = new IndexerThread(this);
  indexer->Create();
  indexer->Run();
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

  /* Current timestep */
  uint64_t tsBegin = 0;

  /* State of each block */
  struct BlockState {
    uint8_t data[BLOCK_SIZE];
    uint64_t lastReadOffset;
    uint64_t lastWriteOffset;
    uint64_t readTotal;
    uint64_t writeTotal;
    uint64_t zeroTotal;
  };
  BlockState *blocks = new BlockState[index->numBlocks];
    
  /*
   * Index the log. Read transactions, keeping track of the state of
   * each memory block. When we cross a timestep boundary, flush any
   * modified blocks to the database.
   */

  LogReader *reader = index->reader;
  mt.Seek(tsBegin);

  while (reader->Read(mt)) {

    reader->Next(mt);

    if (mt.LogOffset() >= tsBegin + TIMESTEP_SIZE) {
      /* End of timestep */
      tsBegin = mt.LogOffset();
      printf("%lld\n", mt.LogOffset());
    }
  }

  delete[] blocks;
}
