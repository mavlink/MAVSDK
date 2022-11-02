#!/usr/bin/env bash

set -e

# Date according to RFC 5322
DATE=$(date -R)
AUTHOR="MAVSDK Team"
EMAIL=""
PRE_RELEASE=1

# Fetch tags from upstream in case they're not synced
git fetch --tags

# Get the tag which points to the current commit hash; if the current commit is not tagged, the version core defaults to the current hash
CURRENT_REF="$(git rev-parse --short HEAD)"
CURRENT_TAG=$(git tag --points-at "$CURRENT_REF" | sed 's/v\([0-9]*\.[0-9]*\.[0-9]*\).*$/\1/')
if [ ! -z "$CURRENT_TAG" ]; then
    VERSION=$CURRENT_TAG
else
    # dpkg-buildpacakge expects the version to start with a digit, hence the 0
    VERSION="0v$CURRENT_REF"
fi

function usage() {
cat << EOF
    Usage:
    $0 <options ...>
        --version <version core; default=0v[CURRENT_REF] or [CURRENT_TAG]>
        --pre <pre-release number; default=1>
        --author <author name; default=MAVSDK Team>
        --email <author email; default=[empty]>
    Examples:
        ./generate_debian_changelog.sh
            --version=1.0.0
            --pre=1
            --author="Example Name"
            --email="mavsdk@example.com"
EOF
}

function usage_and_exit() {
    usage
    exit $1
}

for i in "$@"
do
    case $i in
        --version=*)
        VERSION="${i#*=}"
        ;;
        --pre=*)
        PRE_RELEASE="${i#*=}"
        ;;
        --author=*)
        AUTHOR="${i#*=}"
        ;;
        --email=*)
        EMAIL="${i#*=}"
        ;;
        *) # unknown option
        usage_and_exit 1
        ;;
    esac
done

echo "mavsdk ($VERSION-$PRE_RELEASE) unstable; urgency=medium"
echo ""
echo "  * MAVSDK release"
echo ""
echo " -- $AUTHOR <$EMAIL>  $DATE"
