// Input.cpp : Defines the entry point for the DLL application.
//

#ifdef _WIN32
	#pragma warning( disable : 4786 )  // Disable warning message
#else
	#define stricmp strcasecmp
#endif

#include "input.h"
#include "events.h"
#include <SDL/SDL.h>
#include "osfile.h"
#include <string.h>
#include <string>
#include <map>
#include <math.h>

#define _STICK_CONSTANT 32767.0f;

#define SDL_MWHEEL_UPMASK	SDL_BUTTON(SDL_BUTTON_WHEELUP)
#define SDL_MWHEEL_DNMASK	SDL_BUTTON(SDL_BUTTON_WHEELDOWN)

inline  bool operator < (const std::string &s1,const std::string &s2) { return (s1.compare(s2)<0);}

void GetKeyName ( char *szData )
{
#ifdef	_WIN32
	strupr(szData);
#else
	while(*szData++)
		*szData = toupper(*szData);
#endif
}

KEYTypes GetCurrentKey ( void )
{
	return KEY_UNKNOWN;
}

int		iStickInitCount = 0;

void InitStickSystem ( void )
{
	if (!SDL_WasInit(SDL_INIT_JOYSTICK))
		SDL_InitSubSystem(SDL_INIT_JOYSTICK);

	iStickInitCount++;
}

void ReleaseStickSystem ( void )
{
	iStickInitCount--;

	if (SDL_WasInit(SDL_INIT_JOYSTICK) && (iStickInitCount<1) )
		SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
}

// keyname to key bind maping for input handaler.

typedef std::map<std::string,KEYTypes> tmKeyNameMap;
typedef std::map<KEYTypes,std::string> tmKeyIDMap;

tmKeyNameMap	keyNameMap;
tmKeyIDMap		keyStringMap;

KEYTypes getKeyID ( const char* szData )
{
	if (!szData)
		return KEY_UNKNOWN;
	std::string str = szData;
	GetKeyName((char*)str.c_str());

	tmKeyNameMap::iterator itr = keyNameMap.find(str);
	
	if(itr == keyNameMap.end())
		return KEY_UNKNOWN;

	return itr->second;
}

void initKeyChars ( void )
{
	if (keyStringMap.size()>0)
		return;
	
	keyStringMap[KEY_TAB]= "\t";			
	keyStringMap[KEY_RETURN]= "\n";
	keyStringMap[KEY_SPACE]= " ";
	keyStringMap[KEY_EXCLAIM]= "!";
	keyStringMap[KEY_QUOTEDBL]= "\"";
	keyStringMap[KEY_HASH]= "#";
	keyStringMap[KEY_DOLLAR]= "$";
	keyStringMap[KEY_AMPERSAND]= "&";
	keyStringMap[KEY_QUOTE]= "'";
	keyStringMap[KEY_LEFTPAREN]= "(";
	keyStringMap[KEY_RIGHTPAREN]= ")";
	keyStringMap[KEY_ASTERISK]= "*";
	keyStringMap[KEY_PLUS]= "+";
	keyStringMap[KEY_COMMA]= ",";
	keyStringMap[KEY_MINUS]= "-";
	keyStringMap[KEY_PERIOD]= ".";
	keyStringMap[KEY_SLASH]= "/";
	keyStringMap[KEY_0]= "0";
	keyStringMap[KEY_1]= "1";
	keyStringMap[KEY_2]= "2";
	keyStringMap[KEY_3]= "3";
	keyStringMap[KEY_4]= "4";
	keyStringMap[KEY_5]= "5";
	keyStringMap[KEY_6]= "6";
	keyStringMap[KEY_7]= "7";
	keyStringMap[KEY_8]= "8";
	keyStringMap[KEY_9]= "9";
	keyStringMap[KEY_KP0]= "0";
	keyStringMap[KEY_KP1]= "1";
	keyStringMap[KEY_KP2]= "2";
	keyStringMap[KEY_KP3]= "3";
	keyStringMap[KEY_KP4]= "4";
	keyStringMap[KEY_KP5]= "5";
	keyStringMap[KEY_KP6]= "6";
	keyStringMap[KEY_KP7]= "7";
	keyStringMap[KEY_KP8]= "8";
	keyStringMap[KEY_KP9]= "9";
	keyStringMap[KEY_COLON]= "]";
	keyStringMap[KEY_SEMICOLON]= ";";
	keyStringMap[KEY_LESS]= "<";
	keyStringMap[KEY_EQUALS]= "=";
	keyStringMap[KEY_GREATER]= ">";
	keyStringMap[KEY_QUESTION]= "?";
	keyStringMap[KEY_AT]= "@";
	keyStringMap[KEY_LEFTBRACKET]= "[";
	keyStringMap[KEY_BACKSLASH]= "\\";
	keyStringMap[KEY_RIGHTBRACKET]= "]";
	keyStringMap[KEY_CARET]= "^";
	keyStringMap[KEY_UNDERSCORE]= "_";
	keyStringMap[KEY_BACKQUOTE]= "`";
	keyStringMap[KEY_KP_PERIOD]= ".";
	keyStringMap[KEY_KP_DIVIDE]= "/";
	keyStringMap[KEY_KP_MULTIPLY]= "*";
	keyStringMap[KEY_KP_MINUS]= "-";
	keyStringMap[KEY_KP_PLUS]= "+";
	keyStringMap[KEY_KP_ENTER]= "\n";
	keyStringMap[KEY_KP_EQUALS]= "=";
        keyStringMap[KEY_ESCAPE]= "ESCAPE";
}

