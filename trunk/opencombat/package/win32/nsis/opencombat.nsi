; opencombat.nsi
;
; This script is based on example1.nsi, but adds uninstall support
; and (optionally) start menu shortcuts.
;
; It will install notepad.exe into a directory that the user selects,
;

!define VER_MAJOR 0.0
!define VER_MINOR .1

; Main Installer Options
Name "OpenCombat"
Icon ..\..\..\win32\opencombat.ico
WindowIcon On
EnabledBitmap "EnableCheck.bmp"
DisabledBitmap "DisableCheck.bmp"
Caption "OpenCombat ${VER_MAJOR}${VER_MINOR}: - Setup"

; The file to write
OutFile "..\..\..\dist\OpenCombat${VER_MAJOR}${VER_MINOR}.exe"

; The default installation directory
InstallDir $PROGRAMFILES\OpenCombat${VER_MAJOR}${VER_MINOR}

; Show the lisense
LicenseText "Please read our following license before installing:"
LicenseData ..\..\..\COPYING

; Registry key to check for directory (so if you install again, it will
; overwrite the old one automatically)
InstallDirRegKey HKLM SOFTWARE\OpenCombat${VER_MAJOR}${VER_MINOR} "Install_Dir"

; The text to prompt the user to enter a directory
ComponentText "This will install the OpenCombat ${VER_MAJOR}${VER_MINOR} game and server files on your computer."

; The text to prompt the user to enter a directory
DirText "Please choose a directory to install into:"

CompletedText " Thank you for installing OpenCombat ${VER_MAJOR}${VER_MINOR}."
; The stuff to install

Section "OpenCombat (required)"
	; Set output path to the installation directory.
	SetOutPath $INSTDIR
	; Put file there
	File ..\..\..\src\OpenCombat\OpenCombat.exe
	File ..\..\..\src\ocadmin\ocadmin.exe
	File ..\..\..\src\OpenCombatd\OpenCombatd.exe

	; make the data dir
	SetOutPath $INSTDIR\data
	File ..\..\..\data\*.*
	File ..\..\..\misc\hix.bzw
	File ..\..\..\misc\bzfs.conf
	File ..\..\..\misc\bzfs_conf.html

	; make the l10n dir
	SetOutPath $INSTDIR\data\l10n
	File ..\..\..\data\l10n\*.*

	; make the doc dir
	SetOutPath $INSTDIR\doc
	File ..\..\..\doc\*.*
	File ..\ReadMe.win32.html
	File ..\..\..\COPYING

	; Write the installation path into the registry
	WriteRegStr HKLM SOFTWARE\OpenCombat "Install_Dir" "$INSTDIR"

	; Write the uninstall keys for Windows
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\OpenCombat${VER_MAJOR}${VER_MINOR}" "DisplayName" "OpenCombat(remove only)"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\OpenCombat${VER_MAJOR}${VER_MINOR}" "UninstallString" '"$INSTDIR\uninstall.exe"'
	WriteUninstaller "uninstall.exe"
SectionEnd

