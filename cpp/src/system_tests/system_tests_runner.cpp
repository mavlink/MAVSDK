#if defined(ENABLE_CPPTRACE)
#include <signal.h>
#include <cpptrace/cpptrace.hpp>
#include "log.hpp"
#include <cstring>
#include <string_view>
#endif
#include <gtest/gtest.h>

#if defined(ENABLE_CPPTRACE)
void handler(int sig)
{
    const char* signal_name = strsignal(sig);
    if (signal_name != nullptr) {
        // Use string_view to avoid format string interpretation issues
        LogErr("Got signal: {} ({})", std::string_view(signal_name), sig);
    } else {
        LogErr("Got signal: <unknown> ({})", sig);
    }

    // Note: generate_trace() symbolizes eagerly and is not strictly async-signal-safe,
    // but we exit right after, and in practice it produces a fully symbolized backtrace
    // (function, file, line). cpptrace's signal-safe raw-trace API is not used because it
    // needs the libunwind backend to collect frames; with the default unwinder it returns
    // nothing, which would be worse than this.
    cpptrace::generate_trace().print();
    exit(1);
}
#endif

int main(int argc, char** argv)
{
#if defined(ENABLE_CPPTRACE)
    signal(SIGSEGV, handler);
    signal(SIGQUIT, handler);
    signal(SIGABRT, handler);
    signal(SIGILL, handler);
    signal(SIGBUS, handler);
    signal(SIGPIPE, handler);
    signal(SIGFPE, handler);
#endif

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
