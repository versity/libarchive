#!/bin/bash

# Run all the steps a local developer needs to run. This generates RPMS and a docker container to
# be used in building versity-python-libarchive.

set -e -o pipefail
set -x

# NOTE: travis uses makerelease.sh, which is far more comprehensive. We are assuming the changes
# have already been tested locally and the developer now wants to use those for RPM install or
# Cython development and testing.

rm -fv SOURCES/*.tar.gz
bash build_dist_gzip_in_docker.sh
cp -v ../libarchive-*.tar.gz SOURCES/

docker build -f Dockerfile.build -t versity/libarchive-build .
bash ./build_rpm_in_docker.sh
docker build -f Dockerfile -t versity/python:2.7-libarchive .
