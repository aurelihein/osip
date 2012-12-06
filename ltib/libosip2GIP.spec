%define pfx /opt/freescale/rootfs/%{_target_cpu}

Summary         : SIP stack library
Name            : libosip2GIP
Version         : 3.4.0
Release         : 810
License         : GPL
Vendor          : CASTEL
Packager        : Aurelien BOUIN
Group           : System Environment/Libraries
URL             : http://www.gnu.org
Source          : %{name}-%{version}.tar.gz
BuildRoot       : %{_tmppath}/%{name}
Prefix          : %{pfx}

%Description
%{summary}

%Prep
%setup 

%Build
./configure --prefix=%{_prefix}  --host=$CFGHOST --with-gnu-ld --disable-static
make

%Install
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT/%{pfx}
rm -f $RPM_BUILD_ROOT/%{pfx}/%{_prefix}/lib/*.la
rm -rf $RPM_BUILD_ROOT/%{pfx}/%{_prefix}/share

%Clean
rm -rf $RPM_BUILD_ROOT

%Files
%defattr(-,root,root)
%{pfx}/*
