#!/bin/sh
# Run this to generate all the initial makefiles, etc.

DIE=0

# SETUP_GETTEXT=./setup-gettext
PACKAGE=fyre

echo "Generating configuration files for $PACKAGE, please wait..."

# ($SETUP_GETTEXT --gettext-tool) < /dev/null > /dev/null 2>&1 || {
# 	echo
# 	echo "You must have gettext installed to compile $PACKAGE."
# 	DIE=1
# }

(autoconf --version) < /dev/null > /dev/null 2>&1 || {
	echo
	echo "You must have autoconf installed to compile $PACKAGE."
	echo "Download the appropriate package for your distribution,"
	echo "or get the source tarball at ftp://ftp.gnu.org/pub/gnu/"
	DIE=1
}

# (libtool --version) < /dev/null > /dev/null 2>&1 || {
# 	echo
# 	echo "You must have libtool installed to compile $PACKAGE."
# 	echo "Download the appropriate package for your distribution,"
# 	echo "or get the source tarball at ftp://ftp.gnu.org/pub/gnu/"
# 	DIE=1
# }

(automake --version) < /dev/null > /dev/null 2>&1 || {
	echo
	echo "You must have automake installed to compile $PACKAGE."
	echo "Download the appropriate package for your distribution,"
	echo "or get the source tarball at ftp://ftp.gnu.org/pub/gnu/"
	DIE=1
}

[ $DIE -eq 1 ] && exit 1;

# Backup the po/ChangeLog. This should prevent the annoying
# gettext ChangeLog modifications.

# if test -e po/ChangeLog; then
# 	cp -p po/ChangeLog po/ChangeLog.save
# fi

# echo "Running gettextize, please ignore non-fatal messages...."
# $SETUP_GETTEXT

# Restore the po/ChangeLog file.
# if test -e po/ChangeLog.save; then
# 	mv po/ChangeLog.save po/ChangeLog
# fi

# echo "  libtoolize --copy --force"
# libtoolize --copy --force
echo "  aclocal $ACLOCAL_FLAGS"
aclocal $ACLOCAL_FLAGS
echo "  autoheader"
autoheader
echo "  automake --add-missing"
automake --add-missing
echo "  autoconf"
autoconf

if [ -x config.status -a -z "$*" ]; then
	./config.status --recheck
else
	if test -z "$*"; then
		echo "I am going to run ./configure with no arguments - if you wish"
		echo "to pass any to it, please specify them on the $0  command line."
		echo "If you do not wish to run ./configure, press  Ctrl-C now."
		trap 'echo "configure aborted" ; exit 0' 1 2 15
		sleep 1
	fi
	./configure "$@";
fi
