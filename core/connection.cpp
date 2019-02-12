#include "connection.h"

#include "connection_impl.h"
#include "global_include.h"


namespace dronecode_sdk {
 
Connection::Connection() : _impl{new ConnectionImpl()} {}

Connection::~Connection() {}

Connection::Result Connection::add_any_connection(const std::string &connection_url)
{
  return _impl->add_any_connection(connection_url);
}

Connection::Result Connection::add_udp_connection(int local_port)
{
  return Connection::add_udp_connection(DEFAULT_UDP_BIND_IP, local_port);
}

Connection::Result Connection::add_udp_connection(const std::string &local_bind_ip,
                                                  const int local_port)
{
  return _impl->add_udp_connection(local_bind_ip, local_port);
}

Connection::Result Connection::add_tcp_connection(const std::string &remote_ip,
                                                  const int remote_port)
{
  return _impl->add_tcp_connection(remote_ip, remote_port);
}

Connection::Result Connection::add_serial_connection(const std::string &dev_path,
                                                     const int baudrate)
{
  return _impl->add_serial_connection(dev_path, baudrate);
}

inline const char *Connection::connection_result_str(const ConnectionResult result)
{
  return _impl->connection_result_str(result);
}
  

} // namespace dronecode_sdk
