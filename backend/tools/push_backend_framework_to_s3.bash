#!/usr/bin/env bash

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
FAT_BIN_DIR=${SCRIPT_DIR}/../../build/fat_bin
CURRENT_VERSION=0.1.2

aws s3 cp ${FAT_BIN_DIR}/dronecode-backend.zip s3://dronecode-sdk/dronecode-backend-latest.zip
aws s3api put-object-acl --bucket dronecode-sdk --key dronecode-backend-latest.zip --acl public-read

aws s3 cp ${FAT_BIN_DIR}/dronecode-backend.zip s3://dronecode-sdk/dronecode-backend-${CURRENT_VERSION}.zip
aws s3api put-object-acl --bucket dronecode-sdk --key dronecode-backend-${CURRENT_VERSION}.zip --acl public-read
