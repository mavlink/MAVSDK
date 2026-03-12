#include <jni.h>
#include <vector>
#include "cmavsdk/mavsdk.h"
#include "jni_utils.h"

using namespace mavsdk::jni;

// ===== NewSystem Callback Wrapper =====
struct NewSystemCallbackWrapper {
    GlobalRefHolder callback;
    jmethodID invokeMethod;
    mavsdk_t mavsdkHandle;

    NewSystemCallbackWrapper(JNIEnv* env, jobject callback_obj, mavsdk_t handle)
        : callback(env, callback_obj), invokeMethod(nullptr), mavsdkHandle(handle) {
        if (callback.isValid()) {
            jclass callbackClass = env->GetObjectClass(callback_obj);
            invokeMethod = env->GetMethodID(callbackClass, "invoke", "(Lio/mavsdk/kotlin/System;)V");
            env->DeleteLocalRef(callbackClass);
        }
    }

    void operator()() const {
        if (!callback.isValid() || !invokeMethod || !g_jvm) return;

        JavaVMAttacher attacher(g_jvm);
        JNIEnv* env = attacher.getEnv();
        if (!env) return;

        size_t count = 0;
        mavsdk_system_t* systems = mavsdk_get_systems(mavsdkHandle, &count);
        if (count == 0) {
            if (systems) mavsdk_free_systems_array(systems);
            return;
        }

        jclass systemClass = env->FindClass("io/mavsdk/kotlin/System");
        jmethodID systemCtor = env->GetMethodID(systemClass, "<init>", "(J)V");
        // Emit the most recently discovered system (last in list)
        jobject systemObj = env->NewObject(systemClass, systemCtor,
                                           reinterpret_cast<jlong>(systems[count - 1]));
        env->CallVoidMethod(callback.get(), invokeMethod, systemObj);
        env->DeleteLocalRef(systemObj);
        env->DeleteLocalRef(systemClass);
        mavsdk_free_systems_array(systems);

        if (env->ExceptionCheck()) {
            env->ExceptionDescribe();
            env->ExceptionClear();
        }
    }
};

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

// Mavsdk.serverComponentHandleNative
JNIEXPORT jlong JNICALL
Java_io_mavsdk_kotlin_Mavsdk_serverComponentHandleNative(
    JNIEnv* env,
    jobject obj,
    jint instance) {

    jlong handle = getHandle(env, obj, "io/mavsdk/kotlin/Mavsdk");
    if (!handle) return 0;

    mavsdk_server_component_t sc = mavsdk_server_component(
        reinterpret_cast<mavsdk_t>(handle),
        static_cast<unsigned int>(instance));

    return reinterpret_cast<jlong>(sc);
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

// Mavsdk.subscribeOnNewSystemNative
JNIEXPORT jlong JNICALL
Java_io_mavsdk_kotlin_Mavsdk_subscribeOnNewSystemNative(
    JNIEnv* env,
    jobject obj,
    jobject callback) {

    jlong handle = getHandle(env, obj, "io/mavsdk/kotlin/Mavsdk");
    if (!handle || !callback) return 0;

    auto* wrapper = new NewSystemCallbackWrapper(
        env, callback, reinterpret_cast<mavsdk_t>(handle));

    mavsdk_new_system_handle_t subscription_handle =
        mavsdk_subscribe_on_new_system(
            reinterpret_cast<mavsdk_t>(handle),
            [](void* user_data) {
                auto* w = static_cast<NewSystemCallbackWrapper*>(user_data);
                (*w)();
            },
            wrapper
        );

    auto* handle_pair = new std::pair<mavsdk_new_system_handle_t, NewSystemCallbackWrapper*>(
        subscription_handle, wrapper);

    return reinterpret_cast<jlong>(handle_pair);
}

// Mavsdk.unsubscribeOnNewSystem
JNIEXPORT void JNICALL
Java_io_mavsdk_kotlin_Mavsdk_unsubscribeOnNewSystem(
    JNIEnv* env,
    jobject obj,
    jlong subscriptionHandle) {

    jlong handle = getHandle(env, obj, "io/mavsdk/kotlin/Mavsdk");
    if (!handle || !subscriptionHandle) return;

    auto* handle_pair = reinterpret_cast<
        std::pair<mavsdk_new_system_handle_t, NewSystemCallbackWrapper*>*>(subscriptionHandle);

    if (handle_pair) {
        mavsdk_unsubscribe_on_new_system(
            reinterpret_cast<mavsdk_t>(handle),
            handle_pair->first
        );
        delete handle_pair->second;
        delete handle_pair;
    }
}

} // extern "C"
