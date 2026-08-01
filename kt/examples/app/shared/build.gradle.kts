import org.jetbrains.kotlin.gradle.dsl.JvmTarget

plugins {
    alias(libs.plugins.kotlinMultiplatform)
    alias(libs.plugins.androidMultiplatformLibrary)
    alias(libs.plugins.composeMultiplatform)
    alias(libs.plugins.composeCompiler)
}

kotlin {
    listOf(
        iosArm64(),
        iosSimulatorArm64()
    ).forEach { iosTarget ->
        iosTarget.binaries.framework {
            baseName = "Shared"
            isStatic = true
        }
    }
    
    androidLibrary {
       namespace = "io.mavsdk.kotlin.exampleapp.shared"
       compileSdk = libs.versions.android.compileSdk.get().toInt()
       minSdk = libs.versions.android.minSdk.get().toInt()
    
       compilerOptions {
           // Matches mavsdk-kotlin's android target: Gradle refuses to resolve a
           // dependency built for a higher JVM version than the consumer.
           jvmTarget = JvmTarget.JVM_21
       }
       androidResources {
           enable = true
       }
       withHostTest {
           isIncludeAndroidResources = true
       }
    }
    
    sourceSets {
        androidMain.dependencies {
            implementation(libs.compose.uiToolingPreview)
            // Android-only for now: mavsdk-kotlin ships jvm and android targets
            // but no iOS one yet (see KOTLIN_IOS_PLAN.md), which is why
            // flyMission() is expect/actual rather than common code.
            //
            // Testing a CI-built .aar instead of the local source build:
            //   ./gradlew :androidApp:assembleDebug -PmavsdkAar=/path/to/mavsdk-kotlin.aar
            // and comment out the includeBuild in settings.gradle.kts.
            val mavsdkAar = providers.gradleProperty("mavsdkAar").orNull
            if (mavsdkAar != null) {
                implementation(files(mavsdkAar))
            } else {
                implementation(libs.mavsdk.kotlin)
            }
        }
        commonMain.dependencies {
            implementation(libs.kotlinx.coroutines.core)
            implementation(libs.compose.runtime)
            implementation(libs.compose.foundation)
            implementation(libs.compose.material3)
            implementation(libs.compose.ui)
            implementation(libs.compose.components.resources)
            implementation(libs.compose.uiToolingPreview)
            implementation(libs.androidx.lifecycle.viewmodelCompose)
            implementation(libs.androidx.lifecycle.runtimeCompose)
            // Publishes for android, iosArm64 and iosSimulatorArm64 -- the same
            // targets as this module -- so the map screen can be common code even
            // though flyMission is still Android-only.
            implementation(libs.ramani.maplibre)
        }
        commonTest.dependencies {
            implementation(libs.kotlin.test)
        }
    }
}

dependencies {
    androidRuntimeClasspath(libs.compose.uiTooling)
}