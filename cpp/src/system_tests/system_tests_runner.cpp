#if defined(ENABLE_CPPTRACE)
#include <signal.h>
#include <cpptrace/cpptrace.hpp>
#include "log.h"
#include <cstring>
#endif
#include <gtest/gtest.h>

#if defined(ENABLE_CPPTRACE)
void handler(int sig)
{
    mavsdk::LogErr() << "Got signal: " << strsignal(sig) << " (" << sig << ")";
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
