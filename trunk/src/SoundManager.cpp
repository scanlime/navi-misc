// SoundManager.cpp : Defines the entry point for the DLL application.
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
  #pragma warning( disable : 4786 )  // Disable warning message
#else
  #include <sys/types.h>
  #include <sys/stat.h>
  #include <unistd.h>
  #include <dirent.h>
  #include <ctype.h>
  extern char *strupr(char *s);
/*  char *strupr(char *s)
  {
    char *ptr = s;
    while(*ptr) {
      *ptr = toupper(*ptr);
      ptr++;
    }
    return s;
  }*/
#endif

#include "SoundManager.h"
#include <map>
#include <string>
#include <vector>
#include <algorithm>

#include "fmod.h"
#include "fmod_errors.h"

typedef struct
{
	FSOUND_SAMPLE	*pHandle;
	trSampleInfo	rInfo;
	int				iCount;
}		
trFModSample;

typedef struct
{
	FSOUND_STREAM	*pHandle;
	trBGMInfo		rInfo;
}trFModStreamInfo;

typedef struct 
{
	int				iChannel;
	trSoundEntInfo	rInfo;
}trFModSoundInfo;

inline  bool operator < (const std::string &s1,const std::string &s2) { return (s1.compare(s2)<0);}

typedef std::vector<trFModSample>			tvSampleList;
typedef std::map<std::string,int>			tvSampleNameMap;
typedef std::vector<trFModSoundInfo>		tvSoundInfoList;
typedef std::vector<trFModStreamInfo>		tvStreamInfoList;

#define _m_pSampleList		((tvSampleList*)m_pSampleList)
#define _m_SampleList		(*((tvSampleList*)m_pSampleList))

#define _m_pSampleNameMap	((tvSampleNameMap*)m_pSampleNameMap)
#define _m_SampleNameMap	(*((tvSampleNameMap*)m_pSampleNameMap))

#define _m_pSoundEntList	((tvSoundInfoList*)m_pSoundEntList)
#define _m_SoundEntList		(*((tvSoundInfoList*)m_pSoundEntList))

#define _m_pBGMList			((tvStreamInfoList*)m_pBGMList)
#define _m_BGMList			(*((tvStreamInfoList*)m_pBGMList))


// util functions
void SoundStdToOSDir ( char *szDir )
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

void SoundOSToStdDir ( char *szDir )
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

// here we go yo
SoundManager::SoundManager()
{ 	
	m_pSampleList = (void*) new tvSampleList;
	m_pSampleNameMap = (void*) new tvSampleNameMap;
	m_pSoundEntList = (void*)new tvSoundInfoList;
	m_pBGMList = (void*)new tvStreamInfoList;

	// defaluts
	m_fMasterVolume = 1.0f;

	m_afViewerVec[0] = m_afViewerVec[1] = m_afViewerVec[2] = 0.0f;
	m_afViewerPos[0] = m_afViewerPos[1] = m_afViewerPos[2] = 0.0f;

	m_i3DChannels = 16;
	m_iTotalChannels = 32;

	m_bPaused = false;

	m_bLoop = true;;
	m_pRandom = false;
	m_bPausedBGM = false;
	m_fBGMVol = 0.75f;
	m_iLastBGM = -1;
	m_iThisBGM = -1;
	m_iBGMChannel = -1;

	m_pMemPool = NULL;
	m_iMemPoolSize = 2*(4*1024*1024);// 2 megs
}

SoundManager::~SoundManager(void)
{
	FlushAll();

	if (m_pSoundEntList)
		delete (_m_pSoundEntList);

	if (m_pSampleNameMap)
		delete(_m_pSampleNameMap);

	if (m_pSampleList)
		delete(_m_pSampleList);

	if (m_pBGMList)
		delete (_m_pBGMList);

	FSOUND_Close();

	if (m_pMemPool)
		free(m_pMemPool);
}

void SoundManager::SetMinChanels ( int iChannels )
{
	if (m_pMemPool)	// allready inited
		return;

	m_iTotalChannels = iChannels;

	// can't have more then the total
	if (m_i3DChannels > m_iTotalChannels)
		m_i3DChannels = m_iTotalChannels;
}

void SoundManager::SetMin3DChanels ( int iChannels )
{
	if (m_pMemPool)	// allready inited
		return;

	m_i3DChannels = iChannels;
	
	// total has to include us
	if (m_i3DChannels > m_iTotalChannels)
		m_iTotalChannels = m_i3DChannels;
}

int SoundManager::GetNumDevices ( void )
{
	return FSOUND_GetNumDrivers();
}

const char* SoundManager::GetDeviceName ( int iDevice )
{
	return (const char*)FSOUND_GetDriverName(iDevice);
}

