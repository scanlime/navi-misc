/* 
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
#include "testGameServer.h"
#include "timer.h"

// messages
#define	_MESSAGE_SERVER_INFO 0x5349	// SI 
#define	_MESSAGE_CLIENT_INFO 0x4349	// CI 
#define	_MESSAGE_USER_PART 0x5550		// UP
#define	_MESSAGE_USER_ADD 0x5541		// UA
#define	_MESSAGE_KICK 0x4b4b				// KK
#define _MESSAGE_UPDATE 0x5544			//UD
#define _MESSAGE_ACKNOWLEDGE 0x414b	//AK
#define _MESSAGE_SPAWN 0x5350				//SP

void CTestGameServer::init ( void )
{
}

bool CTestGameServer::think ( void )
{
	std::map<int,trPlayerInfo>::iterator players = users.begin();

	while (players != users.end())
	{
		if (players->second.bot)
		{
			if(players->second.bot->think())	// let the bot think, if it needs an update then let it FLY
			{
				CNetworkMessage message;
				message.SetType(_MESSAGE_UPDATE);
				message.AddI(players->first);
				message.AddV(players->second.pos);
				message.AddV(players->second.rot);
				message.AddV(players->second.vec);
				sendToAllBut(message,-1,false);
			}
		}
		players++;
	}
	return false;
}

void CTestGameServer::sendToAllBut ( CNetworkMessage &message, int player, bool relyable )
{
	std::map<int,trPlayerInfo>::iterator players = users.begin();

	while (players != users.end())
	{
		if (player != players->first && !players->second.bot)
			message.Send(*players->second.peer,relyable);

		if (players->second.bot)
			players->second.bot->message(message);
		players++;
	}
}

bool CTestGameServer::message ( int playerID, CNetworkPeer &peer, CNetworkMessage &message )
{
	std::map<int,trPlayerInfo>::iterator itr = users.find(playerID);
	if (itr == users.end())
		return true;					// we don't know who they are

	// for now just do a simple message relay and add on the user ID

	bool rellay = false;
	bool spawn = false;
	bool relyable = true;

	switch (message.GetType())
	{
		case _MESSAGE_SERVER_INFO:	// SI 
			break;

		case _MESSAGE_CLIENT_INFO:	// CI 
			rellay = true;
			spawn = true;
			itr->second.player = true;
			itr->second.name = message.ReadStr();
			itr->second.material = message.ReadStr();
			break;

		case _MESSAGE_USER_PART:		// UP
			remove(playerID,peer);
			break;

		// this we should never get
		case _MESSAGE_USER_ADD:	// UA
		case _MESSAGE_KICK:			// KK
			rellay = false;
			break;

		// don't care about this one
		case _MESSAGE_ACKNOWLEDGE:
			break;

		case _MESSAGE_UPDATE: //UD
			if (itr->second.player)
			{
				rellay = true;
				relyable = false;
				message.ReadV(itr->second.pos);
				message.ReadV(itr->second.rot);
				message.ReadV(itr->second.vec);
			}
			break;

		default:
			rellay = true;
			break;
	}

	if (rellay)
	{	
		CNetworkMessage	newMessage;
		newMessage.SetType(message.GetType());	// what ever the message type is
		newMessage.AddI(playerID);

		void* mem = malloc(message.GetSize());
		message.GetDataMem(mem);
		newMessage.AddN(message.GetSize(),mem);

		// send an add to everyone else
		sendToAllBut(newMessage,playerID,relyable);
	}

	if (spawn)
	{
		// compute a spawn
		spawnPlayer(playerID);

		CNetworkMessage	newMessage;
		newMessage.SetType(_MESSAGE_SPAWN);
		newMessage.AddI(playerID);
		newMessage.AddV(itr->second.pos);
		newMessage.AddV(itr->second.rot);
		newMessage.AddV(itr->second.vec);

		// send spawn to everyone 
		sendToAllBut(newMessage,-1);
	}
	return false;
}

bool CTestGameServer::add ( int playerID, CNetworkPeer &peer )
{
	std::map<int,trPlayerInfo>::iterator itr = users.find(playerID);
	if (itr != users.end())
		return true;

	trPlayerInfo info;
	info.player = false;
	info.peer = &peer;
	info.pos[0] = info.pos[1] = info.pos[2] = 0;
	info.rot[0] = info.rot[1] = info.rot[2] = 0;
	info.vec[0] = info.vec[1] = info.vec[2] = 0;
	info.bot = NULL;
	users[playerID] = info;

	CNetworkMessage	message;


	// send it a user add and client info for everyone else
	itr = users.begin();
	while (itr != users.end())
	{
		if (itr->first !=playerID)
		{
			message.SetType(_MESSAGE_USER_ADD);
			message.AddI(itr->first);
			message.Send(peer,true);
			message.ClearData();

			message.SetType(_MESSAGE_CLIENT_INFO);	
			message.AddI(itr->first);
			message.AddStr(itr->second.name.c_str());
			message.AddStr(itr->second.material.c_str());	// throw in the last pos too
			message.AddV(itr->second.pos);
			message.AddV(itr->second.rot);
			message.AddV(itr->second.vec);
			message.Send(peer,true);
			message.ClearData();

		}
		itr++;
	}

	message.ClearData();
	message.SetType(_MESSAGE_SERVER_INFO);	// ServerInfo
	message.AddI(playerID);
	message.Send(peer,true);

	// send an add to everyone else
	message.SetType(_MESSAGE_USER_ADD);	// UserAdd
	message.AddI(playerID);
	sendToAllBut(message,playerID);
	return false;
}

void CTestGameServer::addBot (int playerID, const char* name, const char* config )
{
	std::map<int,trPlayerInfo>::iterator itr = users.find(playerID);
	if (itr != users.end())
		return;

	trPlayerInfo info;
	info.name = name;

	info.player = false;
	info.peer = NULL;
	info.pos[0] = info.pos[1] = info.pos[2] = 0;
	info.rot[0] = info.rot[1] = info.rot[2] = 0;
	info.vec[0] = info.vec[1] = info.vec[2] = 0;
	info.bot = new CRobotPlayer;

	users[playerID] = info;

	info.bot->init(name,config,&users[playerID]);

	CNetworkMessage	message;

	// send an add to everyone else
	message.SetType(_MESSAGE_USER_ADD);
	message.AddI(playerID);
	sendToAllBut(message,playerID);
	message.ClearData();

	// it's a bot so it won't send stuff like server info so just send the info now
	message.SetType(_MESSAGE_CLIENT_INFO);	//
	message.AddI(playerID);
	message.AddStr(info.name.c_str());
	message.AddStr(info.material.c_str());
	message.AddV(info.pos);
	message.AddV(info.rot);
	message.AddV(info.vec);
	sendToAllBut(message,playerID);
	message.ClearData();

	// now force it to spawn
	spawnPlayer(playerID);

	message.SetType(_MESSAGE_SPAWN);
	message.AddI(playerID);
	message.AddV(itr->second.pos);
	message.AddV(itr->second.rot);
	message.AddV(itr->second.vec);

	// send spawn to everyone 
	sendToAllBut(message,-1);
}

bool CTestGameServer::remove ( int playerID, CNetworkPeer &peer )
{
	std::map<int,trPlayerInfo>::iterator itr = users.find(playerID);
	if (itr == users.end())
		return true;

	// send the remove to everyone else
	CNetworkMessage	message;
	message.SetType(_MESSAGE_USER_PART);	// UserPart
	message.AddI(playerID);
	sendToAllBut(message,playerID);

	users.erase(itr);
	return false;
}

bool CTestGameServer::kick ( int playerID, CNetworkPeer &peer )
{
	std::map<int,trPlayerInfo>::iterator itr = users.find(playerID);
	if (itr == users.end())
		return true;

	CNetworkMessage	message;

	message.SetType(_MESSAGE_KICK);	// KicK
	message.Send(peer,true);

	// send the remove to everyone else
	message.SetType(_MESSAGE_USER_PART);	// UserPart
	message.AddI(playerID);
	sendToAllBut(message,playerID);

	users.erase(itr);
		return false;
}

void CTestGameServer::kill ( void )
{
	CNetworkMessage	message;
	message.SetType(_MESSAGE_KICK); 	// KicK
	sendToAllBut(message,-1);

	std::map<int,trPlayerInfo>::iterator players = users.begin();
	while (players != users.end())
	{
		if (players->second.bot)
			delete(players->second.bot);
		players++;
	}
	users.clear();
}

// game code
void CTestGameServer::spawnPlayer ( int playerID )
{
	std::map<int,trPlayerInfo>::iterator itr = users.find(playerID);
	if (itr == users.end())
		return;
	
	// this is crap but will work for now
	float xyRange = 300;
	float zRange = 25;
	float grav = -10;

	itr->second.pos[0] = (((float)rand()/(float)RAND_MAX)*xyRange*2)-xyRange;
	itr->second.pos[1] = (((float)rand()/(float)RAND_MAX)*xyRange*2)-xyRange;
	itr->second.pos[2] = (((float)rand()/(float)RAND_MAX)*zRange);

	itr->second.rot[2] = ((float)rand()/(float)RAND_MAX)*360;

	itr->second.vec[0] = itr->second.vec[1] = 0;
	itr->second.vec[2] = grav;
}

// bots
CRobotPlayer::CRobotPlayer()
{
	playerInfo = NULL;
	lastUpdateTime = -1;
}
 CRobotPlayer::~CRobotPlayer()
 {

 }

 void CRobotPlayer::init ( const char* name, const char* config, trPlayerInfo *info )
 {
	playerInfo = info;
	if (playerInfo)
	{
		playerInfo->name = name;
		playerInfo->material = "RedkMK3";
	}
	lastUpdateTime = (float)CTimer::instance().GetTime();
 }

 bool CRobotPlayer::think ( void )
 {
	if (!playerInfo)
		return false;

	// some constants
	float grav = -10.0f;

	// just make it spin
	playerInfo->rot[2] += 0.5f;

	// make em fall if they are all up high
	if (playerInfo->pos[2] > 0)
	{
		if ( playerInfo->vec[2] <= 0)
			playerInfo->vec[2] = grav;
	}
	else
		playerInfo->vec[2] = 0;

	// apply bot physics
	float updateTime = (float)CTimer::instance().GetFrameTime();
	playerInfo->pos[0] += playerInfo->vec[0]*updateTime;
	playerInfo->pos[1] += playerInfo->vec[1]*updateTime;
	playerInfo->pos[2] += playerInfo->vec[2]*updateTime;

	float botUpdateTime = 1.0f/0.5f;
	if (CTimer::instance().GetTime() - lastUpdateTime > botUpdateTime )
		return true;
	else
		return false;
 }

 bool CRobotPlayer::message ( CNetworkMessage &message )
 {
	 return false;
 }
