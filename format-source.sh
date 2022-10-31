#!/bin/sh

set -e

for f in src/*.{h,cpp}; do
    echo "Formatting $f"
    clang-format -i -style=file $f
done
