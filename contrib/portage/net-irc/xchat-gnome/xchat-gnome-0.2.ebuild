# Copyright 1999-2005 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

inherit flag-o-matic eutils

DESCRIPTION="Graphical IRC client based on X-Chat, remade to fit HID standards"
SRC_URI="http://navi.cx/releases/${PN}-${PV}.tar.gz"
HOMEPAGE="http://xchat-gnome.navi.cx"

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
	nls? ( sys-devel/gettext )"

src_unpack() {
	unpack ${A}
	cd ${S}

	epatch ${FILESDIR}/${PV}-fe-text.patch
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
		|| die "Configure failed"

	emake || die "Compile failed"

}

src_install() {

	make DESTDIR=${D} install || die "Install failed"

	# install plugin development header
	insinto /usr/include/xchat
	doins src/common/xchat-plugin.h || die "doins failed"

	dodoc ChangeLog README* || die "dodoc failed"

}
