#include <jni.h>
#include <utility>
#include <vector>
#include "cmavsdk/mavsdk.h"
#include "jni_utils.h"

using namespace mavsdk::jni;

struct NewSystemCallbackWrapper {
    GlobalRefHolder callback;
    jmethodID invoke_method;
    mavsdk_t mavsdk_handle;

    NewSystemCallbackWrapper(JNIEnv* env, jobject callback_obj, mavsdk_t handle)
        : callback(env, callback_obj), invoke_method(nullptr), mavsdk_handle(handle) {
        if (callback.isValid()) {
            jclass callback_class = env->GetObjectClass(callback_obj);
            invoke_method = env->GetMethodID(callback_class, "invoke", "(J)V");
            env->DeleteLocalRef(callback_class);
        }
    }

    void operator()() const {
        if (!callback.isValid() || !invoke_method || !g_jvm) return;

        JavaVMAttacher attacher(g_jvm);
        JNIEnv* env = attacher.getEnv();
        if (!env) return;

        size_t count = 0;
        mavsdk_system_t* systems = mavsdk_get_systems(mavsdk_handle, &count);
        if (count > 0) {
            env->CallVoidMethod(
                callback.get(),
                invoke_method,
                reinterpret_cast<jlong>(systems[count - 1]));
        }
        mavsdk_free_systems_array(systems);

        if (env->ExceptionCheck()) {
            env->ExceptionDescribe();
            env->ExceptionClear();
        }
    }
};

