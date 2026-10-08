package io.mavsdk.jni;

import io.mavsdk.jni.plugins.camera_server.NativeCameraServer;

/**
 * Calls into the library with the JVM checking every JNI call the library
 * makes (-Xcheck:jni, see test.sh). A misuse of JNI that a JVM otherwise
 * lets through, such as using a reference after deleting it, ends the JVM
 * with "FATAL ERROR in native method".
 *
 * Needs no connection and no other MAVLink system.
 */
public final class CheckedJniTest {
    private static final int COMPONENT_TYPE_CAMERA = 3;

    public static void main(String[] args) {
        System.load(args[0]);

        long configuration = NativeConfiguration.createWithComponentType(COMPONENT_TYPE_CAMERA);
        long mavsdk = NativeMavsdk.create(configuration);
        NativeConfiguration.destroy(configuration);
        long serverComponent = NativeMavsdk.serverComponentHandle(mavsdk, 0);

        structWithStrings(serverComponent);

        NativeMavsdk.destroyServerComponent(serverComponent);
        NativeMavsdk.destroy(mavsdk);
        System.out.println("CheckedJniTest passed.");
    }

    /** A struct argument whose fields include strings. */
    private static void structWithStrings(long serverComponent) {
        long cameraServer = NativeCameraServer.create(serverComponent);
        int result = NativeCameraServer.setInformation(cameraServer, new NativeCameraServer.Information(
            "MAVSDK", "Test camera", "1.0.0", 3.0f, 3.68f, 2.76f, 3280, 2464, 0, 0, "", false, false));
        NativeCameraServer.destroy(cameraServer);
        check(result == 0, "setInformation returned " + result);
    }

    private static void check(boolean condition, String message) {
        if (!condition) {
            throw new AssertionError(message);
        }
    }
}
