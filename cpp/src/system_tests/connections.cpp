#include "log.h"
#include "mavsdk.h"
#include "plugins/mavlink_direct/mavlink_direct.h"
#include <chrono>
#include <thread>
#include <atomic>
#include <gtest/gtest.h>

using namespace mavsdk;

TEST(SystemTest, TcpConnectionReconnectionFromServerSide)
{
    // Test that TCP client properly reconnects when server closes the connection

    const std::string tcp_port = "17400";
    const std::string server_url = "tcpin://0.0.0.0:" + tcp_port;
    const std::string client_url = "tcpout://127.0.0.1:" + tcp_port;

    LogInfo() << "=== Phase 1: Start server and client ===";

    // Start server
    Mavsdk server_mavsdk{Mavsdk::Configuration{ComponentType::Autopilot}};
    auto [server_result, server_handle] = server_mavsdk.add_any_connection_with_handle(server_url);
    ASSERT_EQ(server_result, ConnectionResult::Success);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Start client
    Mavsdk client_mavsdk{Mavsdk::Configuration{ComponentType::GroundStation}};
    auto [client_result, client_handle] = client_mavsdk.add_any_connection_with_handle(client_url);
    ASSERT_EQ(client_result, ConnectionResult::Success);

    // Wait for discovery
    LogInfo() << "Waiting for client to discover server...";
    auto maybe_system = client_mavsdk.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system) << "Client failed to discover server";
    auto system = maybe_system.value();

    LogInfo() << "=== Phase 2: Verify initial connectivity ===";

    // Track automatic heartbeats (MAVSDK sends these every second automatically)
    std::atomic<int> message_count{0};
    MavlinkDirect client_mavlink{system};

    auto handle = client_mavlink.subscribe_message(
        "HEARTBEAT", [&message_count](MavlinkDirect::MavlinkMessage) {
            message_count++;
            LogInfo() << "Client received HEARTBEAT (total: " << message_count.load() << ")";
        });

    // Wait for automatic heartbeats to arrive (sent every 1 second by MAVSDK)
    LogInfo() << "Waiting for automatic heartbeats...";
    std::this_thread::sleep_for(std::chrono::seconds(2));
    int messages_before = message_count.load();
    EXPECT_GE(messages_before, 1) << "Client didn't receive automatic heartbeats";
    LogInfo() << "Received " << messages_before << " automatic heartbeat(s) from server";

    LogInfo() << "=== Phase 3: Remove server connection (simulates server closing connection) ===";

    // Remove the server connection - this closes the socket
    server_mavsdk.remove_connection(server_handle);

    // Give client time to detect disconnection
    LogInfo() << "Waiting for client to detect disconnection...";
    LogInfo() << "Expected log: 'TCP connection closed, trying to reconnect...'";
    std::this_thread::sleep_for(std::chrono::seconds(3));

    LogInfo() << "=== Phase 4: Re-add server connection (client should reconnect) ===";

    // Re-add the server connection on same port
    auto [server_result2, server_handle2] =
        server_mavsdk.add_any_connection_with_handle(server_url);
    ASSERT_EQ(server_result2, ConnectionResult::Success);

    LogInfo() << "=== Phase 5: Verify message flow after reconnection ===";

    int messages_before_reconnect = message_count.load();
    LogInfo() << "Messages before waiting: " << messages_before_reconnect;

    // Wait for automatic heartbeats to resume after reconnection
    LogInfo() << "Waiting for automatic heartbeats to resume...";
    std::this_thread::sleep_for(std::chrono::seconds(2));
    int messages_after_reconnect = message_count.load();

    LogInfo() << "Messages after waiting: " << messages_after_reconnect;

    EXPECT_GT(messages_after_reconnect, messages_before_reconnect)
        << "Client did NOT receive automatic heartbeats after reconnection!";

    LogInfo() << "=== SUCCESS: TCP client reconnection verified! ===";
    LogInfo() << "Total messages received: " << message_count.load();

    // Cleanup
    client_mavlink.unsubscribe_message(handle);
    client_mavsdk.remove_connection(client_handle);
    server_mavsdk.remove_connection(server_handle2);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