void initKeys ( void )
{
	initKeyChars();
	if (keyNameMap.size()>0)
		return;

	keyNameMap["UNKNOWN"] = KEY_UNKNOWN;
	keyNameMap["BACKSPACE"] = KEY_BACKSPACE;
	keyNameMap["TAB"] = KEY_TAB;
	keyNameMap["CLEAR"] = KEY_CLEAR;
	keyNameMap["RETURN"] = KEY_RETURN;
	keyNameMap["PAUSE"] = KEY_PAUSE;
	keyNameMap["ESCAPE"] = KEY_ESCAPE;
	keyNameMap["SPACE"] = KEY_SPACE;
	keyNameMap["EXCLAIM"] = KEY_EXCLAIM;
	keyNameMap["!"] = KEY_EXCLAIM;
	keyNameMap["QUOTEDBL"] = KEY_QUOTEDBL;
	keyNameMap["\""] = KEY_QUOTEDBL;
	keyNameMap["HASH"] = KEY_HASH;
	keyNameMap["#"] = KEY_HASH;
	keyNameMap["DOLLAR"] = KEY_DOLLAR;
	keyNameMap["$"] = KEY_DOLLAR;
	keyNameMap["AMPERSAND"] = KEY_AMPERSAND;
	keyNameMap["&"] = KEY_AMPERSAND;
	keyNameMap["QUOTE"] = KEY_QUOTE;
	keyNameMap["'"] = KEY_QUOTE;
	keyNameMap["LEFTPAREN"] = KEY_LEFTPAREN;
	keyNameMap["("] = KEY_LEFTPAREN;
	keyNameMap["RIGHTPAREN"] = KEY_RIGHTPAREN;
	keyNameMap[")"] = KEY_RIGHTPAREN;
	keyNameMap["ASTERISK"] = KEY_ASTERISK;
	keyNameMap["*"] = KEY_ASTERISK;
	keyNameMap["PLUS"] = KEY_PLUS;
	keyNameMap["+"] = KEY_PLUS;
	keyNameMap["COMMA"] = KEY_COMMA;
	keyNameMap[","] = KEY_COMMA;
	keyNameMap["MINUS"] = KEY_MINUS;
	keyNameMap["-"] = KEY_MINUS;
	keyNameMap["PERIOD"] = KEY_PERIOD;
	keyNameMap["."] = KEY_PERIOD;
	keyNameMap["SLASH"] = KEY_SLASH;
	keyNameMap["/"] = KEY_SLASH;
	keyNameMap["COLON"] = KEY_COLON;
	keyNameMap[":"] = KEY_COLON;
	keyNameMap["SEMICOLON"] = KEY_SEMICOLON;
	keyNameMap[";"] = KEY_SEMICOLON;
	keyNameMap["LESS"] = KEY_LESS;
	keyNameMap["<"] = KEY_LESS;
	keyNameMap["EQUALS"] = KEY_EQUALS;
	keyNameMap["="] = KEY_EQUALS;
	keyNameMap["GREATER	"] = KEY_GREATER;
	keyNameMap[">"] = KEY_GREATER;
	keyNameMap["QUESTION"] = KEY_QUESTION;
	keyNameMap["?"] = KEY_QUESTION;
	keyNameMap["AT"] = KEY_AT;
	keyNameMap["@"] = KEY_AT;
	keyNameMap["LEFTBRACKET"] = KEY_LEFTBRACKET;
	keyNameMap["["] = KEY_LEFTBRACKET;
	keyNameMap["BACKSLASH"] = KEY_BACKSLASH;
	keyNameMap["\\"] = KEY_BACKSLASH;
	keyNameMap["RIGHTBRACKET"] = KEY_RIGHTBRACKET;
	keyNameMap["]"] = KEY_RIGHTBRACKET;
	keyNameMap["CARET"] = KEY_CARET;
	keyNameMap["^"] = KEY_CARET;
	keyNameMap["UNDERSCORE"] = KEY_UNDERSCORE;
	keyNameMap["_"] = KEY_UNDERSCORE;
	keyNameMap["BACKQUOTE"] = KEY_BACKQUOTE;
	keyNameMap["BACKTICK"] = KEY_BACKQUOTE;
	keyNameMap["`"] = KEY_BACKQUOTE;
	keyNameMap["DELETE"] = KEY_DELETE;
	// numbers
	keyNameMap["0"] = KEY_0;
	keyNameMap["1"] = KEY_1;
	keyNameMap["2"] = KEY_2;
	keyNameMap["3"] = KEY_3;
	keyNameMap["4"] = KEY_4;
	keyNameMap["5"] = KEY_5;
	keyNameMap["6"] = KEY_6;
	keyNameMap["7"] = KEY_7;
	keyNameMap["8"] = KEY_8;
	keyNameMap["9"] = KEY_9;

	// leters
	keyNameMap["A"] = KEY_a;
	keyNameMap["B"] = KEY_b;
	keyNameMap["C"] = KEY_c;
	keyNameMap["D"] = KEY_d;
	keyNameMap["E"] = KEY_e;
	keyNameMap["F"] = KEY_f;
	keyNameMap["G"] = KEY_g;
	keyNameMap["H"] = KEY_h;
	keyNameMap["I"] = KEY_i;
	keyNameMap["J"] = KEY_j;
	keyNameMap["K"] = KEY_k;
	keyNameMap["L"] = KEY_l;
	keyNameMap["M"] = KEY_m;
	keyNameMap["N"] = KEY_n;
	keyNameMap["O"] = KEY_o;
	keyNameMap["P"] = KEY_p;
	keyNameMap["Q"] = KEY_q;
	keyNameMap["R"] = KEY_r;
	keyNameMap["S"] = KEY_s;
	keyNameMap["T"] = KEY_t;
	keyNameMap["U"] = KEY_u;
	keyNameMap["V"] = KEY_v;
	keyNameMap["W"] = KEY_w;
	keyNameMap["X"] = KEY_x;
	keyNameMap["Y"] = KEY_y;
	keyNameMap["Z"] = KEY_z;
	/* Numeric keypad */
	keyNameMap["KP0"] = KEY_KP0;
	keyNameMap["KP1"] = KEY_KP1;
	keyNameMap["KP2"] = KEY_KP2;
	keyNameMap["KP3"] = KEY_KP3;
	keyNameMap["KP4"] = KEY_KP4;
	keyNameMap["KP5"] = KEY_KP5;
	keyNameMap["KP6"] = KEY_KP6;
	keyNameMap["KP7"] = KEY_KP7;
	keyNameMap["KP8"] = KEY_KP8;
	keyNameMap["KP9"] = KEY_KP9;
	keyNameMap["KEYPAD0"] = KEY_KP0;
	keyNameMap["KEYPAD1"] = KEY_KP1;
	keyNameMap["KEYPAD2"] = KEY_KP2;
	keyNameMap["KEYPAD3"] = KEY_KP3;
	keyNameMap["KEYPAD4"] = KEY_KP4;
	keyNameMap["KEYPAD5"] = KEY_KP5;
	keyNameMap["KEYPAD6"] = KEY_KP6;
	keyNameMap["KEYPAD7"] = KEY_KP7;
	keyNameMap["KEYPAD8"] = KEY_KP8;
	keyNameMap["KEYPAD9"] = KEY_KP9;
	keyNameMap["KEYPAD 0"] = KEY_KP0;
	keyNameMap["KEYPAD 1"] = KEY_KP1;
	keyNameMap["KEYPAD 2"] = KEY_KP2;
	keyNameMap["KEYPAD 3"] = KEY_KP3;
	keyNameMap["KEYPAD 4"] = KEY_KP4;
	keyNameMap["KEYPAD 5"] = KEY_KP5;
	keyNameMap["KEYPAD 6"] = KEY_KP6;
	keyNameMap["KEYPAD 7"] = KEY_KP7;
	keyNameMap["KEYPAD 8"] = KEY_KP8;
	keyNameMap["KEYPAD 9"] = KEY_KP9;
	keyNameMap["KP_PERIOD"] = KEY_KP_PERIOD;
	keyNameMap["KP_DIVIDE"] = KEY_KP_DIVIDE;
	keyNameMap["KP_MULTIPLY"] = KEY_KP_MULTIPLY;
	keyNameMap["KP_MINUS"] = KEY_KP_MINUS;
	keyNameMap["KP_PLUS"] = KEY_KP_PLUS;
	keyNameMap["KP_ENTER"] = KEY_KP_ENTER;
	keyNameMap["KP_EQUALS"] = KEY_KP_EQUALS;
	keyNameMap["KEYPAD PERIOD"] = KEY_KP_PERIOD;
	keyNameMap["KEYPAD ."] = KEY_KP_PERIOD;
	keyNameMap["KEYPAD DIVIDE"] = KEY_KP_DIVIDE;
	keyNameMap["KEYPAD /"] = KEY_KP_DIVIDE;
	keyNameMap["KEYPAD MULTIPLY"] = KEY_KP_MULTIPLY;
	keyNameMap["KEYPAD *"] = KEY_KP_MULTIPLY;
	keyNameMap["KEYPAD MINUS"] = KEY_KP_MINUS;
	keyNameMap["KEYPAD -"] = KEY_KP_MINUS;
	keyNameMap["KEYPAD PLUS"] = KEY_KP_PLUS;
	keyNameMap["KEYPAD +"] = KEY_KP_PLUS;
	keyNameMap["KEYPAD ENTER"] = KEY_KP_ENTER;
	keyNameMap["KEYPAD EQUALS"] = KEY_KP_EQUALS;
	keyNameMap["KEYPAD ="] = KEY_KP_EQUALS;

	/* Arrows + Home/End pad */
	keyNameMap["UP"] = KEY_UP;
	keyNameMap["DOWN"] = KEY_DOWN;
	keyNameMap["RIGHT"] = KEY_RIGHT;
	keyNameMap["LEFT"] = KEY_LEFT;
	keyNameMap["INSERT"] = KEY_INSERT;
	keyNameMap["HOME"] = KEY_HOME;
	keyNameMap["END"] = KEY_END;
	keyNameMap["PAGEUP"] = KEY_PAGEUP;
	keyNameMap["PAGEDOWN"] = KEY_PAGEDOWN;
	keyNameMap["PAGE UP"] = KEY_PAGEUP;
	keyNameMap["PAGE DOWN"] = KEY_PAGEDOWN;

	/* Function keys */
	keyNameMap["F1"] = KEY_F1;
	keyNameMap["F2"] = KEY_F2;
	keyNameMap["F3"] = KEY_F3;
	keyNameMap["F4"] = KEY_F4;
	keyNameMap["F5"] = KEY_F5;
	keyNameMap["F6"] = KEY_F6;
	keyNameMap["F7"] = KEY_F7;
	keyNameMap["F8"] = KEY_F8;
	keyNameMap["F9"] = KEY_F9;
	keyNameMap["F10"] = KEY_F10;
	keyNameMap["F11"] = KEY_F11;
	keyNameMap["F12"] = KEY_F12;
	keyNameMap["F13"] = KEY_F13;
	keyNameMap["F14"] = KEY_F14;
	keyNameMap["F15"] = KEY_F15;

	/* Key state modifier keys */;
	keyNameMap["NUMLOCK"] = KEY_NUMLOCK;
	keyNameMap["CAPSLOCK"] = KEY_CAPSLOCK;
	keyNameMap["SCROLLOCK"] = KEY_SCROLLOCK;
	keyNameMap["RSHIFT"] = KEY_RSHIFT;
	keyNameMap["LSHIFT"] = KEY_LSHIFT;
	keyNameMap["RCTRL"] = KEY_RCTRL;
	keyNameMap["LCTRL"] = KEY_LCTRL;
	keyNameMap["RALT"] = KEY_RALT;
	keyNameMap["LALT"] = KEY_LALT;
	keyNameMap["RMETA"] = KEY_RMETA;
	keyNameMap["LMETA"] = KEY_LMETA;
	keyNameMap["LSUPER"] = KEY_LSUPER;		/* Left "Windows" key */
	keyNameMap["RSUPER"] = KEY_RSUPER;	/* Right "Windows" key */
	keyNameMap["MODE"] = KEY_MODE;		/* "Alt Gr" key */
	keyNameMap["COMPOSE"] = KEY_COMPOSE	;	/* Multi-key compose key */

	/* Miscellaneous function keys */
	keyNameMap["HELP"] = KEY_HELP;
	keyNameMap["PRINT"] = KEY_PRINT;
	keyNameMap["SYSREQ"] = KEY_SYSREQ;
	keyNameMap["BREAK"] = KEY_BREAK;
	keyNameMap["MENU"] = KEY_MENU;
	keyNameMap["POWER"] = KEY_POWER;	/* Power Macintosh power key */
	keyNameMap["EURO"] = KEY_EURO;		/* Some european keyboards */
	keyNameMap["UNDO"] = KEY_UNDO;		/* Atari keyboard has Undo */
}


