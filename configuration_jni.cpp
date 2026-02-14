#include <jni.h>
#include "cmavsdk/mavsdk.h"
#include "jni_utils.h"

using namespace mavsdk::jni;

extern "C" {

// Configuration.createWithComponentType
JNIEXPORT jobject JNICALL
Java_io_mavsdk_kotlin_Configuration_createWithComponentType(
    JNIEnv* env,
    jclass clazz,
    jobject componentType) {

    // Get the component type value
    jclass enumClass = env->GetObjectClass(componentType);
    jmethodID getValueMethod = env->GetMethodID(enumClass, "getValue", "()I");
    jint componentTypeValue = env->CallIntMethod(componentType, getValueMethod);
    env->DeleteLocalRef(enumClass);

    // Create configuration
    mavsdk_configuration_t handle = mavsdk_configuration_create_with_component_type(
        static_cast<mavsdk_component_type_t>(componentTypeValue));

    if (!handle) {
        throwMavsdkError(env, "OperationError", "Failed to create configuration");
        return nullptr;
    }

    // Create Configuration object
    jclass configClass = env->FindClass("io/mavsdk/kotlin/Configuration");
    jmethodID constructor = env->GetMethodID(configClass, "<init>", "(J)V");
    jobject configObj = env->NewObject(configClass, constructor, reinterpret_cast<jlong>(handle));
    
    env->DeleteLocalRef(configClass);
    return configObj;
}

// Configuration.createManual
JNIEXPORT jobject JNICALL
Java_io_mavsdk_kotlin_Configuration_createManual(
    JNIEnv* env,
    jclass clazz,
    jint systemId,
    jint componentId,
    jboolean alwaysSendHeartbeats) {

    mavsdk_configuration_t handle = mavsdk_configuration_create_manual(
        static_cast<uint8_t>(systemId),
        static_cast<uint8_t>(componentId),
        alwaysSendHeartbeats);

    if (!handle) {
        throwMavsdkError(env, "OperationError", "Failed to create configuration");
        return nullptr;
    }

    // Create Configuration object
    jclass configClass = env->FindClass("io/mavsdk/kotlin/Configuration");
    jmethodID constructor = env->GetMethodID(configClass, "<init>", "(J)V");
    jobject configObj = env->NewObject(configClass, constructor, reinterpret_cast<jlong>(handle));
    
    env->DeleteLocalRef(configClass);
    return configObj;
}

// Configuration.getSystemIdNative
JNIEXPORT jint JNICALL
Java_io_mavsdk_kotlin_Configuration_getSystemIdNative(
    JNIEnv* env,
    jobject obj) {

    jlong handle = getHandle(env, obj, "io/mavsdk/kotlin/Configuration");
    if (!handle) return 0;

    return mavsdk_configuration_get_system_id(reinterpret_cast<mavsdk_configuration_t>(handle));
}

// Configuration.setSystemIdNative
JNIEXPORT void JNICALL
Java_io_mavsdk_kotlin_Configuration_setSystemIdNative(
    JNIEnv* env,
    jobject obj,
    jint value) {

    jlong handle = getHandle(env, obj, "io/mavsdk/kotlin/Configuration");
    if (!handle) return;

    mavsdk_configuration_set_system_id(
        reinterpret_cast<mavsdk_configuration_t>(handle),
        static_cast<uint8_t>(value));
}

// Configuration.getComponentIdNative
JNIEXPORT jint JNICALL
Java_io_mavsdk_kotlin_Configuration_getComponentIdNative(
    JNIEnv* env,
    jobject obj) {

    jlong handle = getHandle(env, obj, "io/mavsdk/kotlin/Configuration");
    if (!handle) return 0;

    return mavsdk_configuration_get_component_id(reinterpret_cast<mavsdk_configuration_t>(handle));
}

// Configuration.setComponentIdNative
JNIEXPORT void JNICALL
Java_io_mavsdk_kotlin_Configuration_setComponentIdNative(
    JNIEnv* env,
    jobject obj,
    jint value) {

    jlong handle = getHandle(env, obj, "io/mavsdk/kotlin/Configuration");
    if (!handle) return;

    mavsdk_configuration_set_component_id(
        reinterpret_cast<mavsdk_configuration_t>(handle),
        static_cast<uint8_t>(value));
}

// Configuration.destroy
JNIEXPORT void JNICALL
Java_io_mavsdk_kotlin_Configuration_destroy(
    JNIEnv* env,
    jobject obj) {

    jlong handle = getHandle(env, obj, "io/mavsdk/kotlin/Configuration");
    if (!handle) return;

    mavsdk_configuration_destroy(reinterpret_cast<mavsdk_configuration_t>(handle));
}

} // extern "C"
