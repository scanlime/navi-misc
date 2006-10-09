#!/bin/bash
#
# This script packages up .zip and .exe binary releases of
# Fyre for windows. It requires access to the cross-compilation
# environment to yank out binaries, and it needs Wine installed
# to run the NSIS installer.
#
# This is a modified version of make-runtime.sh from
# the Workrave project.
#

ALL_LINGUAS=""
ALL_DOCS="AUTHORS BUGS COPYING README TODO ChangeLog"

FYREDIR=..
OUTPUTDIR=output
STAGINGDIR=staging

# The NSIS installer, running under wine. This points
# to a trivial wrapper script with the right paths.
NSIS=$HOME/bin/nsis

. $HOME/bin/win32-cross

rm -Rf $OUTPUTDIR $STAGINGDIR


################ Versioning

# Check the current version number
VERSION=`./extract-var.py $FYREDIR/configure.ac VERSION`
TARGET_NAME=fyre-$VERSION

# If this is a svn version, not a release, add a date stamp
if (echo $VERSION | grep -q svn); then
    TARGET_NAME=$TARGET_NAME-`date +%Y%m%d`
fi

TARGETDIR=$STAGINGDIR/$TARGET_NAME


################ Fyre itself

# Copy data files, as marked in Makefile.am
DATAFILES=`./extract-var.py $FYREDIR/data/Makefile.am fyredata_DATA`
mkdir -p $TARGETDIR/data
for datafile in $DATAFILES; do
    cp -av $FYREDIR/data/$datafile $TARGETDIR/data/
done

# Copy and strip the binary itself
mkdir -p $TARGETDIR/lib
cp -av $FYREDIR/src/fyre.exe $TARGETDIR/lib
$STRIP $TARGETDIR/lib/fyre.exe

# Documentation. This adds a .txt extension and converts newlines
for doc in $ALL_DOCS; do
    cat $FYREDIR/$doc | sed 's/$/\r/' > $TARGETDIR/$doc.txt
done

# Add some basic links directly to the target directory.
# These will show up in "Program Files", and in the .zip
# distributions. They'll be enough to start Fyre, and to make
# it a little more obvious that the paths won't be correct if
# you run fyre.exe directly. Note that the links used in the
# start menu are created later by NSIS.

./winlink.py "$TARGETDIR/Fyre $VERSION.lnk" lib/fyre.exe || exit 1
./winlink.py "$TARGETDIR/Fyre Rendering Server.lnk" lib/fyre.exe -vr || exit 1


################ Dependencies

function copy_dir()
{
    sourcedir=$1;
    source=$2
    dest=$3;

    prefix=`dirname $source`
    mkdir -p $TARGETDIR/$dest/$prefix
    cp -av $PREFIX/$sourcedir/$source $TARGETDIR/$dest/$prefix || exit 1
}

#copy_dir  bin    gnet-2.0.dll                                   lib
copy_dir  bin    libglade-2.0-0.dll                             lib
copy_dir  bin    libxml2.dll                                    lib

copy_dir  etc    gtk-2.0                                        etc
copy_dir  etc    pango						etc
copy_dir  bin    zlib1.dll					lib
copy_dir  bin    iconv.dll					lib
copy_dir  bin    intl.dll                           		lib
copy_dir  bin    libpng12.dll                         		lib
copy_dir  bin    libatk-1.0-0.dll                   		lib
copy_dir  bin    libgdk-win32-2.0-0.dll             		lib
copy_dir  bin    libgdk_pixbuf-2.0-0.dll            		lib
copy_dir  bin    libglib-2.0-0.dll                  		lib
copy_dir  bin    libgmodule-2.0-0.dll               		lib
copy_dir  bin    libgobject-2.0-0.dll               		lib
copy_dir  bin    libgthread-2.0-0.dll               		lib
copy_dir  bin    libgtk-win32-2.0-0.dll             		lib
copy_dir  bin    libpango-1.0-0.dll                 		lib
copy_dir  bin    libpangoft2-1.0-0.dll              		lib
copy_dir  bin    libpangowin32-1.0-0.dll            		lib
copy_dir  lib    gtk-2.0/2.4.0/immodules/                       lib
copy_dir  lib    gtk-2.0/2.4.0/loaders/libpixbufloader-png.dll  lib
copy_dir  lib    gtk-2.0/2.4.0/engines/libwimp.dll              lib
copy_dir  lib    pango/1.5.0/modules      			lib
for lang in $ALL_LINGUAS; do
    copy_dir lib locale/$lang lib
done

find $TARGETDIR -name "*.dll" -not -name "iconv.dll" -not \
    -name "intl.dll" -not -name "libwimp.dll" -not -name "libxml2.dll" \
    -print | xargs $STRIP

# Customize the gtkrc
cp gtkrc-win32 $TARGETDIR/etc/gtk-2.0/gtkrc || exit 1


################ Generate a .zip file

mkdir -p $OUTPUTDIR
(cd $STAGINGDIR; zip -r temp.zip $TARGET_NAME) || exit 1
 mv $STAGINGDIR/temp.zip $OUTPUTDIR/$TARGET_NAME.zip || exit 1


################ Generate an NSIS installer

