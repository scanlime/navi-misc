/* Firestarter
* input.h : base SDL input class.
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

#ifndef _INPUT_H_
#define _INPUT_H_

#include "Ogre.h"
#include "OgreConfigFile.h"

using namespace Ogre;

enum InputKeyCode
{
	KEY_ESCAPE          =0x01,
	KEY_1               =0x02,
	KEY_2               =0x03,
	KEY_3               =0x04,
	KEY_4               =0x05,
	KEY_5               =0x06,
	KEY_6               =0x07,
	KEY_7               =0x08,
	KEY_8               =0x09,
	KEY_9               =0x0A,
	KEY_0               =0x0B,
	KEY_MINUS           =0x0C,    /* - on main keyboard */
	KEY_EQUALS          =0x0D,
	KEY_BACK            =0x0E,    /* backspace */
	KEY_TAB             =0x0F,
	KEY_Q               =0x10,
	KEY_W               =0x11,
	KEY_E               =0x12,
	KEY_R               =0x13,
	KEY_T               =0x14,
	KEY_Y               =0x15,
	KEY_U               =0x16,
	KEY_I               =0x17,
	KEY_O               =0x18,
	KEY_P               =0x19,
	KEY_LBRACKET        =0x1A,
	KEY_RBRACKET        =0x1B,
	KEY_RETURN          =0x1C,    /* Enter on main keyboard */
	KEY_LCONTROL        =0x1D,
	KEY_A               =0x1E,
	KEY_S               =0x1F,
	KEY_D               =0x20,
	KEY_F               =0x21,
	KEY_G               =0x22,
	KEY_H               =0x23,
	KEY_J               =0x24,
	KEY_K               =0x25,
	KEY_L               =0x26,
	KEY_SEMICOLON       =0x27,
	KEY_APOSTROPHE      =0x28,
	KEY_GRAVE           =0x29,    /* accent grave */
	KEY_LSHIFT          =0x2A,
	KEY_BACKSLASH       =0x2B,
	KEY_Z               =0x2C,
	KEY_X               =0x2D,
	KEY_C               =0x2E,
	KEY_V               =0x2F,
	KEY_B               =0x30,
	KEY_N               =0x31,
	KEY_M               =0x32,
	KEY_COMMA           =0x33,
	KEY_PERIOD          =0x34,    /* . on main keyboard */
	KEY_SLASH           =0x35,    /* '/' on main keyboard */
	KEY_RSHIFT          =0x36,
	KEY_MULTIPLY        =0x37,    /* * on numeric keypad */
	KEY_LMENU           =0x38,    /* left Alt */
	KEY_SPACE           =0x39,
	KEY_CAPITAL         =0x3A,
	KEY_F1              =0x3B,
	KEY_F2              =0x3C,
	KEY_F3              =0x3D,
	KEY_F4              =0x3E,
	KEY_F5              =0x3F,
	KEY_F6              =0x40,
	KEY_F7              =0x41,
	KEY_F8              =0x42,
	KEY_F9              =0x43,
	KEY_F10             =0x44,
	KEY_NUMLOCK         =0x45,
	KEY_SCROLL          =0x46,    /* Scroll Lock */
	KEY_NUMPAD7         =0x47,
	KEY_NUMPAD8         =0x48,
	KEY_NUMPAD9         =0x49,
	KEY_SUBTRACT        =0x4A,    /* - on numeric keypad */
	KEY_NUMPAD4         =0x4B,
	KEY_NUMPAD5         =0x4C,
	KEY_NUMPAD6         =0x4D,
	KEY_ADD             =0x4E,    /* + on numeric keypad */
	KEY_NUMPAD1         =0x4F,
	KEY_NUMPAD2         =0x50,
	KEY_NUMPAD3         =0x51,
	KEY_NUMPAD0         =0x52,
	KEY_DECIMAL         =0x53,    /* . on numeric keypad */
	KEY_OEM_102         =0x56,    /* < > | on UK/Germany keyboards */
	KEY_F11             =0x57,
	KEY_F12             =0x58,
	KEY_F13             =0x64,    /*                     (NEC PC98) */
	KEY_F14             =0x65,    /*                     (NEC PC98) */
	KEY_F15             =0x66,    /*                     (NEC PC98) */
	KEY_KANA            =0x70,    /* (Japanese keyboard)            */
	KEY_ABNT_C1         =0x73,    /* / ? on Portugese (Brazilian) keyboards */
	KEY_CONVERT         =0x79,    /* (Japanese keyboard)            */
	KEY_NOCONVERT       =0x7B,    /* (Japanese keyboard)            */
	KEY_YEN             =0x7D,    /* (Japanese keyboard)            */
	KEY_ABNT_C2         =0x7E,    /* Numpad . on Portugese (Brazilian) keyboards */
	KEY_NUMPADEQUALS    =0x8D,    /* = on numeric keypad (NEC PC98) */
	KEY_PREVTRACK       =0x90,    /* Previous Track (KEY_CIRCUMFLEX on Japanese keyboard) */
	KEY_AT              =0x91,    /*                     (NEC PC98) */
	KEY_COLON           =0x92,    /*                     (NEC PC98) */
	KEY_UNDERLINE       =0x93,    /*                     (NEC PC98) */
	KEY_KANJI           =0x94,    /* (Japanese keyboard)            */
	KEY_STOP            =0x95,    /*                     (NEC PC98) */
	KEY_AX              =0x96,    /*                     (Japan AX) */
	KEY_UNLABELED       =0x97,    /*                        (J3100) */
	KEY_NEXTTRACK       =0x99,    /* Next Track */
	KEY_NUMPADENTER     =0x9C,    /* Enter on numeric keypad */
	KEY_RCONTROL        =0x9D,
	KEY_MUTE            =0xA0,    /* Mute */
	KEY_CALCULATOR      =0xA1,    /* Calculator */
	KEY_PLAYPAUSE       =0xA2,    /* Play / Pause */
	KEY_MEDIASTOP       =0xA4,    /* Media Stop */
	KEY_VOLUMEDOWN      =0xAE,    /* Volume - */
	KEY_VOLUMEUP        =0xB0,    /* Volume + */
	KEY_WEBHOME         =0xB2,    /* Web home */
	KEY_NUMPADCOMMA     =0xB3,    /* , on numeric keypad (NEC PC98) */
	KEY_DIVIDE          =0xB5,    /* / on numeric keypad */
	KEY_SYSRQ           =0xB7,
	KEY_RMENU           =0xB8,    /* right Alt */
	KEY_PAUSE           =0xC5,    /* Pause */
	KEY_HOME            =0xC7,    /* Home on arrow keypad */
	KEY_UP              =0xC8,    /* UpArrow on arrow keypad */
	KEY_PGUP            =0xC9,    /* PgUp on arrow keypad */
	KEY_LEFT            =0xCB,    /* LeftArrow on arrow keypad */
	KEY_RIGHT           =0xCD,    /* RightArrow on arrow keypad */
	KEY_END             =0xCF,    /* End on arrow keypad */
	KEY_DOWN            =0xD0,    /* DownArrow on arrow keypad */
	KEY_PGDOWN          =0xD1,    /* PgDn on arrow keypad */
	KEY_INSERT          =0xD2,    /* Insert on arrow keypad */
	KEY_DELETE          =0xD3,    /* Delete on arrow keypad */
	KEY_LWIN            =0xDB,    /* Left Windows key */
	KEY_RWIN            =0xDC,    /* Right Windows key */
	KEY_APPS            =0xDD,    /* AppMenu key */
	KEY_POWER           =0xDE,    /* System Power */
	KEY_SLEEP           =0xDF,    /* System Sleep */
	KEY_WAKE            =0xE3,    /* System Wake */
	KEY_WEBSEARCH       =0xE5,    /* Web Search */
	KEY_WEBFAVORITES    =0xE6,    /* Web Favorites */
	KEY_WEBREFRESH      =0xE7,    /* Web Refresh */
	KEY_WEBSTOP         =0xE8,    /* Web Stop */
	KEY_WEBFORWARD      =0xE9,    /* Web Forward */
	KEY_WEBBACK         =0xEA,    /* Web Back */
	KEY_MYCOMPUTER      =0xEB,    /* My Computer */
	KEY_MAIL            =0xEC,    /* Mail */
	KEY_MEDIASELECT     =0xED     /* Media Select */
};

