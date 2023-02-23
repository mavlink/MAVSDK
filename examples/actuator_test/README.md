# How to use actuator test

These instructions are for Ubuntu Linux.

## Install latest MAVSDK v1.4 release:

Install dependencies:
```
sudo apt install cmake build-essential libjsoncpp-dev libcurl4-openssl-dev libtinyxml2-dev
```

Get the library
```
wget https://github.com/mavlink/MAVSDK/releases/download/v1.4.12/libmavsdk-dev_1.4.12_ubuntu20.04_amd64.deb
```

*This works on 22.04 as well as 20.04.*

Install it:
```
sudo dpkg -i libmavsdk-dev_1.4.12_ubuntu20.04_amd64.deb
```

## Get this example, build it, and run it

```
git clone --branch actuator_test https://github.com/mavlink/MAVSDK.git
cd MAVSDK/examples/actuator_test

cmake -Bbuild -H.
cmake --build build
build/actuator_test serial:///dev/dev/ttyACM0:921600
```

