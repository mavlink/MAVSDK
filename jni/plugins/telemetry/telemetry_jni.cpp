#include <jni.h>
#include "cmavsdk/plugins/telemetry/telemetry.h"
#include "../../jni_utils.h"

using namespace mavsdk::jni;

// Callback wrapper that stores Java callback as GlobalRef
struct PositionCallbackWrapper {
    GlobalRefHolder callback;
    jmethodID invokeMethod;

    PositionCallbackWrapper(JNIEnv* env, jobject callback_obj)
        : callback(env, callback_obj), invokeMethod(nullptr) {

        if (callback.isValid()) {
            // Cache the method ID for the callback interface
            // Signature: (Lio/mavsdk/kotlin/plugins/telemetry/Telemetry$Position;)V
            jclass callbackClass = env->GetObjectClass(callback_obj);
            invokeMethod = env->GetMethodID(callbackClass, "invoke",
                "(Lio/mavsdk/kotlin/plugins/telemetry/Telemetry$Position;)V");
            env->DeleteLocalRef(callbackClass);
        }
    }

    void operator()(const mavsdk_telemetry_position_t position) const {
        if (!callback.isValid() || !invokeMethod || !g_jvm) {
            return;
        }

        JavaVMAttacher attacher(g_jvm);
        JNIEnv* env = attacher.getEnv();

        if (!env) {
            return;
        }

        // Create Position object
        jclass positionClass = env->FindClass("io/mavsdk/kotlin/plugins/telemetry/Telemetry$Position");
        jmethodID positionConstructor = env->GetMethodID(positionClass, "<init>", "(DDFF)V");

        jobject positionObj = env->NewObject(
            positionClass,
            positionConstructor,
            position.latitude_deg,
            position.longitude_deg,
            static_cast<jfloat>(position.absolute_altitude_m),
            static_cast<jfloat>(position.relative_altitude_m)
        );

        // Call Java callback
        env->CallVoidMethod(callback.get(), invokeMethod, positionObj);

        // Clean up
        env->DeleteLocalRef(positionClass);
        env->DeleteLocalRef(positionObj);

        // Check for exceptions
        if (env->ExceptionCheck()) {
            env->ExceptionDescribe();
            env->ExceptionClear();
        }
    }
};

extern "C" {

// Telemetry.Companion.createNative
JNIEXPORT jlong JNICALL
Java_io_mavsdk_kotlin_plugins_telemetry_Telemetry_00024Companion_createNative(
    JNIEnv* env,
    jclass clazz,
    jlong systemHandle) {

    if (!systemHandle) {
        throwMavsdkError(env, "OperationError", "Invalid system handle");
        return 0;
    }

    mavsdk_telemetry_t handle = mavsdk_telemetry_create(
        reinterpret_cast<mavsdk_system_t>(systemHandle)
    );

    if (!handle) {
        throwMavsdkError(env, "OperationError", "Failed to create Telemetry plugin");
        return 0;
    }

    return reinterpret_cast<jlong>(handle);
}

// Telemetry.destroy
JNIEXPORT void JNICALL
Java_io_mavsdk_kotlin_plugins_telemetry_Telemetry_destroy(
    JNIEnv* env,
    jobject obj) {

    jlong handle = getHandle(env, obj, "io/mavsdk/kotlin/plugins/telemetry/Telemetry");
    if (!handle) return;

    mavsdk_telemetry_destroy(reinterpret_cast<mavsdk_telemetry_t>(handle));
}

// Telemetry.subscribePositionNative
JNIEXPORT jlong JNICALL
Java_io_mavsdk_kotlin_plugins_telemetry_Telemetry_subscribePositionNative(
    JNIEnv* env,
    jobject obj,
    jobject callback) {

    jlong handle = getHandle(env, obj, "io/mavsdk/kotlin/plugins/telemetry/Telemetry");
    if (!handle || !callback) return 0;

    // Create wrapper with GlobalRef to callback
    auto* wrapper = new PositionCallbackWrapper(env, callback);

    // Subscribe using lambda that captures the wrapper
    mavsdk_telemetry_position_handle_t subscription_handle =
        mavsdk_telemetry_subscribe_position(
            reinterpret_cast<mavsdk_telemetry_t>(handle),
            [](const mavsdk_telemetry_position_t position, void* user_data) {
                auto* w = static_cast<PositionCallbackWrapper*>(user_data);
                (*w)(position);
            },
            wrapper
        );

    // Store both the subscription handle and wrapper pointer
    auto* handle_pair = new std::pair<mavsdk_telemetry_position_handle_t, PositionCallbackWrapper*>(
        subscription_handle, wrapper
    );

    return reinterpret_cast<jlong>(handle_pair);
}

// Telemetry.unsubscribePosition
JNIEXPORT void JNICALL
Java_io_mavsdk_kotlin_plugins_telemetry_Telemetry_unsubscribePosition(
    JNIEnv* env,
    jobject obj,
    jlong subscriptionHandle) {

    jlong handle = getHandle(env, obj, "io/mavsdk/kotlin/plugins/telemetry/Telemetry");
    if (!handle || !subscriptionHandle) return;

    auto* handle_pair = reinterpret_cast<
        std::pair<mavsdk_telemetry_position_handle_t, PositionCallbackWrapper*>*>(subscriptionHandle);

    if (handle_pair) {
        mavsdk_telemetry_unsubscribe_position(
            reinterpret_cast<mavsdk_telemetry_t>(handle),
            handle_pair->first
        );
        delete handle_pair->second; // Delete wrapper (GlobalRef released in destructor)
        delete handle_pair;
    }
}

} // extern "C"
