#!/bin/bash

# Command: rpmbuild.bb
# Author : hobby <ku7d@qq.com>
# Date   : 2015/03/07
# Summary: Build Binary RPM Package.
#          - support BuildRoot Tag in SPEC file. (eg: BuildRoot:/path/to/buildroot/)
#          - support BuildRoot in command line.  (eg: --buildroot=/path/to/buildroot/)
#          - support the default BuildRoot.      (eg: ~/rpmbuild/BUILDROOT/name-version-release.arch/)
#          - support relative BuildRoot.         (eg: --buildroot=../../buildroot/)
#          - support custom output rpm path.     (eg: --rpmdir=../../rpmdir/)
#          * buildroot priority: cmdline-buildroot > specfile-buildroot > default-buildroot
# RPM Ver: rpm 4.4.2, rpm 4.8.0
#
# ----- Usage -----
# Usage  : rpmbuild.bb rpm-spec-file(.spec) [-r|--buildroot=BUILD_DIRECTORY] [-d|--rpmdir=RPM_DIRECTORY] [-h|--help]
# Example:
# $rpmbuild.bb /path/to/specfile --buildroot=/path/to/buildroot --rpmdir=/path/to/rpmdir
#  here will build rpm-package from '/path/to/buildroot/' with /path/to/specfile
#  and save the rpm-package to './path/to/rpmdir/'
#
# ----- Github -----
# github : https://github.com/hobby/tools.git    (at rpm/)
# raw-url: https://raw.githubusercontent.com/hobby/tools/master/rpm/rpmbuild.bb
#
# ----- ChangeLog -----
# 2015/03/08:
# * relative-buildroot、rpm-path support
#
# 2015/03/07
# * simple BuildRoot-Tag support

function normaltext() { echo "$*"; } 
function redtext()    { echo -e "\e[31;1m$*\e[0m"; }
function greentext()  { echo -e "\e[32;1m$*\e[0m"; }
function pinktext()   { echo -e "\e[35;1m$*\e[0m"; }
function showusage()
{
    echo "Usage: $0 $(greentext "rpm-spec-file(*.spec)") [-r|--buildroot=BUILD_DIRECTORY] [-d|--rpmdir=RPM_DIRECTORY] [-h|--help]"
}

# rpm spec file for rpmbuild (required)
specfile=
# build-root for build rpm (optionnal)
buildroot=
# directory for output rpm (optionnal)
rpmdir=
# require short optstring, otherwise no non-opt args.
TEMP=$(getopt -o r:d:h --long buildroot:,rpmdir:,help -n "$0" -- "$@")
if [ $? != 0 ] ; then
    showusage
    exit 1
fi
eval set -- "$TEMP"
while true
do
    case "$1" in
        -r|--buildroot) buildroot="$2" ; shift 2 ;;
        -d|--rpmdir)    rpmdir="$2" ;   shift 2 ;;
        -h|--help)      showusage ; exit 0 ; ;;
        --)             shift ; break ;;
        *)              echo "parse options error!" >&2 ; exit 1 ;;
    esac
done
specfile="$1"
if [ -z "$specfile" ] ; then
    showusage
    exit 1
fi

# check permission (read)
if ! stat "$specfile" 1>/dev/null 2>&1 ; then
    echo "Oops! I found the \`$specfile' NOT OK!"
    echo
    showusage
   exit 1
fi

# no buildroot, then get buildroot from specfile (the first one)
if [ -z "$buildroot" ] ; then
    buildroot=$(grep -av "^[ #]" "$specfile" | grep -ai "^buildroot[ ]*:" | head -n1 | cut -f2 -d:)
    buildroot=$(echo $buildroot)
fi

# support relative path
if [ "$(echo $buildroot | cut -c1)" = "." ] ; then
    buildroot="$(pwd)/$buildroot"
fi

# build info
echo "RPM spec file  : $(greentext $specfile)"
echo "RPM path dir   : $(pinktext  $rpmdir)"
echo "BuildRoot(file): $(pinktext  $buildroot)"

# run rpmbuild
define_rpmdir=
if [ -n "$rpmdir" ] ; then
    if [ -n "$buildroot" ] ; then
        run="rpmbuild -bb $specfile --buildroot=$buildroot --define=\"_rpmdir $rpmdir\""
        echo
        echo "rpmbuild output: $(greentext $run)"
        echo "============================================"
        rpmbuild -bb $specfile --buildroot=$buildroot --define="_rpmdir $rpmdir"
        echo "============================================"
        echo
    else
        run="rpmbuild -bb $specfile --define=\"_rpmdir $rpmdir\""
        echo
        echo "rpmbuild output: $(greentext $run)"
        echo "============================================"
        rpmbuild -bb $specfile --define="_rpmdir $rpmdir"
        echo "============================================"
        echo
    fi
else
    if [ -n "$buildroot" ] ; then
        run="rpmbuild -bb $specfile --buildroot=$buildroot"
        echo
        echo "rpmbuild output: $(greentext $run)"
        echo "============================================"
        $run
        echo "============================================"
        echo
    else
        run="rpmbuild -bb $specfile"
        echo
        echo "rpmbuild output: $(greentext $run)"
        echo "============================================"
        $run
        echo "============================================"
        echo
    fi
fi