TEST(SystemTest, TcpConnectionReconnectionFromClientSide)
{
    // Test that TCP client properly reconnects when it closes and reopens its own connection

    const std::string tcp_port = "17500";
    const std::string server_url = "tcpin://0.0.0.0:" + tcp_port;
    const std::string client_url = "tcpout://127.0.0.1:" + tcp_port;

    LogInfo() << "=== Phase 1: Start server ===";

    // Start server
    Mavsdk server_mavsdk{Mavsdk::Configuration{ComponentType::Autopilot}};
    auto [server_result, server_handle] = server_mavsdk.add_any_connection_with_handle(server_url);
    ASSERT_EQ(server_result, ConnectionResult::Success);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    LogInfo() << "=== Phase 2: Start client and verify initial connectivity ===";

    // Start client
    Mavsdk client_mavsdk{Mavsdk::Configuration{ComponentType::GroundStation}};
    auto [client_result, client_handle] = client_mavsdk.add_any_connection_with_handle(client_url);
    ASSERT_EQ(client_result, ConnectionResult::Success);

    // Wait for discovery
    LogInfo() << "Waiting for client to discover server...";
    auto maybe_system = client_mavsdk.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system) << "Client failed to discover server";
    auto system = maybe_system.value();

    // Track automatic heartbeats (MAVSDK sends these every second automatically)
    std::atomic<int> message_count{0};
    MavlinkDirect client_mavlink{system};

    auto handle = client_mavlink.subscribe_message(
        "HEARTBEAT", [&message_count](MavlinkDirect::MavlinkMessage) {
            message_count++;
            LogInfo() << "Client received HEARTBEAT (total: " << message_count.load() << ")";
        });

    // Wait for automatic heartbeats to arrive (sent every 1 second by MAVSDK)
    LogInfo() << "Waiting for automatic heartbeats...";
    std::this_thread::sleep_for(std::chrono::seconds(2));
    int messages_before = message_count.load();
    EXPECT_GE(messages_before, 1) << "Client didn't receive automatic heartbeats";
    LogInfo() << "Received " << messages_before << " automatic heartbeat(s) from server";

    LogInfo() << "=== Phase 3: Remove client connection (client closes its connection) ===";

    // Unsubscribe before removing connection to avoid callback issues
    client_mavlink.unsubscribe_message(handle);

    // Remove the client connection - this closes the socket on the client side
    client_mavsdk.remove_connection(client_handle);

    // Give time for disconnection to be processed
    LogInfo() << "Waiting after client disconnection...";
    std::this_thread::sleep_for(std::chrono::seconds(2));

    LogInfo() << "=== Phase 4: Re-add client connection (client reconnects) ===";

    // Re-add the client connection
    auto [client_result2, client_handle2] =
        client_mavsdk.add_any_connection_with_handle(client_url);
    ASSERT_EQ(client_result2, ConnectionResult::Success);

    LogInfo() << "=== Phase 5: Verify message flow after reconnection ===";

    // Re-subscribe using the original system (system objects persist)
    std::atomic<int> message_count2{0};

    auto handle2 = client_mavlink.subscribe_message(
        "HEARTBEAT", [&message_count2](MavlinkDirect::MavlinkMessage) {
            message_count2++;
            LogInfo() << "Client received HEARTBEAT after reconnection (total: "
                      << message_count2.load() << ")";
        });

    // Wait for automatic heartbeats to arrive after reconnection
    LogInfo() << "Waiting for automatic heartbeats after reconnection...";
    std::this_thread::sleep_for(std::chrono::seconds(3));
    int messages_after = message_count2.load();

    LogInfo() << "Messages after reconnection: " << messages_after;

    EXPECT_GE(messages_after, 1)
        << "Client did NOT receive automatic heartbeats after reconnection!";

    LogInfo() << "=== SUCCESS: TCP client-side reconnection verified! ===";
    LogInfo() << "Messages before disconnect: " << messages_before;
    LogInfo() << "Messages after reconnect: " << messages_after;

    // Cleanup
    client_mavlink.unsubscribe_message(handle2);
    client_mavsdk.remove_connection(client_handle2);
    server_mavsdk.remove_connection(server_handle);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

