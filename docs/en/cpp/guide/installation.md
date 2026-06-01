# Installation {#installation}

MAVSDK C++ is a library that can be installed and then used in your C++ code.

The latest release can be installed using prebuilt artefacts. If you need the latest state or want to debug the library itself, you need to [build it from source](build.md).

### Linux

**Ubuntu**

If you have an older version already installed remove that first:
```
sudo apt remove mavsdk
```

The prebuilt C++ library can be downloaded as a **.deb** from [releases](https://github.com/mavlink/MAVSDK/releases), e.g.:

```
wget https://github.com/mavlink/MAVSDK/releases/download/v3.0.0/libmavsdk-dev_3.0.0_ubuntu24.04_amd64.deb
sudo dpkg -i libmavsdk-dev_3.0.0_ubuntu24.04_amd64.deb
```

### macOS

Install [Homebrew](https://brew.sh/) and use it to install the library:
```
brew install mavsdk
```

### Windows

For Windows you can download the **mavsdk-windows-x64-release.zip** file from [MAVSDK releases](https://github.com/mavlink/MAVSDK/releases) containing the headers and library and extract it locally (see [information how to use a locally installed library](toolchain.md#sdk_local_install)).

For more detailed instructions or other platforms check out how to [build from source](build.md).

