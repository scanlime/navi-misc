/* Firestarter
* mainMenu.h : 
*
* Copyright (C) 2004 Jeffrey Myers
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
* email: jeffm2501@sbcglobal.net
*/


#ifndef _MAIN_MENU_H_
#define _MAIN_MENU_H_

#include "ui.h"
#include "gameloop.h"

class CMainMenu : public CBaseUIPanel
{
public:
	CMainMenu();
	virtual ~CMainMenu();

	virtual void Init ( void  );
	virtual void Attach ( void );
	virtual void Release ( void );
	virtual tePanelReturn Process ( std::string &next );

protected:
	SceneNode		*ships[4];
	Overlay*		mainMenu;
	SceneNode		*mGroundNode;

	int					currentItem;
};

#endif //_MAIN_MENU_H_
