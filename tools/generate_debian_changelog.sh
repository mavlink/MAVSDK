#!/usr/bin/env bash

# We want to extract "1.2.3" from "v1.2.3-5-g123abc".
tag_version=`git describe --always --tags | sed 's/v\([0-9]*\.[0-9]*\.[0-9]*\).*$/\1/'`

# Default to 1 for package version
if [ "$#" == 1 ]; then
    package_version=$1
else
    package_version="1"
fi

# Date according to RFC 5322
date=`date -R`

echo "mavsdk ($tag_version-$package_version) unstable; urgency=medium"
echo ""
echo "  * Initial release"
echo ""
echo " -- Helge Bahmann <helge@auterion.com>  $date"
