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
// mainMenu.h


#ifndef _MAIN_MENU_H_
#define _MAIN_MENU_H_

#include "ui.h"


class CMainMenu : public CBaseUIPanel
{
public:
	CMainMenu();
	virtual ~CMainMenu();

	virtual Init ( CBaseGameLoop * pGameLoop );
	virtual void Attach ( void );
	virtual void Release ( void );
	virtual tePanelReturn Process ( std::string &next );

protected:
};

#endif //_MAIN_MENU_H_
