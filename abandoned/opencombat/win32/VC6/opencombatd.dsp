# Microsoft Developer Studio Project File - Name="opencombatd" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=opencombatd - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "opencombatd.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "opencombatd.mak" CFG="opencombatd - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "opencombatd - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "opencombatd - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "opencombatd - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\opencombatd"
# PROP Intermediate_Dir "..\..\src\opencombatd\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\include" /I "..\..\win32" /I ".\\" /D "_CONSOLE" /D "WIN32" /D "NDEBUG" /D "_MBCS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 ws2_32.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /pdb:"../src/opencombatd/opencombatd.pdb" /machine:I386 /nodefaultlib:"LIBCMT"
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy ..\..\src\opencombatd\*.exe ..\..\*.exe
# End Special Build Tool

!ELSEIF  "$(CFG)" == "opencombatd - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\opencombatd\Debug"
# PROP Intermediate_Dir "..\..\src\opencombatd\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /GX /ZI /Od /I "..\..\include" /I "..\..\win32" /I ".\\" /D "_CONSOLE" /D "WIN32" /D "_DEBUG" /D "_MBCS" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ws2_32.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /nodefaultlib:"LIBCMT" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy ..\..\src\opencombatd\debug\*.pdb ..\..\*.pdb	copy  ..\..\src\opencombatd\debug\*.exe ..\..\*.exe
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "opencombatd - Win32 Release"
# Name "opencombatd - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\src\opencombatd\AccessControlList.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\opencombatd\bzfs.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\opencombatd\BZWError.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\opencombatd\BZWReader.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\opencombatd\RecordReplay.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\opencombatd\CmdLineOptions.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\opencombatd\commands.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\opencombatd\CustomBase.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\opencombatd\CustomBox.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\opencombatd\CustomGate.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\opencombatd\CustomLink.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\opencombatd\CustomPyramid.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\opencombatd\CustomWeapon.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\opencombatd\CustomWorld.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\opencombatd\CustomZone.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\opencombatd\EntryZones.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\opencombatd\FlagInfo.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\opencombatd\FlagHistory.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\opencombatd\GameKeeper.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\opencombatd\ListServerConnection.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\opencombatd\Permissions.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\opencombatd\RejoinList.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\opencombatd\Score.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\opencombatd\SpawnPosition.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\opencombatd\TeamBases.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\opencombatd\TextChunkManager.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\opencombatd\WorldFileLocation.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\opencombatd\WorldFileObject.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\opencombatd\WorldFileObstacle.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\opencombatd\WorldInfo.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\opencombatd\WorldWeapons.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\src\opencombatd\AccessControlList.h
# End Source File
# Begin Source File

SOURCE=..\..\include\Address.h
# End Source File
# Begin Source File

SOURCE=..\..\include\BZDBCache.h
# End Source File
# Begin Source File

SOURCE=..\..\include\bzfio.h
# End Source File
# Begin Source File

SOURCE=..\..\src\opencombatd\bzfs.h
# End Source File
# Begin Source File

SOURCE=..\..\include\bzsignal.h
# End Source File
# Begin Source File

SOURCE=..\..\src\opencombatd\BZWError.h
# End Source File
# Begin Source File

SOURCE=..\..\src\opencombatd\BZWReader.h
# End Source File
# Begin Source File

SOURCE=..\..\include\CallbackList.h
# End Source File
# Begin Source File

SOURCE=..\..\src\opencombatd\RecordReplay.h
# End Source File
# Begin Source File

SOURCE=..\..\src\opencombatd\CmdLineOptions.h
# End Source File
# Begin Source File

SOURCE=..\..\include\CommandManager.h
# End Source File
# Begin Source File

SOURCE=..\..\src\opencombatd\commands.h
# End Source File
# Begin Source File

SOURCE=..\..\include\common.h
# End Source File
# Begin Source File

SOURCE=.\config.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ConfigFileManager.h
# End Source File
# Begin Source File

SOURCE=..\..\src\opencombatd\CustomBase.h
# End Source File
# Begin Source File

SOURCE=..\..\src\opencombatd\CustomBox.h
# End Source File
# Begin Source File

