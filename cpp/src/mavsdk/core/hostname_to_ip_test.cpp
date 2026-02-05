#include "hostname_to_ip.h"
#include <gtest/gtest.h>

using namespace mavsdk;

TEST(HostnameToIp, Localhost)
{
    auto host = "localhost";
    auto ip = resolve_hostname_to_ip(host);
    ASSERT_TRUE(ip);
    EXPECT_EQ(ip.value(), "127.0.0.1");
}

TEST(HostnameToIp, Ip)
{
    auto host = "127.0.0.1";
    auto ip = resolve_hostname_to_ip(host);
    ASSERT_TRUE(ip);
    EXPECT_EQ(ip.value(), host);
}

TEST(HostnameToIp, Empty)
{
    auto host = "";
    auto ip = resolve_hostname_to_ip(host);
    EXPECT_FALSE(ip);
}

TEST(HostnameToIp, Something)
{
    auto host = "something";
    auto ip = resolve_hostname_to_ip(host);
    EXPECT_FALSE(ip);
}
