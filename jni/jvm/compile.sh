#!/bin/bash

set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source_dir="$script_dir/src/main/java"
build_dir="$script_dir/build"
sources_file="$build_dir/sources.list"

cleanup() {
    rm -rf "$build_dir"
}
trap cleanup EXIT

rm -rf "$build_dir"
mkdir -p "$build_dir/classes"
find "$source_dir" -name '*.java' -type f | LC_ALL=C sort > "$sources_file"

if [ ! -s "$sources_file" ]; then
    echo "No neutral JNI Java sources found" >&2
    exit 1
fi

javac --release 8 -d "$build_dir/classes" "@$sources_file"
echo "Compiled $(wc -l < "$sources_file" | tr -d ' ') neutral JNI Java contracts."