bool SoundManager::Init ( int iDevice )
{
	FSOUND_SetOutput(-1);

	m_pMemPool = malloc(m_iMemPoolSize); //  give us 2 megs
	if (!m_pMemPool)
		return false;
	
	if (!FSOUND_SetMemorySystem(m_pMemPool, m_iMemPoolSize, NULL, NULL, NULL)) 
		return false;

	int iRealDevice = iDevice;

	if (iDevice > GetNumDevices())
		iRealDevice = 0;

	if (!FSOUND_SetDriver(iRealDevice))
		return false;

	FSOUND_SetMinHardwareChannels(m_i3DChannels);

	if (!FSOUND_Init(44100,m_iTotalChannels, FSOUND_INIT_USEDEFAULTMIDISYNTH))
		return false;

	return true;
}

void SoundManager::Update ( void )
{
	if (m_bPaused)
		return;

	bool bPlaying;
	tvSoundInfoList::iterator	itr = _m_pSoundEntList->begin();

	bool	bHitSomething = false;

	while (itr != _m_pSoundEntList->end())
	{
		bHitSomething = true;
		if (!itr->rInfo.bPaused)
		{
			bPlaying = FSOUND_IsPlaying(itr->iChannel) == 1;
				
			if (FSOUND_GetCurrentSample(itr->iChannel) == _m_SampleList[itr->rInfo.iSampleID].pHandle)
			{
				// we are not playing what we started with someone got our channel, just bail
				_m_SampleList[itr->rInfo.iSampleID].iCount--;
				itr = _m_pSoundEntList->erase(itr);
			}
			else
			{
				if ( !bPlaying && !itr->rInfo.bLoop )
				{
					_m_SampleList[itr->rInfo.iSampleID].iCount--;
					itr = _m_pSoundEntList->erase(itr);
				}
				else
				{
					// it's still playing
					// check if it's a 3d sample played 2d style
					if (!itr->rInfo.b3D && _m_SampleList[itr->rInfo.iSampleID].rInfo.b3D)	// 3d sample with no pos, set it to the same as the listener
						FSOUND_3D_SetAttributes(itr->iChannel,m_afViewerPos,m_afViewerVec);

					itr++;
				}
			}
		}
	}

	// do bgm stream stuff
	if (!m_bPausedBGM & (m_iBGMChannel != -1))
	{
		bPlaying = FSOUND_IsPlaying(m_iBGMChannel) ==1;

		if (!bPlaying)
		{
			if (m_pRandom || (m_bLoop) || (!m_bLoop && (m_iThisBGM < _m_pBGMList->size())) )
				NextBGM();
			else
			{
				m_iLastBGM = m_iThisBGM;
				m_iThisBGM = -1;
				m_iBGMChannel = -1;
			}
		}
	}
	
	if (bHitSomething)
		FSOUND_3D_Update();

	UpdateBGM();
}

// sample loaders

int SoundManager::LoadSample ( char* szFileName, float fMinDist, float fMaxDist  )
{
	trFModSample	rSample;
	char	*p;
	char	name[512];

	if (!szFileName)
		return -1;

	rSample.pHandle = NULL;
	rSample.iCount = 0;
	rSample.rInfo.bLoaded = false;
	rSample.rInfo.fMaxDist = fMaxDist;
	rSample.rInfo.fMinDist = fMinDist;
	rSample.rInfo.b3D = fMaxDist > 0.0f;
	
	// save the file name
	strcpy(rSample.rInfo.szFile,szFileName);

	strcpy(name,szFileName);
	p = strrchr(name,'.');
	if (p)
		*p = '\0';
	SoundOSToStdDir(name);

	strcpy(rSample.rInfo.szName,name);
	std::string str = name;

	// add it to the sample list
	rSample.rInfo.iListID = _m_pSampleList->size();

	_m_pSampleList->push_back(rSample);
	_m_SampleNameMap[str] = rSample.rInfo.iListID;

	rSample.iCount = -1;

	return rSample.rInfo.iListID;
}

bool SoundManager::Win32LoadAllSamples ( const char *szPathName, float fMinDist, float fMaxDist )
{
#ifdef WIN32
	struct _finddata_t fileInfo;
	long	hFile;
	char	searchstr[1024];

	char	FilePath[1024];

	bool	bDone = false;

	int		iFiles = 0;

	strcpy(searchstr,szPathName);
	strcat(searchstr,"\\*.*");

	hFile = _findfirst(searchstr,&fileInfo);

	if (hFile != -1)
	{
		while (!bDone)
		{
			if ( (strcmp(fileInfo.name,".") != 0) && (strcmp(fileInfo.name,"..") != 0))
			{
				strcpy(FilePath,szPathName);
				strcat(FilePath,"\\");
				strcat(FilePath,fileInfo.name);

				if (fileInfo.attrib & _A_SUBDIR)
					iFiles += Win32LoadAllSamples(FilePath,fMinDist,fMaxDist);
				else if (LoadSample(FilePath,fMinDist,fMaxDist)!= -1)
					iFiles++;
			}
			if (_findnext(hFile,&fileInfo) == -1)
				bDone = true;
		}
	}
	return iFiles>0;
#else
	return 0;
#endif 
}

