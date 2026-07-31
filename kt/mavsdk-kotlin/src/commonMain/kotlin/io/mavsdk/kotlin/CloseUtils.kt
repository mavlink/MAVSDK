package io.mavsdk.kotlin

internal fun closeAll(actions: List<() -> Unit>) {
    var failure: Throwable? = null
    actions.forEach { action ->
        try {
            action()
        } catch (error: Throwable) {
            if (failure == null) {
                failure = error
            } else {
                failure.addSuppressed(error)
            }
        }
    }
    failure?.let { throw it }
}
