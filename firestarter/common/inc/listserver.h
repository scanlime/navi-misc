/* Firestarter
* osfile.h :
*
* Copyright (C) 2004 Jeffrey Myers
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
* email: jeffm2501@sbcglobal.net
*/

#ifndef _LISTSERVER_H_
#define _LISTSERVER_H_

#include <string>

typedef struct 
{
	std::string		name;
	std::string		address;
	int						port;
	std::string		game;
	float					version;
	std::string		os;
	int						maxPlayers;
	int						currentPlayers;
	int						token;
}trServerInfo;

class CListServerServerConnection
{
public:
	CListServerServerConnection();
	~CListServerServerConnection();

	bool add ( trServerInfo &info );
	bool update ( trServerInfo &info );
	bool remove ( trServerInfo &info );

	size_t writeMemoryCallback(void *ptr, size_t inSize, size_t nmemb, void *data);

protected:
	void clearPageData ( void );
	char			*memory;
	size_t		size;
};

#endif //_LISTSERVER_H_