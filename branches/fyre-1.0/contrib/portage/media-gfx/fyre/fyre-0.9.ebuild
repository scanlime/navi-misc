# Copyright 1999-2003 Gentoo Technologies, Inc.
# Distributed under the terms of the GNU General Public License v2
# $Header: $

inherit eutils fdo-mime

DESCRIPTION="Gyre provides a rendering of the Peter de Jong map."
SRC_URI="http://flapjack.navi.cx/releases/${PN}/${PN}-${PV}.tar.bz2"
HOMEPAGE="http://fyre.navi.cx/"

SLOT="0"
LICENSE="GPL-2"
KEYWORDS="~x86"
IUSE="binreloc binreloc-threads cluster exr"

DEPEND="media-libs/libpng
	cluster? ( >=net-libs/gnet-2.0 )
	exr? ( media-libs/openexr )
	dev-util/pkgconfig
	>=dev-libs/glib-2.0
	>=gnome-base/libglade-2.0
	>=x11-libs/gtk+-2.0"

PATCHES="${FILESDIR}/${P}-nomimeupdates.diff
	 ${FILESDIR}/${P}-initscript.diff"

pkg_setup() {
	if ! use binreloc && use binreloc-threads; then
		ewarn "+binreloc-threads implies +binreloc!"
	fi
}

src_unpack() {
	unpack ${A}
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
	local myconf=""
	use cluster || myconf="${myconf} --disable-gnet"
	use exr     || myconf="${myconf} --disable-openexr"

	(use binreloc || use bin-reloc-threads) && myconf="${myconf} --enable-binreloc"
	use binreloc-threads && myconf="${myconf} --enable-binreloc-threads"

	econf \
		--disable-dependency-tracking \
		${myconf} || die

	emake || die "compile problem"
}

src_install() {
	make DESTDIR=${D} install || die
	dodoc AUTHORS COPYING ChangeLog README NEWS

	exeinto /etc/init.d
	newexe ${S}/contrib/fyre.gentoo-initscript fyre 
}

pkg_postinst() {
	update-mime-database /usr/share/mime
}
