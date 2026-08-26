#include <jni.h>
#include <utility>
#include <vector>
#include "cmavsdk/system.h"
#include "jni_utils.h"

using namespace mavsdk::jni;

namespace mavsdk {
namespace jni {
JavaVM* g_jvm = nullptr;
} // namespace jni
} // namespace mavsdk

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void*) {
    mavsdk::jni::g_jvm = vm;

    JNIEnv* env = nullptr;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) == JNI_OK && env) {
        mavsdk::jni::initClassLoader(env);
    }

    return JNI_VERSION_1_6;
}

template<typename... Args> struct CallbackWrapper {
    GlobalRefHolder callback;
    jmethodID invoke_method;

    CallbackWrapper(JNIEnv* env, jobject callback_obj, const char* signature)
        : callback(env, callback_obj), invoke_method(nullptr) {
        if (callback.isValid()) {
            jclass callback_class = env->GetObjectClass(callback_obj);
            invoke_method = env->GetMethodID(callback_class, "invoke", signature);
            env->DeleteLocalRef(callback_class);
        }
    }

    void invoke(Args... args) const {
        if (!callback.isValid() || !invoke_method || !g_jvm) return;
        JavaVMAttacher attacher(g_jvm);
        JNIEnv* env = attacher.getEnv();
        if (!env) return;
        env->CallVoidMethod(callback.get(), invoke_method, args...);
        if (env->ExceptionCheck()) {
            env->ExceptionDescribe();
            env->ExceptionClear();
        }
    }
};

using BooleanCallbackWrapper = CallbackWrapper<jboolean>;
using ComponentCallbackWrapper = CallbackWrapper<jint>;
using ComponentIdCallbackWrapper = CallbackWrapper<jint, jint>;

