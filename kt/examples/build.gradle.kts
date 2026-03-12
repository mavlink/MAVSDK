plugins {
    kotlin("jvm") version "2.0.0"
    application
}

group = "io.mavsdk.examples"
version = "1.0.0"

repositories {
    mavenCentral()
}

dependencies {
    implementation("io.mavsdk:mavsdk-kotlin:1.0.0")
    implementation("org.jetbrains.kotlinx:kotlinx-coroutines-core:1.7.3")
}

application {
    mainClass.set("io.mavsdk.kotlin.examples.MainKt")
}

kotlin {
    jvmToolchain(21)
}
