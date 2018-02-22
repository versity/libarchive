#!/bin/bash

set -x
set -e -o pipefail

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