class CAction;
class CInputManager;

class CActionListener
{
public:
	virtual ~CActionListener() {return;}
	virtual void eventCall ( CAction *caller, bool newActionPointer ) = 0;
};

typedef std::vector<CActionListener*> tvActionListenerList;

class CAction
{
public:
	CAction();
	CAction( const CAction &r);

	virtual ~CAction();

	virtual float getRelative(void);
	virtual float getAbs(void);
	virtual float getRaw(void);

	virtual float getMinRange ( void );
	virtual float getMaxRange ( void );

	virtual float getMaxRaw ( void );
	virtual float getMinRaw ( void );

	// listener functions
	void addListener ( CActionListener* listener );
	void removeListener ( CActionListener *listener );

	void clearAllListeners ( void );

	// muters
	void setMute ( bool value ) { mute = value;}
	bool isMute ( void ) { return mute;}

	// groups
	void setGroup ( const char* theGroup );
	const char* getGroup ( void ) {return group.c_str();}

	// name
	void setName ( const char* theName );
	const char* getName( void ) {return name.c_str();}

	friend CInputManager;
protected:
	bool									mute;
	tvActionListenerList	listeners;
	std::string						group;
	std::string						name;
	CInputManager					*manager;
};

typedef std::map<std::string, CAction*> tmActionMap;
typedef std::vector<CAction*> tvActionList;
typedef std::map<std::string, tvActionList> tmActionListMap;