bool SoundManager::LinuxLoadAllSamples ( const char *szPathName, float fMinDist, float fMaxDist )
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

	strcpy(searchstr, szPathName);
	if (searchstr[strlen(searchstr) - 1] != '/')
		strcat(searchstr, "/");
	directory = opendir(searchstr);
	if (!directory)
		return false;
	while ((fileInfo = readdir(directory)))
	{
		if (!((strcmp(fileInfo->d_name, ".") == 0) || (strcmp(fileInfo->d_name, "..") == 0)))
		{
			strcpy(FilePath, searchstr);
			strcat(FilePath, fileInfo->d_name);

			stat(FilePath, &statbuf);

			if (S_ISDIR(statbuf.st_mode))
				iFiles += LinuxLoadAllSamples(FilePath,fMinDist,fMaxDist);
			else if (LoadSample(FilePath,fMinDist,fMaxDist) != -1)
				iFiles++;
		}
	}
	closedir(directory);
	return true;
#endif// !Win32
}

bool SoundManager::LoadAllSamples ( char* szPathName, float fMinDist, float fMaxDist )
{
	char	szOSPath[512];
	strcpy(szOSPath,szPathName);
	SoundStdToOSDir(szOSPath); //load all's allways get OS dirs

#ifdef WIN32
	return Win32LoadAllSamples(szOSPath,fMinDist,fMaxDist);
#else
	return LinuxLoadAllSamples(szOSPath,fMinDist,fMaxDist);
#endif// Win32

}

bool SoundManager::LoadOneSample ( char *szFileName, float fMinDist, float fMaxDist)
{
	char	szOSPath[512];
	strcpy(szOSPath,szFileName);
	SoundStdToOSDir(szOSPath); //load all's allways get OS dirs
	
	return LoadSample(szOSPath,fMinDist,fMaxDist) >= 0;
}

// sample inits
void SoundManager::InitSample ( int iSample )
{
	if ( (iSample<0) || (iSample>_m_SampleList.size()) )
	if (_m_SampleList[iSample].pHandle)
		return;
	
	int iMode = FSOUND_NORMAL;

	if (_m_SampleList[iSample].rInfo.b3D)
		iMode |= FSOUND_HW3D;

	_m_SampleList[iSample].pHandle = FSOUND_Sample_Load(FSOUND_FREE,_m_SampleList[iSample].rInfo.szFile,iMode,0);

	if (_m_SampleList[iSample].rInfo.b3D)
		FSOUND_Sample_SetMinMaxDistance(_m_SampleList[iSample].pHandle,_m_SampleList[iSample].rInfo.fMinDist,_m_SampleList[iSample].rInfo.fMaxDist);

	_m_SampleList[iSample].rInfo.bLoaded = true;
}

void SoundManager::InitSample ( char *szName )
{
	std::string	str = szName;

	tvSampleNameMap::iterator	itr = _m_SampleNameMap.find(str);
	
	int id = -1;

	if (itr == _m_SampleNameMap.end())
		return;
	else
		id = itr->second;

	InitSample(id);
}

int	SoundManager::GetSampleID ( char *szName )
{
	std::string	str = szName;

	tvSampleNameMap::iterator	itr = _m_SampleNameMap.find(str);
	
	int id = -1;

	if (itr == _m_SampleNameMap.end())
		return -1;
	else
		id = itr->second;

	if (!_m_SampleList[id].rInfo.bLoaded)
		InitSample(id);
	
	return id;
}

