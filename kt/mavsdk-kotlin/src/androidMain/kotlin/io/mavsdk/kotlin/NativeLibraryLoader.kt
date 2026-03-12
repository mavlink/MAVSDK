package io.mavsdk.kotlin

import java.io.File
import java.io.FileOutputStream

/**
 * Utility to load the native MAVSDK library for JVM
 */
internal object NativeLibraryLoader {
    private var loaded = false

    @Synchronized
    fun loadLibrary() {
        if (loaded) return

        val libraryName = getLibraryName()
        val resourcePath = "/native/$libraryName"

        try {
            // Try to load from java.library.path first
            java.lang.System.loadLibrary("mavsdk_jni")
            loaded = true
            println("✓ Loaded libmavsdk_jni from java.library.path")
            return
        } catch (e: UnsatisfiedLinkError) {
            // Continue to try loading from resources
        }

        try {
            // Extract from resources to temp file
            val inputStream = NativeLibraryLoader::class.java.getResourceAsStream(resourcePath)
                ?: throw MavsdkError.LibraryNotFoundError(
                    "Native library not found in resources: $resourcePath"
                )

            val tempFile = File.createTempFile("libmavsdk_jni", getNativeExtension())
            tempFile.deleteOnExit()

            inputStream.use { input ->
                FileOutputStream(tempFile).use { output ->
                    input.copyTo(output)
                }
            }

            java.lang.System.load(tempFile.absolutePath)
            loaded = true
            println("✓ Loaded libmavsdk_jni from resources: ${tempFile.absolutePath}")
        } catch (e: Exception) {
            throw MavsdkError.LibraryNotFoundError(
                "Failed to load native library: ${e.message}"
            )
        }
    }

    private fun getLibraryName(): String {
        val os = java.lang.System.getProperty("os.name").lowercase()
        return when {
            os.contains("mac") || os.contains("darwin") -> "libmavsdk_jni.dylib"
            os.contains("linux") -> "libmavsdk_jni.so"
            os.contains("windows") -> "mavsdk_jni.dll"
            else -> throw MavsdkError.LibraryNotFoundError("Unsupported OS: $os")
        }
    }

    private fun getNativeExtension(): String {
        val os = java.lang.System.getProperty("os.name").lowercase()
        return when {
            os.contains("mac") || os.contains("darwin") -> ".dylib"
            os.contains("linux") -> ".so"
            os.contains("windows") -> ".dll"
            else -> ""
        }
    }
}
