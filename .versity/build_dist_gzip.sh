#!/bin/bash

set -x
set -e -o pipefail

# just a subsection of makerelease.sh to speed things up for development
pwd
ls -al

bash build/clean.sh
export MAKE_LIBARCHIVE_RELEASE="1"
/bin/sh build/autogen.sh

./configure
make dist-gzip
