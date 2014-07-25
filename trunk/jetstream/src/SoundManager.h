#ifdef WIN32
#ifdef SOUNDMNGR_EXPORTS
#define SOUNDMNGR_API __declspec(dllexport)
#else
#define SOUNDMNGR_API __declspec(dllimport)
#endif
#else
#define SOUNDMNGR_API  
#endif

#ifndef _SOUND_MANANGER_H_
#define _SOUND_MANANGER_H_

#define BGM_CURRENT	-1

typedef struct
{
	int			iListID;
	bool			bLoaded;
	char			szName[512];
	char			szFile[512];
	bool			b3D;
	float			fMinDist;
	float			fMaxDist;
}trSampleInfo;

typedef struct
{
	bool	bPlaying;
	char	szName[512];
	char	szFile[512];
	char	szTitle[64];
}trBGMInfo;

typedef struct
{
	int	iSampleID;
	bool	bLoop;
	bool	bPaused;
	float	fVol;
	float	fPan;
	bool	b3D;
	float	afPos[3];
	float	afVec[3];
	float	afDist[3];
}trSoundEntInfo;

class SOUNDMNGR_API SoundManager 
{
public:
	SoundManager(void);
	~SoundManager(void);

	// pre init stuff
	void SetMinChanels ( int iChannels );	// default is 32
	void SetMin3DChanels ( int iChannels );	// default is 16

	int GetNumDevices ( void );
	const char* GetDeviceName ( int iDevice );

	// Init is called one to set up the sound system
	bool Init ( int iDevice = 0 );

	// Update is to be called once per frame
	void Update ( void );

	// loaders
	// zero for max dist will mean that it's a non 3d sound
	bool LoadOneSample ( char *szFileName, float fMinDist = 0.0f, float fMaxDist = 0.0f );
	bool LoadAllSamples ( char* szPathName, float fMinDist = 0.0f, float fMaxDist = 0.0f );

	// Sample info
	// must call one of these 2 before a play or there will be a hitch in the sound as it's loaded
	void InitSample ( char *szName );
	void InitSample ( int iSample );
	int	GetSampleID ( char *szName );

	void SetSampleInfo ( int iID, float fMinDist = 0.0f, float fMaxDist = 0.0f, bool bLooping = false );
	bool GetSampleInfo ( int iID, float *fMinDist = NULL, float *fMaxDist = NULL, bool bLooping = false );

	// sample playing
	// plays a sound if the pos is !NULL then vol is ignored and it's a positional sound
	int PlaySound ( int iSampleID, float fVol = 0.75f, float *pPos = NULL, float *pVec = NULL, bool bLooping = false );

	// sound ent management
	bool StopSound ( int iSoundEntID );
	bool PauseSound ( int iSoundEntID );
	bool RestartSound ( int iSoundEntID );

	// gloabal management
	bool PauseAll ( void );
	bool RestartAll ( void );
	bool Paused ( void ) { return m_bPaused; }

	void SetMasterVolume ( float fVol );
	float GetMasterVolume ( void );

	// global clean up functions
	bool KillAll ( bool bBGM_Also = true );	// stops all sounds
	void FlushAll ( bool bClear = true );	// kills all sounds and unloads all samples, if clear is true then also wipes the list

	// 3d pos stuff
	void SetViewerPos ( float *pPos = NULL, float *pFacing = NULL, float *pVec = NULL );
	void GetViewerPos ( float *pPos = NULL, float *pFacing = NULL, float *pVec = NULL );
	bool UpdateSoundInfo ( int iSoundEntID, float *pPos = NULL,float *pVec = NULL, float fVol = 0.75f, bool bLoop = false );

	// BGM 
	// BGMs are streamed from disk as they are rather large
	
	// loads a bunch of BGMs and puts them in the que
	void LoadAllBGM ( char *szPathName );

	// adds a aong into the end of the playlist
	void AddBGM ( char *szFileName );

	// the standard plays
	void PlayBGM ( void );
	void PauseBGM ( void );
	void StopBGM ( void );
	void ResumeBGM ( void );
	void NextBGM ( void );
	void LastBGM ( void );

	// stops playback of the current BGM and clears the que
	void ClearBGM ( void );
	
	// bgm global info;
	// looping means if the current play list is replayed when it's end is reached, default is true
	void SetBGMLooping ( bool bLooping );
	bool GetBGMLooping ( void );

	// random means that the playlist is randmomised on each push, default is false;
	void SetBGMRandom (  bool bRandom );
	bool GetBGMRandom ( void );

	void SetBGMVol ( float fVol = 0.5f );
	float GetBGMVol ( void );

	// iterators
	int GetSampleCount ( void );
	trSampleInfo GetSampleInfo( int iSampleID );

	int	GetSoundEntCount ( void );
	trSoundEntInfo GetSoundEntInfo ( int iSoundEntID );
	
	int	GetBGMCount ( void );
	trBGMInfo GetBGMInfo ( int iBGMID = BGM_CURRENT );	// BGM_CURRENT will give the one that is playing

private:
	// "real" load funcs for samples
	bool Win32LoadAllSamples ( const char *szPathName, float fMinDist, float fMaxDist );
	bool LinuxLoadAllSamples  ( const char *szPathName, float fMinDist, float fMaxDist );

	int LoadSample ( char* szFileName, float fMinDist, float fMaxDist );

	// real load functions for streams
	bool Win32LoadAllStreams ( const char *szPathName );
	bool LinuxLoadAllStreams  ( const char *szPathName );
	int LoadStream ( char* szFileName );

	void UpdateBGM ( void );

	// the lists
	void	*m_pSampleList;				// the list of unique sounds
	void	*m_pSampleNameMap;			// the map from name to sample	
	void	*m_pSoundEntList;			// the list of sound "things"
	void	*m_pBGMList;				// BGM "playlist"

	// Sound status
	bool	m_bPaused;
	float	m_fMasterVolume;

	// 3d stuff
	float	m_afViewerVec[3];
	float	m_afViewerPos[3];
	float	m_afViewerFace[3];

	// global options
	int		m_i3DChannels;
	int		m_iTotalChannels;

	// BGM Options
	bool	m_bLoop;
	bool	m_pRandom;
	bool	m_bPausedBGM;
	float	m_fBGMVol;
	int		m_iLastBGM;
	int		m_iThisBGM;
	int		m_iBGMChannel;

	// mem
	void	*m_pMemPool;
	unsigned int m_iMemPoolSize;
};

#endif //_SOUND_MANANGER_H_

/* The End */
