#ifndef s11n_net_SQLITE3X_HPP_INCLUDED
#define s11n_net_SQLITE3X_HPP_INCLUDED
/*

	Copyright (C) 2004-2005 Cory Nelson
	Copyright (C) 2006 stephan beal (stephan s11n net)

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
	

This file has been modified from the original sources by <stephan at
s11n net>, as described briefly below.

The original code, by Cory Nelson, is available from:

http://dev.int64.org/sqlite.html

This hacked copy's home is:

http://wanderinghorse.net/computing/sqlite/

Contributors to the hacked version include:

stephan beal <stephan at s11n net>
- Maintainer, documentor.

Thomas Sailer <t.sailer at alumni ethz ch>:
- A fix for wide-char support on 64-bit Linux.

Artem Gr <artem at bizlink ru>
- Fixes to enable/disable wide-char support with a macro.

- Xose Anton Otero Ferreira submitted patches to remove 'long long'
decls and replace those with sqlite_int64. He also submitted
the isnull() functions.


Significant changes from the original sqlite3x distribution include:

- Removed dependency on boost library, since it was only a dependency
on boost::non_copyable (this same effect is easily achieved without
the dependency on the huge Boost library).

- Reordered some code to get it to compile under gcc.

- Added some missing #includes.

- database_error was reimplemented to allow use of a varargs ctor.

- Added a few helpful functions, like sqlite3_cursor::colcount().

- Removed various (char const * ...) overloads which were inherently
already covered by implicit conversions via (std::string const &)
overloads. Re-added them on 2006.09.25 after Artem Gr pointed out that
those overloads avoid a potential extra copy of the strings, and that
this could be a significant performance factor for some applications.

- Added lots of API docs.

- Improved some exception messages.

- Added table_generator class.

- Added sqlite3_connection::executecallback().

- sqlite3_cursor renamed to sqlite3_cursor (2007.01.22).

- Added sqlite3_cursor::close()

- sqlite3_cursor::read() renamed to step() (2007.01.22).

*/

#include <string>
#include <stdexcept>
#include <sqlite3.h> // only for sqlite3_callback :/

// Enable WCHAR support when it's there. Thanks to Artem Gr <artem@bizlink.ru>
// for this...
#ifndef SQLITE3X_USE_WCHAR
#  ifdef _GLIBCXX_USE_WCHAR_T
#    define SQLITE3X_USE_WCHAR 1
#  elif defined(UNICODE) // Windows uses this
#    define SQLITE3X_USE_WCHAR 1
#  else
#    define SQLITE3X_USE_WCHAR 0 // default
#  endif
#endif

/**
   This namespace encapsulates a C++ API wrapper for sqlite3
   databases. It was originally written by Cory Nelson and was hacked
   slightly by stephan beal.

   The home page for the original sources note that all of the
   w_char/wstring functions *probably* only work on Windows
   platforms. Your mileage may vary on other platforms. Users of this
   API are recommended to NOT use the wchar_t/wstring variants of any
   functions, as those functions may be removed at some point.


   Note that this API does not include support for all sqlite3
   features. However, the most commonly used features are available.

*/
namespace sqlite3x {

	/**
	   64-bit integer type used by this code.
	*/
	typedef sqlite_int64 int64_t;

	class sqlite3_command;

	/**
	   rc_is_okay() is an easy way to check if rc is one of
	   SQLITE_OK, SQLITE_ROW, or SQLITE_DONE.  This function
	   returns true if rc is one of those values, else false.
	   When writing code which accepts arbitrary client-supplied
	   SQL, any of those three codes can signal success, depending
	   on the SQL code and the context.
	*/
	bool rc_is_okay( int rc );


	/**
	   Represents a connection to an sqlite3 database.

	   About the only reason to subclass this type would be to do
	   customizations to the underlying sqlite3 db handle upon
	   construction of each object, e.g.  to add custom sqlite
	   functions or load custom modules.
   	*/
	class sqlite3_connection
	{
	private:
		// copy operations not implemented
		sqlite3_connection & operator=( sqlite3_connection const & );
		sqlite3_connection( sqlite3_connection const & );

