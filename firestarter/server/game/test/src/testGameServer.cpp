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

void CTestGameServer::init ( void )
{
}

bool CTestGameServer::think ( void )
{
	return false;
}

bool CTestGameServer::message ( int playerID, CNetworkPeer &peer, CNetworkMessage &message )
{
	std::map<int,trPlayerInfo>::iterator itr = users.find(playerID);
	if (itr == users.end())
		return true;					// we don't know who they are

	// for now just do a simple message relay and add on the user ID
	CNetworkMessage	newMessage;
	newMessage.SetType(message.GetType());	// what ever the message type is
	newMessage.AddI(playerID);

	void* mem = malloc(message.GetSize());
	message.GetDataMem(mem);
	newMessage.AddN(message.GetSize(),mem);

	// send an add to everyone else
	std::map<int,trPlayerInfo>::iterator players = users.begin();

	while (players != users.end())
	{
		if (playerID != players->first)
			message.Send(*players->second.peer,true);
		players++;
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
	std::map<int,trPlayerInfo>::iterator players = users.begin();

	message.SetType("UA");	// UserAdd
	message.AddI(playerID);

	while (players != users.end())
	{
		if (playerID != players->first)
			message.Send(*players->second.peer,true);
		players++;
	}

	return false;
}

bool CTestGameServer::remove ( int playerID, CNetworkPeer &peer )
{
	std::map<int,trPlayerInfo>::iterator itr = users.find(playerID);
	if (itr == users.end())
		return true;

	// send the remove to everyone else
	std::map<int,trPlayerInfo>::iterator players = users.begin();

	CNetworkMessage	message;
	message.SetType("UP");	// UserPart
	message.AddI(playerID);

	while (players != users.end())
	{
		if (playerID != players->first)
			message.Send(*players->second.peer,true);
		players++;
	}
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

	std::map<int,trPlayerInfo>::iterator players = users.begin();

	message.SetType("UP");	// UserPart
	message.AddI(playerID);

	while (players != users.end())
	{
		if (playerID != players->first)
			message.Send(*players->second.peer,true);
		players++;
	}

	users.erase(itr);
		return false;
}

void CTestGameServer::kill ( void )
{
	std::map<int,trPlayerInfo>::iterator itr = users.begin();

	CNetworkMessage	message;
	message.SetType("KK"); 	// KicK

	while (itr != users.end())
	{
		message.Send(*itr->second.peer,true);
		itr++;
	}
	users.clear();
}

