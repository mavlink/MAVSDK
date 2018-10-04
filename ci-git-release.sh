#!/usr/bin/env bash

set -e

git_branch=`git rev-parse --abbrev-ref HEAD`

# Make sure we're on the 'develop' branch.
if [ ! "$git_branch" = "develop" ]; then
    echo "Error: branch is '$git_branch' instead of 'develop'"
    exit 1
fi

# We want to create a git tag when the version file changes, and also merge
# the latest 'develop' branch into the 'master' branch.

# Read the version file.
version_file=`cat version`

# We want to extract "1.2.3" from "v1.2.3-5-g123abc".
version_tag=`git describe --always --tags | sed 's/v\([0-9]*\.[0-9]*\.[0-9]*\).*$/\1/'`

echo "Current version file: $version_file"
echo "Current version tag:  $version_tag"

if [ "$version_file" = "$version_tag" ]; then
    echo "Version tag is up to date"
else
    new_tag="v$version_file"
    echo "Tags don't match, adding new tag $new_tag"
    git checkout master
    git merge -ff-only develop
    git tag $new_tag
    git push origin master $new_tag --dry-run
fi
