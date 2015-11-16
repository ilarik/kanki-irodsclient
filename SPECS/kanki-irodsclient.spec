Name:		kanki-irodsclient
Version:	1.1.0b1
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
mkdir -p %{buildroot}/usr/share/icons/hicolor/128x128/apps
mkdir -p %{buildroot}/usr/share/applications
cp %{name}-%{version}/src/irodsclient %{buildroot}/usr/bin
cp %{name}-%{version}/src/schema.xml %{buildroot}/etc/irods
cp %{name}-%{version}/irodsclient.desktop %{buildroot}/usr/share/applications
cp %{name}-%{version}/src/icons/irodsclient.png %{buildroot}/usr/share/icons/hicolor/128x128/apps

%clean
rm -rf %{buildroot}

%files
%defattr(-,root,root,-)
%attr(755, root, root) /usr/bin/irodsclient
%attr(644, root, root) /etc/irods/schema.xml
%attr(644, root, root) /usr/share/applications/irodsclient.desktop
%attr(644, root, root) /usr/share/icons/hicolor/128x128/apps/irodsclient.png

%changelog
* Mon Nov 16 2015 Ilari Korhonen <ilari.korhonen@jyu.fi>
* beta version 1.1.0b1 - iRODS object search functionality

* Wed Nov 04 2015 Ilari Korhonen <ilari.korhonen@jyu.fi>
* version 1.0.7 - bug fixes and error reporting

* Thu Oct 08 2015 Ilari Korhonen <ilari.korhonen@jyu.fi>
* version 1.0.6 - storage resource selection UI added

* Wed Oct 07 2015 Ilari Korhonen <ilari.korhonen@jyu.fi>
* version 1.0.5 - refresh related bug fixes 

* Tue Sep 29 2015 Ilari Korhonen <ilari.korhonen@jyu.fi>
* version 1.0.4 - improved download + bug fixes

* Fri Sep 18 2015 Ilari Korhonen <ilari.korhonen@jyu.fi>
* initial rpm release version 1.0.3