TEST(SystemTest, UdpConnectionReconnectionFromClientSide)
{
    // Test that UDP client (udpout) can be removed and re-added properly
    // UDP is connectionless, so this tests the connection management APIs

    const std::string udp_port = "17600";
    const std::string server_url = "udpin://0.0.0.0:" + udp_port;
    const std::string client_url = "udpout://127.0.0.1:" + udp_port;

    LogInfo() << "=== Phase 1: Start server and client ===";

    // Start server
    Mavsdk server_mavsdk{Mavsdk::Configuration{ComponentType::Autopilot}};
    auto [server_result, server_handle] = server_mavsdk.add_any_connection_with_handle(server_url);
    ASSERT_EQ(server_result, ConnectionResult::Success);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Start client
    Mavsdk client_mavsdk{Mavsdk::Configuration{ComponentType::GroundStation}};
    auto [client_result, client_handle] = client_mavsdk.add_any_connection_with_handle(client_url);
    ASSERT_EQ(client_result, ConnectionResult::Success);

    // Wait for discovery
    LogInfo() << "Waiting for client to discover server...";
    auto maybe_system = client_mavsdk.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system) << "Client failed to discover server";
    auto system = maybe_system.value();

    LogInfo() << "=== Phase 2: Verify initial connectivity ===";

    // Track automatic heartbeats
    std::atomic<int> message_count{0};
    MavlinkDirect client_mavlink{system};

    auto handle = client_mavlink.subscribe_message(
        "HEARTBEAT", [&message_count](MavlinkDirect::MavlinkMessage) {
            message_count++;
            LogInfo() << "Client received HEARTBEAT (total: " << message_count.load() << ")";
        });

    // Wait for automatic heartbeats to arrive
    LogInfo() << "Waiting for automatic heartbeats...";
    std::this_thread::sleep_for(std::chrono::seconds(2));
    int messages_before = message_count.load();
    EXPECT_GE(messages_before, 1) << "Client didn't receive automatic heartbeats";
    LogInfo() << "Received " << messages_before << " automatic heartbeat(s) from server";

    LogInfo() << "=== Phase 3: Remove and re-add client connection ===";

    // Unsubscribe before removing connection
    client_mavlink.unsubscribe_message(handle);

    // Remove the client connection
    client_mavsdk.remove_connection(client_handle);

    // Give time for disconnection
    LogInfo() << "Waiting after client disconnection...";
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // Re-add the client connection
    auto [client_result2, client_handle2] =
        client_mavsdk.add_any_connection_with_handle(client_url);
    ASSERT_EQ(client_result2, ConnectionResult::Success);

    LogInfo() << "=== Phase 4: Verify message flow after reconnection ===";

    // Re-subscribe using the original system (system objects persist)
    std::atomic<int> message_count2{0};

    auto handle2 = client_mavlink.subscribe_message(
        "HEARTBEAT", [&message_count2](MavlinkDirect::MavlinkMessage) {
            message_count2++;
            LogInfo() << "Client received HEARTBEAT after reconnection (total: "
                      << message_count2.load() << ")";
        });

    // Wait for automatic heartbeats to arrive after reconnection
    LogInfo() << "Waiting for automatic heartbeats after reconnection...";
    std::this_thread::sleep_for(std::chrono::seconds(3));
    int messages_after = message_count2.load();

    LogInfo() << "Messages after reconnection: " << messages_after;

    EXPECT_GE(messages_after, 1)
        << "Client did NOT receive automatic heartbeats after reconnection!";

    LogInfo() << "=== SUCCESS: UDP client-side reconnection verified! ===";
    LogInfo() << "Messages before disconnect: " << messages_before;
    LogInfo() << "Messages after reconnect: " << messages_after;

    // Cleanup
    client_mavlink.unsubscribe_message(handle2);
    client_mavsdk.remove_connection(client_handle2);
    server_mavsdk.remove_connection(server_handle);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

