# MAVSDK-Kotlin

Kotlin Multiplatform wrapper for MAVSDK with JNI bindings for high performance on both Android and Desktop JVM.

## Features

- ✨ Idiomatic Kotlin API with null safety
- 🚀 High performance JNI bindings
- 🔄 Coroutines and Flow support (ready)
- 📦 Result types instead of exceptions
- 🎯 Type-safe enums and data classes
- 🤖 Android and Desktop JVM support via Kotlin Multiplatform
- 🧪 Works on macOS, Linux, and Android

## Project Structure

This is a Gradle composite build with two projects:

```
mavsdk-kotlin/          # The library
└── src/
    ├── commonMain/     # Shared Kotlin code (platform-agnostic)
    ├── jvmMain/        # Desktop JVM implementation
    ├── androidMain/    # Android implementation
    │   └── cpp/        # JNI C++ code (shared by both platforms)
    └── ...

examples/               # Example applications
└── src/main/kotlin/    # Example code that uses the library
```

## Quick Start

### Prerequisites

- JDK 11 or higher (21 recommended)
- CMake 3.10 or higher (for native build)
- Pre-compiled `libcmavsdk.dylib` (macOS) or `libcmavsdk.so` (Linux/Android)

### Setup

1. **Place the native libraries:**

For desktop (macOS/Linux):
```bash
cp /path/to/libcmavsdk.dylib mavsdk-kotlin/libs/
```

For Android:
```bash
mkdir -p mavsdk-kotlin/src/androidMain/jniLibs/arm64-v8a
cp /path/to/libcmavsdk.so mavsdk-kotlin/src/androidMain/jniLibs/arm64-v8a/
```

2. **Build the library:**

```bash
cd mavsdk-kotlin
./gradlew build
```

For Android, the CMake build is automatic via `externalNativeBuild`.

For desktop, you need to build the JNI wrapper manually first:
```bash
cd mavsdk-kotlin
mkdir -p build && cd build
cmake ../src/androidMain/cpp
make
```

3. **Run the example:**

```bash
cd examples
./gradlew run
```

## Usage

### Basic Example

```kotlin
import io.mavsdk.kotlin.*

fun main() {
    val config = Configuration.createWithComponentType(ComponentType.GROUND_STATION)
    
    Mavsdk(config).use { mavsdk ->
        mavsdk.addAnyConnection("udp://:14540")
            .onSuccess { println("Connected!") }
            .onFailure { println("Failed: $it") }
        
        val system = mavsdk.firstAutopilot()
        system?.let {
            println("System ${it.getSystemId()}")
            println("Type: ${it.getAutopilotType()}")
        }
    }
}
```

### Connection URLs

```kotlin
// UDP (SITL, companion computers)
mavsdk.addAnyConnection("udp://:14540")

// Serial (direct connection)
mavsdk.addAnyConnection("serial:///dev/ttyUSB0:57600")

// TCP
mavsdk.addAnyConnection("tcp://192.168.1.100:5760")
```

## Platform Support

- ✅ macOS (Desktop JVM)
- ✅ Linux (Desktop JVM)  
- ✅ Android (API 24+, arm64-v8a, armeabi-v7a, x86_64)
- ⚠️ Windows (ready, untested)

## API Overview

### Core Classes

- **`Mavsdk`**: Main entry point
- **`Configuration`**: MAVSDK configuration
- **`System`**: MAVLink system representation

### Enums

- **`ConnectionResult`**: Connection operation results
- **`ComponentType`**: MAVLink component types
- **`Autopilot`**: Autopilot types (PX4, ArduPilot, etc.)
- **`Vehicle`**: Vehicle types (multicopter, fixed-wing, etc.)

### Error Types

- **`MavsdkError`**: Base sealed class
  - `LibraryNotFoundError`
  - `ConnectionError`
  - `OperationError`
  - `TimeoutError`
  - `SystemNotFoundError`

## Building for Android

The library uses Android's `externalNativeBuild` with CMake:

```kotlin
android {
    externalNativeBuild {
        cmake {
            path = File("src/androidMain/cpp/CMakeLists.txt")
        }
    }
}
```

Place your `libcmavsdk.so` files in:
```
src/androidMain/jniLibs/
├── arm64-v8a/libcmavsdk.so
├── armeabi-v7a/libcmavsdk.so
└── x86_64/libcmavsdk.so
```

## Development

### Adding the Library as a Dependency

In your project's `build.gradle.kts`:

```kotlin
dependencies {
    implementation("io.mavsdk:mavsdk-kotlin-jvm:1.0.0")  // For JVM
    // or
    implementation("io.mavsdk:mavsdk-kotlin-android:1.0.0")  // For Android
}
```

Or use the composite build (as in the examples):

```kotlin
// settings.gradle.kts
includeBuild("../mavsdk-kotlin")
```

## Testing with PX4 SITL

```bash
# Start PX4 SITL
cd PX4-Autopilot
make px4_sitl gazebo

# In another terminal, run the example
cd mavsdk-kotlin/examples
./gradlew run
```

## Architecture

```
┌─────────────────────────────────────┐
│   Kotlin Application                │
│   (commonMain - shared code)        │
└────────────────┬────────────────────┘
                 │
        ┌────────┴────────┐
        │                 │
┌───────┴──────┐  ┌───────┴──────┐
│  jvmMain     │  │ androidMain  │
│  (Desktop)   │  │  (Android)   │
└───────┬──────┘  └───────┬──────┘
        │                 │
        └────────┬────────┘
                 │ JNI
┌────────────────┴────────────────────┐
│   C++ JNI Wrapper                   │
│   (androidMain/cpp - shared)        │
└────────────────┬────────────────────┘
                 │
┌────────────────┴────────────────────┐
│   libcmavsdk (C Library)            │
└─────────────────────────────────────┘
```

## License

BSD 3-Clause License (matching MAVSDK)

## Contributing

Contributions welcome! Please ensure:
- Code follows Kotlin idioms
- JNI code is memory-safe
- Both Android and JVM targets work
- Documentation is updated

## Roadmap

- [x] Kotlin Multiplatform structure
- [x] Core MAVSDK functionality
- [x] Android and JVM support
- [ ] Async operations with Flows
- [ ] Generated plugin support
- [ ] Action plugin
- [ ] Telemetry plugin
- [ ] Mission plugin

## See Also

- [MAVSDK](https://github.com/mavlink/MAVSDK)
- [MAVSDK-Python](https://github.com/mavlink/MAVSDK-Python)
- [MAVLink](https://mavlink.io/)
