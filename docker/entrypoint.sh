#!/usr/bin/env bash

# Utility to use local user, initially taken from:
# https://denibertovic.com/posts/handling-permissions-with-docker-volumes/

# Use LOCAL_USER_ID if passed in at runtime.

if [ -n "${LOCAL_USER_ID}" ]; then
    echo "Starting with UID: $LOCAL_USER_ID"
    useradd --shell /bin/bash -u $LOCAL_USER_ID -o -c "" -M user
    export HOME=/home/user
    chown -R user:user $HOME

    exec su-exec user "$@"
else
    exec "$@"
fi
