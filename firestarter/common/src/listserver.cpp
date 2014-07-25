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

#include "listserver.h"
#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>

#include "commandargs.h"
#include "prefs.h"
#include "textUtils.h"

size_t serverWriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data)
{
	return ((CBaseWebConnectClass*)data)->writeMemoryCallback(ptr,size,nmemb,data);
}

CListServerServerConnection::CListServerServerConnection()
{
	clearPageData();
}

CListServerServerConnection::~CListServerServerConnection()
{
	clearPageData();
}

void CListServerServerConnection::clearPageData ( void )
{
	if (memory)
		free (memory);
	memory = NULL;
	size = 0;
}

bool CListServerServerConnection::add ( trServerInfo &info )
{
	CURL *curl;
	CURLcode res;

	std::string		url;
	char					temp[512];

	CCommandLineArgs	&args = CCommandLineArgs::instance();
	CPrefsManager &prefs = CPrefsManager::instance();

	// clear out the token as we want to get a new one
	info.token = -1;
	// get the base URL
	std::string serverbaseURL = "firestarter.bakadigital.com/list/";

	if (args.Exists("listserver"))
		serverbaseURL = args.GetDataS("listserver");
	else if (prefs.ItemExists("listserver"))
		serverbaseURL = prefs.GetItemS("listserver");

	// buildup the URL

	url = "http://" + serverbaseURL;// add in the base URL
	url += "add.php?"; 
	url += "servername="+url_encode(info.name);
	url += "&address="+url_encode(info.address);

	sprintf(temp,"&port=%d",info.port);
	url += temp;

	sprintf(temp,"&version=%f",info.version);
	url += temp;

	url += "&game="+url_encode(info.game);
	url += "&os="+url_encode(info.os);

	sprintf(temp,"&maxplayers=%d",info.maxPlayers);
	url += temp;

	sprintf(temp,"&currentplayers=%d",info.currentPlayers);
	url += temp;

	clearPageData();

	curl = curl_easy_init();
	if(curl)
	{
		// set us up the URL
		curl_easy_setopt(curl, CURLOPT_URL,url.c_str() );
		/* send all data to this function  */
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, serverWriteMemoryCallback);
		/* we pass our 'chunk' struct to the callback function */
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)this);
		res = curl_easy_perform(curl);
		/* always cleanup */
		curl_easy_cleanup(curl);
	}
	// parse that shit for the token
	if (size == 0 || res != CURLE_OK)
		return false;
	
	char * idtag = strstr(memory,"id=");
	if (!idtag)
		return false;
	
	idtag += 3;
	info.token = atoi(idtag);

	return info.token != -1;
}
bool CListServerServerConnection::update ( trServerInfo &info )
{
	CURL *curl;
	CURLcode res;

	std::string		url;
	char					temp[512];

	CCommandLineArgs	&args = CCommandLineArgs::instance();
	CPrefsManager &prefs = CPrefsManager::instance();

	if (info.token == -1)
		return add(info);

	// get the base URL
	std::string serverbaseURL = "firestarter.bakadigital.com/list/";

	if (args.Exists("listserver"))
		serverbaseURL = args.GetDataS("listserver");
	else if (prefs.ItemExists("listserver"))
		serverbaseURL = prefs.GetItemS("listserver");

	// buildup the URL

	url = "http://" + serverbaseURL;// add in the base URL
	url += "update.php?"; 

	sprintf(temp,"id=%d",info.token);
	url += temp;

	url += "&servername="+url_encode(info.name);
	url += "&address="+url_encode(info.address);

	sprintf(temp,"&port=%d",info.port);
	url += temp;

	sprintf(temp,"&version=%f",info.version);
	url += temp;

	url += "&game="+url_encode(info.game);
	url += "&os="+url_encode(info.os);

	sprintf(temp,"&maxplayers=%d",info.maxPlayers);
	url += temp;

	sprintf(temp,"&currentplayers=%d",info.currentPlayers);
	url += temp;
//	url = url_encode(url);

	clearPageData();

	curl = curl_easy_init();
	if(curl)
	{
		// set us up the URL
		curl_easy_setopt(curl, CURLOPT_URL,url.c_str() );
		/* send all data to this function  */
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, serverWriteMemoryCallback);
		/* we pass our 'chunk' struct to the callback function */
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)this);
		res = curl_easy_perform(curl);
		/* always cleanup */
		curl_easy_cleanup(curl);
	}
	// parse that shit for the token
	if (size == 0 || res != CURLE_OK)
		return false;

	char * tag = strstr(memory,"update processed");
	if (!tag)
	{
		info.token = -1;
		return false;
	}
	return true;
}

