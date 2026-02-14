#include <jni.h>
#include "cmavsdk/system.h"
#include "jni_utils.h"

using namespace mavsdk::jni;

// Global JavaVM pointer - needed for attaching threads in callbacks
static JavaVM* g_jvm = nullptr;

// Initialize JVM pointer (call this from JNI_OnLoad)
extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
    g_jvm = vm;
    return JNI_VERSION_1_6;
}

// Callback wrapper that stores Java callback as GlobalRef
struct IsConnectedCallbackWrapper {
    GlobalRefHolder callback;
    jmethodID invokeMethod;

    IsConnectedCallbackWrapper(JNIEnv* env, jobject callback_obj)
        : callback(env, callback_obj), invokeMethod(nullptr) {

        if (callback.isValid()) {
            // Cache the method ID for the callback interface
            jclass callbackClass = env->GetObjectClass(callback_obj);
            invokeMethod = env->GetMethodID(callbackClass, "invoke", "(Z)V");
            env->DeleteLocalRef(callbackClass);
        }
    }

    void operator()(bool is_connected) const {
        if (!callback.isValid() || !invokeMethod || !g_jvm) {
            return;
        }

        JavaVMAttacher attacher(g_jvm);
        JNIEnv* env = attacher.getEnv();

        if (!env) {
            return;
        }

        env->CallVoidMethod(callback.get(), invokeMethod, static_cast<jboolean>(is_connected));

        // Check for exceptions
        if (env->ExceptionCheck()) {
            env->ExceptionDescribe();
            env->ExceptionClear();
        }
    }
};

struct ComponentDiscoveredCallbackWrapper {
    GlobalRefHolder callback;
    jmethodID invokeMethod;

    ComponentDiscoveredCallbackWrapper(JNIEnv* env, jobject callback_obj)
        : callback(env, callback_obj), invokeMethod(nullptr) {

        if (callback.isValid()) {
            jclass callbackClass = env->GetObjectClass(callback_obj);
            invokeMethod = env->GetMethodID(callbackClass, "invoke", "(Lio/mavsdk/kotlin/ComponentType;)V");
            env->DeleteLocalRef(callbackClass);
        }
    }

    void operator()(mavsdk_component_type_t component_type) const {
        if (!callback.isValid() || !invokeMethod || !g_jvm) {
            return;
        }

        JavaVMAttacher attacher(g_jvm);
        JNIEnv* env = attacher.getEnv();

        if (!env) {
            return;
        }

        // Convert C enum to Java enum
        jclass enumClass = env->FindClass("io/mavsdk/kotlin/ComponentType");
        jmethodID valueOfMethod = env->GetStaticMethodID(enumClass, "fromValue", "(I)Lio/mavsdk/kotlin/ComponentType;");
        jobject enumValue = env->CallStaticObjectMethod(enumClass, valueOfMethod, static_cast<jint>(component_type));

        env->CallVoidMethod(callback.get(), invokeMethod, enumValue);

        env->DeleteLocalRef(enumClass);
        env->DeleteLocalRef(enumValue);

        if (env->ExceptionCheck()) {
            env->ExceptionDescribe();
            env->ExceptionClear();
        }
    }
};

struct ComponentDiscoveredIdCallbackWrapper {
    GlobalRefHolder callback;
    jmethodID invokeMethod;

    ComponentDiscoveredIdCallbackWrapper(JNIEnv* env, jobject callback_obj)
        : callback(env, callback_obj), invokeMethod(nullptr) {

        if (callback.isValid()) {
            jclass callbackClass = env->GetObjectClass(callback_obj);
            invokeMethod = env->GetMethodID(callbackClass, "invoke", "(Lio/mavsdk/kotlin/ComponentType;I)V");
            env->DeleteLocalRef(callbackClass);
        }
    }

    void operator()(mavsdk_component_type_t component_type, uint8_t component_id) const {
        if (!callback.isValid() || !invokeMethod || !g_jvm) {
            return;
        }

        JavaVMAttacher attacher(g_jvm);
        JNIEnv* env = attacher.getEnv();

        if (!env) {
            return;
        }

        // Convert C enum to Java enum
        jclass enumClass = env->FindClass("io/mavsdk/kotlin/ComponentType");
        jmethodID valueOfMethod = env->GetStaticMethodID(enumClass, "fromValue", "(I)Lio/mavsdk/kotlin/ComponentType;");
        jobject enumValue = env->CallStaticObjectMethod(enumClass, valueOfMethod, static_cast<jint>(component_type));

        env->CallVoidMethod(callback.get(), invokeMethod, enumValue, static_cast<jint>(component_id));

        env->DeleteLocalRef(enumClass);
        env->DeleteLocalRef(enumValue);

        if (env->ExceptionCheck()) {
            env->ExceptionDescribe();
            env->ExceptionClear();
        }
    }
};

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