// string from data

void ParseKeyMessage ( KEYTypes key, std::string &str, SDLMod modifiyers )
{
	char	theChar = 0;

	// yay words
	if ( (key >= SDLK_a) && (key <= SDLK_z) )
	{
		theChar = (char)key;

		if ( (modifiyers & KMOD_LSHIFT) || (modifiyers & KMOD_RSHIFT) || (modifiyers & KMOD_CAPS) )
			theChar = toupper(key);
		str += theChar;

		return;
	}
	// now for the fun

	switch(key)
	{
		case KEY_BACKSPACE:
		case KEY_DELETE:
			str += (char)KEY_BACKSPACE;
		//	str.resize(str.size()-1);
			break;
		case KEY_CLEAR:
			str += (char)KEY_CLEAR;
			break;
			
		default:
			{
				tmKeyIDMap::iterator itr = keyStringMap.find(key);

				if(itr != keyStringMap.end())
					str += itr->second;
			}
			break;
	}
}

// base device
CInputDevice::CInputDevice()
{
}

CInputDevice::~CInputDevice()
{
}

bool CInputDevice::Init ( void )
{
	return false;
}

bool CInputDevice::Release ( void )
{
	return false;
}

bool CInputDevice::Update ( void )
{
	return false;
}

int CInputDevice::Bind ( const char *command, const char *item )
{
	return -1;
}