class CInputManager
{
	public:
		CInputManager();
		~CInputManager();

		// old API
		void Init ( RenderWindow *theWindow );
		void Process ( void );
		bool KeyDown ( InputKeyCode key );

		// new API
		// action API

		void init ( RenderWindow *theWindow );
		void update ( void );
		
		// action binding/retreval
		CAction* getAction ( const char* name );

		CAction* bindAction ( const char* name, const char* device, const char* maxItem, const char* minItem = NULL );
		CAction* bindAction ( CAction* action, const char* device, const char* maxItem, const char* minItem = NULL );
		CAction* bindAction ( const char* name, const char* device, int maxItem, int minItem = -1 );
		CAction* bindAction ( CAction* action, const char* device, int maxItem, int minItem = -1 );

		CAction* unbindAction ( const char* name );
		CAction* unbindAction ( CAction* action );

		CAction* renameAction ( const char* oldName, const char* newName );
		CAction* renameAction ( CAction* oldAction, const char* newName );

		// groups
		void setActionGroup ( const char* name, const char* group );
		void setActionGroup ( CAction* action, const char* group );

		void muteGroup ( bool mute, const char* group );

		// enumeration
		// devices
		int getDeviceCount ( void );
		const char* getDeviceName ( int device );
		int getDeviceItemCount ( int device );
		const char* getDeviceItemName ( int device, int item );

		// actions
		int getActionCount ( void );
		const char* getActionName ( int action );

		bool configDevice ( int device );
		bool configDevice ( const char* device );

		const char* getCurentDeviceItem ( int device );
		const char* getCurentDeviceItem ( const char* device );

protected:
	// ogre input stuff
	EventProcessor* mEventProcessor;
	InputReader* mInputDevice;

	// actions
	tvActionList			actions;
	tmActionMap				actionNames;
	tmActionListMap		actionGroups;

	// the pimple
	struct trInfo;
	trInfo	*info;
};

#endif //_INPUT_H_