// System.subscribeIsConnectedNative
JNIEXPORT jlong JNICALL
Java_io_mavsdk_kotlin_System_subscribeIsConnectedNative(
    JNIEnv* env,
    jobject obj,
    jobject callback) {

    jlong handle = getHandle(env, obj, "io/mavsdk/kotlin/System");
    if (!handle || !callback) return 0;

    // Create wrapper with GlobalRef to callback
    auto* wrapper = new IsConnectedCallbackWrapper(env, callback);

    // Subscribe using lambda that captures the wrapper
    mavsdk_is_connected_handle_t subscription_handle = mavsdk_system_subscribe_is_connected(
        reinterpret_cast<mavsdk_system_t>(handle),
        [](bool is_connected, void* user_data) {
            auto* w = static_cast<IsConnectedCallbackWrapper*>(user_data);
            (*w)(is_connected);
        },
        wrapper
    );

    // Store both the subscription handle and wrapper pointer
    // We'll package them into a single pointer for Java
    auto* handle_pair = new std::pair<mavsdk_is_connected_handle_t, IsConnectedCallbackWrapper*>(
        subscription_handle, wrapper
    );

    return reinterpret_cast<jlong>(handle_pair);
}

// System.unsubscribeIsConnected
JNIEXPORT void JNICALL
Java_io_mavsdk_kotlin_System_unsubscribeIsConnected(
    JNIEnv* env,
    jobject obj,
    jlong subscriptionHandle) {

    jlong handle = getHandle(env, obj, "io/mavsdk/kotlin/System");
    if (!handle || !subscriptionHandle) return;

    auto* handle_pair = reinterpret_cast<std::pair<mavsdk_is_connected_handle_t, IsConnectedCallbackWrapper*>*>(subscriptionHandle);

    if (handle_pair) {
        mavsdk_system_unsubscribe_is_connected(
            reinterpret_cast<mavsdk_system_t>(handle),
            handle_pair->first
        );
        delete handle_pair->second; // Delete wrapper (GlobalRef released in destructor)
        delete handle_pair;
    }
}

// System.subscribeComponentDiscoveredNative
JNIEXPORT jlong JNICALL
Java_io_mavsdk_kotlin_System_subscribeComponentDiscoveredNative(
    JNIEnv* env,
    jobject obj,
    jobject callback) {

    jlong handle = getHandle(env, obj, "io/mavsdk/kotlin/System");
    if (!handle || !callback) return 0;

    auto* wrapper = new ComponentDiscoveredCallbackWrapper(env, callback);

    mavsdk_component_discovered_handle_t subscription_handle = mavsdk_system_subscribe_component_discovered(
        reinterpret_cast<mavsdk_system_t>(handle),
        [](mavsdk_component_type_t component_type, void* user_data) {
            auto* w = static_cast<ComponentDiscoveredCallbackWrapper*>(user_data);
            (*w)(component_type);
        },
        wrapper
    );

    auto* handle_pair = new std::pair<mavsdk_component_discovered_handle_t, ComponentDiscoveredCallbackWrapper*>(
        subscription_handle, wrapper
    );

    return reinterpret_cast<jlong>(handle_pair);
}

// System.unsubscribeComponentDiscovered
JNIEXPORT void JNICALL
Java_io_mavsdk_kotlin_System_unsubscribeComponentDiscovered(
    JNIEnv* env,
    jobject obj,
    jlong subscriptionHandle) {

    jlong handle = getHandle(env, obj, "io/mavsdk/kotlin/System");
    if (!handle || !subscriptionHandle) return;

    auto* handle_pair = reinterpret_cast<std::pair<mavsdk_component_discovered_handle_t, ComponentDiscoveredCallbackWrapper*>*>(subscriptionHandle);

    if (handle_pair) {
        mavsdk_system_unsubscribe_component_discovered(
            reinterpret_cast<mavsdk_system_t>(handle),
            handle_pair->first
        );
        delete handle_pair->second;
        delete handle_pair;
    }
}

// System.subscribeComponentDiscoveredIdNative
JNIEXPORT jlong JNICALL
Java_io_mavsdk_kotlin_System_subscribeComponentDiscoveredIdNative(
    JNIEnv* env,
    jobject obj,
    jobject callback) {

    jlong handle = getHandle(env, obj, "io/mavsdk/kotlin/System");
    if (!handle || !callback) return 0;

    auto* wrapper = new ComponentDiscoveredIdCallbackWrapper(env, callback);

    mavsdk_component_discovered_id_handle_t subscription_handle = mavsdk_system_subscribe_component_discovered_id(
        reinterpret_cast<mavsdk_system_t>(handle),
        [](mavsdk_component_type_t component_type, uint8_t component_id, void* user_data) {
            auto* w = static_cast<ComponentDiscoveredIdCallbackWrapper*>(user_data);
            (*w)(component_type, component_id);
        },
        wrapper
    );

    auto* handle_pair = new std::pair<mavsdk_component_discovered_id_handle_t, ComponentDiscoveredIdCallbackWrapper*>(
        subscription_handle, wrapper
    );

    return reinterpret_cast<jlong>(handle_pair);
}

// System.unsubscribeComponentDiscoveredId
JNIEXPORT void JNICALL
Java_io_mavsdk_kotlin_System_unsubscribeComponentDiscoveredId(
    JNIEnv* env,
    jobject obj,
    jlong subscriptionHandle) {

    jlong handle = getHandle(env, obj, "io/mavsdk/kotlin/System");
    if (!handle || !subscriptionHandle) return;

    auto* handle_pair = reinterpret_cast<std::pair<mavsdk_component_discovered_id_handle_t, ComponentDiscoveredIdCallbackWrapper*>*>(subscriptionHandle);

    if (handle_pair) {
        mavsdk_system_unsubscribe_component_discovered_id(
            reinterpret_cast<mavsdk_system_t>(handle),
            handle_pair->first
        );
        delete handle_pair->second;
        delete handle_pair;
    }
}

} // extern "C"