float CInputDevice::GetValue ( const char *command )
{
	return 0;
}

float CInputDevice::GetValue (  int command )
{
	return 0;
}

bool CInputDevice::SetItem ( const char *key, int item )
{
	return false;
}

bool CInputDevice::Active ( void )
{
	return false;
}

const char* CInputDevice::GetName ( void )
{
	return "NULL::DEVICE";
}

const char* CInputDevice::GetNextAction ( void )
{
	return NULL;
}


// the global
//CBaseDeviceInput	inputDevice;

// keyboard and mouse devices

class CSDLKeyboardDevice : public CInputDevice
{
public:
	CSDLKeyboardDevice();
	virtual ~CSDLKeyboardDevice();

	virtual bool Init ( void );
	virtual bool Release ( void );

	virtual bool Update ( void );

	virtual int Bind ( const char *command, const char *item );
	virtual float GetValue ( const char *command );
	virtual float GetValue ( int command );

	// used to send in init data or whatnot ( stick number, etc );
	virtual bool SetItem ( const char *key, int item );

	virtual bool Active ( void );
	virtual const char* GetName ( void );
	virtual const char* GetNextAction ( void );

protected:
	std::map<std::string,KEYTypes> KeyComandMap;
	std::vector<KEYTypes>	KeyCommandList;

	unsigned char	*keymap;
	tmKeyNameMap::iterator	listItr;
};

const char* CSDLKeyboardDevice::GetName ( void )
{
	return "SDL_KEYBOARD";
}

const char* CSDLKeyboardDevice::GetNextAction ( void )
{
	if (keyNameMap.size() ==0)
		return NULL;

	if (listItr == keyNameMap.end())
	{
		listItr = keyNameMap.begin();
		return listItr->first.c_str();
	}
	else
	{
		listItr++;
		if (listItr == keyNameMap.end())
			return NULL;

		return listItr->first.c_str();
	}
}	

CSDLKeyboardDevice::CSDLKeyboardDevice()
{
	keymap = NULL;
	listItr = keyNameMap.end();
}

CSDLKeyboardDevice::~CSDLKeyboardDevice()
{
}

bool CSDLKeyboardDevice::Init ( void )
{
	initKeys();
	return true;
}

bool CSDLKeyboardDevice::Release ( void )
{
	return true;
}

bool CSDLKeyboardDevice::Update ( void )
{
        int numKeys;
	keymap = SDL_GetKeyState(&numKeys);
	return true;
}

int CSDLKeyboardDevice::Bind ( const char *command, const char *item )
{
	if (!command || !item)
		return -1;

	std::string str = command;

	GetKeyName((char*)str.c_str());
	KeyComandMap[str] = getKeyID(item);

	KeyCommandList.push_back(KeyComandMap[str]);

	return (int)KeyCommandList.size()-1;
}

float CSDLKeyboardDevice::GetValue ( const char *command )
{
	if (!command)
		return 0.0f;

	std::string str = command;

	std::map<std::string,KEYTypes>::iterator itr = KeyComandMap.find(str);
	
	if (itr == KeyComandMap.end())
		return 0.0f;

	return keymap[itr->second] != 0 ? 1.0f : 0.0f;
}

float CSDLKeyboardDevice::GetValue ( int command )
{
	if (command < 0)
		return 0.0f;

	int keyCommand = KeyCommandList[command];
	unsigned char key = keymap[keyCommand]; 
	return key != 0 ? 1.0f : 0.0f;
}

bool CSDLKeyboardDevice::SetItem ( const char *key, int item )
{
	return true;
}

bool CSDLKeyboardDevice::Active ( void )
{
	return true;
}

typedef enum
{
	eNoMouseAxis = 0,
	eXAxis,
	eYAxis,
	eXInc,
	eYInc,
	eWheel,
	eB1,
	eB2,
	eB3,
	eXScreen,
	eYScreen
}teMouseAxis;

class CSDLMouseDevice : public CInputDevice
{
public:
	CSDLMouseDevice();
	virtual ~CSDLMouseDevice();

	virtual bool Init ( void );
	virtual bool Release ( void );

	virtual bool Update ( void );

	virtual int Bind ( const char *command, const char *item );
	virtual float GetValue ( const char *command );
	virtual float GetValue ( int command );

	// used to send in init data or whatnot ( stick number, etc );
	virtual bool SetItem ( const char *key, int item );

	virtual bool Active ( void );
	virtual const char* GetName ( void );
	virtual const char* GetNextAction ( void );

	int Event (unsigned char event, void* message );

protected:
	teMouseAxis	GetMouseAxis ( const char *Axis );

	std::map<std::string,teMouseAxis> MouseComandMap;
	std::vector<teMouseAxis> MouseComandList;

	int		xReal,yReal,xInc,yInc,wheel,lastX,lastY;
	bool	b1,b2,b3,b4,b5,w1,w2;
	int		screenX,screenY;
	float	xRatio,yRatio;
	int		listCount;
};

int SDLMouseDeviceEventCallback(unsigned char event, void* message, void *param)
{
	return ((CSDLMouseDevice*)param)->Event(event,message);
}

