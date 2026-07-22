#include "connection_result.hpp"
#include <gtest/gtest.h>
#include <sstream>
#include <string>

using namespace mavsdk;

TEST(ConnectionResult, ToStringKnownValues)
{
    EXPECT_EQ(to_string(ConnectionResult::Success), "Success");
    EXPECT_EQ(to_string(ConnectionResult::Timeout), "Timeout");
    EXPECT_EQ(to_string(ConnectionResult::SocketError), "Socket error");
    EXPECT_EQ(to_string(ConnectionResult::BindError), "Bind error");
    EXPECT_EQ(to_string(ConnectionResult::SocketConnectionError), "Socket connection error");
    EXPECT_EQ(to_string(ConnectionResult::ConnectionError), "Connection error");
    EXPECT_EQ(to_string(ConnectionResult::NotImplemented), "Not implemented");
    EXPECT_EQ(to_string(ConnectionResult::SystemNotConnected), "System not connected");
    EXPECT_EQ(to_string(ConnectionResult::SystemBusy), "System busy");
    EXPECT_EQ(to_string(ConnectionResult::CommandDenied), "Command denied");
    EXPECT_EQ(to_string(ConnectionResult::DestinationIpUnknown), "Destination IP unknown");
    EXPECT_EQ(to_string(ConnectionResult::ConnectionsExhausted), "Connections exhausted");
    EXPECT_EQ(to_string(ConnectionResult::ConnectionUrlInvalid), "Invalid connection URL");
    EXPECT_EQ(to_string(ConnectionResult::BaudrateUnknown), "Baudrate unknown");
}

TEST(ConnectionResult, StreamOperator)
{
    std::ostringstream oss;
    oss << ConnectionResult::Timeout;
    EXPECT_EQ(oss.str(), "Timeout");
}

TEST(ConnectionResult, UnknownFallback)
{
    auto bogus = static_cast<ConnectionResult>(250);
    EXPECT_EQ(to_string(bogus), "Unknown");
}
