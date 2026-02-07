#ifndef MAVSDK_JNI_UTILS_H
#define MAVSDK_JNI_UTILS_H

#include <jni.h>
#include <string>

namespace mavsdk {
namespace jni {

/**
 * RAII wrapper for JNI string
 */
class JStringHolder {
public:
    JStringHolder(JNIEnv* env, jstring str)
        : env_(env), jstr_(str), chars_(nullptr) {
        if (str) {
            chars_ = env->GetStringUTFChars(str, nullptr);
        }
    }

    ~JStringHolder() {
        if (chars_) {
            env_->ReleaseStringUTFChars(jstr_, chars_);
        }
    }

    const char* c_str() const { return chars_; }
    operator const char*() const { return chars_; }

    JStringHolder(const JStringHolder&) = delete;
    JStringHolder& operator=(const JStringHolder&) = delete;

private:
    JNIEnv* env_;
    jstring jstr_;
    const char* chars_;
};

/**
 * Convert C string to Java string
 */
inline jstring toJavaString(JNIEnv* env, const char* str) {
    if (!str) return nullptr;
    return env->NewStringUTF(str);
}

/**
 * Throw Java exception
 */
inline void throwException(JNIEnv* env, const char* className, const char* message) {
    jclass exClass = env->FindClass(className);
    if (exClass) {
        env->ThrowNew(exClass, message);
        env->DeleteLocalRef(exClass);
    }
}

/**
 * Throw MavsdkError
 */
inline void throwMavsdkError(JNIEnv* env, const char* errorType, const char* message) {
    std::string className = "io/mavsdk/kotlin/MavsdkError$";
    className += errorType;
    throwException(env, className.c_str(), message);
}

/**
 * Get handle from Java object
 */
inline jlong getHandle(JNIEnv* env, jobject obj, const char* className) {
    jclass clazz = env->FindClass(className);
    if (!clazz) return 0;
    
    jmethodID getHandleMethod = env->GetMethodID(clazz, "getHandle", "()J");
    env->DeleteLocalRef(clazz);
    
    if (!getHandleMethod) return 0;
    
    return env->CallLongMethod(obj, getHandleMethod);
}

} // namespace jni
} // namespace mavsdk

#endif // MAVSDK_JNI_UTILS_H