int  CSDLMouseDevice::Event (unsigned char event, void* message )
{
	SDL_MouseButtonEvent	*buttonEvent = NULL;

	switch(event)
	{
		case SDL_MOUSEBUTTONDOWN:
			buttonEvent = ((SDL_MouseButtonEvent*)message);
			if ( (buttonEvent->button & SDL_MWHEEL_UPMASK) && buttonEvent->state == SDL_PRESSED )
				w1 = true;
			else if ( (buttonEvent->button & SDL_MWHEEL_DNMASK) && buttonEvent->state == SDL_PRESSED )
				w2 = true;
				
			return 1;

		default:
			return 0;
	}
	return 0;
}

const char* CSDLMouseDevice::GetName ( void )
{
	return "STD:MOUSE";
}

const char* CSDLMouseDevice::GetNextAction ( void )
{
	if (listCount > 10)
	{
		listCount = 0;
		return NULL;
	}

	static std::string	str;
	
	switch((teMouseAxis)(listCount+1))
	{
		case eXAxis:
			str == "XAXIS";
			break;

		case eYAxis:
			str == "YAXIS";
			break;

		case eXInc:
			str == "XINC";
			break;

		case eYInc:
			str == "YINC";
			break;

		case eWheel:
			str == "WHEEL";
			break;

		case eB1:
			str == "B1";
			break;

		case eB2:
			str == "B2";
			break;

		case eB3:
			str == "B3";
			break;

		case eXScreen:
			str == "XSCREEN";
			break;

		case eYScreen:
			str == "YSCREEN";
			break;
	}

	listCount++;
	return str.c_str();
}

teMouseAxis	CSDLMouseDevice::GetMouseAxis ( const char *Axis )
{
	if (!Axis)
		return eNoMouseAxis;
	
	std::string str = Axis;
	GetKeyName((char*)str.c_str());
	
	if ( str == "XAXIS")
		return eXAxis;

	if ( str == "YAXIS")
		return eYAxis;

	if ( str == "XINC")
		return eXInc;

	if ( str == "YINC")
		return eYInc;

	if ( str == "WHEEL")
		return eWheel;

	if ( str == "B1")
		return eB1;

	if ( str == "B2")
		return eB2;

	if ( str == "B3")
		return eB3;

	if ( str == "XSCREEN")
		return eXScreen;

	if ( str == "YSCREEN")
		return eYScreen;

	return eNoMouseAxis;
}

CSDLMouseDevice::CSDLMouseDevice()
{
	listCount = 0;
	RegisterEventHandaler(SDL_MOUSEBUTTONDOWN,SDLMouseDeviceEventCallback,this);
}

CSDLMouseDevice::~CSDLMouseDevice()
{
	RemoveEventHandaler(SDL_MOUSEBUTTONDOWN,SDLMouseDeviceEventCallback,this);
}

bool CSDLMouseDevice::Init ( void )
{
	return true;
}

bool CSDLMouseDevice::Release ( void )
{
	return true;
}

bool CSDLMouseDevice::Update ( void )
{
	// poll the mouse
	lastX = xReal;
	lastY = yReal;

	Uint8	iFlags = SDL_GetMouseState(&xReal,&yReal);

	b1 = b2 = b3 = false;
	
	if (SDL_BUTTON_LMASK & iFlags)
		b1 = true;

	if (SDL_BUTTON_MMASK & iFlags)
		b3 = true;

	if (SDL_BUTTON_RMASK & iFlags)
		b3 = true;

	if (SDL_MWHEEL_UPMASK & iFlags || w1)
		wheel = 1;
	else if (SDL_MWHEEL_DNMASK & iFlags || w2)
		wheel = -1;
	else
		wheel = 0;

	w1 = w2 = false;
	return true;
}

int CSDLMouseDevice::Bind ( const char *command, const char *item )
{
	if (!command || !item)
		return -1;

	std::string str = command;

	GetKeyName((char*)str.c_str());
	MouseComandMap[str] = GetMouseAxis(item);

	MouseComandList.push_back(MouseComandMap[str]);
	return (int)MouseComandList.size()-1;
}

float CSDLMouseDevice::GetValue ( const char *command )
{
	if (!command)
		return 0.0f;

	std::string str = command;

	std::map<std::string,teMouseAxis>::iterator itr = MouseComandMap.find(str);
	
	if (itr == MouseComandMap.end())
		return 0.0f;

	switch(itr->second)
	{
		default:
		case eNoMouseAxis:
			return 0;

		case eXAxis:
			return xReal* xRatio;

		case eYAxis:
			return yReal* yRatio;

		case eXInc:
			return xInc* xRatio;

		case eYInc:
			return xInc* xRatio;

		case eWheel:
			return (float)wheel;

		case eB1:
			return b1 ? 1.0f : 0.0f;

		case eB2:
			return b2 ? 1.0f : 0.0f;

		case eB3:
			return b3 ? 1.0f : 0.0f;

		case eXScreen:
			return (float)xReal;

		case eYScreen:
			return (float)yReal;
	}
	return 0;
}

float CSDLMouseDevice::GetValue ( int command )
{
	if (command < 0)
		return 0.0f;

	switch(MouseComandList[command])
	{
		default:
		case eNoMouseAxis:
			return 0;

		case eXAxis:
			return xReal* xRatio;

		case eYAxis:
			return yReal* yRatio;

		case eXInc:
			return xInc* xRatio;

		case eYInc:
			return xInc* xRatio;

		case eWheel:
			return (float)wheel;

		case eB1:
			return b1 ? 1.0f : 0.0f;

		case eB2:
			return b2 ? 1.0f : 0.0f;

		case eB3:
			return b3 ? 1.0f : 0.0f;

		case eXScreen:
			return (float)xReal;

		case eYScreen:
			return (float)yReal;
	}
	return 0;
}

bool CSDLMouseDevice::SetItem ( const char *key, int item )
{
	std::string str = key;

	GetKeyName((char*)str.c_str());

	if (str == "SCREENX")
		screenX = item;
	else if (str == "SCREENY")
		screenY = item;

	xRatio = 1.0f/(float)screenX;
	yRatio = 1.0f/(float)screenY;
	return true;
}

bool CSDLMouseDevice::Active ( void )
{
	return true;
}

// SDL stick

typedef enum
{
	eAxis,
	eButton,
	eHat
}teStickPart;

typedef struct
{
	teStickPart	part;
	int			partID;
	int			item;
	bool		side;
}trStickAxisDescriptor;

class CSDLJoyStick : public CInputDevice
{
public:
	CSDLJoyStick();
	virtual ~CSDLJoyStick();

	virtual bool Init ( void );
	virtual bool Release ( void );

