#include <string>
#include "cli_arg.h"
#include <gtest/gtest.h>

using namespace mavsdk;

TEST(CliArg, UDPConnections)
{
    CliArg ca;
    EXPECT_EQ(ca.get_protocol(), CliArg::Protocol::None);

    ca.parse("udp://127.0.0.1");
    EXPECT_EQ(ca.get_protocol(), CliArg::Protocol::Udp);
    EXPECT_STREQ(ca.get_path().c_str(), "127.0.0.1");
    EXPECT_EQ(0, ca.get_port());

    ca.parse("udp://");
    EXPECT_EQ(ca.get_protocol(), CliArg::Protocol::Udp);
    EXPECT_STREQ(ca.get_path().c_str(), "");
    EXPECT_EQ(0, ca.get_port());

    // Not a valid hostname
    EXPECT_FALSE(ca.parse("udp://555"));

    ca.parse("udp://:777");
    EXPECT_EQ(ca.get_protocol(), CliArg::Protocol::Udp);
    EXPECT_STREQ(ca.get_path().c_str(), "");
    EXPECT_EQ(777, ca.get_port());

    ca.parse("udp://0.0.0.0");
    EXPECT_EQ(ca.get_protocol(), CliArg::Protocol::Udp);
    EXPECT_STREQ(ca.get_path().c_str(), "0.0.0.0");
    EXPECT_EQ(0, ca.get_port());

    ca.parse("udp://0.0.0.0:7");
    EXPECT_EQ(ca.get_protocol(), CliArg::Protocol::Udp);
    EXPECT_STREQ(ca.get_path().c_str(), "0.0.0.0");
    EXPECT_EQ(7, ca.get_port());

    ca.parse("udp://localhost:99");
    EXPECT_EQ(ca.get_protocol(), CliArg::Protocol::Udp);
    EXPECT_STREQ(ca.get_path().c_str(), "localhost");
    EXPECT_EQ(99, ca.get_port());

    ca.parse("udp://example.com");
    EXPECT_EQ(ca.get_protocol(), CliArg::Protocol::Udp);
    EXPECT_STREQ(ca.get_path().c_str(), "example.com");
    EXPECT_EQ(0, ca.get_port());

    ca.parse("udp://something.local:42");
    EXPECT_EQ(ca.get_protocol(), CliArg::Protocol::Udp);
    EXPECT_STREQ(ca.get_path().c_str(), "something.local");
    EXPECT_EQ(42, ca.get_port());

    // All the wrong combinations.
    EXPECT_FALSE(ca.parse(""));
    EXPECT_FALSE(ca.parse("udp:/localhost:99"));
    EXPECT_FALSE(ca.parse("/udp://localhost:99"));
    EXPECT_FALSE(ca.parse("udp:/localhost:99"));
    EXPECT_FALSE(ca.parse("udp:localhost:99"));
    EXPECT_FALSE(ca.parse("udp//localhost:99"));
    EXPECT_FALSE(ca.parse("ud://localhost:99"));
    EXPECT_FALSE(ca.parse("udp://0.0.0.0:100000")); // highest is 65535
    EXPECT_FALSE(ca.parse("udp://0.0.0.0:-5"));
}

