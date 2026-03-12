package io.mavsdk.kotlin.examples

import io.mavsdk.kotlin.*
import io.mavsdk.kotlin.plugins.param.Param
import kotlinx.coroutines.*

/**
 * Example demonstrating reading and writing vehicle parameters using the Param plugin.
 *
 * Usage (hardcoded for simplicity — adapt as needed):
 *   action = "get_all"     → list all int, float, and custom parameters
 *   action = "get"         → read a single int or float parameter
 *   action = "set_int"     → write an integer parameter
 *   action = "set_float"   → write a float parameter
 *
 * Prerequisites: PX4 SITL running on UDP port 14540.
 *   make px4_sitl gazebo
 */
fun params(action: String = "get_all", paramName: String = "", value: String = "") = runBlocking {
    println("=== MAVSDK-Kotlin Params Example ===\n")

    val config = Configuration.createWithComponentType(ComponentType.GROUND_STATION)
    config.use { cfg ->
        Mavsdk(cfg).use { mavsdk ->
            println("Connecting to udp://:14540...")
            mavsdk.addAnyConnection("udp://:14540")
                .onFailure { error ->
                    println("✗ Connection failed: $error")
                    return@runBlocking
                }

            println("Waiting for autopilot...")
            val system = mavsdk.firstAutopilot(timeoutSeconds = 10.0)
            if (system == null) {
                println("✗ No autopilot found")
                return@runBlocking
            }
            println("✓ Autopilot found!\n")

            Param.create(system).use { param ->
                when (action) {
                    "get_all" -> getAllParams(param)
                    "get" -> getParam(param, paramName)
                    "set_int" -> setIntParam(param, paramName, value.toInt())
                    "set_float" -> setFloatParam(param, paramName, value.toFloat())
                    else -> println("Unknown action '$action'. Use: get_all / get / set_int / set_float")
                }
            }
        }
    }
}

fun paramsGetSet() = runBlocking {
    println("=== MAVSDK-Kotlin Params Get/Set Example ===\n")

    val config = Configuration.createWithComponentType(ComponentType.GROUND_STATION)
    config.use { cfg ->
        Mavsdk(cfg).use { mavsdk ->
            println("Connecting to udp://:14540...")
            mavsdk.addAnyConnection("udp://:14540")
                .onFailure { error ->
                    println("✗ Connection failed: $error")
                    return@runBlocking
                }

            println("Waiting for autopilot...")
            val system = mavsdk.firstAutopilot(timeoutSeconds = 10.0)
            if (system == null) {
                println("✗ No autopilot found")
                return@runBlocking
            }
            println("✓ Autopilot found!\n")

            Param.create(system).use { param ->
                setFloatParam(param, "MIS_TAKEOFF_ALT", 5.0f)
                getParam(param, "MIS_TAKEOFF_ALT")
                setFloatParam(param, "MIS_TAKEOFF_ALT", 2.5f)
                getParam(param, "MIS_TAKEOFF_ALT")
            }
        }
    }
}

private fun getAllParams(param: Param) {
    val all = param.getAllParams()
    println("Int params (${all.intParams.size}):")
    all.intParams.forEach { println("  ${it.name} = ${it.value}") }
    println("Float params (${all.floatParams.size}):")
    all.floatParams.forEach { println("  ${it.name} = ${it.value}") }
    if (all.customParams.isNotEmpty()) {
        println("Custom params (${all.customParams.size}):")
        all.customParams.forEach { println("  ${it.name} = ${it.value}") }
    }
}

private fun getParam(param: Param, name: String) {
    // Try float first, then int — mirrors the C example's type-probe approach.
    print("Reading '$name' as float... ")
    try {
        val v = param.getParamFloat(name)
        println("$v")
        return
    } catch (e: Exception) {
        println("failed (${e.message})")
    }

    print("Reading '$name' as int... ")
    try {
        val v = param.getParamInt(name)
        println("$v")
    } catch (e: Exception) {
        println("failed (${e.message})")
    }
}

private fun setIntParam(param: Param, name: String, value: Int) {
    print("Setting '$name' = $value (int)... ")
    val result = param.setParamInt(name, value)
    println(if (result == Param.Result.SUCCESS) "✓ OK" else "✗ $result")
}

private fun setFloatParam(param: Param, name: String, value: Float) {
    print("Setting '$name' = $value (float)... ")
    val result = param.setParamFloat(name, value)
    println(if (result == Param.Result.SUCCESS) "✓ OK" else "✗ $result")
}