	virtual bool Update ( void );

	virtual int Bind ( const char *command, const char *item );
	virtual float GetValue ( const char *command );
	virtual float GetValue ( int command );

	// used to send in init data or whatnot ( stick number, etc );
	virtual bool SetItem ( const char *key, int item );

	virtual bool Active ( void );
	virtual const char* GetName ( void );
	virtual const char* GetNextAction ( void );
	
protected:
	void Open ( void );
	void Close ( void );
	bool  GetHatState ( int iHat, int &iHatX, int &iHatY );
	float GetStickValue ( trStickAxisDescriptor *descriptor);
	int		iStick;
	
	std::map<std::string,trStickAxisDescriptor>	StickCommandMap;
	std::vector<trStickAxisDescriptor*>	StickCommandList;
	
	SDL_Joystick			*theStick;

	int						listCount;
};

CSDLJoyStick::CSDLJoyStick()
{
	InitStickSystem();
	iStick = -1;
	listCount = 0;

	theStick = NULL;
}

CSDLJoyStick::~CSDLJoyStick()
{
	Release();
	ReleaseStickSystem();
}

const char* CSDLJoyStick::GetNextAction ( void )
{
//	if(listCount > GetNum
	return NULL;
}

bool CSDLJoyStick::Init ( void )
{
	return true;
}

bool CSDLJoyStick::Release ( void )
{
	if (Active())
		Close();
	
	iStick = -1;
	return true;
}

void CSDLJoyStick::Open ( void )
{
	Close();
	if (iStick != -1)
		theStick = SDL_JoystickOpen(iStick);
}

void CSDLJoyStick::Close ( void )
{
	if (!theStick)
		return;

//	SDL_JoystickClose(theStick);
	theStick = NULL;
}

bool CSDLJoyStick::Update ( void )
{
	if (!Active())
		return false;

	SDL_JoystickUpdate();
	return true;
}

int CSDLJoyStick::Bind ( const char *command, const char *item )
{
	if (!command || ! item)
		return -1;

	if (iStick == -1)
		return -1;

	if (!Active())
		Open();

	// axes are X+-, Y+- ,Z+- , A1+-... An+-
	// buttons are B1....Bn
	// hat axes are HnX+-, HnY+-

	trStickAxisDescriptor	stickItem;

	char temp[512];
	strcpy(temp,item);
	GetKeyName(temp);
	
	switch(temp[0])
	{
		case 'H':
			// it's a hat;
			stickItem.part = eHat;
			stickItem.partID = atoi(&temp[1])-1;
			if (temp[strlen(temp)-2] == 'X')
				stickItem.item = 0;
			else
				stickItem.item = 1;

			if (temp[strlen(temp)-1] == '-')
				stickItem.side = false;
			else
				stickItem.side = true;
			break;

		case 'B':
			// it's a button
			stickItem.part = eButton;
			stickItem.partID = atoi(&temp[1])-1;
			stickItem.item = 0;
			stickItem.side = false;
			break;

			break;

		default:
			// it's some sort of axis
			stickItem.part = eAxis;
			switch (temp[0])
			{
				case 'X':
					stickItem.partID = 0;
					break;
				case 'Y':
					stickItem.partID = 1;
					break;
				case 'Z':
					stickItem.partID = 2;
					break;
				default:
					stickItem.partID = atoi(&temp[1]);
					break;

			}

			if (temp[strlen(temp)-1] == '-')
				stickItem.side = false;
			else
				stickItem.item = true;

			break;
	}

	std::string	str = command;
	GetKeyName((char*)str.c_str());

	StickCommandMap[str] = stickItem;
	StickCommandList.push_back(&StickCommandMap[str]);

	return (int)StickCommandList.size()-1;
}

bool  CSDLJoyStick::GetHatState ( int iHat, int &iHatX, int &iHatY )
{
	if (!theStick)
		return 0;

	iHatX = 0;
	iHatY = 0;

	int hatState = SDL_JoystickGetHat(theStick,iHat);

	if( hatState & SDL_HAT_UP)
		iHatY = 1;
	else if ( hatState & SDL_HAT_DOWN)
		iHatY = -1;

	if( hatState & SDL_HAT_RIGHT)
		iHatX = 1;
	else if ( hatState & SDL_HAT_LEFT)
		iHatX = -1;

	return true;
}

float CSDLJoyStick::GetStickValue ( trStickAxisDescriptor *descriptor)
{
	if (!descriptor)
		return 0.0f;

	float	raw;
	int		hatX,hatY;
	switch(descriptor->part)
	{
		default:
		case eAxis:
			raw = (float)SDL_JoystickGetAxis(theStick,descriptor->partID)/_STICK_CONSTANT;
			if ( (descriptor->side!=0) == (raw>0) )
				return fabs(raw);
			else
				return 0;
			break;

		case eButton:
			return (SDL_JoystickGetButton(theStick,descriptor->partID) != 0) ? 1.0f : 0.0f;
			break;

		case eHat:
			GetHatState(descriptor->partID,hatX,hatY);
			if (descriptor->item == 0)
				raw = (float)hatX;
			else
				raw = (float)hatY;

			if ( (descriptor->side!=0) == (raw>0) )
				return fabs(raw);
			else
				return 0;
			break;
	}
}

float CSDLJoyStick::GetValue ( const char *command )
{
	if (!command)
		return 0.0f;

	std::string	str = command;
	std::map<std::string,trStickAxisDescriptor>::iterator itr = StickCommandMap.find(str);
	if(itr == StickCommandMap.end())
		return 0.0;

	return GetStickValue(&(itr->second));
}

float CSDLJoyStick::GetValue ( int command )
{
	if (command<0)
		return 0.0f;

	return GetStickValue(StickCommandList[command]);
}

bool CSDLJoyStick::SetItem ( const char *key, int item )
{
	if (iStick == -1)
		iStick = item;
	else if (Active())
	{
		Close();
		iStick = item;
		Open();
	}
	else
		iStick = item;

	return true;
}

bool CSDLJoyStick::Active ( void )
{
	return theStick != NULL;
}

const char* CSDLJoyStick::GetName ( void )
{
	if (iStick == -1)
		return "NO_DEVICE";

	return SDL_JoystickName(iStick);
}

//  input manager

typedef struct
{
	CInputDevice	*device;
	std::string		item;
	int				itemID;
	float			last;
}trDeviceItemDesciptor;

