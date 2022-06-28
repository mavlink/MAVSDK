#!/usr/bin/env bash

set -e

# Match hash from configure step
REPO=$(grep GIT_REPOSITORY third_party/mavlink/CMakeLists.txt | awk '{print $2}')
TAG=$(grep GIT_TAG third_party/mavlink/CMakeLists.txt | awk '{print $2}')
MAVLINK_DIALECT=common

MAVLINK_PATH=mavlink
MAVLINK_HEADERS_PATH=mavlink-headers

if [ ! -d $MAVLINK_PATH ]
then
    git clone --recursive $REPO $MAVLINK_PATH
else
    (cd $MAVLINK_PATH && git fetch)
fi

(cd $MAVLINK_PATH && git checkout $TAG && git submodule update --init --recursive)

OUTPUT_PATH="$MAVLINK_HEADERS_PATH/include/mavlink/v2.0/"
mkdir -p "$OUTPUT_PATH"
OUTPUT_PATH=$(realpath $OUTPUT_PATH)

(cd $MAVLINK_PATH && \
    python3 \
        -m pymavlink.tools.mavgen \
        --lang=C \
        --wire-protocol=2.0 \
        --output=$OUTPUT_PATH \
        message_definitions/v1.0/$MAVLINK_DIALECT.xml)

echo ""
echo "Now run cmake with -DMAVLINK_HEADERS=\"$MAVLINK_HEADERS_PATH/include\""
