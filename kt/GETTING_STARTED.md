# Getting Started with MAVSDK-Kotlin

This guide will help you get started with the Kotlin Multiplatform MAVSDK library.

## Setup

### 1. Prerequisites

**For Desktop (macOS/Linux):**
- JDK 11+ (JDK 21 recommended)
- CMake 3.10+
- Pre-compiled `libcmavsdk.dylib` (macOS) or `libcmavsdk.so` (Linux)

**For Android:**
- Android Studio or Gradle with Android plugin
- Android SDK (API 24+)
- NDK (for building JNI)
- Pre-compiled `libcmavsdk.so` for Android ABIs

### 2. Get the Native Libraries

You need the pre-compiled `libcmavsdk` library from the MAVSDK C project.

**For Desktop:**
```bash
# macOS
cp /path/to/libcmavsdk.dylib mavsdk-kotlin/libs/

# Linux
cp /path/to/libcmavsdk.so mavsdk-kotlin/libs/
```

**For Android:**
```bash
mkdir -p mavsdk-kotlin/src/androidMain/jniLibs/arm64-v8a
mkdir -p mavsdk-kotlin/src/androidMain/jniLibs/armeabi-v7a
mkdir -p mavsdk-kotlin/src/androidMain/jniLibs/x86_64

cp /path/to/android/arm64-v8a/libcmavsdk.so \
   mavsdk-kotlin/src/androidMain/jniLibs/arm64-v8a/

cp /path/to/android/armeabi-v7a/libcmavsdk.so \
   mavsdk-kotlin/src/androidMain/jniLibs/armeabi-v7a/

cp /path/to/android/x86_64/libcmavsdk.so \
   mavsdk-kotlin/src/androidMain/jniLibs/x86_64/
```

### 3. Build the Library

**For Desktop:**

First, build the repository-level JNI wrapper:
```bash
cmake -S ../jni -B ../jni/build -DCMAKE_BUILD_TYPE=Release
cmake --build ../jni/build
```

Then build the Kotlin library:
```bash
./gradlew :mavsdk-kotlin:build
```

**For Android:**

Build `cmavsdk` and `mavsdk-jni` for each Android ABI, place the libraries in
`mavsdk-kotlin/src/androidMain/jniLibs/<abi>/`, then assemble:
```bash
cd mavsdk-kotlin
./gradlew assembleDebug
```

### 4. Run the Example

```bash
cd examples/cli-jvm
./gradlew run
```

## Project Structure

```
mavsdk-kotlin-root/
├── mavsdk-kotlin/              # The library
│   ├── src/
│   │   ├── commonMain/         # Shared Kotlin code
│   │   │   └── kotlin/io/mavsdk/kotlin/
│   │   │       ├── Mavsdk.kt
│   │   │       ├── System.kt
│   │   │       ├── Configuration.kt
│   │   │       └── ...
│   │   ├── jvmMain/            # Desktop JVM
│   │   │   ├── kotlin/         # JVM-specific implementations
│   │   │   └── resources/native/<os>-<arch>/  # Where JNI libs go
│   │   ├── jvmAndroidMain/     # Shared neutral-JNI adapters
│   │   └── androidMain/
│   │       ├── kotlin/         # Android-specific implementations
│   │       └── jniLibs/        # Prebuilt Android libs
│   └── libs/                   # Desktop native libs
│
└── examples/                   # Example applications
    └── cli-jvm/                # Desktop JVM example
        └── src/main/kotlin/
            └── io/mavsdk/kotlin/examples/
                └── Main.kt
```

## Using the Library

### In Your Own Project

**Option 1: Composite Build (Development)**

Add to your `settings.gradle.kts`:
```kotlin
includeBuild("/path/to/mavsdk-kotlin")
```

Add to your `build.gradle.kts`:
```kotlin
dependencies {
    implementation("io.mavsdk:mavsdk-kotlin-jvm:1.0.0")
}
```

**Option 2: Maven/Local Repository**

Publish to local Maven:
```bash
cd mavsdk-kotlin
./gradlew publishToMavenLocal
```