typedef std::vector<trDeviceItemDesciptor> tmCommandActionList;

typedef struct
{
	int id;
	std::string			command;
	bool				poll;
	tmCommandActionList	actions;
}trCommandActionRecord;

typedef std::map<std::string,trCommandActionRecord> tmCommandDeviceMap;
typedef std::vector<trCommandActionRecord*> tvCommandDeviceList;
typedef std::map<std::string,CInputDevice*> tmDeviceList;
typedef std::vector<CInputDevice*> tvDevicePtrList;

// events
typedef struct
{
	float result;
	int	  id;
}trInputEvent;

typedef std::vector<trInputEvent> tvEventList;

// SDL event handaler
int InputManangerEventCallback(unsigned char event, void* message, void *param)
{
	return ((CInputMananger*)param)->Event(event,message);
}

// info
struct CInputMananger::trInputInfo
{
	tmCommandDeviceMap		commandMap;
	tvCommandDeviceList		commandList;
	tmDeviceList			deviceList;
	tvDevicePtrList			stdDevices;
	tvDevicePtrList			deviceIDList;

	tvEventList				eventList;
	int						thisEvent;
	int						w1,w2;
	std::string				text;
}trInputInfo;

CInputMananger::CInputMananger()
{
	RegisterEventHandaler(SDL_KEYDOWN,InputManangerEventCallback,this);
	RegisterEventHandaler(SDL_KEYUP,InputManangerEventCallback,this);
	RegisterEventHandaler(SDL_MOUSEBUTTONDOWN,InputManangerEventCallback,this);
	InitStickSystem();
	info = new trInputInfo;
}

CInputMananger::~CInputMananger()
{
	RemoveEventHandaler(SDL_KEYDOWN,InputManangerEventCallback,this);
	RemoveEventHandaler(SDL_KEYUP,InputManangerEventCallback,this);
	RemoveEventHandaler(SDL_MOUSEBUTTONDOWN,InputManangerEventCallback,this);
	Release();
	ReleaseStickSystem();
}

void CInputMananger::Init ( void )
{
	InitStandardDevices();
	tmDeviceList::iterator itr = info->deviceList.begin();

	while (itr !=info->deviceList.end())
	{
		itr->second->Init();
		itr++;
	}
}

void CInputMananger::Release ( void )
{
	tvDevicePtrList::iterator	stdItr = info->stdDevices.begin();

	while (stdItr != info->stdDevices.end())
		delete(*stdItr++);

	info->stdDevices.clear();
	info->commandList.clear();
	info->commandMap.clear();
	info->deviceList.clear();
}

void CInputMananger::ReadConfig ( const char *file )
{

	COSFile theFile(file,"rt");

	if(!theFile.IsOpen())
		return;

	bool done = false;
	while (!done)
	{
		std::string tag;
		std::string	command;
		std::string device;
		std::string item;

		tag = theFile.ScanStr();
		command = theFile.ScanStr();
		device = theFile.ScanStr();
		item = theFile.ScanStr();

		if (!tag.size() || !command.size() || !device.size() || !item.size())
			done = true;
		else
		{
			GetKeyName((char*)tag.c_str());

			if ( tag == "BIND")
				Bind(command.c_str(),device.c_str(),item.c_str());
		}
	}
	theFile.Close();
}

int CInputMananger::Bind ( const char *command, const char *device, const char *item )
{
	if (!command || !device || !item)
		return -1;

	std::string cmd = command;
	GetKeyName((char*)cmd.c_str());

	tmCommandDeviceMap::iterator commandMapItr = info->commandMap.find(cmd);

	int returnID = -1;

	if (commandMapItr == info->commandMap.end())
	{
		trCommandActionRecord	actionItem;
		
		actionItem.poll = false;
		actionItem.command = cmd;
		actionItem.id = returnID = (int)info->commandList.size();

		info->commandMap[cmd] = actionItem;
		info->commandList.push_back(&info->commandMap[cmd]);
	}
	else
		returnID = commandMapItr->second.id;

	trDeviceItemDesciptor		deviceItem;

	deviceItem.device = GetDevice(device);
	if (!deviceItem.device)
		return -1;

	deviceItem.last = 0;

	deviceItem.item = item;
	GetKeyName((char*)deviceItem.item.c_str());

	deviceItem.itemID = deviceItem.device->Bind(cmd.c_str(),deviceItem.item.c_str());

	info->commandMap[cmd].actions.push_back(deviceItem);

	return returnID;
}

int CInputMananger::GetCommandID ( const char *command )
{
	if (!command)
		return -1;

	std::string cmd = command;
	GetKeyName((char*)cmd.c_str());

	tmCommandDeviceMap::iterator commandMapItr = info->commandMap.find(cmd);

	if (commandMapItr == info->commandMap.end())
		return -1;

	return commandMapItr->second.id;
}

void CInputMananger::Unbind ( const char *command )
{
	if (!command)
		return;

	std::string cmd = command;
	GetKeyName((char*)cmd.c_str());

	tmCommandDeviceMap::iterator commandMapItr = info->commandMap.find(cmd);

	if (commandMapItr == info->commandMap.end())
		return;

	info->commandList[commandMapItr->second.id] = NULL;
	info->commandMap.erase(commandMapItr);
}

void CInputMananger::Unbind ( int command )
{
	if (command < 0)
		return;

	tmCommandDeviceMap::iterator commandMapItr = info->commandMap.find(info->commandList[command]->command);
	info->commandList[command] = NULL;

	if (commandMapItr != info->commandMap.end())
		info->commandMap.erase(commandMapItr);
}

void CInputMananger::Update ( void )
{	
	info->text = "";
	info->w1 = info->w2 = 0;
	for (tvDevicePtrList::iterator itr = info->deviceIDList.begin(); itr != info->deviceIDList.end(); itr++)
		(*itr)->Update();
}

void CInputMananger::SetPollCommand ( const char* command )
{
	if (!command)
		return;

	std::string cmd = command;
	GetKeyName((char*)cmd.c_str());

	tmCommandDeviceMap::iterator commandMapItr = info->commandMap.find(cmd);

	if (commandMapItr == info->commandMap.end())
		return;

	commandMapItr->second.poll = true;
}

void CInputMananger::SetPollCommand ( int command )
{
	if (command < 0)
		return;

	info->commandList[command]->poll = true;
}

