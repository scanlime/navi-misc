/* Firestarter
* firestarterd.cpp :
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

#include "commandargs.h"
#include "prefs.h"

#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>
#include <string>

struct MemoryStruct
{
	char		*memory;
	size_t	size;
};

size_t WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data)
{
	size_t realsize = size * nmemb;
	struct MemoryStruct *mem = (struct MemoryStruct *)data;

	mem->memory = (char *)realloc(mem->memory, mem->size + realsize + 1);
	if (mem->memory)
	{
		memcpy(&(mem->memory[mem->size]), ptr, realsize);
		mem->size += realsize;
		mem->memory[mem->size] = 0;
	}
	return realsize;
}

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

trServerInfo	info;

bool sendListServerAdd ( void )
{
	CURL *curl;
	CURLcode res;

	struct MemoryStruct chunk;

	chunk.memory=NULL; /* we expect realloc(NULL, size) to work */
	chunk.size = 0;    /* no data at this point */

	std::string		url;
	char					temp[512];

	// buildup the URL

	url = "http://firestarter.bakadigital.com/list/add.php?"; // add in the base URL
	url += "servername="+info.name;
	url += "&address="+info.address;

	sprintf(temp,"&port=%d",info.port);
	url += temp;

	sprintf(temp,"&version=%f",info.version);
	url += temp;

	url += "&game="+info.game;
	url += "&os="+info.os;

	sprintf(temp,"&maxplayers=%d",info.maxPlayers);
	url += temp;

	sprintf(temp,"&currentplayers=%d",info.currentPlayers);
	url += temp;

	curl = curl_easy_init();
	if(curl)
	{
		// set us up the URL
		curl_easy_setopt(curl, CURLOPT_URL,url.c_str() );
		/* send all data to this function  */
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
		/* we pass our 'chunk' struct to the callback function */
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
		res = curl_easy_perform(curl);
		/* always cleanup */
		curl_easy_cleanup(curl);
	}
	printf("%s",chunk.memory);

	return true;
}

bool getServerInfo ( void )
{
	CCommandLineArgs	&args = CCommandLineArgs::instance();
	CPrefsManager &prefs = CPrefsManager::instance();

	// get the name of the server
	if (args.Exists("name"))
		info.name = args.GetDataS("name");
	else if (prefs.ItemExists("servername"))
		info.name = prefs.GetItemS("servername");
	else
		info.name = "some_unknown_server";

	// get the public address
	if (args.Exists("address"))
		info.address = args.GetDataS("address");
	else if (prefs.ItemExists("serveraddress"))
		info.address = prefs.GetItemS("servernaddress");
	else
		info.address = "localhost";

	// get the game type
	if (args.Exists("game"))
		info.game = args.GetDataS("game");
	else if (prefs.ItemExists("game"))
		info.game = prefs.GetItemS("game");
	else
		info.game = "default";

	// get the OS
#ifdef _WIN32
		info.os = "Windows";
#else
		info.os = "linux";
#endif

	// get the port
	if (args.Exists("port"))
		info.port = args.GetDataI("port");
	else if (prefs.ItemExists("port"))
		info.port = prefs.GetItemI("port");
	else
		info.port = 2501;

	info.version = 1.0f;
	info.currentPlayers = 0;

	// get max players
	if (args.Exists("maxplayers"))
		info.maxPlayers = args.GetDataI("maxplayers");
	else if (prefs.ItemExists("maxplayers"))
		info.maxPlayers = prefs.GetItemI("maxplayers");
	else
		info.maxPlayers = 32;

	info.token = -1;

	return true;
}

int main (int argc, char **argv)
{
	CCommandLineArgs	&args = CCommandLineArgs::instance();
	args.Set(argc,argv);

	if (args.Exists("prefs"))
		CPrefsManager::instance().Init((char*)args.GetDataS("prefs"));
	else
		CPrefsManager::instance().Init("firestarterd");

	getServerInfo();
	sendListServerAdd();

	return 0;
}