package io.mavsdk.kotlin.examples

val examples = mapOf(
    "TakeoffAndLand" to ::takeoffAndLand,
    "Calibrate" to ::calibrate,
    "FlyMission" to ::flyMission,
    "ParamsAll" to { params() },
    "Params" to ::paramsGetSet,
    "CameraClient" to ::cameraClient,
    "CameraServer" to ::cameraServer,
    "Telemetry" to ::telemetry,
)

fun main(args: Array<String>) {
    val name = args.firstOrNull() ?: run {
        println("Usage: ./gradlew run --args=\"<ExampleName>\"")
        println("Available examples: ${examples.keys.joinToString(", ")}")
        return
    }
    val example = examples[name] ?: run {
        println("Unknown example: $name")
        println("Available examples: ${examples.keys.joinToString(", ")}")
        return
    }
    example()
}