		friend class sqlite3_command;

		mutable struct sqlite3 *m_db;
		std::string m_name;

	public:
		/**
		   Returns a handle to the underlying sqlite3
		   database. Friend classes should NEVER call
		   sqlite3_close() on this handle. Doing so will
		   result in undefined behaviour later on (when this
		   class is used or destructs).

		   This function is only public so that clients can
		   do things like register new sqlite3 functions
		   with the database.
		*/
		sqlite3 * db() const;

		/**
		   Default ctor. DB is unusable until open() is
		   called.
		*/
		sqlite3_connection();

		/**
		   Opens a database with the given name. Throws if
		   this->open(dbname) fails.
		*/
		explicit sqlite3_connection(std::string const & dbname);

		/**
		   See take(sqlite3*). This ctor is identical except
		   that it throws if passed a null pointer.
		*/
		sqlite3_connection( sqlite3 * dbh );

		/**
		   Calls this->close() if close() has not already
		   been called. If it calls close() then the exception
		   is silently ignored for the sake of having a no-throw
		   dtor.
		*/
		virtual ~sqlite3_connection();

		/** Returns this object's name. It is only valid if
		    the (char const *) ctor or open(char const *) was
		    used with a non-null name, otherwise it is an
		    empty string.
		*/
		std::string name() const;


		/**
		   Creates/opens the given db, throwing on error.
		   Remember that sqlite3 supports the name ":memory:"
		   as a pseudo-name for an in-memory database.

		   On success it returns, else it throws.
		   
		   Note that sqlite3 supports the special db name
		   ":memory:" to represent an in-memory database. Such
		   databases cannot be saved directly to disk and are
		   lost when this object closes the db.

		   Internal notes:

		   Once an sqlite3_open() succeeds, the protected
		   member this->on_open() in called. That member
		   should throw on error.

		   Subclasses which override this and do not want to
		   call the base implementation should call on_open()
		   when done to allow subclasses to initialize the
		   database if they like.
		*/
		virtual void open( char const * );

		/**
		  Functionally the same as open( char const *).
		 */
		void open(std::string const &dbname);

		/**

		   Transfers control of dbh to this object and makes
		   this object point at dbh. dbh is assumed to be
		   a valid, opened sqlite3 db handle.

		   If this->db() == dbh then this function
		   does nothing.

		   If this object had an opened db handle
		   then it is closed before dbh is taken.
		   Closing may throw, but this function takes
		   ownership of dbh regardless of whether
		   it throws or not.

		   If dbh is null, the effect is identical
		   to calling close().

		   This function triggers the protected on_open()
		   function if dbh is not null.
		*/
		void take( sqlite3 * dbh );

		/**
		   Transfers ownership of the returned handle to the caller.
		   This object is then considered closed. NULL is returned
		   if this object is closed.
		*/
		sqlite3 * take() throw();


		/**
		   Closes this database. If the db is not opened,
		   this is a no-op.
		*/
		void close();

		/**
		   Returns the rowid of the most recently inserted row
		   on this db.
		*/
		int64_t insertid();

  		/**
 		   Returns the number of database rows that were
 		   changed (or inserted or deleted) by the most recently
 		   completed INSERT, UPDATE, or DELETE statement.
 
 		   SQLite implements the command "DELETE FROM table"
 		   without a WHERE clause by dropping and recreating
 		   the table. To get an accurate count of the number
 		   of rows deleted, use "DELETE FROM table WHERE 1"
		   instead.
 		*/
 		int changes();


		/**
		   See sqlite3_busy_timeout().
		*/
		void setbusytimeout(int ms);

		/**
		   Executes a command which is assumed to have
		   a single step and a void result.
		*/
		void executenonquery(const std::string &sql);
		/**
		   Overloaded to avoid an internal copy of sql.
		   sql MUST be non-NULL and null-terminated.
		*/
		void executenonquery(char const * sql);

