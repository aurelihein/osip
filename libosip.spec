Summary: "libosip" - An  implementation of SIP - rfc2543.
Name: libosip
Version: 0.8.0
Release: 1
Group: Development/Libraries
URL: http://www.atosc.org
Source: http://www.atosc.org/download/libosip-%{version}.tar.gz
Prefix: %{_prefix}
Copyright: LGPL
BuildRoot: %{_tmppath}/libosip
Packager: David Sugar <dyfet@ostel.com>

%description
This is the oSIP library (for Open SIP). It has been
designed to provide the Internet Community a simple
way to support the Session Initiation Protocol.
SIP is described in the RFC2543 which is available at
http://www.ietf.org/rfc/rfc2543.txt.

%prep
rm -rf $RPM_BUILD_ROOT

%setup
CFLAGS="$RPM_OPT_FLAGS" ./configure --prefix=%{_prefix} --disable-debug

%build
uname -a|grep SMP && make -j 2 || make

%install
make prefix=$RPM_BUILD_ROOT/%{_prefix} install
strip $RPM_BUILD_ROOT/%{_prefix}/lib/lib*.so.*

%files
%defattr(-,root,root,0755)
%doc AUTHORS COPYING README NEWS TODO ChangeLog doc
%{prefix}/lib/libosip*
%{prefix}/include/osip

%clean
rm -rf $RPM_BUILD_ROOT

%post
/sbin/ldconfig

%postun
/sbin/ldconfig
