// TextureMngr.cpp : Defines the entry point for the DLL application.
//

#ifdef WIN32
  #define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
  #include <windows.h>
  #include <io.h>
  #include <direct.h>
  BOOL APIENTRY DllMain( HANDLE hModule,  DWORD  ul_reason_for_call,  LPVOID lpReserved)
  {
    return TRUE;
  }
  // windows and AIX use stricmp, POSIX uses strcasecmp.
  #define strcasecmp stricmp
#else
  #include <sys/types.h>
  #include <sys/stat.h>
  #include <unistd.h>
  #include <dirent.h>
  #include <ctype.h>
  char *strupr(char *s)
  {
    char *ptr = s;
    while(*ptr) {
      *ptr = toupper(*ptr);
      ptr++;
    }
    return s;
  }
#endif

#include "TextureMngr.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>

inline  bool operator < (const std::string &s1,const std::string &s2) { return (s1.compare(s2)<0);}
typedef std::map<std::string,int>  tmTextureNameMap;
typedef std::vector<trTextureInfo>  tvTextureList;

#define _m_pTextureMap ((tmTextureNameMap*)m_pTextureMap)
#define _m_pTextureList ((tvTextureList*)m_pTextureList)

#define m_TextureMap (*(tmTextureNameMap*)m_pTextureMap)
#define m_TextureList (*(tvTextureList*)m_pTextureList)

// utils 
 // utils

void StdToOSDir ( char *szDir )
{
#ifndef WIN32
	return;
#else
	char *pchar = szDir;

	while ( *pchar != NULL )
	{
		if (*pchar == '/')
			*pchar = '\\';

		pchar++;
	}
#endif//WIN32
}

void OSToStdDir ( char *szDir )
{
#ifndef WIN32
	return;
#else
	char *pchar = szDir;

	while ( *pchar != NULL )
	{
		if (*pchar == '\\')
			*pchar = '/';

		pchar++;
	}
#endif//WIN32
}

CTextureManager::CTextureManager()
{
	m_pTextureMap = (void*) new tmTextureNameMap;
	m_pTextureList = (void*) new tvTextureList;

	m_uiLastBindID = 0xffffffff;

	m_iTexturesLoaded = 0;
	m_iTextureMemUsed = 0;
}

CTextureManager::~CTextureManager()
{
	Flush(-1);

	m_TextureMap.clear();
	m_TextureList.clear();

	delete(_m_pTextureMap);
	delete(_m_pTextureList);

	m_pTextureMap= NULL;
	m_pTextureList = NULL;
}

int	CTextureManager::Load ( const char* szFile, bool bSkin )
{	// load takes OS Dirs
	if (!szFile)
		return -1;

	trTextureInfo	rTexInfo;// = (trTextureInfo*)malloc(sizeof(trTextureInfo));

		// get a name for it
	char	szName[255],*pSlash,extension[32] = {0};

	strcpy(szName,szFile);

	pSlash = strrchr(szName,'.');

	if (pSlash)	
	{
		strcpy(extension,pSlash+1);
		(*pSlash) = 0;
	}

	strcpy(rTexInfo.name,szName);
	strcpy(rTexInfo.file,szFile);

	if (extension[0] && (strcasecmp(extension,"BMP")==0) )
		rTexInfo.type = 0;
	else if (extension[0] && (strcasecmp(extension,"TGA")==0) )
		rTexInfo.type = 1;
	else if (extension[0] && (strcasecmp(extension,"SME")==0) )
		rTexInfo.type = 2;
	else
		return -1;

	rTexInfo.skin = bSkin;
	rTexInfo.glID = 0xffffffff;
	rTexInfo.manID = _m_pTextureList->size();

	// make sure  the name is a standard dir
	OSToStdDir(szName);
	strupr(szName);

	std::string	str = szName;
	m_TextureMap[str] = rTexInfo.manID;
	m_TextureList.push_back(rTexInfo);

	return rTexInfo.manID;
}

int CTextureManager::Win32LoadAll ( const char * szPath, bool bSkin)
{
	// passes in an OS file
#ifndef WIN32
	return 0;
#else//WIN32
	struct _finddata_t fileInfo;
	long	hFile;
	bool	bDone = false;
	char	searchstr[1024];
	char	FilePath[1024];
	int	iFiles = 0;

	strcpy(searchstr, szPath);
	strcat(searchstr, "\\*.*");

	hFile = _findfirst(searchstr,&fileInfo);

	if (hFile != -1)
	{
		while (!bDone)
		{
			if ( (strcmp(fileInfo.name,".") != 0) && (strcmp(fileInfo.name,"..") != 0))
			{
				strcpy(FilePath,szPath);
				strcat(FilePath,"\\");
				strcat(FilePath,fileInfo.name);

				if (fileInfo.attrib & _A_SUBDIR)
					iFiles += Win32LoadAll(FilePath,bSkin);
				else if (Load(FilePath,bSkin)!= -1)
						iFiles++;
			}

			if (_findnext(hFile,&fileInfo) == -1)
				bDone = true;
		}
	}
	return iFiles;
#endif // win32
}

