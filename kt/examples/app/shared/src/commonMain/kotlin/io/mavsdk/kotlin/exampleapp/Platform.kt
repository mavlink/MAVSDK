package io.mavsdk.kotlin.exampleapp

interface Platform {
    val name: String
}

expect fun getPlatform(): Platform