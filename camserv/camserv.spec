%define name camserv
%define version 0.5.0
%define release 1
%define prefix /usr/local

Summary: A streaming web video browser and utilities.
Name: %{name}
Version: %{version}
Release: %{release}
Group: Applications/Web
Copyright: GPL
Source: %{name}-%{version}.tar.gz
Url: http://cserv.sourceforge.net
BuildRoot: /var/tmp/%{name}-%{version}-build
Requires: libjpeg >= 6b
Requires: imlib2 >= 1.0.5
Requires: gdk-pixbuf >= 0.11.0
Prefix: %prefix

%description
Camserv is an extremely modular program for doing streaming video 
from your Unix machine to web clients.  Filters can be added for 
text on the displayed window, and anything else one wants to add. 

In addition portability to other unices should be incredibly easy given
the modularity of the camera plugin modules.

Utilities for camera broadcast webserver offloading are also included.

%prep
%setup

%build
CFLAGS="$RPM_OPT_FLAGS" ./configure --prefix=%{prefix} --with-gzip
make

%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT%{prefix}
make prefix=$RPM_BUILD_ROOT%{prefix} install

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%doc AUTHORS BUGS COPYING ChangeLog NEWS README TODO javascript.txt
%{prefix}/share/camserv/camserv.cfg
%{prefix}/share/camserv/defpage.html
%{prefix}/bin/camserv
%{prefix}/bin/relay
%{prefix}/lib/camserv/lib*.so.*
%{prefix}/lib/camserv/libvideo_v4l.a
%{prefix}/lib/camserv/libvideo_v4l.la
%{prefix}/lib/camserv/libvideo_v4l.so
%{prefix}/lib/camserv/libvideo_basic.a
%{prefix}/lib/camserv/libvideo_basic.la
%{prefix}/lib/camserv/libvideo_basic.so
%{prefix}/lib/camserv/libtext_filter.a
%{prefix}/lib/camserv/libtext_filter.la
%{prefix}/lib/camserv/libtext_filter.so
%{prefix}/lib/camserv/librand_filter.a
%{prefix}/lib/camserv/librand_filter.la
%{prefix}/lib/camserv/librand_filter.so
%{prefix}/lib/camserv/libgdk_pixbuf_filter.a
%{prefix}/lib/camserv/libgdk_pixbuf_filter.la
%{prefix}/lib/camserv/libgdk_pixbuf_filter.so
%{prefix}/lib/camserv/libimlib2_filter.a
%{prefix}/lib/camserv/libimlib2_filter.la
%{prefix}/lib/camserv/libimlib2_filter.so
%{prefix}/lib/camserv/libjpg_filter.a
%{prefix}/lib/camserv/libjpg_filter.la
%{prefix}/lib/camserv/libjpg_filter.so


%changelog
* Sun Mar 10 2002 Jon Travis <jtravis@p00p.org>
- Long overdue release which mainly fixes up build issues

* Sun Feb 13 2000 Jon Travis <jtravis@cse.unl.edu>
- Yet another release.. yay.

* Thu Nov 18 1999 Jon Travis <jon@dsndata.com>
- Another release.. yay.

* Wed Oct 27 1999 Jon Travis <jon@dsndata.com>
- Initial package creation

