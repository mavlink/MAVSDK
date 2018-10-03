#!/usr/bin/env bash

set -e

# We want to create a git tag when the version file changes.

# Read the version file.
version_file=`cat version`

# We want to extract "1.2.3" from "v1.2.3-5-g123abc".
version_tag=`git describe --always --tags | sed 's/v\([0-9]*\.[0-9]*\.[0-9]*\).*$/\1/'`

echo "Current version file: $version_file"
echo "Current version tag:  $version_tag"

if [  "$version_file" = "$version_tag" ]; then
    echo "Version tag is up to date"
else
    new_tag="v$version_file"
    echo "Tags don't match, adding new tag $new_tag"
    git tag $new_tag
    git push origin $new_tag --dry-run
fi
