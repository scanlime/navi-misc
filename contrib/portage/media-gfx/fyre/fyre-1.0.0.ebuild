# Copyright 1999-2003 Gentoo Technologies, Inc.
# Distributed under the terms of the GNU General Public License v2
# $Header: $

inherit eutils fdo-mime

DESCRIPTION="Fyre is a tool for computational art based on a chaotic map"
SRC_URI="http://flapjack.navi.cx/releases/${PN}/${PN}-${PV}.tar.bz2"
HOMEPAGE="http://fyre.navi.cx/"

SLOT="0"
LICENSE="GPL-2"
KEYWORDS="~x86"
IUSE="cluster openexr"

DEPEND="media-libs/libpng
	cluster? ( >=net-libs/gnet-2.0 )
	openexr? ( media-libs/openexr )
	dev-util/pkgconfig
	>=dev-libs/glib-2.0
	>=gnome-base/libglade-2.0
	>=x11-libs/gtk+-2.0"

PATCHES="${FILESDIR}/${P}-nomimeupdates.diff
	 ${FILESDIR}/${P}-initscript.diff"

src_unpack() {
	unpack ${A}
    einfo ${S}
	cd ${S}

	# Path the data/Makefile.am file to not do mime-updates
	# on install.  We take care of that later.
	epatch ${FILESDIR}/${P}-nomimeupdates.diff || die

	# Patch the initscript to drop to nobody before running.
	# Also change the daemon path from /usr/local/bin/fyre to
	# what it actually is.
	epatch ${FILESDIR}/${P}-initscript.diff || die

	# Update the makefiles
	aclocal
	automake
	autoconf
}

src_compile() {
	use cluster || myconf="${myconf} --disable-gnet"
	use openexr || myconf="${myconf} --disable-openexr"

	econf \
		--disable-dependency-tracking \
		${myconf} || die

	emake || die "compile problem"
}

src_install() {
	make DESTDIR=${D} install || die
	dodoc AUTHORS COPYING ChangeLog README NEWS BUGS

	exeinto /etc/init.d
	newexe ${S}/contrib/fyre.gentoo-initscript fyre 
}

pkg_postinst() {
	update-mime-database /usr/share/mime
}

