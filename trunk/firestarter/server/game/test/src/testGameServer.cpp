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
#include "firestarterd.h"

#include "messages.h"
#include "commandargs.h"

void CTestGameServer::init ( void )
{
	CCommandLineArgs	&args = CCommandLineArgs::instance();
	
	bool mapLoaded = false;

	if (args.Exists("map"))
	{
		mapLoaded = world.init(args.GetDataS("map"));
		if (!mapLoaded)
			logOut("worldfile load failed","CTestGameServer::init",eLogLevel1);
		else
			logOut("loaded worldfile","CTestGameServer::init",eLogLevel1);
	}
	
	if (!mapLoaded)
	{
		logOut("loading default world","CTestGameServer::init",eLogLevel1);
		world.initDefaultWorld();
	}
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
				message.AddF(players->second.lastStamp);
				sendToAllBut(message,players->first,false);

				char temp[512];
				sprintf(temp,"update for bot ID %d for %f %f %f",players->first,players->second.pos[0],players->second.pos[1],players->second.pos[2]);
				logOut(temp,"CTestGameServer::think ",eLogLevel5);
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

void CTestGameServer::sendClientInfo ( int playerID )
{
	CNetworkMessage message;

	std::map<int,trPlayerInfo>::iterator itr = users.find(playerID);
	if (itr == users.end())
		return;

	message.SetType(_MESSAGE_CLIENT_INFO);
	message.AddI(playerID);
	message.AddStr(itr->second.name.c_str());
	message.AddStr(itr->second.material.c_str());
	message.AddStr(itr->second.mesh.c_str());
	message.AddV(itr->second.pos);
	message.AddV(itr->second.rot);
	message.AddV(itr->second.vec);
	message.AddF(itr->second.lastStamp);

	sendToAllBut(message,playerID,true);
}

void CTestGameServer::sendUpdate ( int playerID )
{
	CNetworkMessage message;

	std::map<int,trPlayerInfo>::iterator itr = users.find(playerID);
	if (itr == users.end())
		return;

	message.SetType(_MESSAGE_UPDATE);
	message.AddI(playerID);
	message.AddV(itr->second.pos);
	message.AddV(itr->second.rot);
	message.AddV(itr->second.vec);
	message.AddF(itr->second.lastStamp);

	sendToAllBut(message,playerID,false);
}

void CTestGameServer::sendClockPing ( int playerID, int pingID )
{
	CNetworkMessage message;

	std::map<int,trPlayerInfo>::iterator itr = users.find(playerID);
	if (itr == users.end())
		return;

	message.SetType(_MESSAGE_TIME_PING);
	message.AddI(pingID);
	message.AddF((float)CTimer::instance().GetTime());

	message.Send(*itr->second.peer,false);
}

bool CTestGameServer::message ( int playerID, CNetworkPeer &peer, CNetworkMessage &message )
{
	std::map<int,trPlayerInfo>::iterator itr = users.find(playerID);
	if (itr == users.end())
		return true;					// we don't know who they are

	// for now just do a simple message relay and add on the user ID

	bool spawn = false;

	switch (message.GetType())
	{
		case _MESSAGE_SERVER_INFO:	// SI 
			logOut("receve _MESSAGE_SERVER_INFO","CTestGameServer::message",eLogLevel4);
			break;

		case _MESSAGE_CLIENT_INFO:	// CI 
			logOut("receve _MESSAGE_CLIENT_INFO","CTestGameServer::message",eLogLevel4);
			spawn = true;
			itr->second.player = true;
			itr->second.name = message.ReadStr();
			itr->second.material = message.ReadStr();
			itr->second.mesh = message.ReadStr();
			sendClientInfo(playerID);
			break;

		case _MESSAGE_USER_PART:		// UP
			logOut("receve _MESSAGE_USER_PART","CTestGameServer::message",eLogLevel4);
			remove(playerID,peer);
			break;

		// this we should never get
		case _MESSAGE_USER_ADD:	// UA
		case _MESSAGE_KICK:			// KK
			logOut("receve _MESSAGE_USER_ADD OR KICK","CTestGameServer::message",eLogLevel4);
			break;

		// don't care about this one
		case _MESSAGE_ACKNOWLEDGE:
			logOut("receve _MESSAGE_ACKNOWLEDGE","CTestGameServer::message",eLogLevel4);
			break;

		case _MESSAGE_UPDATE: //UD
			logOut("receve _MESSAGE_UPDATE","CTestGameServer::message",eLogLevel5);
			if (itr->second.player)
			{
				message.ReadV(itr->second.pos);
				message.ReadV(itr->second.rot);
				message.ReadV(itr->second.vec);
				message.ReadV(itr->second.vec);
				itr->second.lastStamp = message.ReadF();

				char temp[512];
				sprintf(temp,"update from ID %d for %f %f %f",playerID,itr->second.pos[0],itr->second.pos[1],itr->second.pos[2]);
				logOut(temp,"CTestGameServer::message",eLogLevel5);
				
				sendUpdate(playerID);
			}
			break;

		case _MESSAGE_TIME_PING:
			logOut("receve _MESSAGE_TIME_PING","CTestGameServer::message",eLogLevel5);
			sendClockPing(playerID, message.ReadI());
			break;

		default:
			logOut("receve unknown message","CTestGameServer::message",eLogLevel4);
			break;
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
		newMessage.AddF(itr->second.lastStamp);

		logOut("send spawn","CTestGameServer::message",eLogLevel3);

		// send spawn to everyone 
		sendToAllBut(newMessage,-1);
	}
	return false;
}

bool CTestGameServer::add ( int playerID, CNetworkPeer &peer )
{
	char temp[512];
	sprintf(temp,"add player with ID %d",playerID);
	logOut(temp,"CTestGameServer::add",eLogLevel3);

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
	info.lastStamp = (float)CTimer::instance().GetTime();
	users[playerID] = info;

	CNetworkMessage	message;


	// send it a user add and client info for everyone else
	itr = users.begin();
	while (itr != users.end())
	{
		if (itr->first !=playerID)
		{
			logOut("send _MESSAGE_USER_ADD","CTestGameServer::add",eLogLevel4);

			message.SetType(_MESSAGE_USER_ADD);
			message.AddI(itr->first);
			message.Send(peer,true);
			message.ClearData();

			logOut("send _MESSAGE_CLIENT_INFO","CTestGameServer::add",eLogLevel4);
			message.SetType(_MESSAGE_CLIENT_INFO);	
			message.AddI(itr->first);
			message.AddStr(itr->second.name.c_str());
			message.AddStr(itr->second.material.c_str());
			message.AddStr(itr->second.mesh.c_str());	
			message.AddV(itr->second.pos);// throw in the last pos too
			message.AddV(itr->second.rot);
			message.AddV(itr->second.vec);
			message.AddF(itr->second.lastStamp);
			message.Send(peer,true);
			message.ClearData();
		}
		itr++;
	}

	logOut("send _MESSAGE_SERVER_INFO","CTestGameServer::add",eLogLevel4);
	sprintf(temp,"player ID: %d",playerID);
	logOut(temp,"CTestGameServer::add",eLogLevel4);

	message.ClearData();
	message.SetType(_MESSAGE_SERVER_INFO);	// ServerInfo
	message.AddI(playerID);
	message.Send(peer,true);

	world.sendMapTo(peer);

	// send an add to everyone else
	logOut("send _MESSAGE_USER_ADD","CTestGameServer::add::everyone else",eLogLevel4);
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

	char temp[512];
	sprintf(temp,"add bot with ID %d",playerID);
	logOut(temp,"CTestGameServer::addBot",eLogLevel4);

	trPlayerInfo info;
	info.name = name;

	info.player = false;
	info.peer = NULL;
	info.pos[0] = info.pos[1] = info.pos[2] = 0;
	info.rot[0] = info.rot[1] = info.rot[2] = 0;
	info.vec[0] = info.vec[1] = info.vec[2] = 0;
	info.bot = new CRobotPlayer;
	info.lastStamp = (float)CTimer::instance().GetTime();
	users[playerID] = info;

	info.bot->init(name,config,&users[playerID]);

	CNetworkMessage	message;

	logOut("send _MESSAGE_USER_ADD","CTestGameServer::addBot::everyone else",eLogLevel4);

	// send an add to everyone else
	message.SetType(_MESSAGE_USER_ADD);
	message.AddI(playerID);
	sendToAllBut(message,playerID);
	message.ClearData();

	// it's a bot so it won't send stuff like server info so just send the info now
	logOut("send _MESSAGE_CLIENT_INFO","CTestGameServer::addBot::everyone else",eLogLevel4);
	message.SetType(_MESSAGE_CLIENT_INFO);	//
	message.AddI(playerID);
	message.AddStr(info.name.c_str());
	message.AddStr(info.material.c_str());
	message.AddV(info.pos);
	message.AddV(info.rot);
	message.AddV(info.vec);
	message.AddF(info.lastStamp);
	sendToAllBut(message,playerID);
	message.ClearData();

	// now force it to spawn
	spawnPlayer(playerID);

	message.SetType(_MESSAGE_SPAWN);
	message.AddI(playerID);
	message.AddV(users[playerID].pos);
	message.AddV(users[playerID].rot);
	message.AddV(users[playerID].vec);
	message.AddF(users[playerID].lastStamp);

	// send spawn to everyone 
	logOut("send _MESSAGE_SPAWN","CTestGameServer::addBot::everyone",eLogLevel4);
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
	char temp[512];
	sprintf(temp,"generate gpawn for ID %d",playerID);
	logOut(temp,"CTestGameServer::spawnPlayer",eLogLevel2);

	std::map<int,trPlayerInfo>::iterator itr = users.find(playerID);
	if (itr == users.end())
		return;
	
	// this is crap but will work for now
	float xRange = world.getScaleX()*0.5f;
	float yRange = world.getScaleY()*0.5f;
	float zRange = 25;
	float grav = -10;

	itr->second.pos[0] = (((float)rand()/(float)RAND_MAX)*xRange*2)-xRange;
	itr->second.pos[1] = (((float)rand()/(float)RAND_MAX)*yRange*2)-yRange;
	itr->second.pos[2] = (((float)rand()/(float)RAND_MAX)*zRange);

	itr->second.rot[2] = ((float)rand()/(float)RAND_MAX)*360;

	itr->second.vec[0] = itr->second.vec[1] = 0;
	itr->second.vec[2] = grav;

	itr->second.lastStamp = (float)CTimer::instance().GetTime();
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
		playerInfo->material = "ht8_red";
		playerInfo->mesh = "ht8.mesh";
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

	playerInfo->lastStamp = (float)CTimer::instance().GetTime();
	if (playerInfo->pos[2] < 0 )
		playerInfo->pos[2] = 0;

	float botUpdateTime = 1.0f/0.5f;
	if (CTimer::instance().GetTime() - lastUpdateTime > botUpdateTime )
	{
		lastUpdateTime = (float)CTimer::instance().GetTime();
		return true;
	}
	else
		return false;
}

bool CRobotPlayer::message ( CNetworkMessage &message )
{
	return false;
}

