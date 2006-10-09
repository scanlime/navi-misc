%define _sharedir %{_prefix}/share

Name:      fyre
Summary:   gtk2-based explorer for iterated chaotic functions
Requires:  gtk2 gnet2 OpenEXR
BuildRequires:  gtk2-devel gnet2-devel OpenEXR-devel
Version:   1.0.0
Release:   2
License:   GPL
Vendor:    David Trowbridge <trowbrds@cs.colorado.edu> Micah Dowty <micah@navi.cx>
Packager:  Mirco Mueller <macslow@bangang.de>
Group:     System Environment/Libraries
Source:    http://flapjack.navi.cx/releases/fyre/%{name}-%{version}.tar.gz
URL:       http://fyre.navi.cx

%description
Fyre is a tool for producing computational artwork based on histograms
of iterated chaotic functions. At the moment, it implements the Peter
de Jong map in a fixed-function pipeline with an interactive GTK+
frontend and a command line interface for easy and efficient rendering
of high-resolution, high quality images.

%prep
rm -rf $RPM_BUILD_DIR/%{name}-%{version}
tar zxvf $RPM_SOURCE_DIR/%{name}-%{version}.tar.gz
cd $RPM_BUILD_DIR/%{name}-%{version}
./configure --prefix=%{_prefix} --enable-gnet --enable-openexr

%build
cd $RPM_BUILD_DIR/%{name}-%{version}
make

%install
cd $RPM_BUILD_DIR/%{name}-%{version}
make install

%clean

%post

%postun

%files
%defattr(-,root,root)
%{_bindir}/fyre
%{_sharedir}/applications/fyre.desktop
%{_sharedir}/fyre/
%{_sharedir}/fyre/about-box.fa
%{_sharedir}/fyre/animation-render.glade
%{_sharedir}/fyre/explorer.glade
%{_sharedir}/fyre/fyre-16x16.png
%{_sharedir}/fyre/fyre-32x32.png
%{_sharedir}/fyre/fyre-48x48.png
%{_sharedir}/fyre/metadata-emblem.png
%{_sharedir}/icons/hicolor/48x48/mimetypes/application-x-fyre-animation.png
%{_sharedir}/mime/XMLnamespaces
%{_sharedir}/mime/application/x-fyre-animation.xml
%{_sharedir}/mime/globs
%{_sharedir}/mime/magic
%{_sharedir}/mime/packages/fyre.xml
%{_sharedir}/pixmaps/fyre-48x48.png

%changelog
* Fri Mar 04 2005 Mirco Mueller <macslow@bangang.de> 1.0.0-2
- stupid me, I totally forgot to enable gnet2 and OpenEXR support

* Thu Mar 03 2005 Mirco Mueller <macslow@bangang.de> 1.0.0-1
- initial .spec file written for fyre-1.0.0.tar.gz

