import com.android.build.gradle.LibraryExtension

buildscript {
    repositories {
        google()
        mavenCentral()
    }
    dependencies {
        classpath("com.android.tools.build:gradle:8.7.3")
    }
}

plugins {
    kotlin("multiplatform") version "2.0.0"
    id("maven-publish")
}

apply(plugin = "com.android.library")

group = "io.mavsdk"
version = "1.0.0"

repositories {
    google()
    mavenCentral()
}

kotlin {
    jvm {
        compilations.all {
            kotlinOptions.jvmTarget = "21"
        }
    }
    
    androidTarget {
        publishLibraryVariants("release", "debug")
        compilations.all {
            kotlinOptions.jvmTarget = "21"
        }
    }
    
    sourceSets {
        val commonMain by getting {
            dependencies {
                implementation("org.jetbrains.kotlinx:kotlinx-coroutines-core:1.7.3")
            }
        }
        
        val jvmMain by getting
        val androidMain by getting
    }
}

configure<LibraryExtension> {
    namespace = "io.mavsdk.kotlin"
    compileSdk = 35
    
    defaultConfig {
        minSdk = 24
        ndk {
            abiFilters += listOf("arm64-v8a", "armeabi-v7a", "x86_64")
        }
    }
    
    externalNativeBuild {
        cmake {
            path = file("src/androidMain/cpp/CMakeLists.txt")
        }
    }
    
    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_21
        targetCompatibility = JavaVersion.VERSION_21
    }
}

// Build JNI library for JVM target
tasks.register<Exec>("configureCMake") {
    description = "Configure CMake for JNI build"
    
    doFirst {
        mkdir("build/jni")
    }
    
    workingDir = file("build/jni")
    commandLine("cmake", "../../src/androidMain/cpp")
}

tasks.register<Exec>("buildJniJvm") {
    description = "Build JNI library using CMake"
    dependsOn("configureCMake")
    
    val outputLib = file("build/jni/libmavsdk_jni.dylib")
    outputs.file(outputLib)
    inputs.dir("src/androidMain/cpp")
    
    workingDir = file("build/jni")
    commandLine("cmake", "--build", ".")
}

tasks.register<Copy>("copyJniToResources") {
    dependsOn("buildJniJvm")
    
    from(fileTree("build/jni") {
        include("*.dylib", "*.so", "*.dll")
    })
    into("src/jvmMain/resources/native")
}

tasks.register<Copy>("copyCmavsdkToResources") {
    from("libs")
    into("src/jvmMain/resources/native")
    include("*.dylib", "*.so", "*.dll")
}

tasks.named("jvmProcessResources") {
    dependsOn("copyJniToResources", "copyCmavsdkToResources")
}