		/**
		   Executes the query, which is expected to have an
		   integer field as the first result field.
		*/
		int executeint(const std::string &sql);
		/**
		   Overloaded to avoid an internal copy of sql.
		   sql MUST be non-NULL and null-terminated.
		*/
		int executeint(char const * sql);

		/**
		   Executes the query, which is expected to have a
		   (int64_t) field as the first result field.
		*/
		int64_t executeint64(const std::string &sql);
		/**
		   Overloaded to avoid an internal copy of sql.
		   sql MUST be non-NULL and null-terminated.
		*/
		int64_t executeint64(char const * sql);

		/**
		   Executes the query, which is expected to have a
		   double field as the first result field.
		*/
		double executedouble(const std::string &sql);

		/**
		   Overloaded to avoid an internal copy of sql.
		   sql MUST be non-NULL and null-terminated.
		*/
		double executedouble(char const * sql);

		/**
		   Executes the query, which is expected to have a
		   string or blob field as the first result field. Note
		   that numeric results can be returned using this function,
		   but will come back as a string (lexically cast).
		*/
		std::string executestring(const std::string &sql);

		/**
		   Executes the query, which is expected to have a
		   string or blob field as the first result field. Note
		   that numeric results can be returned using this function,
		   but will come back as a string (lexically cast).
		*/
		std::string executeblob(const std::string &sql);

		/**
		   Executes the given SQL code, calling callback for
		   each row of the data set. The data pointer is
		   passed on as-is to the callback, and may be 0.  If
		   execution generates an error message it is stored
		   in errmsg.

		   If this function intercepts an exception (thrown
		   from the callback) then it propagates that
		   exception back to the caller. If it catches no
		   exception, it returns the result code, with zero
		   being success and non-zero being failure.

		   See sqlite3_exec() for more details.
		*/
		int executecallback( std::string const & sql, sqlite3_callback callback, void * data, std::string & errmsg );

		/**
		   Convenience overload which has a default data value
		   of 0 and ignores any error string passed back by
		   sqlite3_exec().
		*/
		int executecallback( std::string const & sql, sqlite3_callback callback, void * data = 0 );

		/**
		   Returns the equivalent of sqlite3_errmsg(), or an
		   empty string if that function returns
		   null. Reminder: the sqlite3 docs say that
		   sqlite3_errmsg() always returns a non-empty string,
		   even if the string is "not an error" (no joke).
		*/
		std::string errormsg() const;

#if SQLITE3X_USE_WCHAR
	public:
		explicit sqlite3_connection(const wchar_t *dbname);
		void executenonquery(const std::wstring &sql);
		int executeint(const std::wstring &sql);
		int64_t executeint64(const std::wstring &sql);
		double executedouble(const std::wstring &sql);
		std::string executestring(const std::wstring &sql);
		std::wstring executestring16(const std::wstring &sql);
		std::wstring executestring16(const std::string &sql);
		std::string executeblob(const std::wstring &sql);
		void open(const wchar_t *dbname);
#endif

	protected:
		/**
		   This function is called when open() succeeds. Subclasses
		   which wish to do custom db initialization or sanity checks
		   may do them here.
		*/
		virtual void on_open();

	};

	/**
	   Manages an sqlite3 transaction. Remember that sqlite3 does not
	   support nested transactions.

	   All functions of this class throw on error.
	*/
	class sqlite3_transaction {
	private:
		// copy operations not implemented
		sqlite3_transaction & operator=( sqlite3_transaction const & );
		sqlite3_transaction( sqlite3_transaction const & );
		sqlite3_connection &con;
		bool intrans;

	public:
		/**
		   Opens a transaction for the given connection. If
		   start==true (the default) then this->begin() is
		   called.
		 */
		sqlite3_transaction(sqlite3_connection &con, bool start=true);

		/** If destructed before commit() is called,
		    rollback() is called.
		*/
		~sqlite3_transaction();

