package io.mavsdk.kotlin

import kotlin.test.Test
import kotlin.test.assertTrue
import kotlinx.coroutines.cancelAndJoin
import kotlinx.coroutines.delay
import kotlinx.coroutines.launch
import kotlinx.coroutines.runBlocking

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

    @Test
    fun `a connection handle is released once, however often it is removed`() {
        Mavsdk(ComponentType.GROUND_STATION).use { mavsdk ->
            val handle = mavsdk.addAnyConnectionWithHandle("udpin://0.0.0.0:17051").getOrThrow()
            mavsdk.removeConnection(handle)
            mavsdk.removeConnection(handle)

            // A connection that fails to come up is allocated a handle too, and there is
            // none to hand back for it, so it has to go back on the way out.
            assertTrue(mavsdk.addAnyConnectionWithHandle("nonsense://").isFailure)

            // Left in place on purpose: close() releases what the caller kept.
            mavsdk.addAnyConnectionWithHandle("udpin://0.0.0.0:17052").getOrThrow()
        }
    }

    @Test
    fun `a subscription cancelled after close is released once`() = runBlocking {
        Mavsdk(ComponentType.GROUND_STATION).use { mavsdk ->
            mavsdk.addAnyConnectionWithHandle("udpin://0.0.0.0:17053").getOrThrow()

            val collector = launch { mavsdk.subscribeOnNewSystem().collect {} }
            delay(SUBSCRIPTION_SETTLE_MS) // let the subscription get established

            // close() unsubscribes whatever is still subscribed, and the collector's
            // awaitClose then runs against an instance that is already gone.
            mavsdk.close()
            collector.cancelAndJoin()
        }
    }

    @Test
    fun `a subscription cancelled before close is released once`() = runBlocking {
        Mavsdk(ComponentType.GROUND_STATION).use { mavsdk ->
            mavsdk.addAnyConnectionWithHandle("udpin://0.0.0.0:17054").getOrThrow()

            val collector = launch { mavsdk.subscribeOnNewSystem().collect {} }
            delay(SUBSCRIPTION_SETTLE_MS)

            // The other way round: awaitClose unsubscribes, and close() must not then
            // find the same subscription again.
            collector.cancelAndJoin()
            mavsdk.close()
        }
    }

    private companion object {
        const val SUBSCRIPTION_SETTLE_MS = 200L
    }
}
