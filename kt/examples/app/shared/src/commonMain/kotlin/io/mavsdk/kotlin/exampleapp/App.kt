package io.mavsdk.kotlin.exampleapp

import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
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
import androidx.compose.material3.Button
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.OutlinedTextField
import androidx.compose.material3.Text
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import kotlinx.coroutines.launch

@Composable
@Preview
fun App() {
    MaterialTheme {
        val scope = rememberCoroutineScope()
        val greeting = remember { Greeting().greet() }

        var connectionUrl by remember { mutableStateOf("udp://:14540") }
        var running by remember { mutableStateOf(false) }
        val log = remember { mutableStateListOf<String>() }
        val logState = rememberLazyListState()

        // Keep the newest line in view as the mission progresses.
        LaunchedEffect(log.size) {
            if (log.isNotEmpty()) logState.animateScrollToItem(log.lastIndex)
        }

        Column(
            modifier = Modifier
                .safeContentPadding()
                .fillMaxSize()
                .padding(24.dp),
            horizontalAlignment = Alignment.CenterHorizontally,
            verticalArrangement = Arrangement.Center,
        ) {
            Text(greeting, style = MaterialTheme.typography.labelMedium)

            Spacer(Modifier.height(24.dp))

            OutlinedTextField(
                value = connectionUrl,
                onValueChange = { connectionUrl = it },
                label = { Text("Connection URL") },
                singleLine = true,
                enabled = !running,
                modifier = Modifier.fillMaxWidth(),
            )
            Text(
                "Use udpout://<host-ip>:14540 to reach a SITL on another machine.",
                style = MaterialTheme.typography.labelSmall,
                textAlign = TextAlign.Center,
                modifier = Modifier.padding(top = 4.dp),
            )

            Spacer(Modifier.height(24.dp))

            Button(
                enabled = !running,
                onClick = {
                    log.clear()
                    running = true
                    // Collected on the main dispatcher, so appending to `log` is
                    // safe; flyMission does its own work on Dispatchers.IO.
                    scope.launch {
                        try {
                            flyMission(connectionUrl).collect { log += it }
                        } catch (e: Throwable) {
                            log += "✗ ${e::class.simpleName}: ${e.message}"
                        } finally {
                            running = false
                        }
                    }
                },
            ) {
                Text(if (running) "Flying..." else "Fly mission")
            }

            if (log.isNotEmpty()) {
                Spacer(Modifier.height(24.dp))
                LazyColumn(
                    state = logState,
                    modifier = Modifier
                        .fillMaxWidth()
                        .heightIn(max = 320.dp),
                ) {
                    items(log) { line ->
                        Text(line, style = MaterialTheme.typography.bodySmall)
                    }
                }
            }
        }
    }
}
