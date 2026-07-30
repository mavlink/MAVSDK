#ifndef MAVSDK_JNI_UTILS_H
#define MAVSDK_JNI_UTILS_H

#include <jni.h>
#include <algorithm>
#include <map>
#include <mutex>
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
    // FindClass with an exception already pending is not allowed.
    if (env->ExceptionCheck()) {
        env->ExceptionClear();
    }
    jclass exClass = env->FindClass(className);
    if (exClass) {
        env->ThrowNew(exClass, message);
        env->DeleteLocalRef(exClass);
    }
}

/**
 * Throw MavsdkError
 */
inline void throwMavsdkError(JNIEnv* env, const char* message) {
    throwException(env, "java/lang/IllegalStateException", message);
}

inline void throwMavsdkError(JNIEnv* env, const char*, const char* message) {
    throwMavsdkError(env, message);
}

inline bool requireHandle(JNIEnv* env, jlong handle, const char* type) {
    if (handle) return true;
    std::string message = "Invalid ";
    message += type;
    message += " handle";
    throwMavsdkError(env, message.c_str());
    return false;
}

namespace detail {

/**
 * Detaches the current thread from the JVM when the thread exits.
 *
 * Only armed for threads that we attached ourselves - threads the JVM already
 * owns must never be detached by us.
 */
class ThreadDetacher {
public:
    void arm() { armed_ = true; }

    ~ThreadDetacher() {
        if (armed_ && g_jvm) {
            g_jvm->DetachCurrentThread();
        }
    }

private:
    bool armed_ = false;
};

inline ThreadDetacher& threadDetacher() {
    static thread_local ThreadDetacher detacher;
    return detacher;
}

} // namespace detail

/**
 * Helper for attaching the current thread to the JVM
 *
 * MAVSDK invokes callbacks on its own threads, which the JVM knows nothing
 * about. The first callback on such a thread attaches it and the thread stays
 * attached until it exits: attaching and detaching around every callback is
 * expensive (on Android each attach builds and tears down a java.lang.Thread
 * peer object), and telemetry callbacks arrive at high rates.
 *
 * Attaching as a daemon means a thread that outlives our detach hook can still
 * never block JVM shutdown.
 */
class JavaVMAttacher {
public:
    explicit JavaVMAttacher(JavaVM* jvm)
        : env_(nullptr) {

        if (!jvm) return;

        const jint result = jvm->GetEnv(reinterpret_cast<void**>(&env_), JNI_VERSION_1_6);
        if (result == JNI_OK) {
            return;
        }
        if (result != JNI_EDETACHED) {
            env_ = nullptr;
            return;
        }

        if (jvm->AttachCurrentThreadAsDaemon(reinterpret_cast<void**>(&env_), nullptr) == JNI_OK) {
            detail::threadDetacher().arm();
        } else {
            env_ = nullptr;
        }
    }

    JNIEnv* getEnv() const { return env_; }
    bool isAttached() const { return env_ != nullptr; }

    JavaVMAttacher(const JavaVMAttacher&) = delete;
    JavaVMAttacher& operator=(const JavaVMAttacher&) = delete;

private:
    JNIEnv* env_;
};

/**
 * RAII wrapper for JNI global reference
 * Use this for objects that need to outlive the current JNI call (e.g., callbacks)
 */
class GlobalRefHolder {
public:
    GlobalRefHolder(JNIEnv* env, jobject obj)
        : jvm_(nullptr), ref_(nullptr) {
        if (obj) {
            env->GetJavaVM(&jvm_);
            ref_ = env->NewGlobalRef(obj);
        }
    }

    ~GlobalRefHolder() {
        release();
    }

    jobject get() const { return ref_; }
    operator jobject() const { return ref_; }
    bool isValid() const { return ref_ != nullptr; }

    // Prevent copying, allow moving
    GlobalRefHolder(const GlobalRefHolder&) = delete;
    GlobalRefHolder& operator=(const GlobalRefHolder&) = delete;

    GlobalRefHolder(GlobalRefHolder&& other) noexcept
        : jvm_(other.jvm_), ref_(other.ref_) {
        other.ref_ = nullptr;
    }

