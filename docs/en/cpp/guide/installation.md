# Installation {#installation}

MAVSDK C++ is a library that can be installed and then used in your C++ code.

The latest release can be installed using prebuilt artefacts. If you need the latest state or want to debug the library itself, you need to [build it from source](build.md).

## Linux

- **Ubuntu:** Download the **.deb** file for your system from [MAVSDK releases](https://github.com/mavlink/MAVSDK/releases) and install it using `dpkg`:
  ```
  sudo dpkg -i mavsdk_0.37.0_ubuntu20.04_amd64.deb
  ```

- **Fedora:** Download the **.rpm** file for your system from [MAVSDK releases](https://github.com/mavlink/MAVSDK/releases) and install it using `rpm`:
   ```
   sudo rpm -U mavsdk-0.37.0-1.fc33-x86_64.rpm
   ```

- **Arch Linux:** mavsdk is available from the [AUR](https://aur.archlinux.org/) and can be installed e.g. using [yay](https://aur.archlinux.org/packages/yay/):
   ```
   yay -S mavsdk
   ```

## macOS

**MacOS:** Install [Homebrew](https://brew.sh/) and use it to install the library:
```
brew install mavsdk
```

## Windows

For Windows you can download the **mavsdk-windows-x64-release.zip** file from [MAVSDK releases](https://github.com/mavlink/MAVSDK/releases) containing the headers and library and extract it locally (see [information how to use a locally installed library](toolchain.md#sdk_local_install)).

> **Note** As the library is installed locally (in a local directory) you need to point cmake to that directory at configure time using `CMAKE_PREFIX_PATH` when building your application.
> i.e.: `cmake -Bbuild -DCMAKE_PREFIX_PATH=../mavsdk_extracted` -H.