int SoundManager::PlaySound ( int iSampleID, float fVol, float *pPos, float *pVec, bool bLooping )
{
	if ( (iSampleID < 0) || (iSampleID >= _m_SampleList.size()) )
		return -1;

	if (!_m_SampleList[iSampleID].pHandle)
		InitSample(iSampleID);

	trFModSoundInfo	rSoundInfo;

	rSoundInfo.iChannel = -1;
	rSoundInfo.rInfo.iSampleID = iSampleID;

	rSoundInfo.rInfo.fVol = fVol;
	rSoundInfo.rInfo.fPan = 0.0f;
	rSoundInfo.rInfo.bLoop = bLooping;
	rSoundInfo.rInfo.bLoop = bLooping;
	rSoundInfo.rInfo.bPaused = false;

	rSoundInfo.rInfo.b3D = (pPos != NULL) && _m_SampleList[iSampleID].rInfo.b3D;

	if (rSoundInfo.rInfo.b3D)
	{
		memcpy(rSoundInfo.rInfo.afPos,pPos,sizeof(float)*3);
		memcpy(rSoundInfo.rInfo.afVec,pVec,sizeof(float)*3);
	}

	rSoundInfo.iChannel = FSOUND_PlaySoundEx(FSOUND_FREE,_m_SampleList[iSampleID].pHandle,NULL,true);

	if (rSoundInfo.iChannel < 0)
		return -1;

	FSOUND_SetVolume(rSoundInfo.iChannel, (int) fVol*255);

	if (rSoundInfo.rInfo.bLoop)
		FSOUND_SetLoopMode(rSoundInfo.iChannel,1);

	if (rSoundInfo.rInfo.b3D)
		FSOUND_3D_SetAttributes(rSoundInfo.iChannel,rSoundInfo.rInfo.afPos,rSoundInfo.rInfo.afVec);
	else if (_m_SampleList[iSampleID].rInfo.b3D)	// 3d sample with no pos, set it to the same as the listener
		FSOUND_3D_SetAttributes(rSoundInfo.iChannel,m_afViewerPos,m_afViewerVec);

	_m_SampleList[iSampleID].iCount++;
	FSOUND_SetPaused(rSoundInfo.iChannel, false);

	_m_pSoundEntList->push_back(rSoundInfo);
	return _m_pSoundEntList->size()-1;
}

bool SoundManager::StopSound ( int iSoundEntID )
{
	if ( (iSoundEntID < 0) || (iSoundEntID >= _m_SoundEntList.size()) )
		return false;;


	trFModSoundInfo	*pSound = &(_m_SoundEntList[iSoundEntID]);

	if (!pSound || (pSound->iChannel < 0))
		return false;

	// if it's playing and its our sound stop it
	if (FSOUND_IsPlaying(pSound->iChannel) && (FSOUND_GetCurrentSample(pSound->iChannel) == _m_SampleList[pSound->rInfo.iSampleID].pHandle))
			FSOUND_StopSound(pSound->iChannel);

	if (_m_SampleList[pSound->rInfo.iSampleID].iCount>0)
		_m_SampleList[pSound->rInfo.iSampleID].iCount--;

	tvSoundInfoList::iterator	itr = _m_SoundEntList.begin() + (iSoundEntID * sizeof(tvSoundInfoList::iterator));

	_m_SoundEntList.erase(itr);

	return true;
}

bool SoundManager::PauseSound ( int iSoundEntID )
{
	if ( (iSoundEntID < 0) || (iSoundEntID >= _m_SoundEntList.size()) )
		return false;

	tvSoundInfoList::iterator	itr = _m_SoundEntList.begin() + (iSoundEntID * sizeof(tvSoundInfoList::iterator));

	if ((itr == _m_SoundEntList.end()) || (itr->iChannel < 0) || itr->rInfo.bPaused)
		return false;

	itr->rInfo.bPaused = true;

	FSOUND_SetPaused(itr->iChannel, true);

	return true;
}

bool SoundManager::RestartSound ( int iSoundEntID )
{
	if ( (iSoundEntID < 0) || (iSoundEntID >= _m_SoundEntList.size()) )
		return false;

	tvSoundInfoList::iterator	itr = _m_SoundEntList.begin() + (iSoundEntID * sizeof(tvSoundInfoList::iterator));

	if ((itr == _m_SoundEntList.end()) || (itr->iChannel < 0) || !itr->rInfo.bPaused)
		return false;

	itr->rInfo.bPaused = false;

	FSOUND_SetPaused(itr->iChannel, false);

	return true;
}

bool SoundManager::PauseAll ( void )
{
	if (m_bPaused)
		return false;

	m_bPaused = true;
	FSOUND_SetPaused(FSOUND_ALL,true);

	return true;
}

bool SoundManager::RestartAll ( void )
{
	if (!m_bPaused)
		return false;

	m_bPaused = false;
	FSOUND_SetPaused(FSOUND_ALL,false);

	return true;
}

void SoundManager::SetMasterVolume ( float fVol )
{
	m_fMasterVolume = fVol;
	FSOUND_SetSFXMasterVolume((int) fVol * 255);
}

float SoundManager::GetMasterVolume ( void )
{
	return m_fMasterVolume;
}