		/** Starts a transaction. */
		void begin();
		/** Commits a transaction. */
		void commit();
		/** Rolls back a transaction with a commit. */
		void rollback();
	};

	class sqlite3_command;

	/**
	   A type for reading results from an sqlite3_command.
	*/
	class sqlite3_cursor {
	private:
		friend class sqlite3_command;

		sqlite3_command *cmd;


	public:
		/**
		   Creates a cursor by calling cmd->executecursor().
		*/
		sqlite3_cursor(sqlite3_command & cmd);
		/**
		   Creates an empty cursor object, suitable only
		   for use as the target of a copy/assignment.
		 */
		sqlite3_cursor();
		/**
		   Copies the given cursor object. This is a fairly
		   efficient operation, using reference counting.
		*/
		sqlite3_cursor(const sqlite3_cursor &copy);

		/**
		   Closes this cursor, freeing up db resources if this
		   is the last cursor of a copied set.
		 */
		~sqlite3_cursor();

		/**
		   Copies the given cursor object. This is a fairly
		   efficient operation, using reference counting. This
		   object points to the same underlying result set as
		   the original, so both objects should not be used.
		*/
		sqlite3_cursor& operator=(const sqlite3_cursor &copy);

		/**
		   Steps one step through the sql result set and returns
		   true on SQLITE_ROW, false on SQLITE3_DONE, and throws
		   on any other result.
		*/
		bool step();

		/** Resets the underlying prepared statement of
		    this cursor. Throws on error.
		*/
		void reset();

		/**
		   Closes this cursor. Calling it multiple times is a
		   no-op on the second and subsequent calls.
		*/
		void close();

		/**
		   Returns the column count of the result set or
		   throws on error.
		*/
		int colcount();

  		/**
 		   Check if the given field number is NULL. This function
 		   returns true if is NULL, else false.
 		*/
 		bool isnull(int index);


		/**
		   Gets the integer value at the given field number.
		*/
		int getint(int index);

		/**
		   Gets the (int64_t) value at the given field number.
		*/
		int64_t getint64(int index);

		/**
		   Gets the double value at the given field number.
		*/
		double getdouble(int index);

		/**
		   Gets the string value at the given field number.
		*/
		std::string getstring(int index);
		/**
		   Like getstring(index), but returns a C-style
		   string. We hope it is null-terminated, but the
		   sqlite3 docs are ambiguous on this point. size
		   is set to the length of the returned string.

		   The advantage of this over getstring(index) is that
		   this version avoids a potential extra internal copy
		   of the string. Note that there is no guaranty how
		   long this pointer will remain valid - be sure to
		   copy the string if you need it.
		*/
		char const * getstring(int index, int & size);


		/**
		   Gets the blob value at the given field number.
		*/
		std::string getblob(int index);

		/**
		   Overloaded to avoid an internal copy of the blob data.

		   size is set to the number of bytes in the blob and
		   the returned pointer is the blob.
		*/
		void const * getblob(int index, int & size );

		/**
		   Gets the column name for the given column index.
		   Throws on error.
		*/
		std::string getcolname(int index);



#if SQLITE3X_USE_WCHAR
		std::wstring getstring16(int index);
		std::wstring getcolname16(int index);
#endif

	};


	/**
	   Encapsulates a command to send to an sqlite3_connection.
	*/
	class sqlite3_command {
	private:
		// copy operations not implemented
		sqlite3_command & operator=( sqlite3_command const & );
		sqlite3_command( sqlite3_command const & );
 		friend class sqlite3_cursor;

		sqlite3_connection &con;
		mutable sqlite3_stmt *stmt;
		unsigned int refs;
		int argc;

	public:
		/**
		   Creates an unprepared statement. Use prepare()
		   create the statement.
		*/
		explicit sqlite3_command(sqlite3_connection &con);

		/**
		   Creates an sql statement with the given connection object
		   and sql code.
		*/
		sqlite3_command(sqlite3_connection &con, const std::string &sql);

