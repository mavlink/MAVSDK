#!/bin/bash

# This script runs astyle and complains about lines that are too long
# over all files ending in .h, .c, .cpp listed by git in the given
# directory.

# Check for the latest astyle version
ASTYLE_VER_REQUIRED_1="Artistic Style Version 2.05.1"
ASTYLE_VER_REQUIRED_2="Artistic Style Version 2.06"
ASTYLE_VER_REQUIRED_3="Artistic Style Version 3.0"
ASTYLE_VER_REQUIRED_4="Artistic Style Version 3.0.1"
ASTYLE_VER_REQUIRED_5="Artistic Style Version 3.1"

astyle_ver() {
    echo "At least ${ASTYLE_VER_REQUIRED_1} is required"
    echo "You can get the correct version here: https://sourceforge.net/projects/astyle/files/astyle/astyle%202.06/"
}

# check if astyle is installed
condition=$(which astyle 2>/dev/null | grep -v "not found" | wc -l)
if [ $condition -eq 0 ]; then
    echo "astyle is not installed"
    astyle_ver
    exit 1
else

    ASTYLE_VER=`astyle --version`

    if [ "$ASTYLE_VER" != "$ASTYLE_VER_REQUIRED_1" -a \
         "$ASTYLE_VER" != "$ASTYLE_VER_REQUIRED_2" -a \
         "$ASTYLE_VER" != "$ASTYLE_VER_REQUIRED_3" -a \
         "$ASTYLE_VER" != "$ASTYLE_VER_REQUIRED_4" -a \
         "$ASTYLE_VER" != "$ASTYLE_VER_REQUIRED_5" ];
    then
        echo "Error: you're using ${ASTYLE_VER}"
        echo "but should be using ${ASTYLE_VER_REQUIRED} instead"
        exit 1
    fi
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

# Find the directory of this script because the file astylerc should be
# next to it.
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

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
# TODO: add -r argument to include all files
files=`git ls-files | grep -E "\.h$|\.c$|\.cpp$|\.proto"`

while IFS= read file; do

    # We want to check if the file is in the list to ignore.
    # We do this in a brute force way by looping through every
    # line the ignore file and compare it against the filename.
    if [[ -f $SCRIPT_DIR/$style_ignore ]]; then
        need_to_ignore=false
        while IFS= read ignore; do
            if [[ `echo $1/$file | grep "$ignore"` ]]; then
                need_to_ignore=true
            fi
        done < "$SCRIPT_DIR/$style_ignore"
    fi

    if [ "$need_to_ignore" = true ]; then
        # Don't do the astyle and file length checks,
        # go to next file.
        continue
    fi

    # Run astyle with given astylerc
    astyle_result=`astyle --options=$SCRIPT_DIR/astylerc $file | grep "Formatted"`

    if [[ $astyle_result ]]; then
        echo "Formatted $file:"
        $diff_cmd $file $file.orig
        rm $file.orig
        error_found=true
    fi

    # TODO: this is disabled for now
    ## Go line by line
    #count=0
    #while IFS= read -r line; do
    #    # Check for lines too long
    #    len=${#line}
    #    if [ $len -gt 100 ]; then
    #        echo "Line $count too long"
    #        error_found=true
    #    fi
    #    (( count++ ))
    #done < "$file"

# We need to use this clunky way because otherwise
# we lose the value of error_found.
# See http://mywiki.wooledge.org/BashFAQ/024
done < <(echo "$files")

cd - > /dev/null

if [ "$error_found" = true ]; then
    exit 1
fi