bool SoundManager::KillAll ( bool bBGM_Also  )	// stops all sounds
{
	if (bBGM_Also)
	{
		ClearBGM();
		m_bPaused = false;
	}

	FSOUND_StopSound(FSOUND_ALL);
	_m_SoundEntList.clear();

	tvSampleList::iterator	itr = _m_SampleList.begin();

	while (itr != _m_SampleList.end())
	{
		itr->iCount = 0;
		itr++;
	}
	
	return true;
}

void SoundManager::FlushAll ( bool bClear )	// kills all sounds and unloads all samples, if clear is true then also wipes the list
{
	KillAll();
	
	tvSampleList::iterator	itr = _m_SampleList.begin();

	while (itr != _m_SampleList.end())
	{
		itr->iCount = 0;
		FSOUND_Sample_Free(itr->pHandle);
		itr->pHandle = NULL;
		itr->rInfo.bLoaded = false;
		itr++;
	}

	if (bClear)
	{
		_m_SampleList.clear();
		_m_pSampleNameMap->clear();
	}

	m_bPaused = false;
}

// 3d pos stuff
void SoundManager::SetViewerPos ( float *pPos, float *pFacing, float *pVec )
{
	if (!pPos && !pVec && !pFacing)
		return;

	if (pVec)
		memcpy(m_afViewerVec,pVec,sizeof(float)*3);

	if (pFacing)
		memcpy(m_afViewerFace,pFacing,sizeof(float)*3);

	if (pPos)
		memcpy(m_afViewerPos,pPos,sizeof(float)*3);

	FSOUND_3D_Listener_SetAttributes(m_afViewerPos,m_afViewerVec,m_afViewerFace[0],m_afViewerFace[1],m_afViewerFace[2],0,0,1);
}

void SoundManager::GetViewerPos ( float *pPos ,float *pFacing, float *pVec  )
{
	if (pVec)
		memcpy(pVec,m_afViewerVec,sizeof(float)*3);

	if (pFacing)
		memcpy(pFacing,m_afViewerFace,sizeof(float)*3);

	if (pPos)
		memcpy(pPos,m_afViewerPos,sizeof(float)*3);
}

bool SoundManager::UpdateSoundInfo ( int iSoundEntID, float *pPos, float *pVec, float fVol, bool bLoop)
{
	if ( (iSoundEntID < 0) || (iSoundEntID >= _m_SoundEntList.size()) )
		return false;;

	tvSoundInfoList::iterator	itr = _m_SoundEntList.begin() + (iSoundEntID * sizeof(tvSoundInfoList::iterator));

	//tvSoundInfoList::iterator	itr = std::find(_m_SoundEntList.begin(),_m_SoundEntList.end(),iSoundEntID);

	if ((itr == _m_SoundEntList.end()) || (itr->iChannel < 0))
		return false;

	if (!FSOUND_IsPlaying(itr->iChannel))
		return false;
		
	if (FSOUND_GetCurrentSample(itr->iChannel) != _m_SampleList[itr->rInfo.iSampleID].pHandle)
		return false;

	if (itr->rInfo.fVol != fVol)
		FSOUND_SetVolume(itr->iChannel, (int) fVol*255);

	if (itr->rInfo.bLoop != bLoop )
		FSOUND_SetLoopMode(itr->iChannel,bLoop);

	itr->rInfo.bLoop = bLoop;
	itr->rInfo.fVol = fVol;

	if (!itr->rInfo.b3D ||! _m_SampleList[itr->rInfo.iSampleID].rInfo.b3D)
		return true;

	if (pPos)
		memcpy(itr->rInfo.afPos,pPos,sizeof(float)*3);

	if (pVec)
		memcpy(itr->rInfo.afVec,pVec,sizeof(float)*3);

	FSOUND_3D_SetAttributes(itr->iChannel,pPos,pVec);

	return true;
}

// loads a bunch of BGMs and puts them in the que
void SoundManager::LoadAllBGM ( char *szPathName )
{
	char	szOSPath[512];
	strcpy(szOSPath,szPathName);
	SoundStdToOSDir(szOSPath); //load all's allways get OS dirs
#ifdef WIN32
	Win32LoadAllStreams(szOSPath);
#else
	LinuxLoadAllStreams(szOSPath);
#endif// Win32
}

bool SoundManager::Win32LoadAllStreams ( const char *szPathName )
{
#ifdef WIN32
	struct _finddata_t fileInfo;
	long	hFile;
	char	searchstr[1024];

	char	FilePath[1024];

	bool	bDone = false;

	int		iFiles = 0;

	strcpy(searchstr,szPathName);
	strcat(searchstr,"\\*.*");

	hFile = _findfirst(searchstr,&fileInfo);

	if (hFile != -1)
	{
		while (!bDone)
		{
			if ( (strcmp(fileInfo.name,".") != 0) && (strcmp(fileInfo.name,"..") != 0))
			{
				strcpy(FilePath,szPathName);
				strcat(FilePath,"\\");
				strcat(FilePath,fileInfo.name);

				if (fileInfo.attrib & _A_SUBDIR)
					iFiles += Win32LoadAllStreams(FilePath);
				else if (LoadStream(FilePath)!= -1)
					iFiles++;
			}
			if (_findnext(hFile,&fileInfo) == -1)
				bDone = true;
		}
	}
	return iFiles>0;
#else
	return 0;
#endif 
}

