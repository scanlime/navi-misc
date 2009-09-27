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
	
  Changes made by stephan@s11n.net:

  - Changed ~sqlite3_connection() to use this->close() instead of sqlite3_close().

*/

#include <sqlite3.h>
#include "sqlite3x.h"

#include <sstream>
#include <vector>
namespace sqlite3x {

	bool rc_is_okay( int rc )
	{
		return ((SQLITE_DONE==rc) || (SQLITE_OK==rc) || (SQLITE_ROW==rc));
	}

	sqlite3_connection::sqlite3_connection() : m_db(NULL), m_name() {}

	sqlite3_connection::sqlite3_connection(std::string const & dbn)
		: m_db(NULL), m_name(dbn)
	{
		this->open(dbn);
	}

#if SQLITE3X_USE_WCHAR
	sqlite3_connection::sqlite3_connection(const wchar_t *dbn) : m_db(NULL), m_name() { this->open(dbn); }
#endif

	sqlite3_connection::sqlite3_connection( sqlite3 * dbh )
		: m_db(0), m_name()
	{
		if( ! dbh )
		{
			throw database_error( "sqlite3_connection(sqlite3*) ctor was passed a null db handle." );
		}
		this->take( dbh );
	}

	sqlite3_connection::~sqlite3_connection()
	{
		try
		{
			this->close();
		}
		catch(...)
		{
			// ignored for the sake of a no-throw dtor.
		}
	}


	void sqlite3_connection::take( sqlite3 * dbh )
	{

		if( this->m_db == dbh ) return;
		try
		{
			if( this->m_db || (!dbh) )
			{
				this->close();
			}
			this->m_db = dbh;
			if( dbh )
			{
				this->on_open();
			}
		}
		catch( ... )
		{
			this->m_db = dbh;
			throw;
		}
	}

	sqlite3 * sqlite3_connection::take() throw()
	{
		sqlite3 * ret = this->m_db;
		this->m_db = 0;
		return ret;
	}

	sqlite3 * sqlite3_connection::db() const
	{
		return this->m_db;
	}

	std::string sqlite3_connection::name() const
	{
		return this->m_name;
	}

	std::string sqlite3_connection::errormsg() const
	{
		char const * m = this->m_db ? sqlite3_errmsg(this->m_db) : "";
		return m ? m : "";
	}

	void sqlite3_connection::on_open()
	{
		return;
	}
	void sqlite3_connection::open( char const * db) {
		this->close();
		this->m_name = db ? db : "";
		if(sqlite3_open(db, &this->m_db)!=SQLITE_OK)
			throw database_error("unable to open database %s", db ? db : "<null>");
		try
		{
			// Potential bug: when open() is called from
			// the ctor of subclasses as a result of
			// calling the parent class ctor, the subclass
			// part of the subclass may not be complete,
			// and a less derived on_open() may
			// potentially be called. ???
			this->on_open();
		}
		catch(...)
		{
			try { this->close(); }
			catch(...) { /* ignore */ }
			throw;
		}
	}

	void sqlite3_connection::open(std::string const & db)
	{
		return this->open( db.c_str() );
	}

#if SQLITE3X_USE_WCHAR
	void sqlite3_connection::open(const wchar_t *db) {
		if(sqlite3_open16(db, &this->m_db)!=SQLITE_OK)
			throw database_error("unable to open database");
		try
		{
			this->on_open();
		}
		catch(...)
		{
			try { this->close(); }
			catch(...) { /* ignore */ }
			throw;
		}
	}
#endif

	void sqlite3_connection::close() {
		if(this->m_db) {
			sqlite3 * x = this->m_db;
			this->m_db=NULL;
			if(sqlite3_close(x)!=SQLITE_OK)
				throw database_error(*this);
		}
	}

	int64_t sqlite3_connection::insertid() {
		if(!this->m_db) throw database_error("database is not open");
		return sqlite3_last_insert_rowid(this->m_db);
	}

 	int sqlite3_connection::changes() {
 		if(!this->m_db) throw database_error("database is not open");
 		return sqlite3_changes(this->m_db);
 	}


	void sqlite3_connection::setbusytimeout(int ms) {
		if(!this->m_db) throw database_error("database is not open");

		if(sqlite3_busy_timeout(this->m_db, ms)!=SQLITE_OK)
			throw database_error(*this);
	}

	void sqlite3_connection::executenonquery(const std::string &sql) {
		this->executenonquery( sql.c_str() );
	}

	void sqlite3_connection::executenonquery(char const * sql) {
		if(!this->m_db) throw database_error("database is not open");
		sqlite3_command(*this, sql).executenonquery();
	}

#if SQLITE3X_USE_WCHAR
	void sqlite3_connection::executenonquery(const std::wstring &sql) {
		if(!this->m_db) throw database_error("database is not open");
		sqlite3_command(*this, sql).executenonquery();
	}
#endif

	int sqlite3_connection::executeint(char const * sql) {
		if(!this->m_db) throw database_error("database is not open");
		return sqlite3_command(*this, sql).executeint();
	}
	int sqlite3_connection::executeint(const std::string &sql) {
		return this->executeint( sql.c_str() );
	}

#if SQLITE3X_USE_WCHAR
	int sqlite3_connection::executeint(const std::wstring &sql) {
		if(!this->m_db) throw database_error("database is not open");
		return sqlite3_command(*this, sql).executeint();
	}
#endif

