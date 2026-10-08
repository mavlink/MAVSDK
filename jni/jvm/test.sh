#!/bin/bash

set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
library="${1:-"$script_dir/../build/libmavsdk_jni.dylib"}"
# System.load() takes absolute paths only
library="$(cd "$(dirname "$library")" && pwd)/$(basename "$library")"
build_dir="$script_dir/build-test"

cleanup() {
    rm -rf "$build_dir"
}
trap cleanup EXIT

rm -rf "$build_dir"
mkdir -p "$build_dir/classes"
find "$script_dir/src/main/java" "$script_dir/src/test/java" -name '*.java' -type f \
    | LC_ALL=C sort > "$build_dir/sources.list"
javac -d "$build_dir/classes" "@$build_dir/sources.list"

# -Xcheck:jni makes the JVM validate the arguments of every JNI call and abort
# on a bad one, which it otherwise does not notice.
java -Xcheck:jni -cp "$build_dir/classes" io.mavsdk.jni.CheckedJniTest "$library"