bool SoundManager::LinuxLoadAllStreams  ( const char *szPathName )
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

	strcpy(searchstr, szPathName);
	if (searchstr[strlen(searchstr) - 1] != '/')
		strcat(searchstr, "/");
	directory = opendir(searchstr);
	if (!directory)
		return false;
	while ((fileInfo = readdir(directory)))
	{
		if (!((strcmp(fileInfo->d_name, ".") == 0) || (strcmp(fileInfo->d_name, "..") == 0)))
		{
			strcpy(FilePath, searchstr);
			strcat(FilePath, fileInfo->d_name);

			stat(FilePath, &statbuf);

			if (S_ISDIR(statbuf.st_mode))
				iFiles += LinuxLoadAllStreams(FilePath);
			else if (LoadStream(FilePath) != -1)
				iFiles++;
		}
	}
	closedir(directory);
	return true;
#endif// !Win32
}

int SoundManager::LoadStream ( char* szFileName )
{
	if (!szFileName)
		return -1;

	trFModStreamInfo	rInfo;

	rInfo.pHandle = NULL;
	rInfo.rInfo.bPlaying = false;
	strcpy(rInfo.rInfo.szFile,szFileName);

	strcpy(rInfo.rInfo.szName,szFileName);
	char* p = strrchr(rInfo.rInfo.szName,'.');
	if (p)
		*p = '\0';
	SoundOSToStdDir(rInfo.rInfo.szName);

	p = strrchr(rInfo.rInfo.szName,'/');
	if (!p)
		p = rInfo.rInfo.szName;

	strcpy(rInfo.rInfo.szTitle,p);

	_m_BGMList.push_back(rInfo);
	return true;
}

// adds a aong into the end of the playlist
void SoundManager::AddBGM ( char *szFileName )
{
	char	szOSPath[512];
	strcpy(szOSPath,szFileName);
	SoundStdToOSDir(szOSPath); //load all's allways get OS dirs
	LoadStream(szOSPath);
}

// the standard plays
void SoundManager::PlayBGM ( void )
{
	if (_m_BGMList.size() <=0)
		return;

	if (m_iBGMChannel != -1)
		StopBGM();

	tvStreamInfoList::iterator	itr = _m_BGMList.begin();

	if ((itr == _m_BGMList.end()) || itr->rInfo.bPlaying)
		return;

	m_iLastBGM = m_iThisBGM;

	int		iBGMToPlay = 0;

	if (m_pRandom)
	{
		while (iBGMToPlay != m_iThisBGM)
			iBGMToPlay = rand()%_m_BGMList.size();

		itr = _m_BGMList.begin() + (iBGMToPlay * sizeof(tvStreamInfoList::iterator));
	}
	m_iThisBGM = iBGMToPlay;

	if (itr->pHandle)
		FSOUND_Stream_Close(itr->pHandle);

	itr->pHandle = FSOUND_Stream_OpenFile(itr->rInfo.szFile, FSOUND_MPEGACCURATE ,0);

	if (!itr->pHandle)
		return;

	m_iBGMChannel = FSOUND_Stream_PlayEx(FSOUND_FREE,itr->pHandle,NULL,true);
	
	FSOUND_SetVolume(m_iBGMChannel, (int) m_fBGMVol*255);
	FSOUND_SetVolume(m_iBGMChannel, (int) m_fBGMVol*255);
	FSOUND_SetPaused(m_iBGMChannel,false);
	
	itr->rInfo.bPlaying = true;
	m_bPausedBGM = false;
}

void SoundManager::StopBGM ( void )
{
	if (m_bPausedBGM || (m_iThisBGM == -1) || (m_iBGMChannel == -1) )
		return;

	if (!_m_BGMList[m_iThisBGM].rInfo.bPlaying)
		return;

	bool	bPlaying = FSOUND_IsPlaying(m_iBGMChannel)==1;

	if (bPlaying)
	{
		if (((FSOUND_STREAM *)(FSOUND_GetCurrentSample(m_iBGMChannel))) == _m_BGMList[m_iThisBGM].pHandle)
			FSOUND_StopSound(m_iBGMChannel);
	}
	
	if (_m_BGMList[m_iThisBGM].pHandle)
		FSOUND_Stream_Close(_m_BGMList[m_iThisBGM].pHandle);

	_m_BGMList[m_iThisBGM].rInfo.bPlaying = false;
	m_iThisBGM = -1;
	m_bPausedBGM = false;
	m_iBGMChannel = -1;
}

