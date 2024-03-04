#!/usr/bin/env sh

dockerimage=docker.io/mavsdk/mavsdk-dev

if type podman > /dev/null 2> /dev/null
then
    podman run -it --rm -v $(pwd):/home/user/MAVSDK:z $dockerimage "$@"
    echo "sudo needed to repair file ownership after podman ran: sudo chown -R $USER:$USER ."
    sudo chown -R $USER:$USER .

elif type docker > /dev/null 2> /dev/null
then
    docker run -it --rm -v $(pwd):/home/user/MAVSDK:z -e LOCAL_USER_ID=`id -u` $dockerimage "$@"

else
    echo "docker or podman commands not found"
    exit 1
fi
