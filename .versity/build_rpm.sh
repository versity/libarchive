#!/bin/bash

set -x
set -e -o pipefail

# Temp, until we migrate to our generated .tar.gz here
if ! test -f SOURCES/libarchive-3.3.1.tar.gz; then
    yum install -y wget
    (cd SOURCES; wget http://www.libarchive.org/downloads/libarchive-3.3.1.tar.gz)
fi

mkdir -p ~/rpmbuild/{SOURCES,SPECS}
# copying takes care of permission transformation too, without that rpmbuild is mad
rsync -vr SOURCES/ ~/rpmbuild/SOURCES/
cp -v libarchive.spec ~/rpmbuild/SPECS/
#chgrp -v "$(whoami).users" ~/rpmbuild/SOURCES/*
SRPM=$(rpmbuild -bs ~/rpmbuild/SPECS/libarchive.spec | awk -F': ' '{print $2}') || exit 1
test -f "$SRPM"

rpmbuild --rebuild "$SRPM"

rm -frv rpms
mkdir -p rpms
cp -v ~/rpmbuild/RPMS/*/* rpms/
