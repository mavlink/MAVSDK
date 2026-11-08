package io.mavsdk.kotlin

/**
 * Time synchronization state, see [System.enableTimesync].
 *
 * Autopilot time = local time (system clock) + [offsetNs]. Incoming message timestamps are not
 * changed.
 *
 * @property synced Whether the offset is valid.
 * @property offsetNs Autopilot time minus local time, in nanoseconds.
 * @property rttNs Round trip time of the last TIMESYNC, in nanoseconds.
 */
data class TimesyncStatus(val synced: Boolean, val offsetNs: Long, val rttNs: Long)
