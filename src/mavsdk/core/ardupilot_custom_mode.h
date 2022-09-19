#pragma once

namespace ardupilot {

// Enumeration representing the available modes for the Arudpilot rover autopilot.
enum class RoverMode {
    Manual = 0,
    Acro = 1,
    Steering = 3,
    Hold = 4,
    Loiter = 5,
    Follow = 6,
    Simple = 7,
    Auto = 10,
    RTL = 11,
    Smart_RTL = 12,
    Guided = 15,
    Initializing = 16,
    Unknown = 100
};

// Enumeration representing the available modes for the Arudpilot copter autopilot.
enum class CopterMode {
    Stabilize = 0,
    Acro = 1,
    AltHold = 2,
    Auto = 3,
    Guided = 4,
    Loiter = 5,
    Rtl = 6,
    Circle = 7,
    Land = 9,
    Drift = 11,
    Sport = 13,
    Flip = 14,
    AutoTune = 15,
    PosHold = 16,
    Break = 17,
    Throw = 18,
    AvoidAdbs = 19,
    GuidedNoGps = 20,
    SmartRtl = 21,
    FlowHold = 22,
    Follow = 23,
    Zigzag = 24,
    SystemId = 25,
    AutoRotate = 26,
    AutoRtl = 27,
    Turtle = 28,
    Unknown = 100
};

enum class PlaneMode {
    Manual = 0,
    Circle = 1,
    Stabilize = 2,
    Training = 3,
    Acro = 4,
    Fbwa = 5,
    Fbwb = 6,
    Cruise = 7,
    Autotune = 8,
    Auto = 10,
    Rtl = 11,
    Loiter = 12,
    Takeoff = 13,
    AvoidAdsb = 14,
    Guided = 15,
    Initializing = 16,
    QStabilize = 17,
    QHover = 18,
    QLoiter = 19,
    QLand = 20,
    QRtl = 21,
    QAutotune = 22,
    QAcro = 23,
    Thermal = 24,
    Unknown = 100
};

} // namespace ardupilot