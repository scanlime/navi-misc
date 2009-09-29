/*
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

#include "sqlite3x.h"
#include "mem_transfer.h"
#include "log_reader.h"

class LogInstant;


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
  int GetBlockSize() const { return BLOCK_SIZE; }
  int GetNumBlocks() const { return numBlocks; }

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
  boost::shared_ptr<LogInstant> GetInstant(ClockType time, ClockType distance=0);

 private:
  static const int TIMESTEP_SIZE = 256 * 1024;     // Timestep duration, in bytes
  static const int BLOCK_SHIFT = 10;
  static const int BLOCK_SIZE = 1 << BLOCK_SHIFT;  // Spatial block size
  static const int BLOCK_MASK = BLOCK_SIZE - 1;

  void InitDB();
  void Finish();
  bool checkFinished();
  void SetProgress(double progress, State state);
  void StartIndexing();

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
  int numBlocks;
  double logFileSize;
  ClockType duration;

  State state;
  double progress;
  static wxEventType progressEvent;
  wxEvtHandler *progressReceiver;
};


/*
 * A summary of the log's state at a particular instant. LogInstants
 * can be retrieved from a LogIndex, and the LogIndex internally
 * caches them.
 */

class LogInstant {
 public:
  ~LogInstant();
  
  LogIndex::ClockType time;

  uint64_t *blockReadTotals;
  uint64_t *blockWriteTotals;
  uint64_t *blockZeroTotals;

 private:
  // Can only be created by LogIndex.
  LogInstant(LogIndex::ClockType time, int numBlocks);
  friend class LogIndex;
};


#endif /* __LOG_INDEX_H */