SOURCE=..\..\src\opencombatd\CustomGate.h
# End Source File
# Begin Source File

SOURCE=..\..\src\opencombatd\CustomLink.h
# End Source File
# Begin Source File

SOURCE=..\..\src\opencombatd\CustomPyramid.h
# End Source File
# Begin Source File

SOURCE=..\..\src\opencombatd\CustomWeapon.h
# End Source File
# Begin Source File

SOURCE=..\..\src\opencombatd\CustomWorld.h
# End Source File
# Begin Source File

SOURCE=..\..\src\opencombatd\CustomZone.h
# End Source File
# Begin Source File

SOURCE=..\..\include\DelayQueue.h
# End Source File
# Begin Source File

SOURCE=..\..\src\opencombatd\EntryZones.h
# End Source File
# Begin Source File

SOURCE=..\..\include\Flag.h
# End Source File
# Begin Source File

SOURCE=..\..\src\opencombatd\FlagInfo.h
# End Source File
# Begin Source File

SOURCE=..\..\src\opencombatd\FlagHistory.h
# End Source File
# Begin Source File

SOURCE=..\..\src\opencombatd\GameKeeper.h
# End Source File
# Begin Source File

SOURCE=..\..\include\global.h
# End Source File
# Begin Source File

SOURCE=..\..\include\LagInfo.h
# End Source File
# Begin Source File

SOURCE=..\..\src\opencombatd\ListServerConnection.h
# End Source File
# Begin Source File

SOURCE=..\..\include\md5.h
# End Source File
# Begin Source File

SOURCE=..\..\include\multicast.h
# End Source File
# Begin Source File

SOURCE=..\..\include\Pack.h
# End Source File
# Begin Source File

SOURCE=..\..\src\opencombatd\PackVars.h
# End Source File
# Begin Source File

SOURCE=..\..\src\opencombatd\Permissions.h
# End Source File
# Begin Source File

SOURCE=..\..\include\Ping.h
# End Source File
# Begin Source File

SOURCE=..\..\include\PlayerInfo.h
# End Source File
# Begin Source File

SOURCE=..\..\include\PlayerState.h
# End Source File
# Begin Source File

SOURCE=..\..\src\opencombatd\RejoinList.h
# End Source File
# Begin Source File

SOURCE=..\..\src\opencombatd\Score.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ShotUpdate.h
# End Source File
# Begin Source File

SOURCE=..\..\include\Singleton.h
# End Source File
# Begin Source File

SOURCE=..\..\src\opencombatd\SpawnPosition.h
# End Source File
# Begin Source File

SOURCE=..\..\include\StateDatabase.h
# End Source File
# Begin Source File

SOURCE=..\..\include\Team.h
# End Source File
# Begin Source File

SOURCE=..\..\src\opencombatd\TeamBases.h
# End Source File
# Begin Source File

SOURCE=..\..\src\opencombatd\TextChunkManager.h
# End Source File
# Begin Source File

SOURCE=..\..\include\TextUtils.h
# End Source File
# Begin Source File

SOURCE=..\..\include\TimeBomb.h
# End Source File
# Begin Source File

SOURCE=..\..\include\TimeKeeper.h
# End Source File
# Begin Source File

SOURCE=..\..\include\VotingArbiter.h
# End Source File
# Begin Source File

SOURCE=..\..\include\VotingBooth.h
# End Source File
# Begin Source File

SOURCE=..\..\include\win32.h
# End Source File
# Begin Source File

SOURCE=..\..\include\WordFilter.h
# End Source File
# Begin Source File

SOURCE=..\..\src\opencombatd\WorldFileLocation.h
# End Source File
# Begin Source File

SOURCE=..\..\src\opencombatd\WorldFileObject.h
# End Source File
# Begin Source File

SOURCE=..\..\src\opencombatd\WorldFileObstacle.h
# End Source File
# Begin Source File

SOURCE=..\..\src\opencombatd\WorldInfo.h
# End Source File
# Begin Source File

SOURCE=..\..\src\opencombatd\WorldWeapons.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