void SoundManager::PauseBGM ( void )
{
	if (m_bPausedBGM || (m_iThisBGM == -1) || (m_iBGMChannel == -1) )
		return;

	FSOUND_SetPaused(m_iBGMChannel,false);

	m_bPausedBGM = true;
}

void SoundManager::ResumeBGM ( void )
{
	if (!m_bPausedBGM || (m_iThisBGM == -1) || (m_iBGMChannel == -1) )
		return;

	FSOUND_SetPaused(m_iBGMChannel,true);

	m_bPausedBGM = false;
}

void SoundManager::NextBGM ( void )
{
	if (m_bPausedBGM || (m_iThisBGM == -1) || (m_iBGMChannel == -1) )
		return;

	if (!_m_BGMList[m_iThisBGM].rInfo.bPlaying)
		return;

	bool	bPlaying = FSOUND_IsPlaying(m_iBGMChannel)==1;

	if (bPlaying)
	{
		if (((FSOUND_STREAM *)(FSOUND_GetCurrentSample(m_iBGMChannel))) == _m_BGMList[m_iThisBGM].pHandle)
			FSOUND_StopSound(m_iBGMChannel);
	}
	
	if (_m_BGMList[m_iThisBGM].pHandle)
		FSOUND_Stream_Close(_m_BGMList[m_iThisBGM].pHandle);
	
	_m_BGMList[m_iThisBGM].rInfo.bPlaying = false;
	
	m_iLastBGM = m_iThisBGM;

	int		iBGMToPlay = m_iThisBGM+1;

	if (m_pRandom)
	{
		while (iBGMToPlay != m_iThisBGM)
			iBGMToPlay = rand()%_m_BGMList.size();
	}
	else if (m_bLoop)
	{
		if (m_iThisBGM > _m_BGMList.size())
			m_iThisBGM = 0;
	}
	
	m_iThisBGM = iBGMToPlay;

	if (_m_BGMList[m_iThisBGM].pHandle)
		FSOUND_Stream_Close(_m_BGMList[m_iThisBGM].pHandle);

	_m_BGMList[m_iThisBGM].pHandle = FSOUND_Stream_OpenFile(_m_BGMList[m_iThisBGM].rInfo.szFile, FSOUND_HW2D,0);

	if (!_m_BGMList[m_iThisBGM].pHandle)
		return;

	m_iBGMChannel = FSOUND_Stream_PlayEx(FSOUND_FREE,_m_BGMList[m_iThisBGM].pHandle,NULL,true);
	
	FSOUND_SetVolume(m_iBGMChannel, (int) m_fBGMVol*255);
	FSOUND_SetVolume(m_iBGMChannel, (int) m_fBGMVol*255);
	FSOUND_SetPaused(m_iBGMChannel,false);
	
	_m_BGMList[m_iThisBGM].rInfo.bPlaying = true;
	m_bPausedBGM = false;
}

void SoundManager::LastBGM ( void )
{
	if (m_bPausedBGM || (m_iLastBGM == -1) || (m_iBGMChannel == -1))
		return;

	if (!_m_BGMList[m_iThisBGM].rInfo.bPlaying)
		return;

	bool	bPlaying = FSOUND_IsPlaying(m_iBGMChannel)==1;

	if (bPlaying)
	{
		if (((FSOUND_STREAM *)(FSOUND_GetCurrentSample(m_iBGMChannel))) == _m_BGMList[m_iThisBGM].pHandle)
			FSOUND_StopSound(m_iBGMChannel);
	}
	
	if (_m_BGMList[m_iThisBGM].pHandle)
		FSOUND_Stream_Close(_m_BGMList[m_iThisBGM].pHandle);
	
	_m_BGMList[m_iThisBGM].rInfo.bPlaying = false;
	
	m_iThisBGM = m_iLastBGM;

	if (_m_BGMList[m_iThisBGM].pHandle)
		FSOUND_Stream_Close(_m_BGMList[m_iThisBGM].pHandle);

	_m_BGMList[m_iThisBGM].pHandle = FSOUND_Stream_OpenFile(_m_BGMList[m_iThisBGM].rInfo.szFile, FSOUND_HW2D,0);

	if (!_m_BGMList[m_iThisBGM].pHandle)
		return;

	m_iBGMChannel = FSOUND_Stream_PlayEx(FSOUND_FREE,_m_BGMList[m_iThisBGM].pHandle,NULL,true);
	
	FSOUND_SetVolume(m_iBGMChannel, (int) m_fBGMVol*255);
	FSOUND_SetVolume(m_iBGMChannel, (int) m_fBGMVol*255);
	FSOUND_SetPaused(m_iBGMChannel,false);
	
	_m_BGMList[m_iThisBGM].rInfo.bPlaying = true;
	m_bPausedBGM = false;
}

