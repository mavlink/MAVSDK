package io.mavsdk.kotlin

internal object NativeLibraryLoader {
    private var loaded = false

    @Synchronized
    fun loadLibrary() {
        if (loaded) return
        java.lang.System.loadLibrary("mavsdk_jni")
        loaded = true
    }
}