extern "C" {

JNIEXPORT void JNICALL
Java_io_mavsdk_jni_NativeSystem_destroy(
    JNIEnv*, jobject, jlong handle) {
    if (handle) {
        mavsdk_system_destroy(reinterpret_cast<mavsdk_system_t>(handle));
    }
}

JNIEXPORT jboolean JNICALL
Java_io_mavsdk_jni_NativeSystem_hasAutopilot(
    JNIEnv* env, jobject, jlong handle) {
    if (!requireHandle(env, handle, "system")) return false;
    return mavsdk_system_has_autopilot(
        reinterpret_cast<mavsdk_system_t>(handle));
}

JNIEXPORT jboolean JNICALL
Java_io_mavsdk_jni_NativeSystem_isStandalone(
    JNIEnv* env, jobject, jlong handle) {
    if (!requireHandle(env, handle, "system")) return false;
    return mavsdk_system_is_standalone(
        reinterpret_cast<mavsdk_system_t>(handle));
}

JNIEXPORT jboolean JNICALL
Java_io_mavsdk_jni_NativeSystem_hasCamera(
    JNIEnv* env, jobject, jlong handle, jint camera_id) {
    if (!requireHandle(env, handle, "system")) return false;
    return mavsdk_system_has_camera(
        reinterpret_cast<mavsdk_system_t>(handle), camera_id);
}

JNIEXPORT jboolean JNICALL
Java_io_mavsdk_jni_NativeSystem_hasGimbal(
    JNIEnv* env, jobject, jlong handle) {
    if (!requireHandle(env, handle, "system")) return false;
    return mavsdk_system_has_gimbal(
        reinterpret_cast<mavsdk_system_t>(handle));
}

JNIEXPORT jboolean JNICALL
Java_io_mavsdk_jni_NativeSystem_isConnected(
    JNIEnv* env, jobject, jlong handle) {
    if (!requireHandle(env, handle, "system")) return false;
    return mavsdk_system_is_connected(
        reinterpret_cast<mavsdk_system_t>(handle));
}

JNIEXPORT jint JNICALL
Java_io_mavsdk_jni_NativeSystem_getSystemId(
    JNIEnv* env, jobject, jlong handle) {
    if (!requireHandle(env, handle, "system")) return 0;
    // See NativeConfiguration_getSystemId: jint is signed, the bits survive.
    return static_cast<jint>(mavsdk_system_get_system_id(
        reinterpret_cast<mavsdk_system_t>(handle)));
}

JNIEXPORT jintArray JNICALL
Java_io_mavsdk_jni_NativeSystem_getComponentIds(
    JNIEnv* env, jobject, jlong handle) {
    if (!requireHandle(env, handle, "system")) return nullptr;

    size_t count = 0;
    uint8_t* ids = mavsdk_system_component_ids(
        reinterpret_cast<mavsdk_system_t>(handle), &count);
    jintArray result = env->NewIntArray(static_cast<jsize>(count));
    if (result && count > 0) {
        std::vector<jint> values(count);
        for (size_t i = 0; i < count; ++i) {
            values[i] = ids[i];
        }
        env->SetIntArrayRegion(
            result, 0, static_cast<jsize>(count), values.data());
    }
    mavsdk_system_free_component_ids(ids);
    return result;
}

JNIEXPORT jint JNICALL
Java_io_mavsdk_jni_NativeSystem_getAutopilotType(
    JNIEnv* env, jobject, jlong handle) {
    if (!requireHandle(env, handle, "system")) return 0;
    return static_cast<jint>(mavsdk_system_autopilot_type(
        reinterpret_cast<mavsdk_system_t>(handle)));
}

JNIEXPORT jint JNICALL
Java_io_mavsdk_jni_NativeSystem_getVehicleType(
    JNIEnv* env, jobject, jlong handle) {
    if (!requireHandle(env, handle, "system")) return 0;
    return static_cast<jint>(mavsdk_system_vehicle_type(
        reinterpret_cast<mavsdk_system_t>(handle)));
}

JNIEXPORT void JNICALL
Java_io_mavsdk_jni_NativeSystem_enableTimesync(
    JNIEnv* env, jobject, jlong handle) {
    if (!requireHandle(env, handle, "system")) return;
    mavsdk_system_enable_timesync(reinterpret_cast<mavsdk_system_t>(handle));
}

JNIEXPORT jlong JNICALL
Java_io_mavsdk_jni_NativeSystem_subscribeIsConnected(
    JNIEnv* env, jobject, jlong handle, jobject callback) {
    if (!requireHandle(env, handle, "system") || !callback) return 0;
    auto* wrapper = new BooleanCallbackWrapper(env, callback, "(Z)V");
    const auto native_handle = mavsdk_system_subscribe_is_connected(
        reinterpret_cast<mavsdk_system_t>(handle),
        [](bool value, void* user_data) {
            static_cast<BooleanCallbackWrapper*>(user_data)->invoke(
                static_cast<jboolean>(value));
        },
        wrapper);
    auto* pair = new std::pair<mavsdk_is_connected_handle_t, BooleanCallbackWrapper*>(
        native_handle, wrapper);
    return reinterpret_cast<jlong>(pair);
}

JNIEXPORT void JNICALL
Java_io_mavsdk_jni_NativeSystem_unsubscribeIsConnected(
    JNIEnv* env, jobject, jlong handle, jlong subscription_handle) {
    if (!requireHandle(env, handle, "system") || !subscription_handle) return;
    auto* pair = reinterpret_cast<
        std::pair<mavsdk_is_connected_handle_t, BooleanCallbackWrapper*>*>(
        subscription_handle);
    mavsdk_system_unsubscribe_is_connected(
        reinterpret_cast<mavsdk_system_t>(handle), pair->first);
    delete pair->second;
    delete pair;
}

JNIEXPORT jlong JNICALL
Java_io_mavsdk_jni_NativeSystem_subscribeComponentDiscovered(
    JNIEnv* env, jobject, jlong handle, jobject callback) {
    if (!requireHandle(env, handle, "system") || !callback) return 0;
    auto* wrapper = new ComponentCallbackWrapper(env, callback, "(I)V");
    const auto native_handle = mavsdk_system_subscribe_component_discovered(
        reinterpret_cast<mavsdk_system_t>(handle),
        [](mavsdk_component_type_t type, void* user_data) {
            static_cast<ComponentCallbackWrapper*>(user_data)->invoke(
                static_cast<jint>(type));
        },
        wrapper);
    auto* pair = new std::pair<
        mavsdk_component_discovered_handle_t, ComponentCallbackWrapper*>(
        native_handle, wrapper);
    return reinterpret_cast<jlong>(pair);
}

JNIEXPORT void JNICALL
Java_io_mavsdk_jni_NativeSystem_unsubscribeComponentDiscovered(
    JNIEnv* env, jobject, jlong handle, jlong subscription_handle) {
    if (!requireHandle(env, handle, "system") || !subscription_handle) return;
    auto* pair = reinterpret_cast<std::pair<
        mavsdk_component_discovered_handle_t, ComponentCallbackWrapper*>*>(
        subscription_handle);
    mavsdk_system_unsubscribe_component_discovered(
        reinterpret_cast<mavsdk_system_t>(handle), pair->first);
    delete pair->second;
    delete pair;
}

JNIEXPORT jlong JNICALL
Java_io_mavsdk_jni_NativeSystem_subscribeComponentDiscoveredId(
    JNIEnv* env, jobject, jlong handle, jobject callback) {
    if (!requireHandle(env, handle, "system") || !callback) return 0;
    auto* wrapper = new ComponentIdCallbackWrapper(env, callback, "(II)V");
    const auto native_handle = mavsdk_system_subscribe_component_discovered_id(
        reinterpret_cast<mavsdk_system_t>(handle),
        [](mavsdk_component_type_t type, uint8_t id, void* user_data) {
            static_cast<ComponentIdCallbackWrapper*>(user_data)->invoke(
                static_cast<jint>(type), static_cast<jint>(id));
        },
        wrapper);
    auto* pair = new std::pair<
        mavsdk_component_discovered_id_handle_t, ComponentIdCallbackWrapper*>(
        native_handle, wrapper);
    return reinterpret_cast<jlong>(pair);
}

JNIEXPORT void JNICALL
Java_io_mavsdk_jni_NativeSystem_unsubscribeComponentDiscoveredId(
    JNIEnv* env, jobject, jlong handle, jlong subscription_handle) {
    if (!requireHandle(env, handle, "system") || !subscription_handle) return;
    auto* pair = reinterpret_cast<std::pair<
        mavsdk_component_discovered_id_handle_t, ComponentIdCallbackWrapper*>*>(
        subscription_handle);
    mavsdk_system_unsubscribe_component_discovered_id(
        reinterpret_cast<mavsdk_system_t>(handle), pair->first);
    delete pair->second;
    delete pair;
}

} // extern "C"
