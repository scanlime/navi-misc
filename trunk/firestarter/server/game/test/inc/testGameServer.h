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
#include "testWorld.h"

class CRobotPlayer;

typedef struct 
{
	bool							player;
	std::string				name;
	std::string				material;
	std::string				mesh;
	CNetworkPeer			*peer;
	float							pos[3];
	float							rot[3];
	float							vec[3];
	float							lastStamp;
	CRobotPlayer			*bot;
}trPlayerInfo;

class CRobotPlayer
{
public:
	CRobotPlayer();
	virtual ~CRobotPlayer();

	virtual void init ( const char* name, const char* config, trPlayerInfo *info );
	virtual bool think ( void );
	virtual bool message ( CNetworkMessage &message );

protected:
	trPlayerInfo *playerInfo;
	float lastUpdateTime;
};

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

		virtual void addBot (int playerID, const char* name, const char* config );

	protected:
		void sendToAllBut ( CNetworkMessage &message, int player, bool relyable = true );
		std::map<int,trPlayerInfo>	users;

		// game code
		void spawnPlayer ( int playerID );

		// message senders
		void sendClientInfo ( int playerID );
		void sendUpdate ( int playerID );
		void sendClockPing ( int playerID, int pingID ); 

		CTestWorld	world;
};

#endif //_TEST_GAME_H_