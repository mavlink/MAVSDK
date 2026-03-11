#ifndef MAVSDK_JNI_UTILS_H
#define MAVSDK_JNI_UTILS_H

#include <jni.h>
#include <string>

namespace mavsdk {
namespace jni {

// Global JavaVM pointer - shared across all JNI files
// Initialized in JNI_OnLoad, used for thread attachment in callbacks
extern JavaVM* g_jvm;

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
 * RAII wrapper for JNI global reference
 * Use this for objects that need to outlive the current JNI call (e.g., callbacks)
 */
class GlobalRefHolder {
public:
    GlobalRefHolder(JNIEnv* env, jobject obj)
        : env_(env), ref_(nullptr) {
        if (obj) {
            ref_ = env->NewGlobalRef(obj);
        }
    }

    ~GlobalRefHolder() {
        if (ref_) {
            env_->DeleteGlobalRef(ref_);
        }
    }

    jobject get() const { return ref_; }
    operator jobject() const { return ref_; }
    bool isValid() const { return ref_ != nullptr; }

    // Prevent copying, allow moving
    GlobalRefHolder(const GlobalRefHolder&) = delete;
    GlobalRefHolder& operator=(const GlobalRefHolder&) = delete;

    GlobalRefHolder(GlobalRefHolder&& other) noexcept
        : env_(other.env_), ref_(other.ref_) {
        other.ref_ = nullptr;
    }

    GlobalRefHolder& operator=(GlobalRefHolder&& other) noexcept {
        if (this != &other) {
            if (ref_) {
                env_->DeleteGlobalRef(ref_);
            }
            env_ = other.env_;
            ref_ = other.ref_;
            other.ref_ = nullptr;
        }
        return *this;
    }

private:
    JNIEnv* env_;
    jobject ref_;
};

/**
 * Helper for attaching current thread to JVM
 * Automatically detaches when going out of scope if it performed the attach
 */
class JavaVMAttacher {
public:
    JavaVMAttacher(JavaVM* jvm) 
        : jvm_(jvm), env_(nullptr), should_detach_(false) {
        
        if (!jvm_) return;
        
        // Try to get the environment for the current thread
        jint result = jvm_->GetEnv(reinterpret_cast<void**>(&env_), JNI_VERSION_1_6);
        
        if (result == JNI_EDETACHED) {
            // Thread not attached, attach it
            result = jvm_->AttachCurrentThread(reinterpret_cast<void**>(&env_), nullptr);
            if (result == JNI_OK) {
                should_detach_ = true;
            }
        }
    }

    ~JavaVMAttacher() {
        if (should_detach_ && jvm_) {
            jvm_->DetachCurrentThread();
        }
    }

    JNIEnv* getEnv() const { return env_; }
    bool isAttached() const { return env_ != nullptr; }

    JavaVMAttacher(const JavaVMAttacher&) = delete;
    JavaVMAttacher& operator=(const JavaVMAttacher&) = delete;

private:
    JavaVM* jvm_;
    JNIEnv* env_;
    bool should_detach_;
};

/**
 * Get handle from Java object by accessing the private handle field directly
 */
inline jlong getHandle(JNIEnv* env, jobject obj, const char* className) {
    jclass clazz = env->FindClass(className);
    if (!clazz) return 0;
    
    jfieldID handleField = env->GetFieldID(clazz, "handle", "J");
    env->DeleteLocalRef(clazz);
    
    if (!handleField) return 0;
    
    return env->GetLongField(obj, handleField);
}

} // namespace jni
} // namespace mavsdk

#endif // MAVSDK_JNI_UTILS_H
