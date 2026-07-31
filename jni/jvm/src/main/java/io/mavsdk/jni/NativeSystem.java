package io.mavsdk.jni;

public final class NativeSystem {
    private NativeSystem() {}

    @FunctionalInterface
    public interface BooleanCallback {
        void invoke(boolean value);
    }

    @FunctionalInterface
    public interface ComponentCallback {
        void invoke(int componentType);
    }

    @FunctionalInterface
    public interface ComponentIdCallback {
        void invoke(int componentType, int componentId);
    }

    public static native boolean hasAutopilot(long handle);
    public static native boolean isStandalone(long handle);
    public static native boolean hasCamera(long handle, int cameraId);
    public static native boolean hasGimbal(long handle);
    public static native boolean isConnected(long handle);
    public static native int getSystemId(long handle);
    public static native int[] getComponentIds(long handle);
    public static native int getAutopilotType(long handle);
    public static native int getVehicleType(long handle);
    public static native void enableTimesync(long handle);
    public static native void destroy(long handle);
    public static native long subscribeIsConnected(
        long handle, BooleanCallback callback);
    public static native void unsubscribeIsConnected(
        long handle, long subscriptionHandle);
    public static native long subscribeComponentDiscovered(
        long handle, ComponentCallback callback);
    public static native void unsubscribeComponentDiscovered(
        long handle, long subscriptionHandle);
    public static native long subscribeComponentDiscoveredId(
        long handle, ComponentIdCallback callback);
    public static native void unsubscribeComponentDiscoveredId(
        long handle, long subscriptionHandle);
}