TEST(SystemTest, UdpConnectionReconnectionFromServerSide)
{
    // Test that UDP server (udpin) can be removed and re-added properly
    // UDP is connectionless, so this tests the connection management APIs

    const std::string udp_port = "17700";
    const std::string server_url = "udpin://0.0.0.0:" + udp_port;
    const std::string client_url = "udpout://127.0.0.1:" + udp_port;

    LogInfo() << "=== Phase 1: Start server and client ===";

    // Start server
    Mavsdk server_mavsdk{Mavsdk::Configuration{ComponentType::Autopilot}};
    auto [server_result, server_handle] = server_mavsdk.add_any_connection_with_handle(server_url);
    ASSERT_EQ(server_result, ConnectionResult::Success);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Start client
    Mavsdk client_mavsdk{Mavsdk::Configuration{ComponentType::GroundStation}};
    auto [client_result, client_handle] = client_mavsdk.add_any_connection_with_handle(client_url);
    ASSERT_EQ(client_result, ConnectionResult::Success);

    // Wait for discovery
    LogInfo() << "Waiting for client to discover server...";
    auto maybe_system = client_mavsdk.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system) << "Client failed to discover server";
    auto system = maybe_system.value();

    LogInfo() << "=== Phase 2: Verify initial connectivity ===";

    // Track automatic heartbeats
    std::atomic<int> message_count{0};
    MavlinkDirect client_mavlink{system};

    auto handle = client_mavlink.subscribe_message(
        "HEARTBEAT", [&message_count](MavlinkDirect::MavlinkMessage) {
            message_count++;
            LogInfo() << "Client received HEARTBEAT (total: " << message_count.load() << ")";
        });

    // Wait for automatic heartbeats to arrive
    LogInfo() << "Waiting for automatic heartbeats...";
    std::this_thread::sleep_for(std::chrono::seconds(2));
    int messages_before = message_count.load();
    EXPECT_GE(messages_before, 1) << "Client didn't receive automatic heartbeats";
    LogInfo() << "Received " << messages_before << " automatic heartbeat(s) from server";

    LogInfo() << "=== Phase 3: Remove and re-add server connection ===";

    // Unsubscribe before removing connection
    client_mavlink.unsubscribe_message(handle);

    // Remove the server connection
    server_mavsdk.remove_connection(server_handle);

    // Give time for disconnection
    LogInfo() << "Waiting after server disconnection...";
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // Re-add the server connection
    auto [server_result2, server_handle2] =
        server_mavsdk.add_any_connection_with_handle(server_url);
    ASSERT_EQ(server_result2, ConnectionResult::Success);

    LogInfo() << "=== Phase 4: Verify message flow after reconnection ===";

    // Re-subscribe using the original system (system objects persist)
    std::atomic<int> message_count2{0};

    auto handle2 = client_mavlink.subscribe_message(
        "HEARTBEAT", [&message_count2](MavlinkDirect::MavlinkMessage) {
            message_count2++;
            LogInfo() << "Client received HEARTBEAT after reconnection (total: "
                      << message_count2.load() << ")";
        });

    // Wait for automatic heartbeats to arrive after reconnection
    LogInfo() << "Waiting for automatic heartbeats after reconnection...";
    std::this_thread::sleep_for(std::chrono::seconds(3));
    int messages_after = message_count2.load();

    LogInfo() << "Messages after reconnection: " << messages_after;

    EXPECT_GE(messages_after, 1)
        << "Client did NOT receive automatic heartbeats after reconnection!";

    LogInfo() << "=== SUCCESS: UDP server-side reconnection verified! ===";
    LogInfo() << "Messages before disconnect: " << messages_before;
    LogInfo() << "Messages after reconnect: " << messages_after;

    // Cleanup
    client_mavlink.unsubscribe_message(handle2);
    client_mavsdk.remove_connection(client_handle);
    server_mavsdk.remove_connection(server_handle2);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}
