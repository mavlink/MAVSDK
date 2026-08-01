package io.mavsdk.jni;

public final class NativeLog {
    private NativeLog() {}

    @FunctionalInterface
    public interface Callback {
        boolean invoke(int level, String message, String file, int line);
    }

    public static native void subscribe(Callback callback);
    public static native void unsubscribe();
}
