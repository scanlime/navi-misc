
;
; This is a Fyre installer script for use with the
; Nullsoft Install System (http://nsis.sourceforge.net)
;
; To generate an installer for Fyre, you'll need to have
; this file, NSIS, a compiled fyre.exe, and a compiled
; copy of GTK and friends.
;
; This expects to find fyre's dependencies (gtk, msvcr70,
; pango, glib, gobject, freetype, libglade, config files,
; etc) in a 'fyre-win32-deps' directory. This can be
; downloaded from:
;   http://navi.cx/releases/fyre-win32-deps.zip
;
; Alternatively, you could pull the necessary files out
; of another fyre release, an inkscape release, various
; gtk binary packages, or you could compile them yourself.
;
; This file is based on the examples shipped with NSIS.
;

Name "Fyre"

OutFile "fyre-install.exe"

; The default installation directory
InstallDir $PROGRAMFILES\Fyre

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM Software\Fyre "Install_Dir"

;--------------------------------

Page components
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

;--------------------------------

Section "Fyre"

  SectionIn RO
  
  ; Glade files
  SetOutPath $INSTDIR\data
  File "..\..\data\*.glade"

  ; The Fyre binary itself
  SetOutPath $INSTDIR
  File "..\..\fyre.exe"

  ; Give 'em a README file too, renamed to be windows-friendly
  File /oname=README.txt "..\..\README"

  ; Dependencies
  File /r "fyre-win32-deps\*.*"
  
  ; Write the installation path into the registry
  WriteRegStr HKLM Software\Fyre "Install_Dir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Fyre" "DisplayName" "Fyre"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Fyre" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Fyre" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Fyre" "NoRepair" 1
  WriteUninstaller "uninstall.exe"
  
SectionEnd

Section "Start Menu Shortcuts"

  CreateDirectory "$SMPROGRAMS\Fyre"
  CreateShortCut "$SMPROGRAMS\Fyre\Uninstall.lnk" "$INSTDIR\uninstall.exe"
  CreateShortCut "$SMPROGRAMS\Fyre\Fyre.lnk" "$INSTDIR\fyre.exe"
  CreateShortCut "$SMPROGRAMS\Fyre\README.lnk" "$INSTDIR\README.txt"
  
SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Fyre"
  DeleteRegKey HKLM SOFTWARE\Fyre

  ; Remove directories used
  RMDir /r "$SMPROGRAMS\Fyre"
  RMDir /r "$INSTDIR"

SectionEnd