; optional sections
Section "Start Menu Shortcuts"
	; Main start menu shortcuts
	SetOutPath $INSTDIR
	CreateDirectory "$SMPROGRAMS\OpenCombat${VER_MAJOR}${VER_MINOR}"
	CreateShortCut "$SMPROGRAMS\OpenCombat${VER_MAJOR}${VER_MINOR}\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
	CreateShortCut "$SMPROGRAMS\OpenCombat${VER_MAJOR}${VER_MINOR}\OpenCombat ${VER_MAJOR}${VER_MINOR}.lnk" "$INSTDIR\OpenCombat.exe" "" "$INSTDIR\OpenCombat.exe" 0
	CreateShortCut "$SMPROGRAMS\OpenCombat${VER_MAJOR}${VER_MINOR}\OpenCombat ${VER_MAJOR}${VER_MINOR} (Windowed).lnk" "$INSTDIR\OpenCombat.exe"  "-window -geometry 800x600" "$INSTDIR\OpenCombat.exe" 0

	SetOutPath $INSTDIR\data
	CreateDirectory "$SMPROGRAMS\OpenCombat${VER_MAJOR}${VER_MINOR}\Server"
	CreateShortCut "$SMPROGRAMS\OpenCombat${VER_MAJOR}${VER_MINOR}\Server\Start Server (Simple Jump Teleport 1 shot).lnk" "$INSTDIR\OpenCombatd.exe" "-p 5154 -j -t -s 32 +s 16 -h" "$INSTDIR\OpenCombat.exe" 0
	CreateShortCut "$SMPROGRAMS\OpenCombat${VER_MAJOR}${VER_MINOR}\Server\Start Server (Simple Jump Teleport 3 shots).lnk" "$INSTDIR\OpenCombatd.exe" "-p 5155 -j -t -ms 3 -s 32 +s 16 -h" "$INSTDIR\OpenCombat.exe" 0
	CreateShortCut "$SMPROGRAMS\OpenCombat${VER_MAJOR}${VER_MINOR}\Server\Start Server (HIX [Public] FFA).lnk" "$INSTDIR\OpenCombatd.exe" '-p 5156 -j -tkkr 80 -fb -ms 3 -s 32 +s 16 -world HIX.bzw -public "My HIX FFA Server"' "$INSTDIR\OpenCombat.exe" 0
	CreateShortCut "$SMPROGRAMS\OpenCombat${VER_MAJOR}${VER_MINOR}\Server\Start Server (HIX [Public] CTF).lnk" "$INSTDIR\bOpenCombatd.exe" '-p 5157 -c -j -fb -world HIX.bzw -public "My HIX CTF Server"' "$INSTDIR\OpenCombat.exe" 0
	CreateShortCut "$SMPROGRAMS\OpenCombat${VER_MAJOR}${VER_MINOR}\Server\Data Folder.lnk" "$INSTDIR\data" "" "" 0
	CreateShortCut "$SMPROGRAMS\OpenCombat${VER_MAJOR}${VER_MINOR}\Server\OpenCombat Configuration Builder.lnk" "$INSTDIR\data\bzfs_conf.html" "" "" 0
	SetOutPath $INSTDIR\doc
	CreateDirectory "$SMPROGRAMS\OpenCombat${VER_MAJOR}${VER_MINOR}\Doc"
	CreateShortCut "$SMPROGRAMS\OpenCombat${VER_MAJOR}${VER_MINOR}\Doc\OpenCombat [game] Manual Pages (HTML).lnk" "$INSTDIR\doc\OpenCombat.html" "" "" 0
	CreateShortCut "$SMPROGRAMS\OpenCombat${VER_MAJOR}${VER_MINOR}\Doc\OpenCombat [server] Manual Pages (HTML).lnk" "$INSTDIR\doc\bzfs.html" "" "" 0
	CreateShortCut "$SMPROGRAMS\OpenCombat${VER_MAJOR}${VER_MINOR}\Doc\OCadmin [admin] Manual Pages (HTML).lnk" "$INSTDIR\doc\bzadmin.html" "" "" 0
SectionEnd

Section "Quick Launch Shortcuts"
	; shortcut in the "quick launch bar"
	SetOutPath $INSTDIR
	CreateShortCut "$QUICKLAUNCH\OpenCombat${VER_MAJOR}${VER_MINOR}.lnk" "$INSTDIR\OpenCombat.exe" "" "$INSTDIR\OpenCombat.exe" 0
SectionEnd

Function .onInstSuccess
	MessageBox MB_YESNO|MB_ICONQUESTION \
		"Setup has completed. Would you like to view readme file now?" \
		IDNO NoReadme
			ExecShell open '$INSTDIR\doc\ReadMe.win32.html'
		NoReadme:
FunctionEnd

; uninstall stuff
UninstallText "This will uninstall OpenCombat. Please hit next to continue with the removal."

; special uninstall section.
Section "Uninstall"
	; remove registry keys

	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\OpenCombat"
	DeleteRegKey HKLM SOFTWARE\OpenCombat
	; remove files

	Delete $INSTDIR\OpenCombat.exe
	Delete $INSTDIR\OpenCombatd.exe
	Delete $INSTDIR\ocadmin.exe
	Delete $INSTDIR\doc\*.*
	Delete $INSTDIR\data\*.*
	Delete $INSTDIR\data\l10n\*.*

	; MUST REMOVE UNINSTALLER, too
	Delete $INSTDIR\uninstall.exe

	; remove shortcuts, if any.
	Delete "$SMPROGRAMS\OpenCombat${VER_MAJOR}${VER_MINOR}\*.*"
	Delete "$SMPROGRAMS\OpenCombat${VER_MAJOR}${VER_MINOR}\Server\*.*"
	Delete "$SMPROGRAMS\OpenCombat${VER_MAJOR}${VER_MINOR}\Doc\*.*"
	Delete "$$QUICKLAUNCH\OpenCombat${VER_MAJOR}${VER_MINOR}.lnk"

	; remove directories used.
	RMDir "$SMPROGRAMS\OpenCombat${VER_MAJOR}${VER_MINOR}\Server"
	RMDir "$SMPROGRAMS\OpenCombat${VER_MAJOR}${VER_MINOR}\Doc"
	RMDir "$SMPROGRAMS\OpenCombat${VER_MAJOR}${VER_MINOR}"
	RMDir "$INSTDIR\data\l10n"
	RMDir "$INSTDIR\data"
	RMDir "$INSTDIR\doc"
	RMDir "$INSTDIR"
SectionEnd

; eof