	int64_t sqlite3_connection::executeint64(char const  * sql) {
		if(!this->m_db) throw database_error("database is not open");
		return sqlite3_command(*this, sql).executeint64();
	}

	int64_t sqlite3_connection::executeint64(const std::string &sql) {
		return this->executeint64( sql.c_str() );
	}

#if SQLITE3X_USE_WCHAR
	int64_t sqlite3_connection::executeint64(const std::wstring &sql) {
		if(!this->m_db) throw database_error("database is not open");
		return sqlite3_command(*this, sql).executeint64();
	}
#endif

	double sqlite3_connection::executedouble(char const * sql) {
		if(!this->m_db) throw database_error("database is not open");
		return sqlite3_command(*this, sql).executedouble();
	}

	double sqlite3_connection::executedouble(const std::string &sql) {
		return this->executedouble( sql.c_str() );
	}

#if SQLITE3X_USE_WCHAR
	double sqlite3_connection::executedouble(const std::wstring &sql) {
		if(!this->m_db) throw database_error("database is not open");
		return sqlite3_command(*this, sql).executedouble();
	}
#endif

	std::string sqlite3_connection::executestring(const std::string &sql) {
		if(!this->m_db) throw database_error("database is not open");
		return sqlite3_command(*this, sql).executestring();
	}

#if SQLITE3X_USE_WCHAR
	std::string sqlite3_connection::executestring(const std::wstring &sql) {
		if(!this->m_db) throw database_error("database is not open");
		return sqlite3_command(*this, sql).executestring();
	}
#endif

#if SQLITE3X_USE_WCHAR
	std::wstring sqlite3_connection::executestring16(const std::string &sql) {
		if(!this->m_db) throw database_error("database is not open");
		return sqlite3_command(*this, sql).executestring16();
	}
#endif

#if SQLITE3X_USE_WCHAR
	std::wstring sqlite3_connection::executestring16(const std::wstring &sql) {
		if(!this->m_db) throw database_error("database is not open");
		return sqlite3_command(*this, sql).executestring16();
	}
#endif

	std::string sqlite3_connection::executeblob(const std::string &sql) {
		if(!this->m_db) throw database_error("database is not open");
		return sqlite3_command(*this, sql).executeblob();
	}

#if SQLITE3X_USE_WCHAR
	std::string sqlite3_connection::executeblob(const std::wstring &sql) {
		if(!this->m_db) throw database_error("database is not open");
		return sqlite3_command(*this, sql).executeblob();
	}
#endif
	
	int sqlite3_connection::executecallback( std::string const & sql,
						 sqlite3_callback callback,
						 void * data,
						 std::string & errmsg )
	{
		char * cerrmsg = 0;
		int ret = 0;
		try
		{
			// allow callback to safely throw.
			ret = sqlite3_exec( this->m_db, sql.c_str(), callback, data, &cerrmsg );
		}
		catch( ... )
		{
			if( cerrmsg )
			{
				errmsg = cerrmsg;
				sqlite3_free( cerrmsg );
			}
			throw;
		}
		if( cerrmsg )
		{
			errmsg = cerrmsg;
			sqlite3_free( cerrmsg );
		}
		return ret;
	}

	int sqlite3_connection::executecallback( std::string const & sql,
						 sqlite3_callback func,
						 void * data )
	{
		std::string ignored;
		return this->executecallback( sql, func, data, ignored );
	}

	/**
	   An internal implementation detail of table_generator.
	*/
	class table_generator::table_generator_impl
	{

	public:
		sqlite3_connection * db;
		std::string name;
		std::vector<std::string> list;
	};

// 	int sqlite3_function_info8::create( sqlite3 * db )
// 	{
// 		return sqlite3_create_function(
// 					       db,
// 					       this->name,
// 					       this->argc,
// 					       0,
// 					       this->user_data,
// 					       this->func,
// 					       this->step,
// 					       this->final );
// 	}

// 	int sqlite3_function_info16::create( sqlite3 * db )
// 	{
// 		return sqlite3_create_function16(
// 					       db,
// 					       this->name,
// 					       this->argc,
// 					       1,
// 					       this->user_data,
// 					       this->func,
// 					       this->step,
// 					       this->final );
// 	}

	table_generator::table_generator( sqlite3_connection & con, std::string const & n )
		: m_pimpl( new table_generator::table_generator_impl )
	{
		int check = con.executeint( "select count(*) from sqlite_master where type like 'table' and name like '"+n+"'" );
		// ^^^ we use 'like' here because sqlite3 is case-insensitive
		if( 0 != check )
		{
			throw database_error( "table_generator() db table '%s' already exists.", n.c_str() );
		}
		this->m_pimpl->db = &con;
		this->m_pimpl->name = n;
	}

	table_generator::~table_generator() throw()
	{
		delete this->m_pimpl;
	}

	table_generator & table_generator::operator()( std::string const & fld )
	{
		this->m_pimpl->list.push_back( fld );
		return *this;
	}

	void table_generator::create()
	{
		size_t sz = this->m_pimpl->list.size();
		if( ! sz )
		{
			throw database_error( "table_generator::operator(): cannot create a table with no fields. Try using operator()(string) to add fields." );
		}
		std::ostringstream os;
		os << "create table "<< this->m_pimpl->name << "(";
		for( size_t i = 0; i < sz; ++i )
		{
			os << this->m_pimpl->list[i];
			if( i < (sz-1) ) os << ",";
		}
		os << ");";
		this->m_pimpl->db->executenonquery( os.str() );
	}

}
