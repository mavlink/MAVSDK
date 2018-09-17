#!/usr/bin/env bash

BRANCH=$(git rev-parse --abbrev-ref HEAD)
if [[ "$BRANCH" != "master" ]]; then
    echo "Not releasing because not 'master' branch";
    exit 0
fi

VERSION=$(cat version)

if git rev-parse "$TAG" >/dev/null 2>&1; then
    echo "Version tag '$VERSION' already exists, you need to bump the version."
    exit 1
fi

echo "Creating tag '$VERSION' and pushing it."
git tag $VERSION
git push origin $VERSION

