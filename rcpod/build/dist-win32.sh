#!/bin/bash
#
# This script packages up a .exe binary release of pyrcpod, rtgraph,
# and the pyrcpod examples. This isn't a self-contained package,
# but it does provide links to its dependencies. It requires
# a copy of NSIS and Wine.
#
# TODO:
#  - Actually check for dependencies, especially the current version of Python.
#  - Cross-compile _librcpod.dll instead of using a prepackaged version.
#

RCPOD_BIN=bin/librcpod-r4768.dll
DRIVER_BIN=bin/driver
PYRCPOD_SRC=../pyrcpod
EXAMPLES_SRC=../examples
RTGRAPH_SRC=../../rtgraph

OUTPUTDIR=output
STAGINGDIR=staging

# The NSIS installer, running under wine. This points
# to a trivial wrapper script with the right paths.
NSIS=$HOME/bin/nsis

. $HOME/bin/win32-cross

rm -Rf $OUTPUTDIR $STAGINGDIR
mkdir -p $OUTPUTDIR


################ Versioning

VERSION=`date +%Y%m%d`
TARGET_NAME=pyrcpod-$VERSION
TARGETDIR=$STAGINGDIR/$TARGET_NAME


################ Assemble the distribution

mkdir -p $TARGETDIR/py/pyrcpod
mkdir -p $TARGETDIR/py/pyrcpod/i2c
mkdir -p $TARGETDIR/py/rtgraph
mkdir -p $TARGETDIR/bin
mkdir -p $TARGETDIR/bin/data
mkdir -p $TARGETDIR/driver

(cd ..; scons)

cp $RCPOD_BIN $TARGETDIR/py/pyrcpod/_librcpod.dll

cp $PYRCPOD_SRC/*.py $TARGETDIR/py/pyrcpod/
cp $PYRCPOD_SRC/i2c/*.py $TARGETDIR/py/pyrcpod/i2c/
cp $RTGRAPH_SRC/rtgraph/*.py $TARGETDIR/py/rtgraph/
cp $RTGRAPH_SRC/Tweak.py $TARGETDIR/py/

cp $EXAMPLES_SRC/*.py $TARGETDIR/bin
cp $EXAMPLES_SRC/data/*.glade $TARGETDIR/bin/data

cp $DRIVER_BIN/* $TARGETDIR/driver


################ Generate an NSIS installer

PYTHONDIR="C:\\Python23"

$NSIS - <<EOF

    ; Use the trendy Modern UI
    !include "MUI.nsh"

    Name "PyRCPOD"

    OutFile "$OUTPUTDIR\\$TARGET_NAME.exe"

    ; The default installation directory
    InstallDir \$PROGRAMFILES\\PyRCPOD

    ; Registry key to check for directory (so if you install again, it will
    ; overwrite the old one automatically)
    InstallDirRegKey HKLM Software\\PyRCPOD "Install_Dir"

    SetCompressor /FINAL lzma

    ; ---- UI Settings

    !define MUI_HEADERIMAGE
    !define MUI_WELCOMEFINISHPAGE_BITMAP "\${NSISDIR}\\Contrib\\Graphics\\Wizard\\orange.bmp"
    !define MUI_UNWELCOMEFINISHPAGE_BITMAP "\${NSISDIR}\\Contrib\\Graphics\\Wizard\\orange-uninstall.bmp"
    !define MUI_COMPONENTSPAGE_SMALLDESC

    ; ---- Pages

    !define MUI_WELCOMEPAGE_TEXT "This wizard will install a snapshot release of PyRCPOD and its included example programs.\\r\\n\\r\\nNote that this isn't a self-contained package. You should already have Python 2.3, GTK, and PyGTK installed. Currently Python must be installed into c:\\\\Python23. This installer will install a driver, by default into C:\\\\Program Files\\\\PyRCPOD\\\\driver, that you will need to locate the first time you plug in an RCPOD device.\\r\\n\\r\\n\$_CLICK"

    !insertmacro MUI_PAGE_WELCOME
    !insertmacro MUI_PAGE_COMPONENTS
    !insertmacro MUI_PAGE_DIRECTORY
    !insertmacro MUI_PAGE_INSTFILES
    !insertmacro MUI_PAGE_FINISH

    !insertmacro MUI_UNPAGE_WELCOME
    !insertmacro MUI_UNPAGE_CONFIRM
    !insertmacro MUI_UNPAGE_INSTFILES

    ; ---- Installer languages

    !insertmacro MUI_LANGUAGE "English"

    ; ---- Sections

    Section "Python Packages"

      SectionIn RO

      SetOutPath $PYTHONDIR\\Lib
      File /r "$TARGETDIR\\py\\*.*"
      SetOutPath \$INSTDIR

      ; Write the installation path into the registry
      WriteRegStr HKLM Software\PyRCPOD "Install_Dir" "\$INSTDIR"

      ; Write the uninstall keys for Windows
      WriteRegStr HKLM "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\PyRCPOD" "DisplayName" "PyRCPOD"
      WriteRegStr HKLM "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\PyRCPOD" "UninstallString" '"\$INSTDIR\\uninstall.exe"'
      WriteRegDWORD HKLM "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\PyRCPOD" "NoModify" 1
      WriteRegDWORD HKLM "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\PyRCPOD" "NoRepair" 1
      WriteUninstaller "uninstall.exe"

    SectionEnd

    Section "RCPOD Driver"

      SetOutPath \$INSTDIR\\driver
      File /r "$TARGETDIR\\driver\\*.*"

    SectionEnd

    Section "Example Programs"

      SetOutPath \$INSTDIR
      File /r "$TARGETDIR\\bin\\*.*"

      ; Create shortcuts to only the examples that require no command line input
      CreateDirectory "\$SMPROGRAMS\\PyRCPOD"
      CreateShortCut "\$SMPROGRAMS\\PyRCPOD\\Uninstall.lnk" "\$INSTDIR\\uninstall.exe"
      CreateShortCut "\$SMPROGRAMS\\PyRCPOD\\Identify Devices.lnk" "$PYTHONDIR\\python.exe" '"\$INSTDIR\\identify_devs.py"'
      CreateShortCut "\$SMPROGRAMS\\PyRCPOD\\Pulse Counter.lnk" "$PYTHONDIR\\pythonw.exe" '"\$INSTDIR\\pulse_counter.py"'
      CreateShortCut "\$SMPROGRAMS\\PyRCPOD\\PWM Controller.lnk" "$PYTHONDIR\\pythonw.exe" '"\$INSTDIR\\pwm_gui.py"'
      CreateShortCut "\$SMPROGRAMS\\PyRCPOD\\Internal ADC Graph.lnk" "$PYTHONDIR\\pythonw.exe" '"\$INSTDIR\\adc_graph.py"'
      CreateShortCut "\$SMPROGRAMS\\PyRCPOD\\DB-50 ADC Graph.lnk" "$PYTHONDIR\\pythonw.exe" '"\$INSTDIR\\db50_adc_graph.py"'

    SectionEnd

    ;--------------------------------

    ; Uninstaller

    Section "Uninstall"

      ; Remove registry keys
      DeleteRegKey HKLM "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\PyRCPOD"
      DeleteRegKey HKLM SOFTWARE\\PyRCPOD

      ; Remove directories used
      RMDir /r "\$INSTDIR"
      RMDir /r "\$SMPROGRAMS\\PyRCPOD"
      RMDir /r "$PYTHONDIR\\Lib\\pyrcpod"
      RMDir /r "$PYTHONDIR\\Lib\\rtgraph"

    SectionEnd

EOF


### The End ###
