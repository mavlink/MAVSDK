package io.mavsdk.kotlin

import kotlin.test.Test
import kotlin.test.assertTrue

/**
 * Tests for the orders in which Kotlin can let go of the handles the bindings hold.
 *
 * `use` closes an object at the end of a block, a flow unsubscribes from its `awaitClose` when its
 * collector is cancelled, and a caller may close things itself on the way out -- all of which can
 * happen in either order, or twice. A handle released twice is a native double free, which aborts
 * the JVM rather than failing an assertion, so a regression here shows up as a crashed test worker.
 *
 * UDP ports are hard-coded, as in cpp/src/system_tests and py/test. Pick one that no other test
 * uses.
 */
class TeardownTest {

    @Test
    fun `closing a configuration twice releases it once`() {
        val configuration = Configuration.createWithComponentType(ComponentType.GROUND_STATION)
        Mavsdk(configuration).use { mavsdk -> assertTrue(mavsdk.version().isNotEmpty()) }

        // Reachable twice over in real code: from `use` and from a caller tidying up.
        configuration.close()
        configuration.close()
    }
}
