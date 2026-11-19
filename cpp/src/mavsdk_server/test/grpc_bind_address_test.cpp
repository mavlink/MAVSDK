#include <chrono>
#include <string>

#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>
#include <grpcpp/support/channel_arguments.h>
#include <gtest/gtest.h>

#include "mavsdk_server.hpp"

namespace {

bool can_connect(const std::string& target)
{
    grpc::ChannelArguments args;
    // Talk to the server directly even if a proxy is configured.
    args.SetInt(GRPC_ARG_ENABLE_HTTP_PROXY, 0);
    auto channel = grpc::CreateCustomChannel(target, grpc::InsecureChannelCredentials(), args);
    return channel->WaitForConnected(std::chrono::system_clock::now() + std::chrono::seconds(1));
}

class GrpcBindAddressTest : public ::testing::Test {
protected:
    void TearDown() override { _server.stop(); }

    int run(const std::string& bind_address)
    {
        _server.setGrpcBindAddress(bind_address);
        return _server.startGrpcServer(0);
    }

    MavsdkServer _server;
};

TEST_F(GrpcBindAddressTest, listensOnIpv4Loopback)
{
    const int port = run("127.0.0.1");
    ASSERT_GT(port, 0);

    EXPECT_TRUE(can_connect("127.0.0.1:" + std::to_string(port)));
}

#ifdef __linux__
// The whole 127.0.0.0/8 block is loopback on Linux, so a wildcard listener
// would also accept connections on 127.0.0.2.
TEST_F(GrpcBindAddressTest, ipv4LoopbackDoesNotListenOnOtherAddresses)
{
    const int port = run("127.0.0.1");
    ASSERT_GT(port, 0);

    EXPECT_FALSE(can_connect("127.0.0.2:" + std::to_string(port)));
}

TEST_F(GrpcBindAddressTest, defaultListensOnAllIpv4Addresses)
{
    const int port = run("0.0.0.0");
    ASSERT_GT(port, 0);

    EXPECT_TRUE(can_connect("127.0.0.2:" + std::to_string(port)));
}
#endif

TEST_F(GrpcBindAddressTest, listensOnIpv6Loopback)
{
    const int port = run("::1");
    if (port == 0) {
        GTEST_SKIP() << "IPv6 loopback not available";
    }

    EXPECT_TRUE(can_connect("[::1]:" + std::to_string(port)));
    EXPECT_FALSE(can_connect("127.0.0.1:" + std::to_string(port)));
}

TEST_F(GrpcBindAddressTest, acceptsBracketedIpv6Address)
{
    const int port = run("[::1]");
    if (port == 0) {
        GTEST_SKIP() << "IPv6 loopback not available";
    }

    EXPECT_TRUE(can_connect("[::1]:" + std::to_string(port)));
}

TEST_F(GrpcBindAddressTest, failsForAddressNotOnThisHost)
{
    // TEST-NET-1 (RFC 5737), never assigned to a local interface.
    EXPECT_EQ(run("192.0.2.1"), 0);
}

TEST_F(GrpcBindAddressTest, failsForInvalidAddress)
{
    EXPECT_EQ(run("not a valid address"), 0);
}

} // namespace
