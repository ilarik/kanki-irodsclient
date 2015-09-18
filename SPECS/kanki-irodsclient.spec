Name:		kanki-irodsclient
Version:	1.0.3
Release:	1.el6
Summary:	iRODS GUI Client 

Group:		Applications
License:	BSD
URL:		http://www.irods.org
Source0:	kanki-irodsclient-%{version}-linux-el6.tar.gz

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

%install
mkdir -p %{buildroot}/etc/irods
mkdir -p %{buildroot}/usr/bin
cp irodsclient %{buildroot}/usr/bin
cp schema.xml %{buildroot}/etc/irods

%clean
rm -rf %{buildroot}

%files
%defattr(-,root,root,-)
%attr(755, root, root) /usr/bin/irodsclient
%attr(644, root, root) /etc/irods/schema.xml

%changelog
* Wed Aug 12 2015 Ilari Korhonen <ilari.korhonen@jyu.fi>
* initial version 0.1.0
