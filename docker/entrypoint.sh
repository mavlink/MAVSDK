#!/usr/bin/env bash

# Utility to use local user, taken from:
# https://denibertovic.com/posts/handling-permissions-with-docker-volumes/

# Add local user
# Either use the LOCAL_USER_ID if passed in at runtime or
# make educated guess to default of first user.

USER_ID=${LOCAL_USER_ID:-1000}

echo "Starting with UID : $USER_ID"
useradd --shell /bin/bash -u $USER_ID -o -c "" -M user
export HOME=/home/user

exec /bin/su user -c "$@"
