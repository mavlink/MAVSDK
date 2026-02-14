#include <jni.h>
#include "cmavsdk/system.h"
#include "jni_utils.h"

using namespace mavsdk::jni;

extern "C" {

// System.hasAutopilot
JNIEXPORT jboolean JNICALL
Java_io_mavsdk_kotlin_System_hasAutopilot(
    JNIEnv* env,
    jobject obj) {

    jlong handle = getHandle(env, obj, "io/mavsdk/kotlin/System");
    if (!handle) return false;

    return mavsdk_system_has_autopilot(reinterpret_cast<mavsdk_system_t>(handle));
}

// System.isStandalone
JNIEXPORT jboolean JNICALL
Java_io_mavsdk_kotlin_System_isStandalone(
    JNIEnv* env,
    jobject obj) {

    jlong handle = getHandle(env, obj, "io/mavsdk/kotlin/System");
    if (!handle) return false;

    return mavsdk_system_is_standalone(reinterpret_cast<mavsdk_system_t>(handle));
}

// System.hasCamera
JNIEXPORT jboolean JNICALL
Java_io_mavsdk_kotlin_System_hasCamera(
    JNIEnv* env,
    jobject obj,
    jint cameraId) {

    jlong handle = getHandle(env, obj, "io/mavsdk/kotlin/System");
    if (!handle) return false;

    return mavsdk_system_has_camera(
        reinterpret_cast<mavsdk_system_t>(handle),
        cameraId);
}

// System.hasGimbal
JNIEXPORT jboolean JNICALL
Java_io_mavsdk_kotlin_System_hasGimbal(
    JNIEnv* env,
    jobject obj) {

    jlong handle = getHandle(env, obj, "io/mavsdk/kotlin/System");
    if (!handle) return false;

    return mavsdk_system_has_gimbal(reinterpret_cast<mavsdk_system_t>(handle));
}

// System.isConnected
JNIEXPORT jboolean JNICALL
Java_io_mavsdk_kotlin_System_isConnected(
    JNIEnv* env,
    jobject obj) {

    jlong handle = getHandle(env, obj, "io/mavsdk/kotlin/System");
    if (!handle) return false;

    return mavsdk_system_is_connected(reinterpret_cast<mavsdk_system_t>(handle));
}

// System.getSystemId
JNIEXPORT jint JNICALL
Java_io_mavsdk_kotlin_System_getSystemId(
    JNIEnv* env,
    jobject obj) {

    jlong handle = getHandle(env, obj, "io/mavsdk/kotlin/System");
    if (!handle) return 0;

    return mavsdk_system_get_system_id(reinterpret_cast<mavsdk_system_t>(handle));
}

// System.getComponentIds
JNIEXPORT jintArray JNICALL
Java_io_mavsdk_kotlin_System_getComponentIds(
    JNIEnv* env,
    jobject obj) {

    jlong handle = getHandle(env, obj, "io/mavsdk/kotlin/System");
    if (!handle) return nullptr;

    size_t count = 0;
    uint8_t* ids = mavsdk_system_component_ids(
        reinterpret_cast<mavsdk_system_t>(handle),
        &count);

    if (!ids || count == 0) {
        mavsdk_system_free_component_ids(ids);
        return env->NewIntArray(0);
    }

    jintArray result = env->NewIntArray(count);
    jint* elements = new jint[count];
    
    for (size_t i = 0; i < count; i++) {
        elements[i] = ids[i];
    }
    
    env->SetIntArrayRegion(result, 0, count, elements);
    
    delete[] elements;
    mavsdk_system_free_component_ids(ids);

    return result;
}

// System.getAutopilotTypeNative
JNIEXPORT jint JNICALL
Java_io_mavsdk_kotlin_System_getAutopilotTypeNative(
    JNIEnv* env,
    jobject obj) {

    jlong handle = getHandle(env, obj, "io/mavsdk/kotlin/System");
    if (!handle) return 0;

    return static_cast<jint>(mavsdk_system_autopilot_type(
        reinterpret_cast<mavsdk_system_t>(handle)));
}

// System.getVehicleTypeNative
JNIEXPORT jint JNICALL
Java_io_mavsdk_kotlin_System_getVehicleTypeNative(
    JNIEnv* env,
    jobject obj) {

    jlong handle = getHandle(env, obj, "io/mavsdk/kotlin/System");
    if (!handle) return 0;

    return static_cast<jint>(mavsdk_system_vehicle_type(
        reinterpret_cast<mavsdk_system_t>(handle)));
}

// System.enableTimesync
JNIEXPORT void JNICALL
Java_io_mavsdk_kotlin_System_enableTimesync(
    JNIEnv* env,
    jobject obj) {

    jlong handle = getHandle(env, obj, "io/mavsdk/kotlin/System");
    if (!handle) return;

    mavsdk_system_enable_timesync(reinterpret_cast<mavsdk_system_t>(handle));
}

} // extern "C"
