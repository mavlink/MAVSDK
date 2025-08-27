#!/usr/bin/env bash

# This script runs clang-format over all files ending in .h, .c, .cpp listed
# by git in the given directory.

version_required_major="19"

# Function to print installation instructions
print_installation_instructions() {
    echo "On Ubuntu 24.04, you can install it:"
    echo ""
    echo "    'sudo apt install clang-format-$version_required_major'"
    echo ""
    echo "Otherwise, you can use clang-format from docker:"
    echo ""
    echo "    'tools/run-docker.sh tools/fix_style.sh .'"
}

# Try to find the latest version of clang
if command -v clang-format-19 >/dev/null; then
    clang_format=clang-format-19
elif command -v clang-format >/dev/null; then
    clang_format=clang-format
else
    print_installation_instructions
    exit 1
fi

# Check version of found clang
version=$($clang_format --version)
semver_regex="(0|[1-9][0-9]*)\\.(0|[1-9][0-9]*)\\.(0|[1-9][0-9]*)"
if [[ $version =~ $semver_regex ]]; then
version_major=${BASH_REMATCH[1]}
if [ "$version_required_major" -gt "$version_major" ]; then
    echo "Clang version $version_major too old (required: $version_required_major)"
    echo ""
    print_installation_instructions
    exit 1

elif [ "$version_required_major" -lt "$version_major" ]; then
    echo "Clang version $version_major too new (required: $version_required_major)"
    echo ""
    print_installation_instructions
    exit 1
fi

else
    echo "Could not determine clang-format version"
    exit 1
fi

# Parse optional --quiet flag
quiet_mode=false
if [ $# -gt 0 ] && [[ "$1" == "--quiet" || "$1" == "-q" ]]; then
    quiet_mode=true
    shift
fi

# Check that exactly one directory is given
if [ $# -eq 0 ];
then
    echo "No directory supplied"
    echo "Usage: ./fix_style.sh [--quiet|-q] dir"
    exit 1

elif [ $# -gt 1 ];
then
    echo "Too many directories supplied"
    echo "Usage: ./fix_style.sh [--quiet|-q] dir"
    exit 1
fi

# Ignore files listed in style-ignore.txt
style_ignore="style-ignore.txt"

script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# Keep track of errors for the exit value
error_found=false

# Use colordiff if available
if command -v colordiff >/dev/null 2>&1; then
    diff_cmd=colordiff
else
    diff_cmd=diff
fi

cd $1 > /dev/null

# Go through all .h, c., and .cpp files listed by git
files=`git ls-files | grep -E "\.h$|\.c$|\.cpp$|\.proto"`

while IFS= read file; do

    # We want to check if the file is in the list to ignore.
    # We do this in a brute force way by looping through every
    # line the ignore file and compare it against the filename.
    if [[ -f $script_dir/$style_ignore ]]; then
        need_to_ignore=false
        while IFS= read ignore; do
            if [[ `echo $1/$file | grep "$ignore"` ]]; then
                need_to_ignore=true
            fi
        done < "$script_dir/$style_ignore"
    fi

    if [ "$need_to_ignore" = true ]; then
        # Don't do the style  checks, go to next file.
        continue
    fi

    cp $file $file.orig
    result=`$clang_format -style=file -i $file`

    if ! cmp $file $file.orig >/dev/null 2>&1; then
        if [ "$quiet_mode" = false ]; then
            echo "Changed $file:"
            $diff_cmd $file.orig $file
        fi
        error_found=true
    fi

    rm $file.orig

# We need to use this clunky way because otherwise
# we lose the value of error_found.
# See http://mywiki.wooledge.org/BashFAQ/024
done < <(echo "$files")

cd - > /dev/null

if [ "$error_found" = true ]; then
    exit 1
fi

