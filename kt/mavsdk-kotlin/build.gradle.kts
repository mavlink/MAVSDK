import com.android.build.gradle.LibraryExtension
import org.gradle.api.tasks.compile.JavaCompile
import org.gradle.jvm.tasks.Jar

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

val compileNeutralJniJava by tasks.registering(JavaCompile::class) {
    source(fileTree("../../jni/jvm/src/main/java") { include("**/*.java") })
    classpath = files()
    destinationDirectory.set(layout.buildDirectory.dir("classes/java/neutralJni"))
    options.release.set(8)
}
val neutralJniClasses = files(compileNeutralJniJava.flatMap { it.destinationDirectory })
    .builtBy(compileNeutralJniJava)

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

    targets.all {
        compilations.all {
            kotlinOptions {
                freeCompilerArgs += "-Xexpect-actual-classes"
            }
        }
    }
    
    sourceSets {
        val commonMain by getting {
            dependencies {
                implementation("org.jetbrains.kotlinx:kotlinx-coroutines-core:1.7.3")
            }
        }
        
        val sharedJvmAndroidSources = "src/jvmAndroidMain/kotlin"
        val jvmMain by getting {
            kotlin.srcDir(sharedJvmAndroidSources)
            dependencies {
                implementation(neutralJniClasses)
            }
        }
        val androidMain by getting {
            kotlin.srcDir(sharedJvmAndroidSources)
        }
    }
}

tasks.named<Jar>("jvmJar") {
    from(compileNeutralJniJava.flatMap { it.destinationDirectory })
}

configure<LibraryExtension> {
    namespace = "io.mavsdk.kotlin"
    compileSdk = 35
    
    defaultConfig {
        minSdk = 24
        ndk {
            abiFilters += listOf("arm64-v8a", "armeabi-v7a", "x86_64", "x86")
        }
    }
    
    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_21
        targetCompatibility = JavaVersion.VERSION_21
    }

    sourceSets.getByName("main").java.srcDir("../../jni/jvm/src/main/java")
}
