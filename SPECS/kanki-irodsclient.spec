Name:		kanki-irodsclient
Version:	1.0.3
Release:	1.el6
Summary:	Kanki iRODS GUI Client 

Group:		Applications
License:	BSD
URL:		https://github.com/ilarik/kanki-irodsclient
Source0:	https://github.com/ilarik/kanki-irodsclient/archive/%{version}.tar.gz

Requires:	irods-runtime >= 4.1.3
Requires:	irods-icommands >= 4.1.3
Requires:	qt5-qtbase >= 5.4.1
Requires:	qt5-qtbase-gui >= 5.4.1
Requires:	qt5-qtsvg >= 5.4.1
Requires:	qt5-qtx11extras >= 5.4.1

%description
iRODS GUI client

%prep
%setup -q -c 


%build
(cd %{name}-%{version}; ./build.sh -r el6)

%install
mkdir -p %{buildroot}/etc/irods
mkdir -p %{buildroot}/usr/bin
cp %{name}-%{version}/src/irodsclient %{buildroot}/usr/bin
cp %{name}-%{version}/src/schema.xml %{buildroot}/etc/irods

%clean
rm -rf %{buildroot}

%files
%defattr(-,root,root,-)
%attr(755, root, root) /usr/bin/irodsclient
%attr(644, root, root) /etc/irods/schema.xml

%changelog
* Fri Sep 18 2015 Ilari Korhonen <ilari.korhonen@jyu.fi>
* initial rpm release version 1.0.3
