#include <string>
#include "cli_arg.h"
#include <gtest/gtest.h>

using namespace mavsdk;

TEST(CliArg, NothingInitially)
{
    CliArg ca;
    EXPECT_TRUE(std::get_if<std::monostate>(&ca.protocol));
}

TEST(CliArg, UdpLegacyNoPort)
{
    CliArg ca;
    EXPECT_FALSE(ca.parse("udp://127.0.0.1"));
    EXPECT_TRUE(std::get_if<std::monostate>(&ca.protocol));
}

TEST(CliArg, UdpLegacyNoIp)
{
    CliArg ca;
    EXPECT_FALSE(ca.parse("udp://555"));
    EXPECT_TRUE(std::get_if<std::monostate>(&ca.protocol));
}

TEST(CliArg, UdpLegacyOut)
{
    CliArg ca;
    EXPECT_TRUE(ca.parse("udp://127.0.0.1:555"));
    auto udp = std::get_if<CliArg::Udp>(&ca.protocol);
    ASSERT_TRUE(udp);
    EXPECT_STREQ(udp->host.c_str(), "127.0.0.1");
    EXPECT_EQ(udp->port, 555);
    EXPECT_EQ(udp->mode, CliArg::Udp::Mode::Out);
}

TEST(CliArg, UdpLegacyIn)
{
    CliArg ca;
    EXPECT_TRUE(ca.parse("udp://:777"));
    auto udp = std::get_if<CliArg::Udp>(&ca.protocol);
    ASSERT_TRUE(udp);
    EXPECT_STREQ(udp->host.c_str(), "0.0.0.0");
    EXPECT_EQ(udp->port, 777);
    EXPECT_EQ(udp->mode, CliArg::Udp::Mode::In);
}

TEST(CliArg, UdpLegacyInAllInterfaces)
{
    CliArg ca;
    EXPECT_TRUE(ca.parse("udp://0.0.0.0:888"));
    auto udp = std::get_if<CliArg::Udp>(&ca.protocol);
    ASSERT_TRUE(udp);
    EXPECT_STREQ(udp->host.c_str(), "0.0.0.0");
    EXPECT_EQ(udp->port, 888);
    EXPECT_EQ(udp->mode, CliArg::Udp::Mode::In);
}

TEST(CliArg, UdpIp)
{
    CliArg ca;

    EXPECT_TRUE(ca.parse("udp://0.0.0.0:7"));
    auto udp = std::get_if<CliArg::Udp>(&ca.protocol);
    ASSERT_TRUE(udp);
    EXPECT_STREQ(udp->host.c_str(), "0.0.0.0");
    EXPECT_EQ(7, udp->port);
    EXPECT_EQ(udp->mode, CliArg::Udp::Mode::In);
}

TEST(CliArg, UdpLocalhost)
{
    CliArg ca;
    EXPECT_TRUE(ca.parse("udp://localhost:99"));
    auto udp = std::get_if<CliArg::Udp>(&ca.protocol);
    ASSERT_TRUE(udp);
    EXPECT_STREQ(udp->host.c_str(), "localhost");
    EXPECT_EQ(99, udp->port);
    EXPECT_EQ(udp->mode, CliArg::Udp::Mode::Out);
}

TEST(CliArg, UdpDomain)
{
    CliArg ca;
    EXPECT_TRUE(ca.parse("udp://example.com:77"));
    auto udp = std::get_if<CliArg::Udp>(&ca.protocol);
    ASSERT_TRUE(udp);
    EXPECT_STREQ(udp->host.c_str(), "example.com");
    EXPECT_EQ(77, udp->port);
    EXPECT_EQ(udp->mode, CliArg::Udp::Mode::Out);
}

TEST(CliArg, UdpLocalDomain)
{
    CliArg ca;
    EXPECT_TRUE(ca.parse("udp://something.local:42"));
    auto udp = std::get_if<CliArg::Udp>(&ca.protocol);
    ASSERT_TRUE(udp);
    EXPECT_STREQ(udp->host.c_str(), "something.local");
    EXPECT_EQ(42, udp->port);
}

