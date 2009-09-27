/*
  Copyright (C) 2004-2005 Cory Nelson

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
  claim that you wrote the original software. If you use this software
  in a product, an acknowledgment in the product documentation would be
  appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
  misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
	
*/

#include <sqlite3.h>
#include "sqlite3x.h"

namespace sqlite3x {

	sqlite3_cursor::sqlite3_cursor() : cmd(NULL) {}

	sqlite3_cursor::sqlite3_cursor(const sqlite3_cursor &copy) : cmd(copy.cmd) {
		if(this->cmd) ++this->cmd->refs;
	}

	sqlite3_cursor::sqlite3_cursor(sqlite3_command & cmd) : cmd(&cmd) {
		++this->cmd->refs;
	}

	sqlite3_cursor::~sqlite3_cursor() {
		this->close();
	}

	sqlite3_cursor& sqlite3_cursor::operator=(const sqlite3_cursor &copy) {
		this->close();

		this->cmd=copy.cmd;
		if(this->cmd) ++this->cmd->refs;

		return *this;
	}

	int sqlite3_cursor::colcount()
	{
		if( ! this->cmd )
		{
			throw database_error("sqlite3_cursor::colcount(): reader is closed");
		}
		return this->cmd->colcount();
	}

	bool sqlite3_cursor::step() {
		if(!this->cmd) throw database_error("sqlite3_cursor::step(): reader is closed");

		switch(sqlite3_step(this->cmd->stmt)) {
		  case SQLITE_ROW:
			  return true;
		  case SQLITE_DONE:
			  return false;
		  default:
			  throw database_error(this->cmd->con);
		}
	}

	void sqlite3_cursor::reset() {
		if(!this->cmd) throw database_error("sqlite3_cursor::reset(): reader is closed");

		if(! this->cmd->reset() )
		{
			throw database_error("sqlite3_cursor::reset() db error: %s", this->cmd->con.errormsg().c_str() );
		}
	}

	void sqlite3_cursor::close() {
		if(this->cmd) {
			if(--this->cmd->refs==0) { sqlite3_reset(this->cmd->stmt); }
			this->cmd=NULL;
		}
	}

#define READER_CHECK(FUNC) \
	if( ! this->cmd ) throw database_error( "sqlite3_cursor::%s(%d): reader is closed", # FUNC, index ); \
	if( (index)>(this->cmd->argc-1)) throw database_error("sqlite3_cursor::%s(%d): index out of range", # FUNC, index );

 	bool sqlite3_cursor::isnull(int index) {
 		READER_CHECK(isnull);
 		return sqlite3_column_type(this->cmd->stmt, index) == SQLITE_NULL;
 	}

	int sqlite3_cursor::getint(int index) {
		READER_CHECK(getint);
		return sqlite3_column_int(this->cmd->stmt, index);
	}

	int64_t sqlite3_cursor::getint64(int index) {
		READER_CHECK(getint64);
		return sqlite3_column_int64(this->cmd->stmt, index);
	}

	double sqlite3_cursor::getdouble(int index) {
		READER_CHECK(getdouble);
		return sqlite3_column_double(this->cmd->stmt, index);
	}

	std::string sqlite3_cursor::getstring(int index) {
		READER_CHECK(string);
		return std::string((const char*)sqlite3_column_text(this->cmd->stmt, index), sqlite3_column_bytes(this->cmd->stmt, index));
	}

	char const * sqlite3_cursor::getstring(int index, int & size) {
		READER_CHECK(string);
		size = sqlite3_column_bytes(this->cmd->stmt, index);
		return (char const *)sqlite3_column_text(this->cmd->stmt, index);
	}

#if SQLITE3X_USE_WCHAR
	std::wstring sqlite3_cursor::getstring16(int index) {
		READER_CHECK(wstring);
		return std::wstring((const wchar_t*)sqlite3_column_text16(this->cmd->stmt, index), sqlite3_column_bytes16(this->cmd->stmt, index)/2);
	}
#endif

	std::string sqlite3_cursor::getblob(int index) {
		READER_CHECK(string);
		return std::string((const char*)sqlite3_column_blob(this->cmd->stmt, index), sqlite3_column_bytes(this->cmd->stmt, index));
	}

	void const * sqlite3_cursor::getblob(int index, int & size ) {
		READER_CHECK(string);
		size = sqlite3_column_bytes(this->cmd->stmt, index);
		return sqlite3_column_blob(this->cmd->stmt, index);
	}

	std::string sqlite3_cursor::getcolname(int index) {
		READER_CHECK(string);
		char const * cn = sqlite3_column_name(this->cmd->stmt, index);
		return cn ? cn : "";
	}

// 	char const * sqlite3_cursor::getcolname(int index) {
// 		READER_CHECK(string);
// 		char const * cn = sqlite3_column_name(this->cmd->stmt, index);
// 		return cn ? cn : "";
// 	}

#if SQLITE3X_USE_WCHAR
	std::wstring sqlite3_cursor::getcolname16(int index) {
		READER_CHECK(wstring);
		return (const wchar_t*)sqlite3_column_name16(this->cmd->stmt, index);
	}
#endif

#undef READER_CHECK
}
