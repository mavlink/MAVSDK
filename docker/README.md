## Building and using Docker development images.

These Dockerfiles will create a MAVSDK development container image based on whichever flavour of Ubuntu or Fedora you prefer.

Docker images are built by the project and are hosted on Docker Hub.
You can use them simply by :

```bash
cd /whereever/MAVSDK
./tools/run-docker.sh
```

Should you wish to create and use your own local images, use the following.

To create an image :

```bash
cd /whereever/MAVSDK/docker
docker build -t mavsdk/mavsdk-ubuntu-20.04 -f Dockerfile-Ubuntu-20.04 .
```

To use the image :

```bash
cd /whereever/MAVSDK
./tools/run-docker.sh
```

This will create and run a container based on your image.
/whereever/MAVSDK will be mounted inside the container at /home/user/MAVSDK

```bash
user@a476023ed255:~/MAVSDK$
```
