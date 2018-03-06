#!/bin/bash
set -x

DOT_VERSITY="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
pushd "$DOT_VERSITY"
pwd
env | sort

# Build release artifacts and upload to our artifactory repo

bash ./developer_build.sh

ls -la rpms/*.rpm

# We upload the same set of RPMs to both el7 repos for now, to make it easier to install until we
# refactor the repo... provided we can comingle kmod RPMs.
PATTERN="rpms/*.rpm"

for el_version in "7.4.1708" "7.3.1611"; do
    repo="public-rpms/${el_version}/"
    "$TRAVIS_BUILD_DIR"/jfrog rt upload "${PATTERN}" "${repo}"
done