Then in your project:
```kotlin
repositories {
    mavenLocal()
}

dependencies {
    implementation("io.mavsdk:mavsdk-kotlin-jvm:1.0.0")
}
```

### Basic Usage

```kotlin
import io.mavsdk.kotlin.*

fun main() {
    // 1. Create configuration
    val config = Configuration.createWithComponentType(
        ComponentType.GROUND_STATION
    )

    // 2. Create MAVSDK with automatic cleanup
    config.use { cfg ->
        Mavsdk(cfg).use { mavsdk ->
            println("MAVSDK version: ${mavsdk.version()}")
            
            // 3. Connect
            mavsdk.addAnyConnection("udp://:14540")
                .onSuccess { println("Connected!") }
                .onFailure { error -> println("Failed: $error") }

            // 4. Get first autopilot
            val system = mavsdk.firstAutopilot()
            system?.let {
                println("Found system ${it.getSystemId()}")
                println("Autopilot: ${it.getAutopilotType()}")
                println("Vehicle: ${it.getVehicleType()}")
            }
        }
    }
}
```

## Testing with PX4 SITL

### Install PX4

```bash
git clone https://github.com/PX4/PX4-Autopilot.git --recursive
cd PX4-Autopilot
```

### Run SITL

```bash
make px4_sitl gazebo
```

This starts a simulated drone broadcasting on UDP port 14540.

### Run Your Program

In another terminal:
```bash
cd mavsdk-kotlin/examples/cli-jvm
./gradlew run
```

You should see:
```
=== MAVSDK-Kotlin Example ===

Created configuration:
  System ID: 245
  Component ID: 190

MAVSDK version: v2.0.0

Connecting to udp://:14540...
✓ Connection added successfully

Waiting for autopilot...
✓ Found autopilot!

=== System Information ===
System ID: 1
Connected: true
Has autopilot: true
Autopilot type: PX4
Vehicle type: MULTICOPTER
...
```

## Platform-Specific Notes

### macOS

The JNI library (`libmavsdk_jni.dylib`) must be in one of:
- `java.library.path`
- Packaged in resources (`src/jvmMain/resources/native/<os>-<arch>/`)

The library loader will extract it to a temp file if needed.

The `<os>-<arch>` directory lets a single jar carry every supported platform.
`<os>` is one of `darwin`, `linux`, `windows`; `<arch>` is one of `aarch64`,
`x86_64`, `arm`, `x86` (normalised from the JVM's `os.arch`). On Apple Silicon
that gives `native/darwin-aarch64/libmavsdk_jni.dylib`. A library placed
directly in `native/` is still picked up as a fallback, which is convenient
when building locally for just the current machine.

### Android

- Uses prebuilt `cmavsdk` and `mavsdk-jni` libraries
- Native libraries go in `jniLibs/{ABI}/`
- Supported ABIs: arm64-v8a, armeabi-v7a, x86_64
- Minimum SDK: 24

### Linux

Similar to macOS but with `.so` extension.

## Common Issues

### "Library not found"

**Desktop:**
- Ensure `libcmavsdk.dylib/.so` is in `mavsdk-kotlin/libs/`
- Ensure JNI wrapper was built with CMake

**Android:**
- Ensure `libcmavsdk.so` is in correct ABI directory
- Check Gradle sync completed successfully

### "No autopilot found"

- Ensure PX4 SITL or your drone is running
- Check the connection URL matches your setup
- Try increasing timeout: `firstAutopilot(timeoutSeconds = 10.0)`

### JNI Build Errors

**Desktop:**
```bash
cmake -S ../jni -B ../jni/build
cmake --build ../jni/build
```

**Android:**
Clean and rebuild:
```bash
./gradlew clean
./gradlew assembleDebug
```

## Next Steps

1. **Explore the API**: Check the source files in `commonMain`
2. **Try the examples**: See `examples/cli-jvm/src/main/kotlin/`
3. **Add plugins**: Action, Telemetry, Mission (coming soon)
4. **Contribute**: Help add Flow-based async operations!

## Support

- **Issues**: GitHub Issues
- **Discussions**: GitHub Discussions
- **Examples**: See `examples/` directory

Happy flying! 🚁
