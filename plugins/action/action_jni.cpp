#include <jni.h>
#include "cmavsdk/plugins/action/action.h"
#include "../../jni_utils.h"

using namespace mavsdk::jni;

// Callback wrapper for arm async
struct ArmCallbackWrapper {
    GlobalRefHolder callback;
    jmethodID invokeMethod;

    ArmCallbackWrapper(JNIEnv* env, jobject callback_obj)
        : callback(env, callback_obj), invokeMethod(nullptr) {

        if (callback.isValid()) {
            // Cache the method ID for the callback interface
            // Signature: (I)V - takes int (result enum), returns void
            jclass callbackClass = env->GetObjectClass(callback_obj);
            invokeMethod = env->GetMethodID(callbackClass, "invoke", "(I)V");
            env->DeleteLocalRef(callbackClass);
        }
    }

    void operator()(const mavsdk_action_result_t result) const {
        if (!callback.isValid() || !invokeMethod || !g_jvm) {
            return;
        }

        JavaVMAttacher attacher(g_jvm);
        JNIEnv* env = attacher.getEnv();

        if (!env) {
            return;
        }

        // Call Java callback with result enum value
        env->CallVoidMethod(callback.get(), invokeMethod, static_cast<jint>(result));

        // Check for exceptions
        if (env->ExceptionCheck()) {
            env->ExceptionDescribe();
            env->ExceptionClear();
        }
    }
};

extern "C" {

// Action.create
JNIEXPORT jlong JNICALL
Java_io_mavsdk_kotlin_Action_create(
    JNIEnv* env,
    jclass clazz,
    jlong systemHandle) {

    if (!systemHandle) {
        throwMavsdkError(env, "OperationError", "Invalid system handle");
        return 0;
    }

    mavsdk_action_t handle = mavsdk_action_create(
        reinterpret_cast<mavsdk_system_t>(systemHandle)
    );

    if (!handle) {
        throwMavsdkError(env, "OperationError", "Failed to create Action plugin");
        return 0;
    }

    return reinterpret_cast<jlong>(handle);
}

// Action.destroy
JNIEXPORT void JNICALL
Java_io_mavsdk_kotlin_Action_destroy(
    JNIEnv* env,
    jobject obj) {

    jlong handle = getHandle(env, obj, "io/mavsdk/kotlin/Action");
    if (!handle) return;

    mavsdk_action_destroy(reinterpret_cast<mavsdk_action_t>(handle));
}

// Action.arm (blocking)
JNIEXPORT jint JNICALL
Java_io_mavsdk_kotlin_Action_arm(
    JNIEnv* env,
    jobject obj) {

    jlong handle = getHandle(env, obj, "io/mavsdk/kotlin/Action");
    if (!handle) return MAVSDK_ACTION_RESULT_UNKNOWN;

    mavsdk_action_result_t result = mavsdk_action_arm(
        reinterpret_cast<mavsdk_action_t>(handle)
    );

    return static_cast<jint>(result);
}

// Action.armAsync
JNIEXPORT void JNICALL
Java_io_mavsdk_kotlin_Action_armAsync(
    JNIEnv* env,
    jobject obj,
    jobject callback) {

    jlong handle = getHandle(env, obj, "io/mavsdk/kotlin/Action");
    if (!handle || !callback) return;

    // Create wrapper with GlobalRef to callback
    auto* wrapper = new ArmCallbackWrapper(env, callback);

    // Call async function
    mavsdk_action_arm_async(
        reinterpret_cast<mavsdk_action_t>(handle),
        [](const mavsdk_action_result_t result, void* user_data) {
            auto* w = static_cast<ArmCallbackWrapper*>(user_data);
            (*w)(result);
            // Delete wrapper after callback is invoked (one-time callback)
            delete w;
        },
        wrapper
    );
}

} // extern "C"