void SoundManager::UpdateBGM ( void )
{
	if (m_bPausedBGM || (m_iThisBGM == -1) || (m_iBGMChannel == -1))
		return;

	bool	bPlaying = FSOUND_IsPlaying(m_iBGMChannel)==1;

	if (bPlaying)
	{
		FSOUND_STREAM*	pThisStream = ((FSOUND_STREAM *)(FSOUND_GetCurrentSample(m_iBGMChannel)));
		FSOUND_STREAM*	pOurStream = _m_BGMList[m_iThisBGM].pHandle;
		//if ( pThisStream == pOurStream)
			return; // we are still goin
	}

	if ( (_m_BGMList.size() > 0) || m_bLoop || m_pRandom ) 
		NextBGM();
}

// stops playback of the current BGM and clears the que
void SoundManager::ClearBGM ( void )
{
	StopBGM();
	_m_BGMList.clear();
	m_bPausedBGM = false;
	m_iBGMChannel = -1;
	m_iLastBGM = -1;
	m_iThisBGM = -1;
}

// bgm global info;
// looping means if the current play list is replayed when it's end is reached, default is true
void SoundManager::SetBGMLooping ( bool bLooping )
{
	m_bLoop = bLooping; 
}

bool SoundManager::GetBGMLooping ( void )
{
	return m_bLoop;
}

// random means that the playlist is randmomised on each push, default is false;
void SoundManager::SetBGMRandom (  bool bRandom )
{
	m_pRandom = bRandom;
}

bool SoundManager::GetBGMRandom ( void )
{
	return m_pRandom;
}

void SoundManager::SetBGMVol ( float fVol)
{
	m_fBGMVol = fVol;

	if (m_bPausedBGM || (m_iThisBGM == -1) || (m_iBGMChannel == -1))
		return;
	
	bool	bPlaying = FSOUND_IsPlaying(m_iBGMChannel)==1;

	if (bPlaying)
	{
		if (((FSOUND_STREAM *)(FSOUND_GetCurrentSample(m_iBGMChannel))) == _m_BGMList[m_iThisBGM].pHandle)
			FSOUND_SetVolume(m_iBGMChannel, (int) m_fBGMVol*255);
	}
}

float SoundManager::GetBGMVol ( void )
{
	return m_fBGMVol;
}

int SoundManager::GetSampleCount( void )
{
	return _m_SampleList.size();
}

trSampleInfo SoundManager::GetSampleInfo( int iSampleID )
{
	trSampleInfo	rInfo;
	rInfo.bLoaded = false;
	rInfo.b3D = false;
	rInfo.szFile[0] = 0;
	rInfo.szName[0] = 0;
	rInfo.szName[0] = 0;

	if ( (iSampleID >= 0) || (iSampleID < GetSampleCount()) )
		memcpy(&rInfo,&(_m_SampleList[iSampleID].rInfo),sizeof(trSampleInfo));
	
	return rInfo;
}

int	SoundManager::GetSoundEntCount ( void )
{
	return _m_SoundEntList.size();
}

trSoundEntInfo SoundManager::GetSoundEntInfo ( int iSoundEntID )
{
	trSoundEntInfo	rInfo;

	rInfo.iSampleID = -1;

	if ( (iSoundEntID >= 0) || (iSoundEntID < GetSoundEntCount()) )
		memcpy(&rInfo,&(_m_SoundEntList[iSoundEntID].rInfo),sizeof(trSoundEntInfo));

	return rInfo;
}

int	SoundManager::GetBGMCount ( void )
{
	return _m_BGMList.size();
}

trBGMInfo SoundManager::GetBGMInfo ( int iBGMID )	// BGM_CURRENT will give the one that is playing
{
	trBGMInfo	rInfo;

	rInfo.bPlaying = false;
	rInfo.szFile[0] = 0;
	rInfo.szName[0] = 0;
	rInfo.szTitle[0] = 0;

	int iIDToUse = m_iThisBGM;

	if (iBGMID != BGM_CURRENT)
		iIDToUse = iBGMID;

	if ( (iIDToUse >= 0) || (iIDToUse < GetBGMCount()) )
		memcpy(&rInfo,&(_m_BGMList[iIDToUse].rInfo),sizeof(trBGMInfo));

	return rInfo;
}



/* The End */
