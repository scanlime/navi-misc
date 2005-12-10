#!/bin/sh
# Run this to generate all the initial makefiles, etc.

DIE=0

PACKAGE=fyre
CONFIGURE=configure.ac

echo "Generating configuration files for $PACKAGE, please wait..."

if grep "^AM_[A-Z0-9_]\{1,\}_GETTEXT" "$CONFIGURE" >/dev/null; then
	if grep "sed.*POTFILES" "$CONFIGURE" >/dev/null; then
		GETTEXTIZE=""
	else
		if grep "^AM_GLIB_GNU_GETTEXT" "$CONFIGURE" >/dev/null; then
			GETTEXTIZE="glib-gettextize"
		else
			GETTEXTIZE="gettextize"
		fi

		$GETTEXTIZE --version < /dev/null > /dev/null 2>&1
		if test $? -ne 0; then
			echo
			echo "**Error**: You must have \`$GETTEXTIZE' installed" \
			     "to compile $PACKAGE."
			DIE=1
		fi
	fi
fi
(grep "^AC_PROG_INTLTOOL" "$CONFIGURE" >/dev/null) && {
	(intltoolize --version) < /dev/null > /dev/null 2>&1 || {
		echo
		echo "**Error**: You must have \`intltoolize' installed" \
		     "to compile $PACKAGE."
		DIE=1
	}
}

if test "$GETTEXTIZE"; then
	echo "Creating aclocal.m4 ..."
	test -r aclocal.m4 || touch aclocal.m4
	echo "Running $GETTEXTIZE...  Ignore non-fatal messages."
	echo "no" | $GETTEXTIZE --force --copy
	echo "Making aclocal.m4 writable ..."
	test -r aclocal.m4 && chmod u+w aclocal.m4
fi
if grep "^AC_PROG_INTLTOOL" "$CONFIGURE" >/dev/null; then
	echo "Running intltoolize..."
	intltoolize --copy --force --automake
fi

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

if test -e po/ChangeLog; then
	cp -p po/ChangeLog po/ChangeLog.save
fi

echo "Running gettextize, please ignore non-fatal messages...."
$SETUP_GETTEXT

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
