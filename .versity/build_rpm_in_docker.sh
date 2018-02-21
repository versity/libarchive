#!/bin/bash

set -x
set -e -o pipefail

docker run -it --rm -v "$(pwd):/app" -w /app versity/libarchive-build /bin/bash /app/build_rpm.sh
