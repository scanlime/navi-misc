# Copyright 1999-2005 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

inherit cvs flag-o-matic eutils

DESCRIPTION="Graphical IRC client based on X-Chat, remade to fit HID standards"
HOMEPAGE="http://xchat-gnome.navi.cx"
SRC_URI=""

ECVS_SERVER="cvs.sf.net:/cvsroot/xchat"
ECVS_USER="anonymous"
ECVS_PASS=""
ECVS_MODULE="xchat2"
S="${WORKDIR}/${ECVS_MODULE}"

LICENSE="GPL-2"
SLOT="2"
KEYWORDS="~x86 ~sparc ~hppa ~ppc ~alpha ~amd64 ~ia64 ~mips ~ppc64"
IUSE="perl tcltk python ssl mmx ipv6 nls xchattext xchatnogtk xchatnognome"

# Added for to fix a sparc seg fault issue by Jason Wever <weeve@gentoo.org>
if [ ${ARCH} = "sparc" ]
then
	replace-flags "-O[3-9]" "-O2"
fi

RDEPEND=">=dev-libs/glib-2.0.3
	!xchatnogtk? ( >=x11-libs/gtk+-2.4 )
	ssl? ( >=dev-libs/openssl-0.9.6d )
	perl? ( >=dev-lang/perl-5.6.1 )
	python? ( dev-lang/python )
	tcltk? ( dev-lang/tcl )
	!xchatnognome? ( >=gnome-base/gnome-2.6 )"

DEPEND="${RDEPEND}
	>=dev-util/pkgconfig-0.7
	>=sys-apps/sed-4
	!net-irc/xchat
	!net-irc/xchat-gnome
	nls? ( sys-devel/gettext )"

src_unpack() {
	cvs_src_unpack
	cd ${S}

	# Point svn at some garbage directory rather than
	# using /root/.subversion for config, since that breaks
	# in the sandbox.
	SVN=svn --config-dir .

	epatch ${FILESDIR}/${PN}-fe-text.patch
	$SVN cat http://navi.cx/svn/misc/trunk/xchat-gnome/buildsystem.diff | patch -p0
	$SVN co http://navi.cx/svn/misc/trunk/xchat-gnome/plugins
	pushd src
	$SVN co http://navi.cx/svn/misc/trunk/xchat-gnome/fe-gnome/
	popd
	./autogen.sh
}

src_compile() {

	# xchat's configure script uses sys.path to find library path
	# instead of python-config (#25943)
	unset PYTHONPATH

	econf \
		$(use_enable ssl openssl) \
		$(use_enable perl) \
		$(use_enable python) \
		$(use_enable tcltk tcl) \
		$(use_enable mmx) \
		$(use_enable ipv6) \
		$(use_enable nls) \
		$(use_enable xchattext textfe) \
		$(use_enable !xchatnogtk gtkfe) \
		$(use_enable !xchatnognome gnomefe) \
		--program-suffix=-2 \
		|| die "Configure failed"

	emake || die "Compile failed"

}

src_install() {

	# some magic to create a menu entry for xchat 2
	sed -i \
		-e "s:^Exec=xchat$:Exec=xchat-2:" \
		-e "s:Name=XChat IRC:Name=XChat 2 Gnome IRC:" \
		xchat.desktop

	make DESTDIR=${D} install || die "Install failed"

	# install plugin development header
	insinto /usr/include/xchat
	doins src/common/xchat-plugin.h || die "doins failed"

	dodoc ChangeLog README* || die "dodoc failed"

}
