
#
# makefile for neiku rpm.
#
# usage:
# $make dir   [buildroot=<path>]    # make buildroot directory on <buildroot>
# $make file                        # copy file(s) to buildroot
# $make rpm   [rpmdir=<path>]       # build the neiku rpm package, and save to <rpmdir>
# $make clean [buildroot=<path>]    # clean
#

sh = ../sh/log.sh
bin = ../sh/rpmbuild.bb

specfile  = neiku-1-1.spec
buildroot = `pwd`/buildroot
rpmdir    = `pwd`

all: dir file rpm

dir:
	@mkdir -pv ${buildroot}/usr/local/neiku/sh
	@mkdir -pv ${buildroot}/usr/local/bin

file: dir
	for name in ${sh}  ; do cp -vf $$name ${buildroot}/usr/local/neiku/sh/ ; done
	for name in ${bin} ; do cp -vf $$name ${buildroot}/usr/local/bin/ ; done

rpm:
	../sh/rpmbuild.bb ${specfile} --buildroot=${buildroot} --rpmdir=${rpmdir}

clean:
	rm -vfr ${buildroot}
