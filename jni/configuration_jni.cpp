#include <jni.h>
#include "cmavsdk/mavsdk.h"
#include "jni_utils.h"

using namespace mavsdk::jni;

extern "C" {

JNIEXPORT jlong JNICALL
Java_io_mavsdk_jni_NativeConfiguration_createWithComponentType(
    JNIEnv* env,
    jobject,
    jint component_type) {

    const auto handle = mavsdk_configuration_create_with_component_type(
        static_cast<mavsdk_component_type_t>(component_type));
    if (!handle) {
        throwMavsdkError(env, "Failed to create configuration");
        return 0;
    }
    return reinterpret_cast<jlong>(handle);
}

JNIEXPORT jlong JNICALL
Java_io_mavsdk_jni_NativeConfiguration_createManual(
    JNIEnv* env,
    jobject,
    jint system_id,
    jint component_id,
    jboolean always_send_heartbeats) {

    const auto handle = mavsdk_configuration_create_manual(
        static_cast<uint32_t>(system_id),
        static_cast<uint8_t>(component_id),
        always_send_heartbeats);
    if (!handle) {
        throwMavsdkError(env, "Failed to create configuration");
        return 0;
    }
    return reinterpret_cast<jlong>(handle);
}

JNIEXPORT jint JNICALL
Java_io_mavsdk_jni_NativeConfiguration_getSystemId(
    JNIEnv* env,
    jobject,
    jlong handle) {

    if (!requireHandle(env, handle, "configuration")) return 0;
    // jint is signed, so a system ID above 2^31-1 shows up negative on the
    // Kotlin side. All 32 bits survive; read it back with toUInt() if needed.
    return static_cast<jint>(mavsdk_configuration_get_system_id(
        reinterpret_cast<mavsdk_configuration_t>(handle)));
}

JNIEXPORT void JNICALL
Java_io_mavsdk_jni_NativeConfiguration_setSystemId(
    JNIEnv* env,
    jobject,
    jlong handle,
    jint value) {

    if (!requireHandle(env, handle, "configuration")) return;
    mavsdk_configuration_set_system_id(
        reinterpret_cast<mavsdk_configuration_t>(handle),
        static_cast<uint32_t>(value));
}

JNIEXPORT jint JNICALL
Java_io_mavsdk_jni_NativeConfiguration_getComponentId(
    JNIEnv* env,
    jobject,
    jlong handle) {

    if (!requireHandle(env, handle, "configuration")) return 0;
    return mavsdk_configuration_get_component_id(
        reinterpret_cast<mavsdk_configuration_t>(handle));
}

JNIEXPORT void JNICALL
Java_io_mavsdk_jni_NativeConfiguration_setComponentId(
    JNIEnv* env,
    jobject,
    jlong handle,
    jint value) {

    if (!requireHandle(env, handle, "configuration")) return;
    mavsdk_configuration_set_component_id(
        reinterpret_cast<mavsdk_configuration_t>(handle),
        static_cast<uint8_t>(value));
}

JNIEXPORT void JNICALL
Java_io_mavsdk_jni_NativeConfiguration_destroy(
    JNIEnv*,
    jobject,
    jlong handle) {

    if (handle) {
        mavsdk_configuration_destroy(reinterpret_cast<mavsdk_configuration_t>(handle));
    }
}

} // extern "C"
