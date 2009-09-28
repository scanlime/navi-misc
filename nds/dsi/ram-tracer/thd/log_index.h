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

#include "sqlite3x.h"
#include "mem_transfer.h"
#include "log_reader.h"


class LogIndex {
 public:
  enum State {
    IDLE,
    INDEXING,
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

 private:
  static const int TIMESTEP_SIZE = 256 * 1024;     // Timestep duration, in bytes
  static const int BLOCK_SHIFT = 14;
  static const int BLOCK_SIZE = 1 << BLOCK_SHIFT;  // Spatial block size
  static const int BLOCK_MASK = BLOCK_SIZE - 1;

  void InitDB();
  void InitBlockTables();
  void Finish();
  bool isFinished();
  void SetProgress(double progress, State state);
  void StartIndexing();
  static std::string BlockTableName(char type, int blockNum);

  class IndexerThread : public wxThread {
  public:
     IndexerThread(LogIndex *_index) : index(_index) {}  
     virtual ExitCode Entry();

  private:
     LogIndex *index;
  };

  LogReader *reader;  
  sqlite3x::sqlite3_connection db;
  IndexerThread *indexer;
  int numBlocks;
  double logFileSize;

  State state;
  double progress;
  static wxEventType progressEvent;
  wxEvtHandler *progressReceiver;
};

#endif /* __LOG_INDEX_H */
