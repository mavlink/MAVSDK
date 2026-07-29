import com.android.build.gradle.LibraryExtension
import com.vanniktech.maven.publish.SonatypeHost
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
    id("com.vanniktech.maven.publish") version "0.33.0"
}

apply(plugin = "com.android.library")

group = "io.mavsdk"
// The published version can be overridden without touching this file, because
// the publishing plugin reads the VERSION_NAME property:
// ./gradlew publishToMavenCentral -PVERSION_NAME=2.0.0
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
        publishLibraryVariants("release")
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

// Publishes the KMP artifact set: io.mavsdk:mavsdk-kotlin (root module with
// Gradle Module Metadata), plus -jvm and -android variants.
mavenPublishing {
    publishToMavenCentral(SonatypeHost.CENTRAL_PORTAL)

    // Maven Central requires signatures, but unconditional signing breaks
    // publishToMavenLocal for contributors without keys. CI supplies the key
    // as ORG_GRADLE_PROJECT_signingInMemoryKey, which Gradle exposes as this
    // project property.
    if (findProperty("signingInMemoryKey") != null) {
        signAllPublications()
    }

    // Version is deliberately omitted so the VERSION_NAME property can drive it.
    coordinates(group.toString(), "mavsdk-kotlin")

    pom {
        name.set("MAVSDK-Kotlin")
        description.set(
            "Kotlin Multiplatform library for MAVLink communication with drones, " +
                "backed by the MAVSDK C++ core via JNI."
        )
        url.set("https://github.com/mavlink/MAVSDK")
        inceptionYear.set("2017")

        licenses {
            license {
                name.set("BSD 3-Clause")
                url.set("https://opensource.org/licenses/BSD-3-Clause")
                distribution.set("repo")
            }
        }

        developers {
            developer {
                id.set("jonasvautherin")
                name.set("Jonas Vautherin")
                email.set("dev@jonas.vautherin.ch")
            }
        }

        scm {
            url.set("https://github.com/mavlink/MAVSDK")
            connection.set("scm:git:https://github.com/mavlink/MAVSDK")
            developerConnection.set("scm:git:https://github.com/mavlink/MAVSDK")
        }
    }
}
