// Texture Manager.h

#ifdef WIN32
#ifdef TEXTUREMNGR_EXPORTS
#define TEXMAN_API __declspec(dllexport)
#else
#define TEXMAN_API __declspec(dllimport)
#endif
#else
#define TEXMAN_API  
#endif

#ifndef _Texture_Manager_H_
#define _Texture_Manager_H_

#ifdef WIN32
#pragma warning( disable : 4786 )  // Disable warning message
#endif

TEXMAN_API void StdToOSDir ( char *szDir );
TEXMAN_API void OSToStdDir ( char *szDir );

#include <string>
#include <vector>
#include <map>

typedef struct 
{
	unsigned short	x,y;
	bool		alpha;
	unsigned int 	glID;
	int		manID;
	char		file[512];
	char		name[64];
	int		type;
	bool		skin;
	int		size;
}trTextureInfo;

class TEXMAN_API CTextureManager
{
public:
	CTextureManager();
	~CTextureManager();

	// these don't actualy load the texture they just get all the file info so it can be bound

	int	LoadOne ( const char * szPath, bool	bSkin = false );
	int	LoadAll ( const char * szPath, bool	bSkin = false );

	int	GetID ( char * szName );	// this will return the bound ID of the texture, or load it if not allready bound
	bool	GetInfo ( unsigned int iID, trTextureInfo** pInfo );

	bool	Bind ( unsigned int iID );	// this will bind the texture to the curent context
	
	bool	Flush ( int iID = -1 );

	int	GetNumTexures ( void );
	int	GetNumTexturesLoaded ( void ) { return m_iTexturesLoaded;}
	int	GetTextureMemUsed ( void ) { return m_iTextureMemUsed;}

private:

	int	Load ( const char* szPath, bool	bSkin = false  );
	
	int	LoadSME ( const char* szPath,trTextureInfo	*pTexInfo, bool	bSkin = false  );
	int	LoadBMP ( const char* szPath,trTextureInfo	*pTexInfo, bool	bSkin = false  );
	int	LoadTGA ( const char* szPath,trTextureInfo	*pTexInfo, bool	bSkin = false  );

	int LoadFile ( trTextureInfo &rTexInfo );

	int Win32LoadAll ( const char * szPath, bool bSkin);
	int LinuxLoadAll ( const char * szPath, bool bSkin);

	void* 	m_pTextureMap;
	void*	m_pTextureList;

	unsigned int m_uiLastBindID;

	int	m_iTexturesLoaded;
	int m_iTextureMemUsed;
};


#endif //_Texture_Manager_H_
