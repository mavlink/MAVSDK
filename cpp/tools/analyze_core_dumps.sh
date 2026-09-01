#!/usr/bin/env bash
#
# Print a symbolized backtrace for any core dumps a failing test run left behind.
#
# Without this, a crash in CI shows up as a bare "Segmentation fault (core dumped)" and the
# only way to find out what happened is to download the core plus the matching binaries and
# reconstruct the container's libraries by hand. The core is still uploaded as an artifact
# for deeper digging; this just makes the common case readable straight from the log.
#
# Usage: tools/analyze_core_dumps.sh [build-dir]   (default: build/release)

set -u

build_dir=${1:-build/release}

shopt -s nullglob
cores=(/tmp/core.*)
if [ ${#cores[@]} -eq 0 ]; then
    echo "No core dumps found."
    exit 0
fi

if ! command -v gdb > /dev/null 2>&1; then
    echo "gdb not installed, installing it to analyze $((${#cores[@]})) core dump(s)..."
    if command -v apt-get > /dev/null 2>&1; then
        (apt-get update -qq && apt-get install -y -qq gdb) > /dev/null 2>&1 ||
            (sudo apt-get update -qq && sudo apt-get install -y -qq gdb) > /dev/null 2>&1
    elif command -v apk > /dev/null 2>&1; then
        apk add --no-cache gdb > /dev/null 2>&1 || sudo apk add --no-cache gdb > /dev/null 2>&1
    fi
fi

if ! command -v gdb > /dev/null 2>&1; then
    echo "Could not install gdb; the core dumps are still uploaded as an artifact."
    exit 0
fi

# The core pattern is /tmp/core.%h.%e.%t, so %e gives the executable -- truncated to 15
# characters by the kernel, which is why this matches on a prefix.
find_binary() {
    local core_name exe_field candidate
    core_name=$(basename "$1")
    exe_field=$(echo "$core_name" | cut -d. -f3)

    for candidate in \
        "$build_dir/src/system_tests/system_tests_runner" \
        "$build_dir/src/unit_tests/unit_tests_runner" \
        "$build_dir/src/system_tests/standalone_param_test" \
        "$build_dir/src/system_tests/standalone_ftp_test"; do
        if [ -x "$candidate" ] && [ "${exe_field}" = "$(basename "$candidate" | cut -c1-15)" ]; then
            echo "$candidate"
            return 0
        fi
    done
    return 1
}

for core in "${cores[@]}"; do
    echo
    echo "================================================================"
    echo "Core dump: $core"
    echo "================================================================"

    binary=$(find_binary "$core") || {
        echo "Could not work out which binary produced this core, skipping."
        continue
    }
    echo "Binary: $binary"
    echo

    # 'thread apply all bt' covers the ordinary case. The stack dump below covers the one it
    # cannot: when the crash is a jump through a corrupted pointer, frame #0 is '?? ()' and
    # gdb has nothing to unwind from, but the return addresses are still sitting on the
    # stack and 'info symbol' on them reconstructs the call chain by hand.
    gdb -nx -batch \
        -ex "set auto-load safe-path /" \
        -ex "set print frame-arguments scalars" \
        -ex "thread apply all bt full" \
        -ex "echo \n=== crashing thread: registers and raw stack ===\n" \
        -ex "info registers rip rsp rbp" \
        -ex "x/64a \$rsp" \
        "$binary" "$core" 2>&1 || true
done
