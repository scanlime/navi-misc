# Microsoft Developer Studio Project File - Name="ocadmin" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=ocadmin - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ocadmin.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ocadmin.mak" CFG="ocadmin - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ocadmin - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "ocadmin - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ocadmin - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\src\ocadmin"
# PROP BASE Intermediate_Dir "..\..\src\ocadmin"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\ocadmin"
# PROP Intermediate_Dir "..\..\src\ocadmin\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\win32" /I "..\..\include" /I "..\..\src\opencomabat" /I ".\\" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o "..\..\src\ocadmin/ocadmin.bsc"
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 ws2_32.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "ocadmin - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\src\ocadmin\Debug"
# PROP BASE Intermediate_Dir "..\..\src\ocadmin\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\ocadmin\Debug"
# PROP Intermediate_Dir "..\..\src\ocadmin\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\win32" /I "..\..\include" /I "..\..\src\opencomabat" /I ".\\" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o "..\..\src\ocadmin\Debug/ocadmin.bsc"
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ws2_32.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "ocadmin - Win32 Release"
# Name "ocadmin - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\src\ocadmin\AutoCompleter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ocadmin\bzadmin.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ocadmin\BZAdminClient.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ocadmin\BZAdminUI.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ocadmin\OptionParser.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\opencomabat\ServerLink.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ocadmin\StdBothUI.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ocadmin\StdInUI.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ocadmin\StdOutUI.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ocadmin\UIMap.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\include\Address.h
# End Source File
# Begin Source File

SOURCE=..\..\src\ocadmin\AutoCompleter.h
# End Source File
# Begin Source File

SOURCE=..\..\include\BaseBuilding.h
# End Source File
# Begin Source File

SOURCE=..\..\include\BoxBuilding.h
# End Source File
# Begin Source File

SOURCE=..\..\include\Bundle.h
# End Source File
# Begin Source File

SOURCE=..\..\include\BundleMgr.h
# End Source File
# Begin Source File

SOURCE=..\..\src\ocadmin\BZAdminClient.h
# End Source File
# Begin Source File

SOURCE=..\..\src\ocadmin\BZAdminUI.h
# End Source File
# Begin Source File

SOURCE=..\..\include\BzfEvent.h
# End Source File
# Begin Source File

SOURCE=..\..\include\bzfgl.h
# End Source File
# Begin Source File

SOURCE=..\..\include\bzfio.h
# End Source File
# Begin Source File

SOURCE=..\..\include\CallbackList.h
# End Source File
# Begin Source File

SOURCE=..\..\src\ocadmin\colors.h
# End Source File
# Begin Source File

SOURCE=..\..\include\common.h
# End Source File
# Begin Source File

SOURCE=.\config.h
# End Source File
# Begin Source File

SOURCE=..\..\src\ocadmin\CursesUI.h
# End Source File
# Begin Source File

SOURCE=..\..\include\EighthDimSceneNode.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ErrorHandler.h
# End Source File
# Begin Source File

SOURCE=..\..\include\Flag.h
# End Source File
# Begin Source File

SOURCE=..\..\include\FlagSceneNode.h
# End Source File
# Begin Source File

SOURCE=..\..\include\global.h
# End Source File
# Begin Source File

SOURCE=..\..\src\opencomabat\LocalPlayer.h
# End Source File
# Begin Source File

SOURCE=..\..\include\Obstacle.h
# End Source File
# Begin Source File

SOURCE=..\..\include\OpenGLGState.h
# End Source File
# Begin Source File

SOURCE=..\..\include\OpenGLTexture.h
# End Source File
# Begin Source File

SOURCE=..\..\src\ocadmin\OptionParser.h
# End Source File
# Begin Source File

SOURCE=..\..\include\Pack.h
# End Source File
# Begin Source File

SOURCE=..\..\src\ocadmin\pdcurses_adapter.h
# End Source File
# Begin Source File

SOURCE=..\..\src\opencomabat\Player.h
# End Source File
# Begin Source File

SOURCE=..\..\src\ocadmin\PlayerInfo.h
# End Source File
# Begin Source File

SOURCE=..\..\include\PlayerState.h
# End Source File
# Begin Source File

SOURCE=..\..\include\PyramidBuilding.h
# End Source File
# Begin Source File

SOURCE=..\..\include\Ray.h
# End Source File
# Begin Source File

SOURCE=..\..\include\RenderNode.h
# End Source File
# Begin Source File

SOURCE=..\..\include\SceneNode.h
# End Source File
# Begin Source File

SOURCE=..\..\src\opencomabat\ServerLink.h
# End Source File
# Begin Source File

SOURCE=..\..\src\opencomabat\ShotPath.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ShotUpdate.h
# End Source File
# Begin Source File

SOURCE=..\..\include\Singleton.h
# End Source File
# Begin Source File

SOURCE=..\..\include\StateDatabase.h
# End Source File
# Begin Source File

SOURCE=..\..\src\ocadmin\StdBothUI.h
# End Source File
# Begin Source File

SOURCE=..\..\src\ocadmin\StdInUI.h
# End Source File
# Begin Source File

SOURCE=..\..\src\ocadmin\StdOutUI.h
# End Source File
# Begin Source File

SOURCE=..\..\include\Team.h
# End Source File
# Begin Source File

SOURCE=..\..\include\Teleporter.h
# End Source File
# Begin Source File

SOURCE=..\..\include\TextUtils.h
# End Source File
# Begin Source File

SOURCE=..\..\include\TimeKeeper.h
# End Source File
# Begin Source File

SOURCE=..\..\src\ocadmin\UIMap.h
# End Source File
# Begin Source File

SOURCE=..\..\include\WallObstacle.h
# End Source File
# Begin Source File

SOURCE=..\..\include\win32.h
# End Source File
# Begin Source File

SOURCE=..\..\src\opencomabat\World.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