bool CListServerServerConnection::remove ( trServerInfo &info )
{
	CURL *curl;
	CURLcode res;

	std::string		url;
	char					temp[512];

	CCommandLineArgs	&args = CCommandLineArgs::instance();
	CPrefsManager &prefs = CPrefsManager::instance();

	// clear out the token as we want to get a new one
	info.token = -1;
	// get the base URL
	std::string serverbaseURL = "firestarter.bakadigital.com/list/";

	if (args.Exists("listserver"))
		serverbaseURL = args.GetDataS("listserver");
	else if (prefs.ItemExists("listserver"))
		serverbaseURL = prefs.GetItemS("listserver");

	// buildup the URL

	url = "http://" + serverbaseURL;// add in the base URL
	url += "remove.php?"; 

	sprintf(temp,"&id=%d",info.token);
	url += temp;
	//url = url_encode(url);

	clearPageData();

	curl = curl_easy_init();
	if(curl)
	{
		// set us up the URL
		curl_easy_setopt(curl, CURLOPT_URL,url.c_str() );
		/* send all data to this function  */
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, serverWriteMemoryCallback);
		/* we pass our 'chunk' struct to the callback function */
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)this);
		res = curl_easy_perform(curl);
		/* always cleanup */
		curl_easy_cleanup(curl);
	}
	// parse that shit for the token
	if (size == 0 || res != CURLE_OK)
		return false;

	char * tag = strstr(memory,"remove processed");
	if (!tag)
		return false;

	return true;
}

size_t CListServerServerConnection::writeMemoryCallback(void *ptr, size_t inSize, size_t nmemb, void *data)
{
	size_t realsize = inSize * nmemb;

	memory = (char *)realloc(memory, size + realsize + 1);
	if (memory)
	{
		memcpy(&(memory[size]), ptr, realsize);
		size += realsize;
		memory[size] = 0;
	}
	return realsize;
}


// client stuff
CListServerClientConnection::CListServerClientConnection()
{
	clearPageData();
}

CListServerClientConnection::~CListServerClientConnection()
{
	clearPageData();
}

bool CListServerClientConnection::readNextItem( std::string &item, char** data )
{
	item =**data;
	(*data)++;

	while (**data != '&')
	{
		item +=**data;
		(*data)++;
	}
	return **data != '/0';
}

bool CListServerClientConnection::get ( void )
{
	CURL *curl;
	CURLcode res;

	std::string		url;

	CCommandLineArgs	&args = CCommandLineArgs::instance();
	CPrefsManager &prefs = CPrefsManager::instance();

	// get the base URL
	std::string serverbaseURL = "firestarter.bakadigital.com/list/";

	if (args.Exists("listserver"))
		serverbaseURL = args.GetDataS("listserver");
	else if (prefs.ItemExists("listserver"))
		serverbaseURL = prefs.GetItemS("listserver");

	// buildup the URL

	url = "http://" + serverbaseURL;// add in the base URL
	url += "listserv.php?"; 

	url += "simpleoutput=1";

	clearPageData();

	curl = curl_easy_init();
	if(curl)
	{
		// set us up the URL
		curl_easy_setopt(curl, CURLOPT_URL,url.c_str() );
		/* send all data to this function  */
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, serverWriteMemoryCallback);
		/* we pass our 'chunk' struct to the callback function */
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)this);
		res = curl_easy_perform(curl);
		/* always cleanup */
		curl_easy_cleanup(curl);
	}
	// parse that shit for the token
	if (size == 0 || res != CURLE_OK)
		return false;

	char * tag = strstr(memory,"!beginlist");
	if (!tag)
		return false;

	tag = strstr(tag,"list:");
	if (!tag)
		return false;

	tag += strlen("list:");

	int count = atoi(tag);
	serverList.clear();

	for (int i = 0; i < count; i++ )
	{
			tag = strstr(tag,"&");
			if (!tag)
				return false;
			tag++;

			trServerInfo info;
			std::string	temp;

			// get server name
			readNextItem(temp,&tag);
			tag++;
			info.name = temp;

			// get server address
			readNextItem(temp,&tag);
			tag++;
			info.address = temp;

			// get server port
			readNextItem(temp,&tag);
			tag++;
			info.port = atoi(temp.c_str());

			// get server game
			readNextItem(temp,&tag);
			tag++;
			info.game = temp;

			// get server version
			readNextItem(temp,&tag);
			tag++;
			info.version = (float)atof(temp.c_str());

			// get server OS
			readNextItem(temp,&tag);
			tag++;
			info.os = temp;

			// get server max players
			readNextItem(temp,&tag);
			tag++;
			info.maxPlayers = atoi(temp.c_str());

			// get server current players
			readNextItem(temp,&tag);
			tag++;
			info.currentPlayers = atoi(temp.c_str());

			// the '&' at the end
			tag++;

			serverList.push_back(info);
	}
	return true;
}

int CListServerClientConnection::count ( void )
{
	return (int)serverList.size();
}

trServerInfo& CListServerClientConnection::info ( int item )
{
	return serverList[item];
}

size_t CListServerClientConnection::writeMemoryCallback(void *ptr, size_t inSize, size_t nmemb, void *data)
{
	size_t realsize = inSize * nmemb;

	memory = (char *)realloc(memory, size + realsize + 1);
	if (memory)
	{
		memcpy(&(memory[size]), ptr, realsize);
		size += realsize;
		memory[size] = 0;
	}
	return realsize;
}

void CListServerClientConnection::clearPageData ( void )
{
	if (memory)
		free (memory);
	memory = NULL;
	size = 0;
}