rootProject.name = "ExampleApp"
enableFeaturePreview("TYPESAFE_PROJECT_ACCESSORS")

pluginManagement {
    repositories {
        google {
            mavenContent {
                includeGroupAndSubgroups("androidx")
                includeGroupAndSubgroups("com.android")
                includeGroupAndSubgroups("com.google")
            }
        }
        mavenCentral()
        gradlePluginPortal()
    }
}

dependencyResolutionManagement {
    repositories {
        google {
            mavenContent {
                includeGroupAndSubgroups("androidx")
                includeGroupAndSubgroups("com.android")
                includeGroupAndSubgroups("com.google")
            }
        }
        mavenCentral()
    }
}

include(":androidApp")
include(":shared")

// Builds mavsdk-kotlin from source and substitutes it for the
// io.mavsdk:mavsdk-kotlin dependency, so the app always tracks the local
// library. Same arrangement as examples/cli-jvm. Both builds must apply the
// same AGP version -- AGP refuses to have two of itself in one build.
includeBuild("../../mavsdk-kotlin")
