Name:      neiku
Version:   1
Release:   1
Group:     System Environment/Shells
License:   GPL
Packager:  Hobby <ku7d@qq.com>
Vendor:    Hobby
Url:       https://github.com/hobby/neiku.git
Prefix:    /usr/local
#BuildRoot: 
Summary:   Programmer's neiku.
BuildArch: noarch

%description
neiku for every programmer.

%files
%defattr (0755, root, root)
/usr/local/bin/*
/usr/local/neiku/sh/*.sh

%pre

%post

%preun

%postun

%verifyscript

%clean

%changelog
* Sat Mar  7 2015 Hobby
Version: 1 Release: 1
log.sh:
-- log tools for scripts.
-- rpmbuild.bb tools for rpmbuild.

