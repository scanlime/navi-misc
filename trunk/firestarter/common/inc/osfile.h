/* Firestarter
* osfile.h :
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


// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the OSFILE_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// OSFILE_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.

#ifndef _OSFILE_H_
#define _OSFILE_H_


#ifdef _WIN32
  #pragma warning( disable : 4786 )  // Disable warning message
  #define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
  #include <windows.h>
  #include <io.h>
  #include <direct.h>
#else
  #include <sys/types.h>
  #include <sys/stat.h>
  #include <unistd.h>
  #include <dirent.h>
  #include <ctype.h>
#endif

#include <string>
#include <vector>

#include <stdio.h>

typedef enum 
{
	eFileStart,
	eCurentPos,
	eFileEnd
}teFilePos;

void SetOSFileBaseDir ( const char *szDir );

class COSFile
{
public:
	COSFile ();
	COSFile ( const COSFile &r);
	COSFile& operator = (const COSFile &r);

	COSFile ( const char *szName );
	COSFile ( const char *szName, const char *szMode );
	~COSFile();

	bool Open ( const char *szName, const char *szMode );
	bool Open ( const char *szMode );
	bool Close ( void );

	void StdName ( const char *szName );
	void OSName ( const char *szName );

	FILE* GetFile ( void );

	const char* GetStdName ( void );
	const char* GetOSName ( void );

	const char* GetFileName ( void );

	const char* GetExtension ( void );
	
	const char* GetFullOSPath ( void );

        const char* GetOSFileDir ( void );

	bool IsOpen ( void );

	int Read ( void* data, int size, int count = 1 );
	unsigned char ReadChar ( void );
	int ScanChar ( unsigned char *pChar );
	const char* ScanStr ( void );
	const char* ReadLine ( void );
	int Write ( const void* data, int size );
	void Flush ( void );

	int Seek ( teFilePos ePos, int iOffset );
	unsigned int Size ( void );
	unsigned int Tell ( void );

	void SetUseGlobalPath ( bool use = false );
protected:
	class COSFileInfo;
	COSFileInfo		*info;
};


class COSDir
{
public:
	COSDir();
	COSDir( const COSDir &r);
	COSDir& operator = (const COSDir &r);
	COSDir( const char* szDirName );
	~COSDir();

	void SetStdDir ( const char* szDirName );
	void SetOSDir ( const char* szDirName );

	void MakeStdDir ( const char* szDirName );
	void MakeOSDir ( const char* szDirName );

	bool GetNextFile ( COSFile &oFile, bool bRecursive );
	bool GetNextFile ( COSFile &oFile, const char* fileMask, bool bRecursive );

	int GetFileScanCount ( void );

	const char* GetStdName ( void );
	const char* GetOSName ( void );
	const char* GetFullOSPath ( void );

	const char* GetOSFileDir ( void );

protected:
	class COSDirInfo;
	COSDirInfo		*info;

	bool WindowsAddFileStack ( const char *szPathName, const char* fileMask , bool bRecursive );
	bool LinuxAddFileStack( const char *szPathName, const char* fileMask , bool bRecursive);
};


#endif//_OSFILE_H_
