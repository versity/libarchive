#!/bin/bash

set -x
set -e -o pipefail

cd ..
docker run -it --rm -v "$(pwd):/app" -w /app versity/libarchive-build /bin/bash /app/.versity/build_dist_gzip.sh
