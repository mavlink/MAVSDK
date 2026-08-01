package io.mavsdk.jni;

public final class NativeConfiguration {
    private NativeConfiguration() {}

    public static native long createWithComponentType(int componentType);
    public static native long createManual(
        int systemId, int componentId, boolean alwaysSendHeartbeats);
    public static native int getSystemId(long handle);
    public static native void setSystemId(long handle, int value);
    public static native int getComponentId(long handle);
    public static native void setComponentId(long handle, int value);
    public static native void destroy(long handle);
}