TEST(CliArg, TCPConnections)
{
    CliArg ca;
    EXPECT_EQ(ca.get_protocol(), CliArg::Protocol::None);

    EXPECT_TRUE(ca.parse("tcp://127.0.0.1"));
    EXPECT_EQ(ca.get_protocol(), CliArg::Protocol::Tcp);
    EXPECT_STREQ(ca.get_path().c_str(), "127.0.0.1");
    EXPECT_EQ(0, ca.get_port());

    ca.parse("tcp://");
    EXPECT_EQ(ca.get_protocol(), CliArg::Protocol::Tcp);
    EXPECT_STREQ(ca.get_path().c_str(), "");
    EXPECT_EQ(0, ca.get_port());

    ca.parse("tcp://:8");
    EXPECT_EQ(ca.get_protocol(), CliArg::Protocol::Tcp);
    EXPECT_STREQ(ca.get_path().c_str(), "");
    EXPECT_EQ(8, ca.get_port());

    EXPECT_TRUE(ca.parse("tcp://127.0.0.1:7"));
    EXPECT_EQ(ca.get_protocol(), CliArg::Protocol::Tcp);
    EXPECT_STREQ(ca.get_path().c_str(), "127.0.0.1");
    EXPECT_EQ(7, ca.get_port());

    EXPECT_TRUE(ca.parse("tcp://localhost:99"));
    EXPECT_EQ(ca.get_protocol(), CliArg::Protocol::Tcp);
    EXPECT_STREQ(ca.get_path().c_str(), "localhost");
    EXPECT_EQ(99, ca.get_port());

    EXPECT_TRUE(ca.parse("tcp://example.com:1234"));
    EXPECT_EQ(ca.get_protocol(), CliArg::Protocol::Tcp);
    EXPECT_STREQ(ca.get_path().c_str(), "example.com");
    EXPECT_EQ(1234, ca.get_port());

    EXPECT_TRUE(ca.parse("tcp://something.local:42"));
    EXPECT_EQ(ca.get_protocol(), CliArg::Protocol::Tcp);
    EXPECT_STREQ(ca.get_path().c_str(), "something.local");
    EXPECT_EQ(42, ca.get_port());

    // All the wrong combinations.
    EXPECT_FALSE(ca.parse(""));
    EXPECT_FALSE(ca.parse("tcp:/localhost:99"));
    EXPECT_FALSE(ca.parse("tcp:/localhost:99"));
    EXPECT_FALSE(ca.parse("/tcp://localhost:99"));
    EXPECT_FALSE(ca.parse("tcp:localhost:99"));
    EXPECT_FALSE(ca.parse("tcp//localhost:99"));
    EXPECT_FALSE(ca.parse("tc://localhost:99"));
    EXPECT_FALSE(ca.parse("tcp://127.0.0.1:100000")); // highest is 65535
    EXPECT_FALSE(ca.parse("tcp://127.0.0.1:-5"));
}

TEST(CliArg, SerialConnections)
{
    CliArg ca;
    EXPECT_EQ(ca.get_protocol(), CliArg::Protocol::None);

    EXPECT_TRUE(ca.parse("serial:///dev/ttyS0"));
    EXPECT_EQ(ca.get_protocol(), CliArg::Protocol::Serial);
    EXPECT_STREQ(ca.get_path().c_str(), "/dev/ttyS0");
    EXPECT_EQ(0, ca.get_baudrate());
    EXPECT_EQ(false, ca.get_flow_control());

    EXPECT_TRUE(ca.parse("serial_flowcontrol:///dev/ttyS0:4000000"));
    EXPECT_EQ(ca.get_protocol(), CliArg::Protocol::Serial);
    EXPECT_STREQ(ca.get_path().c_str(), "/dev/ttyS0");
    EXPECT_EQ(4000000, ca.get_baudrate());
    EXPECT_EQ(true, ca.get_flow_control());

    EXPECT_TRUE(ca.parse("serial://COM13:57600"));
    EXPECT_EQ(ca.get_protocol(), CliArg::Protocol::Serial);
    EXPECT_STREQ(ca.get_path().c_str(), "COM13");
    EXPECT_EQ(57600, ca.get_baudrate());
    EXPECT_EQ(false, ca.get_flow_control());

    EXPECT_TRUE(ca.parse("serial:///dev/tty.usbmodem1:115200"));
    EXPECT_EQ(ca.get_protocol(), CliArg::Protocol::Serial);
    EXPECT_STREQ(ca.get_path().c_str(), "/dev/tty.usbmodem1");
    EXPECT_EQ(115200, ca.get_baudrate());
    EXPECT_EQ(false, ca.get_flow_control());

    EXPECT_TRUE(ca.parse("serial://COM3"));
    EXPECT_EQ(ca.get_protocol(), CliArg::Protocol::Serial);
    EXPECT_STREQ(ca.get_path().c_str(), "COM3");
    EXPECT_EQ(0, ca.get_baudrate());
    EXPECT_EQ(false, ca.get_flow_control());

    // All the wrong combinations.
    EXPECT_FALSE(ca.parse(""));
    EXPECT_FALSE(ca.parse("serial///dev/ttyS0"));
    EXPECT_FALSE(ca.parse("/serial:///dev/ttyS0"));
    EXPECT_FALSE(ca.parse("serial:/dev/ttyS0"));
    EXPECT_FALSE(ca.parse("serial://dev/ttyS0"));
    EXPECT_FALSE(ca.parse("serial://"));
    EXPECT_FALSE(ca.parse("serial://COM"));
    EXPECT_FALSE(ca.parse("serial://COM:57600"));
    EXPECT_FALSE(ca.parse("seri://COM3"));
    EXPECT_FALSE(ca.parse("seri://COM3:57600"));
    EXPECT_FALSE(ca.parse("serial://"));
    EXPECT_FALSE(ca.parse("serial://SOM3"));
    EXPECT_FALSE(ca.parse("serial://SOM3:57600"));
    EXPECT_FALSE(ca.parse("serial://COM3:-1"));
}
