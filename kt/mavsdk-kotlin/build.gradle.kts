import org.gradle.api.tasks.compile.JavaCompile
import org.gradle.jvm.tasks.Jar
import org.jetbrains.kotlin.gradle.dsl.JvmTarget

plugins {
    kotlin("multiplatform") version "2.4.10"
    id("com.android.kotlin.multiplatform.library") version "9.3.1"
    id("com.vanniktech.maven.publish") version "0.37.0"
    id("com.ncorti.ktfmt.gradle") version "0.26.0"
}

// Most of the Kotlin here is generated from templates/, so formatting is done by
// ktfmt rather than by getting the whitespace exactly right in the templates:
// ./gradlew ktfmtFormat (ktfmtCheck to verify without writing).
ktfmt { kotlinLangStyle() }

group = "io.mavsdk"

// The published version can be overridden without touching this file, because
// the publishing plugin reads the VERSION_NAME property:
// ./gradlew publishToMavenCentral -PVERSION_NAME=2.0.0
version = "1.0.0"

repositories {
    google()
    mavenCentral()
}

val compileNeutralJniJava by
    tasks.registering(JavaCompile::class) {
        source(fileTree("../../jni/jvm/src/main/java") { include("**/*.java") })
        classpath = files()
        destinationDirectory.set(layout.buildDirectory.dir("classes/java/neutralJni"))
        options.release.set(8)
    }
val neutralJniClasses =
    files(compileNeutralJniJava.flatMap { it.destinationDirectory }).builtBy(compileNeutralJniJava)

kotlin {
    jvm { compilerOptions { jvmTarget.set(JvmTarget.JVM_21) } }

    androidLibrary {
        namespace = "io.mavsdk.kotlin"
        compileSdk = 35
        minSdk = 24

        // The generated JNI contracts are Java, and src/androidMain/java is a
        // symlink to jni/jvm/src/main/java so both targets share one copy.
        // withJava() is what makes AGP compile them into the AAR, and it also
        // puts them on the Kotlin compilation's classpath.
        withJava()

        compilerOptions { jvmTarget.set(JvmTarget.JVM_21) }
    }

    compilerOptions { freeCompilerArgs.add("-Xexpect-actual-classes") }

    sourceSets {
        val commonMain by getting {
            dependencies { api("org.jetbrains.kotlinx:kotlinx-coroutines-core:1.7.3") }
        }

        val sharedJvmAndroidSources = "src/jvmAndroidMain/kotlin"
        val jvmMain by getting {
            kotlin.srcDir(sharedJvmAndroidSources)
            dependencies { implementation(neutralJniClasses) }
        }
        val androidMain by getting { kotlin.srcDir(sharedJvmAndroidSources) }
    }
}

tasks.named<Jar>("jvmJar") { from(compileNeutralJniJava.flatMap { it.destinationDirectory }) }

// Publishes the KMP artifact set: io.mavsdk:mavsdk-kotlin (root module with
// Gradle Module Metadata), plus -jvm and -android variants.
mavenPublishing {
    publishToMavenCentral()

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
