/* Firestarter
* ui.h : 
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