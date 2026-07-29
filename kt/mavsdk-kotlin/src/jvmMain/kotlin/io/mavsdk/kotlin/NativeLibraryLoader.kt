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

        try {
            // Try to load from java.library.path first
            java.lang.System.loadLibrary("mavsdk_jni")
            loaded = true
            println("✓ Loaded libmavsdk_jni from java.library.path")
            return
        } catch (e: UnsatisfiedLinkError) {
            // Continue to try loading from resources
        }

        val libraryName = getLibraryName()
        // Prefer the per-platform layout, so a single jar can carry every
        // supported OS/architecture combination. The flat layout is kept as a
        // fallback for locally built libraries.
        val candidates = listOf(
            "/native/${getOsName()}-${getArchName()}/$libraryName",
            "/native/$libraryName",
        )

        val (resourcePath, inputStream) = candidates.firstNotNullOfOrNull { candidate ->
            NativeLibraryLoader::class.java.getResourceAsStream(candidate)?.let { candidate to it }
        } ?: throw MavsdkError.LibraryNotFoundError(
            "Native library not found in resources, tried: ${candidates.joinToString(", ")}"
        )

        try {
            // Extract from resources to temp file
            val tempFile = File.createTempFile("libmavsdk_jni", getNativeExtension())
            tempFile.deleteOnExit()

            inputStream.use { input ->
                FileOutputStream(tempFile).use { output ->
                    input.copyTo(output)
                }
            }

            java.lang.System.load(tempFile.absolutePath)
            loaded = true
            println("✓ Loaded libmavsdk_jni from resources: $resourcePath")
        } catch (e: Exception) {
            throw MavsdkError.LibraryNotFoundError(
                "Failed to load native library from $resourcePath: ${e.message}"
            )
        }
    }

    private fun getOsName(): String {
        val os = java.lang.System.getProperty("os.name").orEmpty().lowercase()
        return when {
            os.contains("mac") || os.contains("darwin") -> "darwin"
            os.contains("linux") -> "linux"
            os.contains("windows") -> "windows"
            else -> throw MavsdkError.LibraryNotFoundError("Unsupported OS: $os")
        }
    }

    private fun getArchName(): String {
        val arch = java.lang.System.getProperty("os.arch").orEmpty().lowercase()
        return when (arch) {
            "aarch64", "arm64" -> "aarch64"
            "x86_64", "amd64" -> "x86_64"
            "arm", "armv7l" -> "arm"
            "x86", "i386", "i486", "i586", "i686" -> "x86"
            else -> throw MavsdkError.LibraryNotFoundError("Unsupported architecture: $arch")
        }
    }

    private fun getLibraryName(): String = when (val os = getOsName()) {
        "darwin" -> "libmavsdk_jni.dylib"
        "linux" -> "libmavsdk_jni.so"
        "windows" -> "mavsdk_jni.dll"
        else -> throw MavsdkError.LibraryNotFoundError("Unsupported OS: $os")
    }

    private fun getNativeExtension(): String = when (val os = getOsName()) {
        "darwin" -> ".dylib"
        "linux" -> ".so"
        "windows" -> ".dll"
        else -> throw MavsdkError.LibraryNotFoundError("Unsupported OS: $os")
    }
}