TEST(CliArg, UdpLegacyWrong)
{
    CliArg ca;
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

TEST(CliArg, UdpInAll)
{
    CliArg ca;

    EXPECT_TRUE(ca.parse("udpin://0.0.0.0:55"));
    auto udp = std::get_if<CliArg::Udp>(&ca.protocol);
    ASSERT_TRUE(udp);
    EXPECT_STREQ(udp->host.c_str(), "0.0.0.0");
    EXPECT_EQ(55, udp->port);
    EXPECT_EQ(udp->mode, CliArg::Udp::Mode::In);
}

TEST(CliArg, UdpInSpecific)
{
    CliArg ca;

    EXPECT_TRUE(ca.parse("udpin://192.168.0.5:66"));
    auto udp = std::get_if<CliArg::Udp>(&ca.protocol);
    ASSERT_TRUE(udp);
    EXPECT_STREQ(udp->host.c_str(), "192.168.0.5");
    EXPECT_EQ(66, udp->port);
    EXPECT_EQ(udp->mode, CliArg::Udp::Mode::In);
}

TEST(CliArg, UdpOutInvalid)
{
    CliArg ca;

    EXPECT_FALSE(ca.parse("udpout://0.0.0.0:55"));
}

TEST(CliArg, UdpOutpecific)
{
    CliArg ca;

    EXPECT_TRUE(ca.parse("udpout://192.168.0.5:66"));
    auto udp = std::get_if<CliArg::Udp>(&ca.protocol);
    ASSERT_TRUE(udp);
    EXPECT_STREQ(udp->host.c_str(), "192.168.0.5");
    EXPECT_EQ(66, udp->port);
    EXPECT_EQ(udp->mode, CliArg::Udp::Mode::Out);
}

TEST(CliArg, TcpLegacyNoPort)
{
    CliArg ca;

    EXPECT_FALSE(ca.parse("tcp://127.0.0.1"));
    EXPECT_TRUE(std::get_if<std::monostate>(&ca.protocol));
}

TEST(CliArg, TcpLegacyNoHostNoPort)
{
    CliArg ca;

    EXPECT_FALSE(ca.parse("tcp://"));
    EXPECT_TRUE(std::get_if<std::monostate>(&ca.protocol));
}

TEST(CliArg, TcpLegacyInAllHost)
{
    CliArg ca;

    EXPECT_TRUE(ca.parse("tcp://:888"));

    auto tcp = std::get_if<CliArg::Tcp>(&ca.protocol);
    ASSERT_TRUE(tcp);

    EXPECT_STREQ(tcp->host.c_str(), "0.0.0.0");
    EXPECT_EQ(tcp->port, 888);
    EXPECT_EQ(tcp->mode, CliArg::Tcp::Mode::In);
}

TEST(CliArg, TcpLegacyOutIp)
{
    CliArg ca;

    EXPECT_TRUE(ca.parse("tcp://127.0.0.1:999"));

    auto tcp = std::get_if<CliArg::Tcp>(&ca.protocol);
    ASSERT_TRUE(tcp);

    EXPECT_STREQ(tcp->host.c_str(), "127.0.0.1");
    EXPECT_EQ(tcp->port, 999);
    EXPECT_EQ(tcp->mode, CliArg::Tcp::Mode::Out);
}

TEST(CliArg, TcpLegacyOutLocalhost)
{
    CliArg ca;

    EXPECT_TRUE(ca.parse("tcp://localhost:444"));

    auto tcp = std::get_if<CliArg::Tcp>(&ca.protocol);
    ASSERT_TRUE(tcp);

    EXPECT_STREQ(tcp->host.c_str(), "localhost");
    EXPECT_EQ(tcp->port, 444);
    EXPECT_EQ(tcp->mode, CliArg::Tcp::Mode::Out);
}

TEST(CliArg, TcpLegacyOutDomain)
{
    CliArg ca;

    EXPECT_TRUE(ca.parse("tcp://example.com:1234"));

    auto tcp = std::get_if<CliArg::Tcp>(&ca.protocol);
    ASSERT_TRUE(tcp);

    EXPECT_STREQ(tcp->host.c_str(), "example.com");
    EXPECT_EQ(tcp->port, 1234);
    EXPECT_EQ(tcp->mode, CliArg::Tcp::Mode::Out);
}

TEST(CliArg, TcpLegacyOutLocal)
{
    CliArg ca;

    EXPECT_TRUE(ca.parse("tcp://something.local:42"));

    auto tcp = std::get_if<CliArg::Tcp>(&ca.protocol);
    ASSERT_TRUE(tcp);

    EXPECT_STREQ(tcp->host.c_str(), "something.local");
    EXPECT_EQ(tcp->port, 42);
    EXPECT_EQ(tcp->mode, CliArg::Tcp::Mode::Out);
}

TEST(CliArg, TcpLegacyWrong)
{
    CliArg ca;

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

TEST(CliArg, TcpInAll)
{
    CliArg ca;

    EXPECT_TRUE(ca.parse("tcpin://0.0.0.0:55"));
    auto tcp = std::get_if<CliArg::Tcp>(&ca.protocol);
    ASSERT_TRUE(tcp);
    EXPECT_STREQ(tcp->host.c_str(), "0.0.0.0");
    EXPECT_EQ(55, tcp->port);
    EXPECT_EQ(tcp->mode, CliArg::Tcp::Mode::In);
}

TEST(CliArg, TcpInSpecific)
{
    CliArg ca;

    EXPECT_TRUE(ca.parse("tcpin://192.168.0.5:66"));
    auto tcp = std::get_if<CliArg::Tcp>(&ca.protocol);
    ASSERT_TRUE(tcp);
    EXPECT_STREQ(tcp->host.c_str(), "192.168.0.5");
    EXPECT_EQ(66, tcp->port);
    EXPECT_EQ(tcp->mode, CliArg::Tcp::Mode::In);
}

TEST(CliArg, TcpOutInvalid)
{
    CliArg ca;

    EXPECT_FALSE(ca.parse("tcpout://0.0.0.0:55"));
}

TEST(CliArg, TcpOutpecific)
{
    CliArg ca;

    EXPECT_TRUE(ca.parse("tcpout://192.168.0.5:66"));
    auto tcp = std::get_if<CliArg::Tcp>(&ca.protocol);
    ASSERT_TRUE(tcp);
    EXPECT_STREQ(tcp->host.c_str(), "192.168.0.5");
    EXPECT_EQ(66, tcp->port);
    EXPECT_EQ(tcp->mode, CliArg::Tcp::Mode::Out);
}

TEST(CliArg, SerialPort)
{
    CliArg ca;

    EXPECT_TRUE(ca.parse("serial:///dev/ttyS0:115200"));
    auto serial = std::get_if<CliArg::Serial>(&ca.protocol);
    ASSERT_TRUE(serial);
    EXPECT_STREQ(serial->path.c_str(), "/dev/ttyS0");
    EXPECT_EQ(serial->baudrate, 115200);
    EXPECT_EQ(serial->flow_control_enabled, false);
}

TEST(CliArg, SerialName)
{
    CliArg ca;

    EXPECT_TRUE(ca.parse("serial:///dev/tty.usbmodem1:115200"));
    auto serial = std::get_if<CliArg::Serial>(&ca.protocol);
    ASSERT_TRUE(serial);
    EXPECT_STREQ(serial->path.c_str(), "/dev/tty.usbmodem1");
    EXPECT_EQ(serial->baudrate, 115200);
    EXPECT_EQ(serial->flow_control_enabled, false);
}

TEST(CliArg, SerialWithFlowControl)
{
    CliArg ca;

    EXPECT_TRUE(ca.parse("serial_flowcontrol:///dev/ttyS0:4000000"));
    auto serial = std::get_if<CliArg::Serial>(&ca.protocol);
    ASSERT_TRUE(serial);
    EXPECT_STREQ(serial->path.c_str(), "/dev/ttyS0");
    EXPECT_EQ(serial->baudrate, 4000000);
    EXPECT_EQ(serial->flow_control_enabled, true);
}

TEST(CliArg, SerialWindowsComport)
{
    CliArg ca;

    EXPECT_TRUE(ca.parse("serial://COM13:57600"));
    auto serial = std::get_if<CliArg::Serial>(&ca.protocol);
    ASSERT_TRUE(serial);
    EXPECT_STREQ(serial->path.c_str(), "COM13");
    EXPECT_EQ(serial->baudrate, 57600);
    EXPECT_EQ(serial->flow_control_enabled, false);
}

TEST(CliArg, SerialWrong)
{
    CliArg ca;

    // All the wrong combinations.
    EXPECT_FALSE(ca.parse(""));
    EXPECT_FALSE(ca.parse("serial:///dev/ttyS0"));
    EXPECT_FALSE(ca.parse("serial///dev/ttyS0:57600"));
    EXPECT_FALSE(ca.parse("/serial:///dev/ttyS0:57600"));
    EXPECT_FALSE(ca.parse("serial:/dev/ttyS0:57600"));
    EXPECT_FALSE(ca.parse("serial://dev/ttyS0:57600"));
    EXPECT_FALSE(ca.parse("serial://"));
    EXPECT_FALSE(ca.parse("serial://COM:57600"));
    EXPECT_FALSE(ca.parse("serial://COM:57600"));
    EXPECT_FALSE(ca.parse("seri://COM3:57600"));
    EXPECT_FALSE(ca.parse("seri://COM3:57600"));
    EXPECT_FALSE(ca.parse("serial://"));
    EXPECT_FALSE(ca.parse("serial://SOM3:57600"));
    EXPECT_FALSE(ca.parse("serial://SOM3:57600"));
    EXPECT_FALSE(ca.parse("serial://COM3:-1"));
    EXPECT_FALSE(ca.parse("serial://COM3"));
}
