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
	
	CVS Info :
		$Author: sgbeal $
		$Date: 2007/02/26 21:33:39 $
		$Revision: 1.6 $
*/

#include <sqlite3.h>
#include "sqlite3x.h"
#include <cstdarg> // varargs handling
#include <limits> // std::max()
#include <cstring> // strlen()
#include <cstdio> // vsnprintf()
#include <vector>
namespace sqlite3x {

	database_error::~database_error() throw() {}

	database_error::database_error(sqlite3_connection &con)
		: m_what( "sqlite3_connection["+con.name()+"]: "+con.errormsg() )
	{
	}

	char const * database_error::what() const throw()
	{
		return this->m_what.c_str();
	}

	database_error::database_error(const char *format,...)
	{
		const int buffsz = static_cast<int>( std::max( (size_t) 2048, strlen(format) * 2 ) );
		std::vector<char> buffer( buffsz, '\0' );
		va_list vargs;
		va_start ( vargs, format );
		int size = vsnprintf(&buffer[0], buffsz, format, vargs);
		va_end( vargs );
		if (size > (buffsz-1))
		{
			// replace tail of msg with "..."
			size = buffsz-1;
			for( int i = buffsz-4; i < buffsz-1; ++i )
			{
				buffer[i] = '.';
			}
		}
		buffer[size] = '\0';
		this->m_what = std::string( &buffer[0], &buffer[0]+size );
	}
}