float CInputMananger::PollState ( const char *command )
{
	if (!command)
		return -1;

	std::string cmd = command;
	GetKeyName((char*)cmd.c_str());

	tmCommandDeviceMap::iterator commandMapItr = info->commandMap.find(cmd);

	if (commandMapItr == info->commandMap.end())
		return 0;

	return PollState(commandMapItr->second.id);
}

float CInputMananger::PollState ( int command )
{
	if (command < 0)
		return 0;

	if (!info->commandList[command]->poll)
		return 0;

	tmCommandActionList::iterator itr = info->commandList[command]->actions.begin();

	float val = 0;
	
	while  ( itr !=  info->commandList[command]->actions.end())
	{
		itr->last = itr->device->GetValue(itr->itemID);
		if (itr->last > val)
			val = itr->last;
		itr++;
	}
	return val;
}

int CInputMananger::GetEvents ( void )
{
	// blow out the events
	info->eventList.clear();
	info->thisEvent = -1;

	// scan for some events
	tvCommandDeviceList::iterator commandItr = info->commandList.begin();

	while (commandItr != info->commandList.end())
	{
		trInputEvent	event;

		event.id = (*commandItr)->id;
		event.result = -1;

		if ((*commandItr != NULL) && !(*commandItr)->poll)
		{
			tmCommandActionList::iterator itr = (*commandItr)->actions.begin();

			float val = 0;
		
			while  ( itr !=  (*commandItr)->actions.end())
			{
				val = itr->device->GetValue(itr->itemID);
				if (itr->last != val)
				{
					if (val > event.result)
						event.result = val;

					itr->last = val;
				}
				itr++;
			}
		}
		if (event.result > 0)
			info->eventList.push_back(event);
			
		commandItr++;
	}
	return (int)info->eventList.size();
}

bool CInputMananger::GetNextEvent ( void )
{
	if (!info->eventList.size())
	{
		GetEvents();
	}

	info->thisEvent++;
	if (info->thisEvent >= (int)info->eventList.size())
	{
		info->eventList.clear();
		info->thisEvent = -1;
		return false;
	}
	return true;
}

const char *CInputMananger::GetEventName ( void )
{
	if (info->thisEvent < 0)
		return NULL;

	return info->commandList[info->eventList[info->thisEvent].id]->command.c_str();
}

int CInputMananger::GetEventID ( void )
{
	if (info->thisEvent < 0)
		return -1;

	return info->eventList[info->thisEvent].id;
}

float CInputMananger::GetEventValue ( void )
{
	if (info->thisEvent < 0)
		return 0;

	return info->eventList[info->thisEvent].result;
}

int CInputMananger::GetNumDevices ( void )
{
	return (int)info->deviceList.size();
}

CInputDevice*  CInputMananger::GetDevice ( const char *device )
{
	std::string		str = device;
	GetKeyName((char*)str.c_str());

	tmDeviceList::iterator itr = info->deviceList.find(str);
	if (itr == info->deviceList.end())
		return NULL;
	else
		return itr->second;
}

CInputDevice*  CInputMananger::GetDevice ( int device )
{
	if (device < 0)
		return NULL;

	return info->deviceIDList[device];
}

int CInputMananger::RegisterDevice ( const char * name, CInputDevice *device )
{
	std::string		str =name;
	GetKeyName((char*)str.c_str());

	info->deviceList[str] = device;
	info->deviceIDList.push_back(device);
	return (int)info->deviceIDList.size()-1;
}

void CInputMananger::HideOSCursor ( bool hide )
{
	SDL_ShowCursor(!hide);
}

void CInputMananger::GetKeysState ( unsigned char ** keys )
{
	if(!keys)
		return;

	*keys = SDL_GetKeyState(NULL);
}

void CInputMananger::GetMousePos ( int &iX, int &iY, bool bInc )
{
	if (bInc)
		SDL_GetRelativeMouseState(&iX,&iY);
	else
		SDL_GetMouseState(&iX,&iY);
}

void CInputMananger::GetMouseButtons ( bool &b1, bool &b2, bool &b3)
{
	Uint8	iFlags = SDL_GetMouseState(NULL,NULL);

	if (SDL_BUTTON_LMASK & iFlags)
		b1 = true;

	if (SDL_BUTTON_MMASK & iFlags)
		b3 = true;

	if (SDL_BUTTON_RMASK & iFlags)
		b2 = true;
}

int CInputMananger::Event(unsigned char event, void* message)
{
	SDL_MouseButtonEvent	*buttonEvent = ((SDL_MouseButtonEvent*)message);
	SDL_KeyboardEvent		*keyEvent = ((SDL_KeyboardEvent*)message);

	switch(event)
	{
		case SDL_KEYDOWN:
			ParseKeyMessage((KEYTypes)keyEvent->keysym.sym,info->text,keyEvent->keysym.mod);
			return 1;

		case SDL_KEYUP:
			return 1;

		case SDL_MOUSEBUTTONDOWN:
			if ( (buttonEvent->button & SDL_MWHEEL_UPMASK) && buttonEvent->state == SDL_PRESSED )
				info->w1 = true;
			else if ( (buttonEvent->button & SDL_MWHEEL_DNMASK) && buttonEvent->state == SDL_PRESSED )
				info->w2 = true;
			return 1;

		default:
			return 0;
	}
	return 0;
}

const char* CInputMananger::GetEnteredText( void )
{
	return info->text.c_str();
}


bool CInputMananger::InitStandardDevices ( void )
{
	info->stdDevices.push_back(GetDevice(RegisterDevice("KEYBOARD",new CSDLKeyboardDevice())));
	info->stdDevices.push_back(GetDevice(RegisterDevice("MOUSE",new CSDLMouseDevice())));

	for ( int i = 0; i < SDL_NumJoysticks();i++)
	{
		char temp[512];
		sprintf(temp,"JOYSTICK%d",i+1);
		info->stdDevices.push_back(GetDevice(RegisterDevice(temp,new CSDLJoyStick())));
		info->deviceList[std::string(temp)]->SetItem("STICK",i);
	}
	return true;
}

// global "keeper"
CInputMananger	*gInput = NULL;

void SetInputManager ( CInputMananger *pInput  )
{
	gInput  = pInput ;
}

CInputMananger* GetInputManager  ( void )
{
	return gInput;
}




