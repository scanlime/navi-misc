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
// ui.h


#ifndef _UI_H_
#define _UI_H_

#include "gameloop.h"
#include <string>
#include <map>
#include <vector>

typedef enum
{
	ePanelContinue = 0,
	ePanelExit,
	ePanelBack,
	ePanelNext,
	ePanelStart
}tePanelReturn;

class CBaseUIPanel
{
public:
	CBaseUIPanel(){gameLoop = NULL;}
	virtual ~CBaseUIPanel(){return;}

	virtual Init ( CBaseGameLoop * pGameLoop ){gameLoop = pGameLoop;}
	virtual void Attach ( void ) = 0;
	virtual void Release ( void ) = 0;
	virtual tePanelReturn Process ( std::string &next ) = 0;

protected:
	CBaseGameLoop * gameLoop;
};

class CUserInterface
{
public:
		CUserInterface();
		CUserInterface ( CBaseGameLoop * pGameLoop );
		~CUserInterface();

		void Set ( CBaseGameLoop * pGameLoop );

		void Init ( void );
		void Attach ( void );
		void Release ( void );

		bool Think ( void );

protected:
	void LoadPanels ( void );

	CBaseGameLoop * gameLoop;
	std::vector<CBaseUIPanel*>	panelStack;

	typedef std::map<std::string, CBaseUIPanel*>	panelmap;
	panelmap	panels;
};

#endif //_UI_H_