		/**
		   An efficiency overload to avoid an extra copy of the sql
		   code. len must be the length of sql.
		*/
		sqlite3_command(sqlite3_connection &con, char const * sql, size_t len);

		/**
		   Cleans up any resources in use by this object.
		 */
		~sqlite3_command();

		/**
		   Prepares this statement or throws on error.  If len
		   is -1 then sql is assumed to be null-terminated.
		*/
		void prepare( char const * sql, int len = -1 );
		/**
		   Convenience overload taking a std::string.
		*/
		void prepare( std::string const & sql );

		/**
		   Binds NULL to the given index.
		*/
		void bind(int index);
		/**
		   Binds data to the given query index.
		*/
		void bind(int index, int data);
		/**
		   Binds data to the given query index.
		*/
		void bind(int index, int64_t data);
		/**
		   Binds data to the given query index.
		*/
		void bind(int index, double data);
		/**
		   Binds data to the given query index. Data must be
		   exactly datalen bytes long. If datalen == -1 then
		   strlen(data) is used to calculate it.
		*/
		void bind(int index, const char *data, int datalen = -1);

		/**
		   Binds data to the given query index. Data must be
		   exactly datalen bytes long.
		*/
		void bind(int index, const void *data, int datalen);
		/**
		   Binds data to the given query index.
		*/
		void bind(int index, const std::string &data);

		/** Executes the query and returns a cursor object
		    which can be used to iterate over the results.
		 */
		sqlite3_cursor executecursor();
		/**
		   Executes the query and provides no way to get
		   the results. Throws on error.
		*/
		void executenonquery();
		/**
		   Executes the query, which is expected to have an
		   integer field as the first result field.
		*/
		int executeint();
		/**
		   Executes the query, which is expected to have a
		   (int64_t) field as the first result field.
		*/
		int64_t executeint64();
		/**
		   Executes the query, which is expected to have a
		   double field as the first result field.
		*/
		double executedouble();
		/**
		   Executes the query, which is expected to have a
		   string or blob field as the first result field. Note
		   that numeric results can be returned using this function,
		   but will come back as a string (lexically cast).
		*/
		std::string executestring();
		/**
		   Like executestring(), but returns a C-style
		   string. We hope it is null-terminated, but the
		   sqlite3 docs are ambiguous on this point. size
		   is set to the length of the returned string.

		   The advantage of this over executestring() is that
		   this version avoids a potential extra internal copy
		   of the string. Note that there is no guaranty how
		   long this pointer will remain valid - be sure to
		   copy the string if you need it.
		*/
		char const * executestring( int & size );

		/**
		   Executes the query, which is expected to have a
		   string or blob field as the first result field. Note
		   that numeric results can be returned using this function,
		   but will come back as a string (lexically cast).
		*/
		std::string executeblob();

		/**
		   Like executeblob(), but returns a void pointer to
		   the data. size is set to the length of the returned
		   data.

		   The advantage of this over executeblob() is that
		   this version avoids a potential extra internal copy
		   of the string and "should work" on wide-char
		   strings. Note that there is no guaranty how long
		   this pointer will remain valid - be sure to copy it
		   if you need it for very long.
		*/
		void const * executeblob(int & size );

		/**
		   Returns the column count of this object's query,
		   or throws on error.
		*/
		int colcount();

		/** Resets this statement using sqlite3_reset().
		    Errors are considered to be minor and only cause false
		    to be returned.
		*/
		bool reset();


		/**
		   Returns the underlying statement handle. It is not legal to
		   finalize this statement handle, as that will put this object
		   out of sync with the state of the handle.
		*/
		sqlite3_stmt * handle();

		/**
		   Finalizes this statement. Throws if finalization fails.
		   Calling finalize() multiple times is a no-op.
		 */
		void finalize();

#if SQLITE3X_USE_WCHAR
		sqlite3_command(sqlite3_connection &con, const std::wstring &sql);
		void bind(int index, const wchar_t *data, int datalen);
		void bind(int index, const std::wstring &data);
		std::wstring executestring16();
#endif // SQLITE3_USE_WCHAR

	};


