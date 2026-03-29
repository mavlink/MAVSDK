#pragma once

// Detect whether std::format is usable on this platform.
//
// Strategy: if <format> is present and we are not on a known-broken platform,
// include it eagerly so that __cpp_lib_format is defined before we test it.
// GCC 13 and GCC 14 only define __cpp_lib_format when <format> is actually
// included; they do NOT expose it via <bits/c++config.h> or implicitly through
// other standard headers.  Testing the macro *before* including <format> (as
// earlier revisions of this header did) therefore always returns "undefined"
// on those compilers, setting MAVSDK_HAS_STD_FORMAT=0 and silently turning
// MAVSDK_DEFINE_FORMATTER into a no-op — which is the root cause of the
// "std::formatter must be specialized" errors on GCC 13/14 CI jobs.
//
// Conditions for disabling std::format:
//   1. The <format> header is not present (e.g. GCC < 13 cross-toolchains).
//   2. iOS / iOS-simulator deployment target < 16.3: Apple's libc++ std::format
//      floating-point support calls std::to_chars which is only available
//      from iOS 16.3 / iPadOS 16.3 onward.
//   3. Even after including <format>, __cpp_lib_format is still not defined
//      (guards against broken/stub implementations).

// Step 1: reject known-broken iOS targets up front.
#if defined(__IPHONE_OS_VERSION_MIN_REQUIRED) && (__IPHONE_OS_VERSION_MIN_REQUIRED < 160300)
#  define MAVSDK_HAS_STD_FORMAT 0
// Step 2: if <format> is present, include it and then check the feature macro.
#elif defined(__has_include) && __has_include(<format>)
#  include <format>
#  if defined(__cpp_lib_format)
#    define MAVSDK_HAS_STD_FORMAT 1
#  else
#    define MAVSDK_HAS_STD_FORMAT 0
#  endif
// Step 3: <format> is not available at all.
#else
#  define MAVSDK_HAS_STD_FORMAT 0
#endif

#if MAVSDK_HAS_STD_FORMAT
// <format> was already included above.
#include <filesystem>
#include <sstream>
#include <string>
#include <type_traits>

// All std::formatter specializations below are defined inside namespace std
// so that GCC's compile-time format-string checker (_Checking_scanner /
// basic_format_string) can find the constrained partial specialisations via
// normal lookup rather than relying on ADL from a definition in the global
// namespace.  Defining them outside namespace std with the qualified name
// (struct std::formatter<T>) is technically legal C++ but GCC 13/14's
// consteval format-string validation does not consistently apply constrained
// partial specialisations that were introduced that way, causing hard errors
// even though the types do have operator<<.
namespace std {

// std::formatter for std::filesystem::path is not available until C++26.
// Provide a specialization here so paths can be passed directly to std::format.
template<> struct formatter<filesystem::path> : formatter<string> {
    template<typename FormatContext>
    auto format(const filesystem::path& p, FormatContext& ctx) const
    {
        return formatter<string>::format(p.string(), ctx);
    }
};

// Formatter for scoped enum types (enum class) that provide operator<<.
// All MAVSDK plugin Result, Mode, Status, etc. enums are generated with
// operator<< so they print their named value (e.g. "Success") rather than a
// raw integer.  Use an explicit FormatContext template parameter (not
// abbreviated `auto&`) for maximum GCC compatibility.
//
// Note: the constraint intentionally omits the `requires(ostream& os, T v) { os << v; }`
// check.  GCC 13/14's consteval format-string checker (_Checking_scanner) evaluates
// is_default_constructible_v<std::formatter<T>> at consteval time and cannot perform
// ADL lookup for external-namespace operators inside that consteval context, so a
// constrained partial specialisation that depends on finding `mavsdk::operator<<` via
// ADL always appears unsatisfied and the deleted primary template is selected instead.
// Dropping the ostream constraint allows GCC 13/14 to verify the specialisation exists;
// the os << val expression inside format() still produces a clear compile error if a
// scoped enum without operator<< is used.
template<typename T>
    requires(is_enum_v<T> && !is_convertible_v<T, underlying_type_t<T>>)
struct formatter<T> : formatter<string> {
    template<typename FormatContext> auto format(T val, FormatContext& ctx) const
    {
        ostringstream ss;
        ss << val;
        return formatter<string>::format(ss.str(), ctx);
    }
};

// Formatter for class/struct types that provide operator<< but have no native
// std::formatter specialization (e.g. MissionPlan, MissionItem, ParamValue).
template<typename T>
    requires(
        is_class_v<T> && !is_same_v<remove_cvref_t<T>, string> &&
        !is_same_v<remove_cvref_t<T>, string_view> &&
        !is_same_v<remove_cvref_t<T>, filesystem::path> &&
        requires(ostream& os, const T& v) { os << v; })
struct formatter<T> : formatter<string> {
    template<typename FormatContext> auto format(const T& val, FormatContext& ctx) const
    {
        ostringstream ss;
        ss << val;
        return formatter<string>::format(ss.str(), ctx);
    }
};

} // namespace std

// ---------------------------------------------------------------------------
// MAVSDK_DEFINE_FORMATTER(T)
//
// Defines an *explicit* std::formatter<T> specialisation for a type T that
// has operator<< defined.  Use this macro at file scope, after the type T is
// fully declared.
//
// Rationale: GCC 13/14 and Clang 19's consteval format-string checker
// (basic_format_string / _Checking_scanner) evaluates
// is_default_constructible_v<std::formatter<T>> inside a consteval function.
// In that context the compiler cannot find *constrained* partial
// specialisations of std::formatter, so it falls back to the deleted primary
// template and emits:
//   error: std::formatter must be specialized for each type being formatted
//
// An explicit full specialisation (template <> struct formatter<ConcreteType>)
// IS found by all compilers in consteval context and therefore works reliably.
// ---------------------------------------------------------------------------
#define MAVSDK_DEFINE_FORMATTER(T) \
    namespace std { \
    template<> struct formatter<T> : formatter<string> { \
        template<typename FormatContext> auto format(const T& val, FormatContext& ctx) const \
        { \
            ostringstream ss; \
            ss << val; \
            return formatter<string>::format(ss.str(), ctx); \
        } \
    }; \
    } /* namespace std */

#else // !MAVSDK_HAS_STD_FORMAT

// No-op when std::format is not available.
#define MAVSDK_DEFINE_FORMATTER(T)

#endif // MAVSDK_HAS_STD_FORMAT
