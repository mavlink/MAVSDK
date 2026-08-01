This is a Kotlin Multiplatform project targeting Android, iOS.

* [/iosApp](./iosApp/iosApp) contains an iOS application. Even if you’re sharing your UI with Compose Multiplatform,
  you need this entry point for your iOS app. This is also where you should add SwiftUI code for your project.

* [/shared](./shared/src) is for code that will be shared across your Compose Multiplatform applications.
  It contains several subfolders:
  - [commonMain](./shared/src/commonMain/kotlin) is for code that’s common for all targets.
  - Other folders are for Kotlin code that will be compiled for only the platform indicated in the folder name.
    For example, if you want to use Apple’s CoreCrypto for the iOS part of your Kotlin app,
    the [iosMain](./shared/src/iosMain/kotlin) folder would be the right place for such calls.
    Similarly, if you want to edit the Desktop (JVM) specific part, the [jvmMain](./shared/src/jvmMain/kotlin)
    folder is the appropriate location.

### What the app does

A single "Fly mission" button runs the same waypoint mission as the
`FlyMission` example in [examples/cli-jvm](../cli-jvm), streaming its log into
the screen. The connection URL is editable — `udp://:14540` listens on the
device, `udpout://<host-ip>:14540` reaches a SITL running on your machine.

The implementation is `expect`/`actual`
([MissionRunner.kt](./shared/src/commonMain/kotlin/io/mavsdk/kotlin/exampleapp/MissionRunner.kt)):
Android drives real MAVSDK, iOS reports that it is not supported yet, because
`mavsdk-kotlin` has no iOS target so far (see `KOTLIN_IOS_PLAN.md` at the
repository root). When it does, the Android implementation moves to
`commonMain` unchanged.

### Prerequisites for Android

`mavsdk-kotlin` itself is built from source — `settings.gradle.kts` includes it
as a composite build, so no publishing step is needed. But the native libraries
it loads are not built by Gradle: build `cmavsdk` and `mavsdk_jni` for each ABI
you want and drop them in `kt/mavsdk-kotlin/src/androidMain/jniLibs/<abi>/` —
see [GETTING_STARTED.md](../../GETTING_STARTED.md). Without them the app
installs but crashes on `System.loadLibrary("mavsdk_jni")` when you press the
button.

Note that a composite build requires both sides to apply the same AGP version,
so bumping AGP here means bumping it in `kt/mavsdk-kotlin` too.

### Running the apps

Use the run configurations provided by the run widget in your IDE's toolbar. You can also use these commands and options:

- Android app: `./gradlew :androidApp:assembleDebug`
- iOS app: open the [/iosApp](./iosApp) directory in Xcode and run it from there.

---

Learn more about [Kotlin Multiplatform](https://www.jetbrains.com/help/kotlin-multiplatform-dev/get-started.html)…