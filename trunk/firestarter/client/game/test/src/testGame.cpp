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

// the drawables for this game
#include "worldDrawables.h"

// messages
#define	_MESSAGE_SERVER_INFO 0x5349	// SI 
#define	_MESSAGE_CLIENT_INFO 0x4349	// CI 
#define	_MESSAGE_USER_PART 0x5550		// UP
#define	_MESSAGE_USER_ADD 0x5541		// UA
#define	_MESSAGE_KICK 0x4b4b				// KK
#define _MESSAGE_UPDATE 0x5544			//UD
#define _MESSAGE_ACKNOWLEDGE 0x414b	//AK

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
	// get the connection info
	CFirestarterLoop &loop = CFirestarterLoop::instance();

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
}

void CTestGame::Release ( void )
{
}

bool CTestGame::Think ( void )
{
	//if (network.Connected())
		network.ProcessMessages();

	if (CInputManager::instance().KeyDown(KEY_ESCAPE))
		return true;
	// do some game like things
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
		case _MESSAGE_SERVER_INFO:
			outMessage.SetType("CI");
			outMessage.AddStr(prefs.GetItemS("PlayerName"));
			outMessage.Send(peer,true);
			break;

		default:
			outMessage.SetType("AK");
			outMessage.Send(peer,true);
	}
}
