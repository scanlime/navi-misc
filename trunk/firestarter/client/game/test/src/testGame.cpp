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
#include "testGame.h"
#include "input.h"
#include "firestarter.h"
#include "prefs.h"
#include "timer.h"
#include "input.h"

// the drawables for this game
#include "worldDrawables.h"
#include "playerDrawables.h"

// messages
#define	_MESSAGE_SERVER_INFO 0x5349	// SI 
#define	_MESSAGE_CLIENT_INFO 0x4349	// CI 
#define	_MESSAGE_USER_PART 0x5550		// UP
#define	_MESSAGE_USER_ADD 0x5541		// UA
#define	_MESSAGE_KICK 0x4b4b				// KK
#define _MESSAGE_UPDATE 0x5544			//UD
#define _MESSAGE_ACKNOWLEDGE 0x414b	//AK
#define _MESSAGE_SPAWN 0x5350				//SP

void CTestGame::registerFactory (const char* name, CBaseDrawableFactory* factory)
{
	CDrawManager::instance().Register(factory,name);
	factoryList.push_back(factory);
}

void CTestGame::Init ( void )
{
	InitNetwork();
	// do one time init stuff here
	registerFactory("sky",new CSkyboxObjectFactory);
	registerFactory("ground",new CGroundObjectFactory);
	registerFactory("playerTank",new CPlayerObjectFactory);
	registerFactory("camera",new CCameraObjectFactory);
}

void CTestGame::Kill ( void )
{
	FreeNetwork();
	std::vector<CBaseDrawableFactory*>::iterator	itr = factoryList.begin();
	while (itr != factoryList.end())
		delete(*(itr++));
}

void CTestGame::Attach ( void )
{
	CPrefsManager &prefs = CPrefsManager::instance();
	CFirestarterLoop &loop = CFirestarterLoop::instance();

	players.clear();
	localPlayer = NULL;
	// get the connection info

	// log into that sucker
	network.SetMesageHandaler(this);

	std::string url = loop.GetGameStartString();
	
	char temp2[512];
	strcpy(temp2,url.c_str());
	char *portChr = strrchr(temp2,':');
	int port = 2501;
	if (portChr)
	{
		portChr++;
		port = atoi(portChr);
		*(portChr-1)= '\0';
	}
	network.Connect(temp2,port);

	// build up some simple world
	trWorldInfo	info;

	info.ambientColor[0] = info.ambientColor[1] = info.ambientColor[2] = 0.5f;
	info.sunPos[0] = info.sunPos[1] = info.sunPos[2] = 100;
	info.sunColor[0] = info.sunColor[1] = info.sunColor[2] = 0.75f;
	info.groundSize = 800;
	info.groundTextureRepeat = 60;

	info.groundTexture = "ground_mat";
	info.skybox = "malrav1sky";

	world.Load(info,true);

	// build up the local player
	localPlayer = new CPlayerObject;
	localPlayer->active = false;
	localPlayer->idNumber = -1;
	localPlayer->name = prefs.GetItemS("PlayerName");
	if (prefs.ItemExists("PlayerSkin"))
		localPlayer->material = prefs.GetItemS("PlayerSkin");
}

void CTestGame::Release ( void )
{
}

bool CTestGame::Think ( void )
{
	// get some network data
	if (network.Connected())
		network.ProcessMessages();

	if (CInputManager::instance().KeyDown(KEY_ESCAPE))
		return true;

	// do some game like things here, logic and stuff like that
	
	if (processPlayerInput())
		return true;

	// let the drawables update themseves ( if they exist )
	CDrawManager::instance().ThinkAll();

	return false;
}

void CTestGame::OnConnect ( CNetworkPeer &peer )
{
	CNetworkMessage message;
	message.SetType(_MESSAGE_ACKNOWLEDGE);
	message.Send(peer,true);
}

void CTestGame::OnDisconnect ( CNetworkPeer &peer )
{
}	