extern "C" {

JNIEXPORT jlong JNICALL
Java_io_mavsdk_jni_NativeMavsdk_create(
    JNIEnv* env,
    jobject,
    jlong configuration_handle) {

    if (!requireHandle(env, configuration_handle, "configuration")) return 0;
    const auto handle = mavsdk_create(
        reinterpret_cast<mavsdk_configuration_t>(configuration_handle));
    if (!handle) {
        throwMavsdkError(env, "Failed to create MAVSDK instance");
        return 0;
    }
    return reinterpret_cast<jlong>(handle);
}

JNIEXPORT jstring JNICALL
Java_io_mavsdk_jni_NativeMavsdk_version(
    JNIEnv* env,
    jobject,
    jlong handle) {

    if (!requireHandle(env, handle, "MAVSDK")) return nullptr;
    return toJavaString(env, mavsdk_version(reinterpret_cast<mavsdk_t>(handle)));
}

JNIEXPORT jlong JNICALL
Java_io_mavsdk_jni_NativeMavsdk_serverComponentHandle(
    JNIEnv* env,
    jobject,
    jlong handle,
    jint instance) {

    if (!requireHandle(env, handle, "MAVSDK")) return 0;
    return reinterpret_cast<jlong>(mavsdk_server_component(
        reinterpret_cast<mavsdk_t>(handle),
        static_cast<unsigned int>(instance)));
}

JNIEXPORT jint JNICALL
Java_io_mavsdk_jni_NativeMavsdk_addAnyConnection(
    JNIEnv* env,
    jobject,
    jlong handle,
    jstring connection_url) {

    if (!requireHandle(env, handle, "MAVSDK")) return -1;
    JStringHolder url(env, connection_url);
    return static_cast<jint>(mavsdk_add_any_connection(
        reinterpret_cast<mavsdk_t>(handle),
        url.c_str()));
}

JNIEXPORT jlongArray JNICALL
Java_io_mavsdk_jni_NativeMavsdk_addAnyConnectionWithHandle(
    JNIEnv* env,
    jobject,
    jlong handle,
    jstring connection_url) {

    if (!requireHandle(env, handle, "MAVSDK")) return nullptr;
    JStringHolder url(env, connection_url);
    const auto native_result = mavsdk_add_any_connection_with_handle(
        reinterpret_cast<mavsdk_t>(handle),
        url.c_str());

    const jlong values[] = {
        static_cast<jlong>(native_result.result),
        reinterpret_cast<jlong>(native_result.handle),
    };
    jlongArray result = env->NewLongArray(2);
    if (result) {
        env->SetLongArrayRegion(result, 0, 2, values);
    }
    return result;
}

JNIEXPORT void JNICALL
Java_io_mavsdk_jni_NativeMavsdk_removeConnection(
    JNIEnv* env,
    jobject,
    jlong handle,
    jlong connection_handle) {

    if (!requireHandle(env, handle, "MAVSDK")) return;
    mavsdk_remove_connection(
        reinterpret_cast<mavsdk_t>(handle),
        reinterpret_cast<mavsdk_connection_handle_t>(connection_handle));
}

JNIEXPORT jint JNICALL
Java_io_mavsdk_jni_NativeMavsdk_systemCount(
    JNIEnv* env,
    jobject,
    jlong handle) {

    if (!requireHandle(env, handle, "MAVSDK")) return 0;
    return static_cast<jint>(
        mavsdk_system_count(reinterpret_cast<mavsdk_t>(handle)));
}

JNIEXPORT jlongArray JNICALL
Java_io_mavsdk_jni_NativeMavsdk_getSystems(
    JNIEnv* env,
    jobject,
    jlong handle) {

    if (!requireHandle(env, handle, "MAVSDK")) return nullptr;

    size_t count = 0;
    mavsdk_system_t* systems =
        mavsdk_get_systems(reinterpret_cast<mavsdk_t>(handle), &count);
    jlongArray result = env->NewLongArray(static_cast<jsize>(count));
    if (result && count > 0) {
        std::vector<jlong> values(count);
        for (size_t i = 0; i < count; ++i) {
            values[i] = reinterpret_cast<jlong>(systems[i]);
        }
        env->SetLongArrayRegion(
            result, 0, static_cast<jsize>(count), values.data());
    }
    mavsdk_free_systems_array(systems);
    return result;
}

JNIEXPORT jlong JNICALL
Java_io_mavsdk_jni_NativeMavsdk_firstAutopilot(
    JNIEnv* env,
    jobject,
    jlong handle,
    jdouble timeout_seconds) {

    if (!requireHandle(env, handle, "MAVSDK")) return 0;
    return reinterpret_cast<jlong>(mavsdk_first_autopilot(
        reinterpret_cast<mavsdk_t>(handle),
        timeout_seconds));
}

JNIEXPORT void JNICALL
Java_io_mavsdk_jni_NativeMavsdk_destroy(
    JNIEnv*,
    jobject,
    jlong handle) {

    if (handle) {
        mavsdk_destroy(reinterpret_cast<mavsdk_t>(handle));
    }
}

JNIEXPORT jlong JNICALL
Java_io_mavsdk_jni_NativeMavsdk_subscribeOnNewSystem(
    JNIEnv* env,
    jobject,
    jlong handle,
    jobject callback) {

    if (!requireHandle(env, handle, "MAVSDK") || !callback) return 0;
    auto* wrapper = new NewSystemCallbackWrapper(
        env, callback, reinterpret_cast<mavsdk_t>(handle));
    const auto subscription_handle = mavsdk_subscribe_on_new_system(
        reinterpret_cast<mavsdk_t>(handle),
        [](void* user_data) {
            (*static_cast<NewSystemCallbackWrapper*>(user_data))();
        },
        wrapper);
    auto* pair = new std::pair<mavsdk_new_system_handle_t, NewSystemCallbackWrapper*>(
        subscription_handle, wrapper);
    return reinterpret_cast<jlong>(pair);
}

JNIEXPORT void JNICALL
Java_io_mavsdk_jni_NativeMavsdk_unsubscribeOnNewSystem(
    JNIEnv* env,
    jobject,
    jlong handle,
    jlong subscription_handle) {

    if (!requireHandle(env, handle, "MAVSDK") || !subscription_handle) return;
    auto* pair = reinterpret_cast<
        std::pair<mavsdk_new_system_handle_t, NewSystemCallbackWrapper*>*>(
        subscription_handle);
    mavsdk_unsubscribe_on_new_system(
        reinterpret_cast<mavsdk_t>(handle), pair->first);
    delete pair->second;
    delete pair;
}

} // extern "C"