    GlobalRefHolder& operator=(GlobalRefHolder&& other) noexcept {
        if (this != &other) {
            release();
            jvm_ = other.jvm_;
            ref_ = other.ref_;
            other.ref_ = nullptr;
        }
        return *this;
    }

private:
    void release() {
        if (!ref_ || !jvm_) return;

        JavaVMAttacher attacher(jvm_);
        if (JNIEnv* env = attacher.getEnv()) {
            env->DeleteGlobalRef(ref_);
        }
        ref_ = nullptr;
    }

    JavaVM* jvm_;
    jobject ref_;
};

namespace detail {

struct ClassCache {
    // Recursive: loadClass() below runs Java code, which can in principle
    // re-enter findClass() through a static initializer.
    std::recursive_mutex mutex;
    std::map<std::string, jclass> classes; // values are global refs, never freed
    jobject loader = nullptr;              // global ref to the app's ClassLoader
    jmethodID load_class = nullptr;        // ClassLoader.loadClass(String)
};

inline ClassCache& classCache() {
    static ClassCache cache;
    return cache;
}

} // namespace detail

/**
 * Capture the class loader that loaded this library. Must be called from
 * JNI_OnLoad, where FindClass still resolves against that loader.
 *
 * Without this, looking up our own classes from a MAVSDK callback thread fails
 * on Android: a thread attached from native code has no Java call stack, so
 * FindClass resolves against the system class loader, which cannot see
 * application classes. (On desktop JVMs it happens to work, because
 * application classes live in the system class loader there.)
 */
inline void initClassLoader(JNIEnv* env) {
    auto& cache = detail::classCache();
    std::lock_guard<std::recursive_mutex> lock(cache.mutex);

    jclass anchor = env->FindClass("io/mavsdk/jni/NativeMavsdk");
    if (!anchor) {
        env->ExceptionClear();
        return;
    }

    jclass classClass = env->GetObjectClass(anchor);
    jmethodID getClassLoader =
        env->GetMethodID(classClass, "getClassLoader", "()Ljava/lang/ClassLoader;");
    jobject loader = getClassLoader ? env->CallObjectMethod(anchor, getClassLoader) : nullptr;

    if (loader) {
        cache.loader = env->NewGlobalRef(loader);
        jclass loaderClass = env->GetObjectClass(loader);
        cache.load_class =
            env->GetMethodID(loaderClass, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
        env->DeleteLocalRef(loaderClass);
        env->DeleteLocalRef(loader);
    }

    env->DeleteLocalRef(classClass);
    env->DeleteLocalRef(anchor);
}

/**
 * Look up a class by JNI name ("java/lang/String", "io/mavsdk/jni/X$Y").
 *
 * Returns a cached global reference - do NOT call DeleteLocalRef on the result.
 * Falls back to the class loader captured in initClassLoader() when a plain
 * FindClass fails, which is what makes lookups work from callback threads.
 */
inline jclass findClass(JNIEnv* env, const char* name) {
    auto& cache = detail::classCache();
    std::lock_guard<std::recursive_mutex> lock(cache.mutex);

    const auto it = cache.classes.find(name);
    if (it != cache.classes.end()) {
        return it->second;
    }

    jclass found = env->FindClass(name);
    if (!found) {
        if (env->ExceptionCheck()) {
            env->ExceptionClear();
        }
        if (cache.loader && cache.load_class) {
            std::string dotted(name);
            std::replace(dotted.begin(), dotted.end(), '/', '.');
            if (jstring jname = env->NewStringUTF(dotted.c_str())) {
                found = static_cast<jclass>(
                    env->CallObjectMethod(cache.loader, cache.load_class, jname));
                if (env->ExceptionCheck()) {
                    env->ExceptionClear();
                    found = nullptr;
                }
                env->DeleteLocalRef(jname);
            }
        }
    }

    if (!found) {
        return nullptr;
    }

    jclass global = static_cast<jclass>(env->NewGlobalRef(found));
    env->DeleteLocalRef(found);
    if (global) {
        cache.classes.emplace(name, global);
    }
    return global;
}

} // namespace jni
} // namespace mavsdk

#endif // MAVSDK_JNI_UTILS_H