$NSIS - <<EOF

    ; Use the trendy Modern UI
    !include "MUI.nsh"

    Name "Fyre"

    OutFile "$OUTPUTDIR\\$TARGET_NAME.exe"

    ; The default installation directory
    InstallDir \$PROGRAMFILES\\Fyre

    ; Registry key to check for directory (so if you install again, it will
    ; overwrite the old one automatically)
    InstallDirRegKey HKLM Software\\Fyre "Install_Dir"

    ; LZMA gives us very nice compression ratios- about 0.5MB smaller than the .zip
    SetCompressor /FINAL lzma

    ; ---- UI Settings

    !define MUI_HEADERIMAGE
    !define MUI_WELCOMEFINISHPAGE_BITMAP "images\installer-swirl.bmp"
    !define MUI_UNWELCOMEFINISHPAGE_BITMAP "\${NSISDIR}\\Contrib\\Graphics\\Wizard\\orange-uninstall.bmp"
    !define MUI_COMPONENTSPAGE_SMALLDESC

    ; ---- Pages

    !define MUI_WELCOMEPAGE_TEXT "This wizard will install Fyre $VERSION.\\r\\n\\r\\nFyre is Open Source software, licensed under the GNU General Public License. We won't ask you to agree to it, because it doesn't restrict your rights in using our software. If you are a developer and wish to modify Fyre or use parts of it in other projects, the GPL grants you rights to do so as long as you pass those rights on to others. For details, see the full text of the license in your Start menu after installation.\\r\\n\\r\\n \$_CLICK"

    !insertmacro MUI_PAGE_WELCOME
    !insertmacro MUI_PAGE_COMPONENTS
    !insertmacro MUI_PAGE_DIRECTORY
    !insertmacro MUI_PAGE_INSTFILES

    !define MUI_FINISHPAGE_LINK "Visit the Fyre web site for the latest news and updates"
    !define MUI_FINISHPAGE_LINK_LOCATION "http://fyre.navi.cx"

    !define MUI_FINISHPAGE_SHOWREADME "\$INSTDIR\\README.txt"

    !insertmacro MUI_PAGE_FINISH

    !insertmacro MUI_UNPAGE_WELCOME
    !insertmacro MUI_UNPAGE_CONFIRM
    !insertmacro MUI_UNPAGE_INSTFILES

    ; ---- Installer languages

    !insertmacro MUI_LANGUAGE "English"

    ; ---- Sections

    Section "Fyre"

      SectionIn RO

      SetOutPath \$INSTDIR
      File /r "$TARGETDIR\\*.*"

      ; Write the installation path into the registry
      WriteRegStr HKLM Software\Fyre "Install_Dir" "\$INSTDIR"

      ; Write the uninstall keys for Windows
      WriteRegStr HKLM "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Fyre" "DisplayName" "Fyre"
      WriteRegStr HKLM "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\Fyre" "UninstallString" '"\$INSTDIR\\uninstall.exe"'
      WriteRegDWORD HKLM "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Fyre" "NoModify" 1
      WriteRegDWORD HKLM "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Fyre" "NoRepair" 1
      WriteUninstaller "uninstall.exe"

      ; Fyre owns all .fa (Fyre Animation) files, give them a nice icon and command
      WriteRegStr HKCR ".fa" "" "Fyre.fa"
      WriteRegStr HKCR ".fa" "Content Type" "application/x-fyre-animation"
      WriteRegStr HKCR "Fyre.fa" "" "Fyre Animation"
      WriteRegStr HKCR "Fyre.fa\\shell" "" "Open"
      WriteRegStr HKCR "Fyre.fa\\shell\\Open\\command" "" '\$INSTDIR\\lib\\fyre.exe --chdir "\$INSTDIR" -n "%1"'
      WriteRegStr HKCR "Fyre.fa\\DefaultIcon" "" '\$INSTDIR\\lib\\fyre.exe,1'

      ; Let the user open .png files in Fyre easily, but don't make it the default
      WriteRegStr HKCR "pngfile\\shell" "" "open"
      WriteRegStr HKCR "pngfile\\shell\\Open in Fyre\\command" "" '\$INSTDIR\\lib\\fyre.exe --chdir "\$INSTDIR" -i "%1"'

    SectionEnd

    Section "Start Menu Shortcuts"

      CreateDirectory "\$SMPROGRAMS\\Fyre"
      CreateShortCut "\$SMPROGRAMS\\Fyre\\Uninstall.lnk" "\$INSTDIR\\uninstall.exe"
      CreateShortCut "\$SMPROGRAMS\\Fyre\\Read Me.lnk" "\$INSTDIR\\README.txt"
      CreateShortCut "\$SMPROGRAMS\\Fyre\\License.lnk" "\$INSTDIR\\COPYING.txt"
      CreateShortCut "\$SMPROGRAMS\\Fyre\\Fyre $VERSION.lnk" "\$INSTDIR\\lib\\fyre.exe"
      CreateShortCut "\$SMPROGRAMS\\Fyre\\Fyre Rendering Server.lnk" "\$INSTDIR\\lib\\fyre.exe" "-vr"

    SectionEnd

    Section "Desktop Shortcut"

      CreateShortCut "\$DESKTOP\\Fyre.lnk" "\$INSTDIR\\lib\\fyre.exe"

    SectionEnd

    ;--------------------------------

    ; Uninstaller

    Section "Uninstall"

      ; Remove registry keys
      DeleteRegKey HKLM "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Fyre"
      DeleteRegKey HKLM SOFTWARE\\Fyre

      DeleteRegKey HKCR ".fa"
      DeleteRegKey HKCR "Fyre.fa"
      DeleteRegKey HKCR "pngfile\\shell\\Open in Fyre"

      ; Remove directories used
      RMDir /r "\$SMPROGRAMS\\Fyre"
      RMDir /r "\$INSTDIR"

      Delete "\$DESKTOP\\Fyre.lnk"

    SectionEnd

EOF


### The End ###
