package io.mavsdk.kotlin.exampleapp

import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.ColumnScope
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.heightIn
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.safeContentPadding
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.foundation.lazy.rememberLazyListState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.Button
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.OutlinedTextField
import androidx.compose.material3.Text
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import kotlin.math.roundToInt
import kotlinx.coroutines.launch
import org.ramani.compose.CameraPosition
import org.ramani.compose.CenterState
import org.ramani.compose.DefaultMarkerImage
import org.ramani.compose.LatLng
import org.ramani.compose.MapLibre
import org.ramani.compose.MapStyle
import org.ramani.compose.Polyline
import org.ramani.compose.Symbol
import org.ramani.compose.rememberCameraPositionState

/**
 * Keyless tiles, so the example runs without signing up for anything. Note this
 * is a third-party public service; swap in your own style URL for real use.
 */
private const val MAP_STYLE_URL = "https://tiles.openfreemap.org/styles/liberty"

private val SITL_HOME = LatLng(47.3977, 8.5456)

@Composable
@Preview
fun App() {
    MaterialTheme {
        val scope = rememberCoroutineScope()

        var connectionUrl by remember { mutableStateOf("udpin://0.0.0.0:14540") }
        var running by remember { mutableStateOf(false) }
        var showLog by remember { mutableStateOf(false) }
        val log = remember { mutableStateListOf<String>() }
        val logState = rememberLazyListState()

        // Null until the first position arrives; that is what hides the marker.
        var drone by remember { mutableStateOf<MissionEvent.Telemetry?>(null) }
        val track = remember { mutableStateListOf<LatLng>() }

        val cameraPositionState = rememberCameraPositionState(
            CameraPosition(target = SITL_HOME, zoom = 16.0)
        )

        // Keep the newest line in view as the mission progresses.
        LaunchedEffect(log.size) {
            if (log.isNotEmpty()) logState.animateScrollToItem(log.lastIndex)
        }

        Box(Modifier.fillMaxSize()) {
            MapLibre(
                modifier = Modifier.fillMaxSize(),
                style = MapStyle.Uri(MAP_STYLE_URL),
                cameraPositionState = cameraPositionState,
            ) {
                if (track.size > 1) {
                    Polyline(points = track, color = "#1E88E5", lineWidth = 3.0F)
                }
                drone?.let { position ->
                    Symbol(
                        centerState = CenterState(LatLng(position.latitude, position.longitude)),
                        // The icon points north, so the heading maps straight onto
                        // the rotation; null leaves it upright until the first one.
                        imageRotation = position.headingDeg,
                        imageId = droneIconResource ?: DefaultMarkerImage,
                        size = 1.5F,
                    )
                }
            }

            Column(
                modifier = Modifier
                    .align(Alignment.TopCenter)
                    .safeContentPadding()
                    .fillMaxWidth()
                    .padding(16.dp),
                horizontalAlignment = Alignment.CenterHorizontally,
            ) {
                Surfaceish {
                    OutlinedTextField(
                        value = connectionUrl,
                        onValueChange = { connectionUrl = it },
                        label = { Text("Connection URL") },
                        singleLine = true,
                        enabled = !running,
                        modifier = Modifier.fillMaxWidth(),
                    )
                    Text(
                        "Use udpout://<host-ip>:<host-port> to reach the drone.",
                        style = MaterialTheme.typography.labelSmall,
                        textAlign = TextAlign.Center,
                        modifier = Modifier.padding(top = 4.dp),
                    )
                    drone?.let {
                        Text(
                            "lat ${it.latitude.format(6)}  lon ${it.longitude.format(6)}" +
                                "  alt ${it.relativeAltitudeM.roundToInt()} m" +
                                (it.headingDeg?.let { h -> "  hdg ${h.toInt()}°" } ?: ""),
                            style = MaterialTheme.typography.labelSmall,
                            modifier = Modifier.padding(top = 4.dp),
                        )
                    }
                }
            }

            Column(
                modifier = Modifier
                    .align(Alignment.BottomCenter)
                    .safeContentPadding()
                    .fillMaxWidth()
                    .padding(16.dp),
                horizontalAlignment = Alignment.CenterHorizontally,
                verticalArrangement = Arrangement.Bottom,
            ) {
                if (showLog && log.isNotEmpty()) {
                    Surfaceish {
                        LazyColumn(
                            state = logState,
                            modifier = Modifier.fillMaxWidth().heightIn(max = 220.dp),
                        ) {
                            items(log) { line ->
                                Text(line, style = MaterialTheme.typography.bodySmall)
                            }
                        }
                    }
                    Spacer(Modifier.height(8.dp))
                }

                Controls(
                    onFly = {
                        log.clear()
                        track.clear()
                        drone = null
                        running = true
                        showLog = true
                        // Collected on the main dispatcher, so touching the state
                        // below is safe; flyMission works on Dispatchers.IO.
                        scope.launch {
                            try {
                                flyMission(connectionUrl).collect { event ->
                                    when (event) {
                                        is MissionEvent.Log -> log += event.line
                                        is MissionEvent.Telemetry -> {
                                            drone = event
                                            val point = LatLng(event.latitude, event.longitude)
                                            track += point
                                            cameraPositionState.position =
                                                cameraPositionState.position.copy(target = point)
                                        }
                                    }
                                }
                            } catch (e: Throwable) {
                                log += "✗ ${e::class.simpleName}: ${e.message}"
                            } finally {
                                running = false
                            }
                        }
                    },
                    running = running,
                    showLog = showLog,
                    onToggleLog = { showLog = !showLog },
                    hasLog = log.isNotEmpty(),
                )
            }
        }
    }
}

/** Translucent panel so the map stays readable underneath. */
@Composable
private fun Surfaceish(content: @Composable ColumnScope.() -> Unit) {
    Column(
        modifier = Modifier
            .fillMaxWidth()
            .background(Color.White.copy(alpha = 0.85f), RoundedCornerShape(8.dp))
            .padding(horizontal = 12.dp, vertical = 8.dp),
        horizontalAlignment = Alignment.CenterHorizontally,
        content = content,
    )
}

@Composable
private fun Controls(
    onFly: () -> Unit,
    running: Boolean,
    showLog: Boolean,
    onToggleLog: () -> Unit,
    hasLog: Boolean,
) {
    Column(horizontalAlignment = Alignment.CenterHorizontally) {
        Button(enabled = !running, onClick = onFly) {
            Text(if (running) "Flying..." else "Fly mission")
        }
        if (hasLog) {
            Button(onClick = onToggleLog) {
                Text(if (showLog) "Hide log" else "Show log")
            }
        }
    }
}

/** `Double.toString()` on Kotlin/Native prints more digits than we want here. */
private fun Double.format(decimals: Int): String {
    val factor = generateSequence(1.0) { it * 10 }.take(decimals + 1).last()
    val rounded = kotlin.math.round(this * factor) / factor
    return rounded.toString().take(rounded.toString().indexOf('.') + decimals + 1)
}
