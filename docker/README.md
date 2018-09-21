## Building and using Docker development images.

These Dockerfiles will create a DronecodeSDK development container image based on whichever flavour of Ubuntu or Fedora you prefer.

Docker images are built by the project and are hosted on Docker Hub.
You can use them simply by :

```bash
cd /whereever/DroncodeSDK
./run-docker.sh
```

Should you wish to create and use your own local images, use the following.

To create an image :

```bash
cd /whereever/DronecodeSDK/docker
docker build -t dronecode/dronecode-sdk-ubuntu-18.04 -f Dockerfile-Ubuntu-18.04 .
```

To use the image :

```bash
cd /whereever/DronecodeSDK
./run-docker.sh
```

This will create and run a container based on your image.
/whereever/DronecodeSDK will be mounted inside the container at /home/dronecode/DronecodeSDK

```bash
dronecode@a476023ed255:~/DronecodeSDK$ make default
```
