package io.mavsdk.jni;

public final class NativeMavsdk {
    private NativeMavsdk() {}

    @FunctionalInterface
    public interface NewSystemCallback {
        void invoke();
    }

    public static native long create(long configurationHandle);
    public static native String version(long handle);
    public static native long serverComponentHandle(long handle, int instance);
    public static native void destroyServerComponent(long handle);
    public static native int addAnyConnection(long handle, String connectionUrl);
    public static native long[] addAnyConnectionWithHandle(long handle, String connectionUrl);
    public static native void removeConnection(long handle, long connectionHandle);
    public static native int systemCount(long handle);
    public static native long[] getSystems(long handle);
    public static native long firstAutopilot(long handle, double timeoutSeconds);
    public static native void destroy(long handle);
    public static native long subscribeOnNewSystem(
        long handle, NewSystemCallback callback);
    public static native void unsubscribeOnNewSystem(
        long handle, long subscriptionHandle);
}