	/**
	   Exception type used by the sqlite3x classes.
	*/
	class database_error : public std::exception {
	public:
		/**
		   Takes a format specifier compatible with printf.

		   If the message length surpasses a hard-coded limit (2k?)
		   then it is truncated to fit within that limit.
		 */
		explicit database_error(const char *format, ... );

		/**
		   Creates an exception with con.errormsg()
		   as the what() text.
		*/
		database_error(sqlite3_connection &con);

		virtual ~database_error() throw();

		/**
		   Returns this object's error string.
		*/
		virtual char const * what() const throw();
	private:
		std::string m_what;
	};


// 	/**
// 	   EXPERIMENTAL.

// 	   A helper type for storing information on
// 	   functions to register with sqlite.
// 	*/
// 	struct sqlite3_function_info_base
// 	{
// 	public:
// 		enum {
// 		TextUTF8 = SQLITE_UTF8,
// 		TextUTF16 = SQLITE_UTF16,
// 		TextUTF16BE = SQLITE_UTF16BE,
// 		TextUTF16LE = SQLITE_UTF16LE,
// 		TextAny = SQLITE_ANY
// 		};
// 		int argc;
// 		int text_rep; /* 1: UTF-16.  0: UTF-8 */
// 		void * user_data;
// 		void (*func)(sqlite3_context*,int,sqlite3_value**);
// 		void (*step)(sqlite3_context*,int,sqlite3_value**);
// 		void (*final)(sqlite3_context*);
// 	protected:
// 		sqlite3_function_info_base()
// 			: argc(0),
// 			  text_rep(TextUTF8),
// 			  user_data(0),
// 			  func(0), step(0), final(0)
// 		{}

// 		virtual ~sqlite3_function_info_base() {}

// 		virtual int create( sqlite3 * db ) = 0;
// 	};

// 	/**
// 	   EXPERIMENTAL.
// 	*/
// 	struct sqlite3_function_info8 : sqlite3_function_info_base
// 	{
// 		const char * name;
// 		explicit sqlite3_function_info8( char const * n )
// 			: sqlite3_function_info_base(),
// 			  name(n)
// 		{
// 			this->text_rep = TextUTF8;
// 		}
// 		virtual ~sqlite3_function_info8(){}
// 		virtual int create( sqlite3 * db );
// 	};

// 	/**
// 	   EXPERIMENTAL.
// 	*/
// 	struct sqlite3_function_info16 : sqlite3_function_info_base
// 	{
// 		void const * name;
// 		explicit sqlite3_function_info16( void const * n )
// 			: sqlite3_function_info_base(),
// 			  name(n)
// 		{
// 			this->text_rep = TextUTF16;
// 		}
// 		virtual ~sqlite3_function_info16(){}
// 		virtual int create( sqlite3 * db );
// 	};

	/**
	   A helper class to generate db tables.

	   It is used like so:

	   table_generator( connection, "table_name" )( "field1" )( "field2" )("field3").create();

	   That creates the named table with the given fields. It
	   throws if table_name already exists in the db or if
	   creation of the table fails.

	   An arbitrary number of fields can be added using
	   operator()(string), up to the internal limits set by
	   sqlite3.
	*/
	class table_generator
	{
	private:
		class table_generator_impl;
		table_generator_impl * m_pimpl;
	public:
		/**
		   Initializes the table generation process. Throws if
		   con contains a table with the same name.
		*/
		explicit table_generator( sqlite3_connection & con, std::string const & name );

		/** Frees up internal resources. */
		~table_generator() throw();

		/**
		   Adds field_name as a field of this table. Checks
		   for duplicate field names are deferred until
		   create() is called.
		*/
		table_generator & operator()( std::string const & field_name );

		/**
		   Executes the 'create table' statements. Throws on error.
		*/
		void create();
	};

}

#endif // s11n_net_SQLITE3X_HPP_INCLUDED
