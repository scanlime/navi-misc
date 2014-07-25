# Microsoft Developer Studio Project File - Name="geometry" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=geometry - Win32 SDL_Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "geometry.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "geometry.mak" CFG="geometry - Win32 SDL_Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "geometry - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "geometry - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "geometry - Win32 SDL_Release" (based on "Win32 (x86) Static Library")
!MESSAGE "geometry - Win32 SDL_Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "geometry - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\geometry"
# PROP Intermediate_Dir "..\..\src\geometry\Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\include" /I "..\..\win32" /I ".\\" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "geometry - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\geometry\Debug"
# PROP Intermediate_Dir "..\..\src\geometry\Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\include" /I "..\..\win32" /I ".\\" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "geometry - Win32 SDL_Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "geometry___Win32_SDL_Release"
# PROP BASE Intermediate_Dir "geometry___Win32_SDL_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\src\geometry\SDL_Release"
# PROP Intermediate_Dir "..\..\src\geometry\SDL_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "..\..\include" /I "..\..\win32" /I ".\\" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\include" /I "..\..\win32" /I ".\\" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "HAVE_SDL" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "geometry - Win32 SDL_Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "geometry___Win32_SDL_Debug"
# PROP BASE Intermediate_Dir "geometry___Win32_SDL_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\src\geometry\SDL_Debug"
# PROP Intermediate_Dir "..\..\src\geometry\SDL_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\include" /I "..\..\win32" /I ".\\" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\include" /I "..\..\win32" /I ".\\" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "HAVE_SDL" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "geometry - Win32 Release"
# Name "geometry - Win32 Debug"
# Name "geometry - Win32 SDL_Release"
# Name "geometry - Win32 SDL_Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "tank"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\geometry\models\tank\high_barrel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\geometry\models\tank\high_body.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\geometry\models\tank\high_ltread.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\geometry\models\tank\high_rtread.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\geometry\models\tank\high_turret.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\geometry\models\tank\low_barrel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\geometry\models\tank\low_body.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\geometry\models\tank\low_ltread.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\geometry\models\tank\low_rtread.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\geometry\models\tank\low_turret.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\geometry\models\tank\medium_barrel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\geometry\models\tank\medium_body.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\geometry\models\tank\medium_ltread.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\geometry\models\tank\medium_rtread.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\geometry\models\tank\medium_turret.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\src\geometry\BillboardSceneNode.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\geometry\BoltSceneNode.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\geometry\EighthDBaseSceneNode.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\geometry\EighthDBoxSceneNode.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\geometry\EighthDimSceneNode.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\geometry\EighthDPyrSceneNode.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\geometry\FlagSceneNode.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\geometry\FlagWarpSceneNode.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\geometry\LaserSceneNode.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\geometry\PolyWallSceneNode.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\geometry\PTSceneNode.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\geometry\QuadWallSceneNode.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\geometry\Ray.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\geometry\SceneNode.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\geometry\ShellSceneNode.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\geometry\SphereSceneNode.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\geometry\TankSceneNode.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\geometry\TracerSceneNode.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\geometry\TriWallSceneNode.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\geometry\ViewFrustum.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\geometry\WallSceneNode.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\include\BillboardSceneNode.h
# End Source File
# Begin Source File

SOURCE=..\..\include\BoltSceneNode.h
# End Source File
# Begin Source File

SOURCE=..\..\include\BZDBCache.h
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

SOURCE=..\..\include\common.h
# End Source File
# Begin Source File

SOURCE=.\config.h
# End Source File
# Begin Source File

SOURCE=..\..\include\EighthDBaseSceneNode.h
# End Source File
# Begin Source File

SOURCE=..\..\include\EighthDBoxSceneNode.h
# End Source File
# Begin Source File

SOURCE=..\..\include\EighthDimSceneNode.h
# End Source File
# Begin Source File

SOURCE=..\..\include\EighthDPyrSceneNode.h
# End Source File
# Begin Source File

SOURCE=..\..\include\FlagSceneNode.h
# End Source File
# Begin Source File

SOURCE=..\..\include\FlagWarpSceneNode.h
# End Source File
# Begin Source File

SOURCE=..\..\include\global.h
# End Source File
# Begin Source File

SOURCE=..\..\include\LaserSceneNode.h
# End Source File
# Begin Source File

SOURCE=..\..\include\OpenGLGState.h
# End Source File
# Begin Source File

SOURCE=..\..\include\OpenGLLight.h
# End Source File
# Begin Source File

SOURCE=..\..\include\OpenGLMaterial.h
# End Source File
# Begin Source File

SOURCE=..\..\include\OpenGLTexture.h
# End Source File
# Begin Source File

SOURCE=..\..\include\PolyWallSceneNode.h
# End Source File
# Begin Source File

SOURCE=..\..\include\PTSceneNode.h
# End Source File
# Begin Source File

SOURCE=..\..\include\QuadWallSceneNode.h
# End Source File
# Begin Source File

SOURCE=..\..\include\Ray.h
# End Source File
# Begin Source File

SOURCE="C:\Program Files\Microsoft SDK\include\Reason.h"
# End Source File
# Begin Source File

SOURCE=..\..\include\RenderNode.h
# End Source File
# Begin Source File

SOURCE=..\..\include\SceneNode.h
# End Source File
# Begin Source File

SOURCE=..\..\include\SceneRenderer.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ShellSceneNode.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ShotSceneNode.h
# End Source File
# Begin Source File

SOURCE=..\..\include\Singleton.h
# End Source File
# Begin Source File

SOURCE=..\..\include\SphereSceneNode.h
# End Source File
# Begin Source File

SOURCE=..\..\include\StateDatabase.h
# End Source File
# Begin Source File

SOURCE=..\..\include\TankSceneNode.h
# End Source File
# Begin Source File

SOURCE=..\..\include\TracerSceneNode.h
# End Source File
# Begin Source File

SOURCE=..\..\include\TriWallSceneNode.h
# End Source File
# Begin Source File

SOURCE=..\..\include\ViewFrustum.h
# End Source File
# Begin Source File

SOURCE=..\..\include\WallSceneNode.h
# End Source File
# Begin Source File

SOURCE=..\..\include\win32.h
# End Source File
# End Group
# End Target
# End Project
