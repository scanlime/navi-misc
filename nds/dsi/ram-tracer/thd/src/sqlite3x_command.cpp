/*
  Copyright (C) 2004-2005 Cory Nelson
  Copyright (C) 2006 stephan beal

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
#include <cstring> // strlen()
#include <iostream> // only for debuggin
namespace sqlite3x {

	sqlite3_command::sqlite3_command(sqlite3_connection &con)
		: con(con),stmt(0),refs(0),argc(0)
	{
	}


	sqlite3_command::sqlite3_command(sqlite3_connection &con, const std::string &sql)
		: con(con),stmt(0),refs(0),argc(0)
	{
		this->prepare( sql );
	}

	sqlite3_command::sqlite3_command(sqlite3_connection &con, char const * sql, size_t len )
		: con(con),stmt(0),refs(0),argc(0)
	{
		this->prepare( sql, static_cast<int>( len ) );
	}

#if SQLITE3X_USE_WCHAR
	sqlite3_command::sqlite3_command(sqlite3_connection &con, const std::wstring &sql) : con(con),stmt(0),refs(0),argc(0) {
		const void *tail=NULL;
		if(sqlite3_prepare16(con.db(), sql.data(), (int)sql.length()*2, &this->stmt, &tail)!=SQLITE_OK)
			throw database_error(con);

		this->argc=sqlite3_column_count(this->stmt);
	}
#endif

	void sqlite3_command::prepare( char const * sql, int len )
	{
		if( this->stmt ) this->finalize();
		const char *tail=NULL;
		int rc = sqlite3_prepare( this->con.db(), sql, len, &(this->stmt), &tail );
		if( SQLITE_OK != rc )
		{
			throw database_error("sqlite3_command::prepare([%s]) failed. Reason=[%s]",
					     sql, sqlite3_errmsg( this->con.db() ) );
		}
		this->argc=sqlite3_column_count(this->stmt);
	}

	void sqlite3_command::prepare( std::string const & sql )
	{
		this->prepare( sql.c_str(),  static_cast<int>(  sql.size()) );
	}


	sqlite3_command::~sqlite3_command() {
		try
		{
			this->finalize();
		}
		catch(...)
		{
			// std::cout << "sqlite3_command::~sqlite3_command() ignoring an exception!\n";
			// silently ignore
		}
	}

	void sqlite3_command::finalize()
	{
		if( this->stmt )
		{
			if(sqlite3_finalize(this->stmt)!=SQLITE_OK)
				throw database_error(this->con);
			this->stmt = 0;
		}
	}

	void sqlite3_command::bind(int index) {
		if(sqlite3_bind_null(this->stmt, index)!=SQLITE_OK)
			throw database_error(this->con);
	}

	void sqlite3_command::bind(int index, int data) {
		if(sqlite3_bind_int(this->stmt, index, data)!=SQLITE_OK)
			throw database_error(this->con);
	}

	void sqlite3_command::bind(int index, int64_t data) {
		if(sqlite3_bind_int64(this->stmt, index, data)!=SQLITE_OK)
			throw database_error(this->con);
	}

	void sqlite3_command::bind(int index, double data) {
		if(sqlite3_bind_double(this->stmt, index, data)!=SQLITE_OK)
			throw database_error(this->con);
	}

	void sqlite3_command::bind(int index, const char *data, int datalen) {
		if(sqlite3_bind_text(this->stmt, index, data,
			static_cast<int>(
			((-1==datalen)
			? std::strlen(data)
			: datalen)
			),
			SQLITE_TRANSIENT)!=SQLITE_OK)
			throw database_error(this->con);
	}

#if SQLITE3X_USE_WCHAR
	void sqlite3_command::bind(int index, const wchar_t *data, int datalen) {
		if(sqlite3_bind_text16(this->stmt, index, data, datalen, SQLITE_TRANSIENT)!=SQLITE_OK)
			throw database_error(this->con);
	}
#endif

	void sqlite3_command::bind(int index, const void *data, int datalen) {
		if(sqlite3_bind_blob(this->stmt, index, data, datalen, SQLITE_TRANSIENT)!=SQLITE_OK)
			throw database_error(this->con);
	}

	void sqlite3_command::bind(int index, const std::string &data) {
		if(sqlite3_bind_text(this->stmt, index, data.data(), (int)data.length(), SQLITE_TRANSIENT)!=SQLITE_OK)
			throw database_error(this->con);
	}

#if SQLITE3X_USE_WCHAR
	void sqlite3_command::bind(int index, const std::wstring &data) {
		if(sqlite3_bind_text16(this->stmt, index, data.data(), (int)data.length()*2, SQLITE_TRANSIENT)!=SQLITE_OK)
			throw database_error(this->con);
	}
#endif

	sqlite3_cursor sqlite3_command::executecursor() {
		return sqlite3_cursor(*this);
	}

	void sqlite3_command::executenonquery() {
		this->executecursor().step();
	}

	int sqlite3_command::executeint() {
		sqlite3_cursor reader=this->executecursor();
		if(!reader.step()) throw database_error("nothing to read");
		return reader.getint(0);
	}

	int64_t sqlite3_command::executeint64() {
		sqlite3_cursor reader=this->executecursor();
		if(!reader.step()) throw database_error("nothing to read");
		return reader.getint64(0);
	}

	double sqlite3_command::executedouble() {
		sqlite3_cursor reader=this->executecursor();
		if(!reader.step()) throw database_error("nothing to read");
		return reader.getdouble(0);
	}

	char const * sqlite3_command::executestring( int & size ) {
		sqlite3_cursor reader=this->executecursor();
		if(!reader.step()) throw database_error("nothing to read");
	        return reader.getstring( 0, size );
	}

	std::string sqlite3_command::executestring() {
		sqlite3_cursor reader=this->executecursor();
		if(!reader.step()) throw database_error("nothing to read");
		return reader.getstring(0);
	}

#if SQLITE3X_USE_WCHAR
	std::wstring sqlite3_command::executestring16() {
		sqlite3_cursor reader=this->executecursor();
		if(!reader.step()) throw database_error("nothing to read");
		return reader.getstring16(0);
	}
#endif

	std::string sqlite3_command::executeblob() {
		sqlite3_cursor reader=this->executecursor();
		if(!reader.step()) throw database_error("nothing to read");
		return reader.getblob(0);
	}

	void const * sqlite3_command::executeblob( int & size ) {
		sqlite3_cursor reader=this->executecursor();
		if(!reader.step()) throw database_error("nothing to read");
		return reader.getblob(0, size);
	}

	int sqlite3_command::colcount()
	{
		if( ! this->stmt )
		{
			throw database_error("sqlite3_command::colcount(): statement has not been prepared");
		}
		return sqlite3_column_count( this->stmt );
	}


	bool sqlite3_command::reset()
	{
		int rc = SQLITE_OK;
		if( this->stmt )
		{
			rc = sqlite3_reset( this->stmt );
		}
		return rc == SQLITE_OK;
	}

	sqlite3_stmt * sqlite3_command::handle()
	{
		return this->stmt;
	}


}