int CTextureManager::LinuxLoadAll ( const char * szPath, bool bSkin)
{
#ifdef WIN32
	return 0;//WIN32
#else
	DIR	*directory;
	dirent	*fileInfo;
	struct stat	statbuf;
	char	searchstr[1024];
	char	FilePath[1024];
	int	iFiles = 0;

	strcpy(searchstr, szPath);
	if (searchstr[strlen(searchstr) - 1] != '/')
		strcat(searchstr, "/");
	directory = opendir(searchstr);
	if (!directory)
		return 0;
	while ((fileInfo = readdir(directory)))
	{
		if (!((strcmp(fileInfo->d_name, ".") == 0) || (strcmp(fileInfo->d_name, "..") == 0)))
		{
			strcpy(FilePath, searchstr);
			strcat(FilePath, fileInfo->d_name);

			stat(FilePath, &statbuf);

			if (S_ISDIR(statbuf.st_mode))
				iFiles += LinuxLoadAll(FilePath, bSkin);
			else if (Load(FilePath, bSkin) != -1)
				iFiles++;
		}
	}
	closedir(directory);
	return iFiles;
#endif// !Win32
}

int CTextureManager::LoadOne ( const char * szPath, bool bSkin )
{
	char	szOSPath[512];
	strcpy(szOSPath,szPath);
	StdToOSDir(szOSPath); //load One's allways get OS dirs

	return Load(szOSPath,bSkin);
}

int CTextureManager::LoadAll ( const char * szPath, bool bSkin )
{
	char	szOSPath[512];
	strcpy(szOSPath,szPath);
	StdToOSDir(szOSPath); //load all's allways get OS dirs

#ifdef WIN32
	return Win32LoadAll(szOSPath,bSkin);
#else
	return LinuxLoadAll(szOSPath,bSkin);
#endif// Win32
}

int CTextureManager::GetNumTexures ( void )
{
	return m_TextureList.size();
}

int CTextureManager::GetID ( char * szName )
{
	std::string	str = szName;
	strupr((char*) str.c_str());

	tmTextureNameMap::iterator	itr = m_TextureMap.find(str);
	
	if (itr == m_TextureMap.end())
		return -1;
	else
	{
		int id = itr->second;
		if (m_TextureList[id].glID == 0xffffffff)
			LoadFile(m_TextureList[id]);
		return id;
	}
}

bool CTextureManager::GetInfo ( unsigned int iID, trTextureInfo** pInfo )
{
	if ( (iID>=_m_pTextureList->size()) || !pInfo)
		return false;

	*pInfo = &m_TextureList[iID];
	return true;
}

bool CTextureManager::Bind ( unsigned int iID )
{
	if ( iID >= _m_pTextureList->size() )
		return false;

	if (m_TextureList[iID].glID == 0xffffffff)
		LoadFile(m_TextureList[iID]);

	if (m_TextureList[iID].glID == 0xffffffff)
		return false; // it failed
	
	glBindTexture(GL_TEXTURE_2D,m_TextureList[iID].glID);
	return true;
}

bool CTextureManager::Flush ( int iID )
{
	if (iID == -1)
	{
		tvTextureList::iterator itr = m_TextureList.begin();

		while (itr !=  m_TextureList.end())
		{
			if ((*itr).glID != 0xffffffff)
			{
				glDeleteTextures(1,&((*itr).glID));
				m_iTexturesLoaded--;
				m_iTextureMemUsed -= itr->size;

				(*itr).glID = 0xffffffff;
			}
			itr++;
		}
		m_TextureList.clear();
		m_TextureMap.clear();
		return true;
	}
	else
	{
		if ( (iID<0) || (iID>=(int)_m_pTextureList->size()) )
			return false;

		if (m_TextureList[iID].glID != 0xffffffff)
		{
			glDeleteTextures(1,&m_TextureList[iID].glID);
			m_iTexturesLoaded--;
			m_iTextureMemUsed -= m_TextureList[iID].size;
		}

		m_TextureList[iID].glID = 0xffffffff;
		return true;
	}
}
