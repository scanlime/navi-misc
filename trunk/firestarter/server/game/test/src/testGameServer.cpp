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

// messages
#define	_MESSAGE_SERVER_INFO 0x5349	// SI 
#define	_MESSAGE_CLIENT_INFO 0x4349	// CI 
#define	_MESSAGE_USER_PART 0x5550		// UP
#define	_MESSAGE_USER_ADD 0x5541		// UA
#define	_MESSAGE_KICK 0x4b4b				// KK
#define _MESSAGE_UPDATE 0x5544			//UD

void CTestGameServer::init ( void )
{
}

bool CTestGameServer::think ( void )
{
	return false;
}

void CTestGameServer::sendToAllBut ( CNetworkMessage &message, int player )
{
	std::map<int,trPlayerInfo>::iterator players = users.begin();

	while (players != users.end())
	{
		if (player != players->first)
			message.Send(*players->second.peer,true);
		players++;
	}
}

bool CTestGameServer::message ( int playerID, CNetworkPeer &peer, CNetworkMessage &message )
{
	std::map<int,trPlayerInfo>::iterator itr = users.find(playerID);
	if (itr == users.end())
		return true;					// we don't know who they are

	// for now just do a simple message relay and add on the user ID

	bool bRellay = false;

	switch (message.GetType())
	{
		case _MESSAGE_SERVER_INFO:	// SI 
			break;

		case _MESSAGE_CLIENT_INFO:	// CI 
			bRellay = true;
			itr->second.player = true;
			itr->second.name = message.ReadStr();
			break;

		case _MESSAGE_USER_PART:		// UP
			remove(playerID,peer);
			break;

		// this we should never get
		case _MESSAGE_USER_ADD:	// UA
		case _MESSAGE_KICK:			// KK
			break;

		case _MESSAGE_UPDATE: //UD
			if (itr->second.player)
			{
				bRellay = true;
				message.ReadV(itr->second.pos);
				message.ReadV(itr->second.rot);
				message.ReadV(itr->second.vec);
			}
			break;

		default:
			bRellay = true;

			break;
	}

	if (bRellay)
	{	
		CNetworkMessage	newMessage;
		newMessage.SetType(message.GetType());	// what ever the message type is
		newMessage.AddI(playerID);

		void* mem = malloc(message.GetSize());
		message.GetDataMem(mem);
		newMessage.AddN(message.GetSize(),mem);

		// send an add to everyone else
		sendToAllBut(newMessage,playerID);
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

	users[playerID] = info;

	CNetworkMessage	message;

	message.SetType("SI");	// ServerInfo
	message.Send(peer,true);

	// send an add to everyone else
	message.SetType("UA");	// UserAdd
	message.AddI(playerID);
	sendToAllBut(message,playerID);
	return false;
}

bool CTestGameServer::remove ( int playerID, CNetworkPeer &peer )
{
	std::map<int,trPlayerInfo>::iterator itr = users.find(playerID);
	if (itr == users.end())
		return true;

	// send the remove to everyone else
	CNetworkMessage	message;
	message.SetType("UP");	// UserPart
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

	message.SetType("KK");	// KicK
	message.Send(peer,true);

	// send the remove to everyone else
	message.SetType("UP");	// UserPart
	message.AddI(playerID);
	sendToAllBut(message,playerID);

	users.erase(itr);
		return false;
}

void CTestGameServer::kill ( void )
{
	CNetworkMessage	message;
	message.SetType("KK"); 	// KicK
	sendToAllBut(message,-1);
	users.clear();
}

