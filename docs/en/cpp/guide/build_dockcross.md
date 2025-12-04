# Cross-compilation with Dockcross and Android Builds

This guide explains how to build MAVSDK using dockcross for cross-compilation and how to build for Android.

## Building with Dockcross

[Dockcross](https://github.com/dockcross/dockcross) is a cross-compiling toolchain that can be used to build MAVSDK for various platforms.

### Requirements

- Docker installed and running
- Git

### Getting Started with Dockcross

1. Clone the MAVSDK repository:
```bash
git clone https://github.com/mavlink/MAVSDK.git
cd MAVSDK
git submodule update --init --recursive
```

2. Pull and run the appropriate dockcross image for your target platform
3. Use the generated dockcross script to run your build commands

### Example: Cross-compiling for ARM

```bash
# Get the dockcross script for ARM
docker run --rm dockcross/linux-armv7 > ./dockcross-linux-armv7
chmod +x ./dockcross-linux-armv7

# Use the script to run cmake and build
./dockcross-linux-armv7 bash -c 'cmake -DCMAKE_BUILD_TYPE=Release -Bbuild -S. && cmake --build build'
```

## Building for Android

### Requirements

- Android NDK
- CMake
- Git
- Java Development Kit (JDK)

### Setup Android NDK

1. Download and install [Android Studio](https://developer.android.com/studio)
2. Install the NDK and CMake through Android Studio's SDK Manager:
   - Open Android Studio
   - Go to Tools -> SDK Manager
   - Select SDK Tools tab
   - Check "NDK (Side by side)" and "CMake"
   - Click Apply to download and install

### Building

1. Clone the repository:
```bash
git clone https://github.com/mavlink/MAVSDK.git
cd MAVSDK
git submodule update --init --recursive
```

2. Set up environment variables:
```bash
export ANDROID_NDK_HOME=/path/to/android/ndk
export ANDROID_CMAKE=/path/to/android/cmake
```

3. Configure and build for your target Android architecture:
```bash
cmake \
    -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK_HOME/build/cmake/android.toolchain.cmake \
    -DANDROID_ABI=arm64-v8a \
    -DANDROID_PLATFORM=android-28 \
    -DCMAKE_BUILD_TYPE=Release \
    -Bbuild/android \
    -S.

cmake --build build/android
```

### Android Build Options

- `ANDROID_ABI`: Specify the target architecture
  - `armeabi-v7a`: ARMv7 devices
  - `arm64-v8a`: ARMv8 64-bit devices
  - `x86`: x86 devices
  - `x86_64`: x86 64-bit devices

- `ANDROID_PLATFORM`: Minimum Android API level
  - Example: `android-28` for Android 9.0 (Pie)

### Additional Build Flags

The same build flags available for regular builds can be used:

- `BUILD_SHARED_LIBS`: Set to `ON` for shared libraries (.so)
- `SUPERBUILD`: Set to `OFF` to use system dependencies
- `BUILD_MAVSDK_SERVER`: Set to `ON` to build mavsdk_server
- `BUILD_WITHOUT_CURL`: Set to `ON` to build without CURL support

## Troubleshooting

### Git Submodules Out of Date

If you encounter build issues, try updating the submodules:
```bash
git submodule update --recursive
rm -rf build
```

### Android NDK Version Issues

If you encounter issues with the NDK version:
1. Make sure you're using a compatible NDK version
2. Update your Android Studio and NDK to the latest stable versions
3. Verify that `ANDROID_NDK_HOME` points to the correct NDK version 