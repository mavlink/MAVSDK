This is a Kotlin Multiplatform project currently targeting Android only.

* [/androidApp](./androidApp) is the Android entry point: a single activity
  hosting the shared Compose UI.

* [/shared](./shared/src) is the Compose Multiplatform module holding the app itself.
  It contains several subfolders:
  - [commonMain](./shared/src/commonMain/kotlin) is for code that’s common for all targets.
  - Other folders are for Kotlin code that will be compiled for only the platform indicated in the folder name,
    so [androidMain](./shared/src/androidMain/kotlin) for the Android-specific parts. Adding a platform back
    means declaring its target in [shared/build.gradle.kts](./shared/build.gradle.kts) and adding the matching
    source set with the `actual` declarations.

### What the app does

A single "Fly mission" button runs the same waypoint mission as the
`FlyMission` example in [examples/cli-jvm](../cli-jvm), streaming its log into
the screen. The connection URL is editable — `udp://:14540` listens on the
device, `udpout://<host-ip>:14540` reaches a SITL running on your machine.

The implementation is `expect`/`actual`
([MissionRunner.kt](./shared/src/commonMain/kotlin/io/mavsdk/kotlin/exampleapp/MissionRunner.kt)),
with the only `actual` driving real MAVSDK on Android. It is kept that way
because `mavsdk-kotlin` has no iOS target so far (see `KOTLIN_IOS_PLAN.md` at
the repository root); once it does, the Android implementation moves to
`commonMain` unchanged.

### Prerequisites

`mavsdk-kotlin` itself is built from source — `settings.gradle.kts` includes it
as a composite build, so no publishing step is needed. But the native libraries
it loads are not built by Gradle: build `cmavsdk` and `mavsdk_jni` for each ABI
you want and drop them in `kt/mavsdk-kotlin/src/androidMain/jniLibs/<abi>/` —
see [GETTING_STARTED.md](../../GETTING_STARTED.md). Without them the app
installs but crashes on `System.loadLibrary("mavsdk_jni")` when you press the
button.

Note that a composite build requires both sides to apply the same AGP version,
so bumping AGP here means bumping it in `kt/mavsdk-kotlin` too.

### Running the app

Use the run configuration provided by the run widget in your IDE's toolbar, or
build it from the command line with `./gradlew :androidApp:assembleDebug`.

---

Learn more about [Kotlin Multiplatform](https://www.jetbrains.com/help/kotlin-multiplatform-dev/get-started.html)…