/* 3dScreamers
* Copyright (c) 2002 - 2003 Jeffrey Myers
*
* This package is free software;  you can redistribute it and/or
* modify it under the terms of the license found in the file
* named license.txt that should have accompanied this file.
*
* THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
* WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
*/
// testGameServer.h

#ifndef _TEST_GAME_SERVER_H_
#define _TEST_GAME_SERVER_H_

#include "serverGameManager.h"
#include <map>
#include <string>

typedef struct 
{
	bool player;
	std::string name;
	CNetworkPeer	*peer;
	float					pos[3];
	float					rot[3];
	float					vec[3];
}trPlayerInfo;

class CTestGameServer : public CBaseServerGame
{
	public:
		CTestGameServer(){return;}
		virtual	~CTestGameServer(){return;}

		virtual void init ( void );
		virtual bool think ( void );

		virtual bool message ( int playerID, CNetworkPeer &peer, CNetworkMessage &message );
		virtual bool add ( int playerID, CNetworkPeer &peer );
		virtual bool remove ( int playerID, CNetworkPeer &peer );
		virtual bool kick ( int playerID, CNetworkPeer &peer );

		virtual void kill ( void );

	protected:
		void sendToAllBut ( CNetworkMessage &message, int player );
		std::map<int,trPlayerInfo>	users;
};

#endif //_TEST_GAME_H_