void CTestGame::OnMessage ( CNetworkPeer &peer, CNetworkMessage &message )
{
	CNetworkMessage outMessage;
	CPrefsManager &prefs = CPrefsManager::instance();

	switch(message.GetType())
	{
		case _MESSAGE_SERVER_INFO:	// the server is telling us our ID and wants us to send back our info
			// get our ID
			localPlayer->idNumber = outMessage.ReadI();
			players[localPlayer->idNumber] = localPlayer;
			localPlayer->active = false;

			// send back a client info
			outMessage.SetType("CI");
			outMessage.AddStr(localPlayer->name.c_str());
			outMessage.AddStr(localPlayer->material.c_str());
			outMessage.Send(peer,true);
			break;

		case _MESSAGE_USER_ADD: // here comes a new chalenger
			{			
				int newPlayerID = message.ReadI();
				CPlayerObject	*newPlayer = new CPlayerObject();
				newPlayer->active = false;
				players[newPlayerID] = newPlayer;
			}
			break;

		case _MESSAGE_USER_PART:	// somone left
			{
				int playerID = message.ReadI();
				if (playerID != localPlayer->idNumber)	// umm we should never get a part for us
				{
					tmPlayerMap::iterator itr = players.find(playerID);
					if (itr != players.end())
					{
						itr->second->Kill();
						delete(itr->second);
						players.erase(itr);
					}
				}
			}
			break;

		case _MESSAGE_CLIENT_INFO:	// we got some info for a new players
			{
				CPlayerObject	*newPlayer = players[message.ReadI()];
				if (newPlayer)
				{
					newPlayer->name = message.ReadStr();
					newPlayer->material = message.ReadStr();
				}
			}
			break;

		case _MESSAGE_SPAWN:	// somone spawned, it may have been us
			{
				int playerID = message.ReadI();
				CPlayerObject	*newPlayer = localPlayer;
				if (playerID != localPlayer->idNumber)
					newPlayer = players[playerID];

				if (!newPlayer->active)
				{
					newPlayer->Init(true);
					message.ReadV(newPlayer->pos);
					message.ReadV(newPlayer->rot);
					message.ReadV(newPlayer->vec);
					newPlayer->active = true;
				}
			}
			break;

		case _MESSAGE_UPDATE:// yes we even accept updates to oursleves
			{	
				int playerID = message.ReadI();
				CPlayerObject	*newPlayer = localPlayer;
				if (playerID != localPlayer->idNumber)
					newPlayer = players[playerID];

				newPlayer->updateTime = CTimer::instance().GetTime();

				if (newPlayer->active)
				{
					message.ReadV(newPlayer->pos);
					message.ReadV(newPlayer->rot);
					message.ReadV(newPlayer->vec);
				}
			}
			break;

		case _MESSAGE_KICK: // um we got kicked for some reason

			break;

		default:	// hell if I know what this is, but say OK
			outMessage.SetType("AK");
			outMessage.Send(peer,false);
	}
}

bool CTestGame::GetPos ( float *pos )
{
	if (!localPlayer)
		return false;

	memcpy(pos,localPlayer->pos,sizeof(float)*3);
	return true;
}

bool CTestGame::GetRot ( float *rot )
{	
	if (!localPlayer)
		return false;

	memcpy(rot,localPlayer->rot,sizeof(float)*3);
		return true;
}

bool CTestGame::processPlayerInput ( void )
{
	CInputManager	&input = CInputManager::instance();
	CTimer	&timer = CTimer::instance();

	if (!localPlayer)
		return false;

	float rotspeed = 60.0f;
	float linspeed = 100.0f;
	float reversemod = 0.5f;
	float	friction = 0.95f;
	float stoptol = 1.0f;
	float grav = -10.0f;

	// compute a new rotation
	if (input.KeyDown(KEY_LEFT))
	{
		localPlayer->rot[2] += rotspeed*timer.GetFrameTime();
	}
	else if (input.KeyDown(KEY_RIGHT))
	{
		localPlayer->rot[2] -= rotspeed*timer.GetFrameTime();
	}

	float h,v;
	float deg2rad = 0.017453292519943295769236907684886f;

	h = sin(localPlayer->rot[2]*deg2rad);
	v = cos(localPlayer->rot[2]*deg2rad);

	// compute new thrust vector
	if (input.KeyDown(KEY_UP))
	{
	//	float 
	}
	else if (input.KeyDown(KEY_UP))
	{

	}
	else
	{

	}

	// apply gravity if above 0;
}

