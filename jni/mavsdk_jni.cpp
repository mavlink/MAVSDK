#include <jni.h>
#include <vector>
#include "cmavsdk/mavsdk.h"
#include "jni_utils.h"

using namespace mavsdk::jni;

extern "C" {

// Mavsdk.create
JNIEXPORT jlong JNICALL
Java_io_mavsdk_kotlin_Mavsdk_create(
    JNIEnv* env,
    jobject obj,
    jlong configHandle) {

    mavsdk_t handle = mavsdk_create(
        reinterpret_cast<mavsdk_configuration_t>(configHandle));

    if (!handle) {
        throwMavsdkError(env, "OperationError", "Failed to create MAVSDK instance");
        return 0;
    }

    return reinterpret_cast<jlong>(handle);
}

// Mavsdk.version
JNIEXPORT jstring JNICALL
Java_io_mavsdk_kotlin_Mavsdk_version(
    JNIEnv* env,
    jobject obj) {

    jlong handle = getHandle(env, obj, "io/mavsdk/kotlin/Mavsdk");
    if (!handle) return nullptr;

    const char* version = mavsdk_version(reinterpret_cast<mavsdk_t>(handle));
    return toJavaString(env, version);
}

// Mavsdk.addAnyConnectionNative
JNIEXPORT jint JNICALL
Java_io_mavsdk_kotlin_Mavsdk_addAnyConnectionNative(
    JNIEnv* env,
    jobject obj,
    jstring connectionUrl) {

    jlong handle = getHandle(env, obj, "io/mavsdk/kotlin/Mavsdk");
    if (!handle) return -1;

    JStringHolder urlStr(env, connectionUrl);
    
    mavsdk_connection_result_t result = mavsdk_add_any_connection(
        reinterpret_cast<mavsdk_t>(handle),
        urlStr.c_str());

    return static_cast<jint>(result);
}

// Mavsdk.addAnyConnectionWithHandleNative
JNIEXPORT jobject JNICALL
Java_io_mavsdk_kotlin_Mavsdk_addAnyConnectionWithHandleNative(
    JNIEnv* env,
    jobject obj,
    jstring connectionUrl) {

    jlong handle = getHandle(env, obj, "io/mavsdk/kotlin/Mavsdk");
    if (!handle) return nullptr;

    JStringHolder urlStr(env, connectionUrl);
    
    mavsdk_connection_result_with_handle_t result = mavsdk_add_any_connection_with_handle(
        reinterpret_cast<mavsdk_t>(handle),
        urlStr.c_str());

    // Create Pair<Int, Long>
    jclass pairClass = env->FindClass("kotlin/Pair");
    jmethodID pairConstructor = env->GetMethodID(pairClass, "<init>", "(Ljava/lang/Object;Ljava/lang/Object;)V");
    
    jclass integerClass = env->FindClass("java/lang/Integer");
    jmethodID intValueOf = env->GetStaticMethodID(integerClass, "valueOf", "(I)Ljava/lang/Integer;");
    jobject firstObj = env->CallStaticObjectMethod(integerClass, intValueOf, static_cast<jint>(result.result));
    
    jclass longClass = env->FindClass("java/lang/Long");
    jmethodID longValueOf = env->GetStaticMethodID(longClass, "valueOf", "(J)Ljava/lang/Long;");
    jobject secondObj = env->CallStaticObjectMethod(longClass, longValueOf, reinterpret_cast<jlong>(result.handle));
    
    jobject pair = env->NewObject(pairClass, pairConstructor, firstObj, secondObj);
    
    env->DeleteLocalRef(pairClass);
    env->DeleteLocalRef(integerClass);
    env->DeleteLocalRef(longClass);
    env->DeleteLocalRef(firstObj);
    env->DeleteLocalRef(secondObj);
    
    return pair;
}

// Mavsdk.removeConnection
JNIEXPORT void JNICALL
Java_io_mavsdk_kotlin_Mavsdk_removeConnection(
    JNIEnv* env,
    jobject obj,
    jlong connectionHandle) {

    jlong handle = getHandle(env, obj, "io/mavsdk/kotlin/Mavsdk");
    if (!handle) return;

    mavsdk_remove_connection(
        reinterpret_cast<mavsdk_t>(handle),
        reinterpret_cast<mavsdk_connection_handle_t>(connectionHandle));
}

// Mavsdk.systemCount
JNIEXPORT jint JNICALL
Java_io_mavsdk_kotlin_Mavsdk_systemCount(
    JNIEnv* env,
    jobject obj) {

    jlong handle = getHandle(env, obj, "io/mavsdk/kotlin/Mavsdk");
    if (!handle) return 0;

    return static_cast<jint>(mavsdk_system_count(reinterpret_cast<mavsdk_t>(handle)));
}

// Mavsdk.getSystems
JNIEXPORT jobject JNICALL
Java_io_mavsdk_kotlin_Mavsdk_getSystems(
    JNIEnv* env,
    jobject obj) {

    jlong handle = getHandle(env, obj, "io/mavsdk/kotlin/Mavsdk");
    if (!handle) return nullptr;

    size_t count = 0;
    mavsdk_system_t* systems = mavsdk_get_systems(
        reinterpret_cast<mavsdk_t>(handle),
        &count);

    // Create ArrayList
    jclass arrayListClass = env->FindClass("java/util/ArrayList");
    jmethodID arrayListConstructor = env->GetMethodID(arrayListClass, "<init>", "()V");
    jmethodID addMethod = env->GetMethodID(arrayListClass, "add", "(Ljava/lang/Object;)Z");
    jobject list = env->NewObject(arrayListClass, arrayListConstructor);

    // Create System objects
    jclass systemClass = env->FindClass("io/mavsdk/kotlin/System");
    jmethodID systemConstructor = env->GetMethodID(systemClass, "<init>", "(J)V");

    for (size_t i = 0; i < count; i++) {
        jobject systemObj = env->NewObject(systemClass, systemConstructor, 
                                          reinterpret_cast<jlong>(systems[i]));
        env->CallBooleanMethod(list, addMethod, systemObj);
        env->DeleteLocalRef(systemObj);
    }

    mavsdk_free_systems_array(systems);

    env->DeleteLocalRef(arrayListClass);
    env->DeleteLocalRef(systemClass);

    return list;
}

// Mavsdk.firstAutopilot
JNIEXPORT jobject JNICALL
Java_io_mavsdk_kotlin_Mavsdk_firstAutopilot(
    JNIEnv* env,
    jobject obj,
    jdouble timeoutSeconds) {

    jlong handle = getHandle(env, obj, "io/mavsdk/kotlin/Mavsdk");
    if (!handle) return nullptr;

    mavsdk_system_t systemHandle = mavsdk_first_autopilot(
        reinterpret_cast<mavsdk_t>(handle),
        timeoutSeconds);

    if (!systemHandle) {
        return nullptr;
    }

    // Create System object
    jclass systemClass = env->FindClass("io/mavsdk/kotlin/System");
    jmethodID constructor = env->GetMethodID(systemClass, "<init>", "(J)V");
    jobject systemObj = env->NewObject(systemClass, constructor, reinterpret_cast<jlong>(systemHandle));
    
    env->DeleteLocalRef(systemClass);
    return systemObj;
}

// Mavsdk.destroy
JNIEXPORT void JNICALL
Java_io_mavsdk_kotlin_Mavsdk_destroy(
    JNIEnv* env,
    jobject obj) {

    jlong handle = getHandle(env, obj, "io/mavsdk/kotlin/Mavsdk");
    if (!handle) return;

    mavsdk_destroy(reinterpret_cast<mavsdk_t>(handle));
}

} // extern "C"
