#include <jni.h>
#include "cmavsdk/log.h"
#include "jni_utils.h"

using namespace mavsdk::jni;

// Callback wrapper that stores Java callback as GlobalRef
struct LogCallbackWrapper {
    GlobalRefHolder callback;
    jmethodID invokeMethod;

    LogCallbackWrapper(JNIEnv* env, jobject callback_obj)
        : callback(env, callback_obj), invokeMethod(nullptr) {

        if (callback.isValid()) {
            // Cache the method ID for the callback interface
            // Signature: (ILjava/lang/String;Ljava/lang/String;I)Z
            // Parameters: (level, message, file, line) -> Boolean
            jclass callbackClass = env->GetObjectClass(callback_obj);
            invokeMethod = env->GetMethodID(callbackClass, "invoke",
                "(ILjava/lang/String;Ljava/lang/String;I)Z");
            env->DeleteLocalRef(callbackClass);
        }
    }

    bool operator()(mavsdk_log_level_t level, const char* message,
                    const char* file, int line) const {
        if (!callback.isValid() || !invokeMethod || !g_jvm) {
            return false; // Keep default logging
        }

        JavaVMAttacher attacher(g_jvm);
        JNIEnv* env = attacher.getEnv();

        if (!env) {
            return false;
        }

        // Convert strings to Java
        jstring jMessage = toJavaString(env, message);
        jstring jFile = file ? toJavaString(env, file) : nullptr;

        // Call Java method
        jboolean result = env->CallBooleanMethod(
            callback.get(),
            invokeMethod,
            static_cast<jint>(level),
            jMessage,
            jFile,
            static_cast<jint>(line)
        );

        // Clean up local references
        if (jMessage) env->DeleteLocalRef(jMessage);
        if (jFile) env->DeleteLocalRef(jFile);

        // Check for exceptions
        if (env->ExceptionCheck()) {
            env->ExceptionDescribe();
            env->ExceptionClear();
            return false;
        }

        return static_cast<bool>(result);
    }
};

// Global wrapper pointer (only one log callback allowed at a time)
static LogCallbackWrapper* g_log_wrapper = nullptr;

// C callback bridge function
static bool log_callback_bridge(mavsdk_log_level_t level, const char* message,
                                 const char* file, int line, void* user_data) {
    auto* wrapper = static_cast<LogCallbackWrapper*>(user_data);
    if (!wrapper) {
        return false;
    }
    return (*wrapper)(level, message, file, line);
}

extern "C" {

// Log.subscribeNative
JNIEXPORT void JNICALL
Java_io_mavsdk_kotlin_Log_subscribeNative(
    JNIEnv* env,
    jclass clazz,
    jobject callback) {

    if (!callback) {
        return;
    }

    // Clean up any existing subscription
    if (g_log_wrapper) {
        mavsdk_log_unsubscribe();
        delete g_log_wrapper;
        g_log_wrapper = nullptr;
    }

    // Create new wrapper with GlobalRef to callback
    g_log_wrapper = new LogCallbackWrapper(env, callback);

    // Subscribe to log messages
    mavsdk_log_subscribe(log_callback_bridge, g_log_wrapper);
}

// Log.unsubscribe
JNIEXPORT void JNICALL
Java_io_mavsdk_kotlin_Log_unsubscribe(
    JNIEnv* env,
    jclass clazz) {

    if (g_log_wrapper) {
        mavsdk_log_unsubscribe();
        delete g_log_wrapper;
        g_log_wrapper = nullptr;
    }
}

} // extern "C"
