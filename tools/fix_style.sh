#!/usr/bin/env bash

# This script runs clang-format over all files ending in .h, .c, .cpp listed
# by git in the given directory.

version_required_major="9"

# Try to find the latest version of clang
if command -v clang-format-9 >/dev/null; then
    clang_format=clang-format-9
elif command -v clang-format-8 >/dev/null; then
    clang_format=clang-format-8
elif command -v clang-format-7 >/dev/null; then
    clang_format=clang-format-7
elif command -v clang-format >/dev/null; then
    clang_format=clang-format
else
    echo "clang-format not found"
    echo "--> check: https://mavsdk.mavlink.io/develop/en/contributing/code_style.html#formatting-and-white-space"
    exit 1
fi

# Check version of found clang
version=$($clang_format --version)
semver_regex="(0|[1-9][0-9]*)\\.(0|[1-9][0-9]*)\\.(0|[1-9][0-9]*)"
if [[ $version =~ $semver_regex ]]; then
version_major=${BASH_REMATCH[1]}
if [ "$version_required_major" -gt "$version_major" ]; then
    echo "Clang version $version_major too old (required: $version_required_major)"
    echo "You can use clang-format-9 from docker:"
    echo ""
    echo "    'tools/run-docker.sh tools/fix_style.sh .'"
    exit 1

elif [ "$version_required_major" -lt "$version_major" ]; then
    echo "Clang version $version_major too new (required: $version_required_major)"
    echo "You can use clang-format-9 from docker:"
    echo ""
    echo "    'tools/run-docker.sh tools/fix_style.sh .'"
    exit 1
fi

else
    echo "Could not determine clang-format version"
    exit 1
fi

# Check that exactly one directory is given
if [ $# -eq 0 ];
then
    echo "No directory supplied"
    echo "Usage: ./fix_style.sh dir"
    exit 1

elif [ $# -gt 1 ];
then
    echo "Too many directories supplied"
    echo "Usage: ./fix_style.sh dir"
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
        echo "Changed $file:"
        $diff_cmd $file.orig $file
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

