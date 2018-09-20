## Building and using Docker development images.

These Dockerfiles will create a DronecodeSDK development container image based on whichever flavour of Ubuntu or Fedora you prefer.

To create an image :-

```bash
cd /whereever/DronecodeSDK/docker
docker build -t dronecode -f Dockerfile-Ubuntu-18.04 .
```

To use the image :-

```bash
cd /whereever/DronecodeSDK
./run-docker
```

This will create and run a container based on your image.
/whereever/DronecodeSDK will be mounted inside the container at /home/dronecode/DronecodeSDK

```bash
dronecode@a476023ed255:~/DronecodeSDK$ make default
```
