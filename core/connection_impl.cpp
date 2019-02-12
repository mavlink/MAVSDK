#include "connection_impl.h"

#include "connection.h"
#include "global_include.h"
#include "tcp_connection.h"
#include "udp_connection.h"
#include "system.h"
#include "system_impl.h"
#include "serial_connection.h"
#include "cli_arg.h"
#include "version.h"


namespace dronecode_sdk {

ConnectionImpl::ConnectionImpl() :
  _connections_mutex(),
  _connections() 
{ 
}

ConnectionImpl::~ConnectionImpl()
{  
  {
    std::lock_guard<std::mutex> lock(_connections_mutex);
    _connections.clear();
  }
}
      
void ConnectionImpl::receive_message(const mavlink_message_t &message)
{
    // Don't ever create a system with sysid 0.
    if (message.sysid == 0) {
        return;
    }

    // FIXME: Ignore messages from QGroundControl for now. Usually QGC identifies
    //        itself with sysid 255.
    //        A better way would probably be to parse the heartbeat message and
    //        look at type and check if it is MAV_TYPE_GCS.
    if (message.sysid == 255) {
        return;
    }

    std::lock_guard<std::recursive_mutex> lock(_systems_mutex);

    // Change system id of null system
    if (_systems.find(0) != _systems.end()) {
        auto null_system = _systems[0];
        _systems.erase(0);
        null_system->system_impl()->set_system_id(message.sysid);
        _systems.insert(system_entry_t(message.sysid, null_system));
    }

    if (!does_system_exist(message.sysid)) {
        make_system_with_component(message.sysid, message.compid);
    } else {
        _systems.at(message.sysid)->system_impl()->add_new_component(message.compid);
    }

    if (_should_exit) {
        // Don't try to call at() if systems have already been destroyed
        // in descructor.
        return;
    }

    if (_systems.find(message.sysid) != _systems.end()) {
        _systems.at(message.sysid)->system_impl()->process_mavlink_message(message);
    }
}

bool ConnectionImpl::send_message(const mavlink_message_t &message)
{
    std::lock_guard<std::mutex> lock(_connections_mutex);

    for (auto it = _connections.begin(); it != _connections.end(); ++it) {
        if (!(**it).send_message(message)) {
            LogErr() << "send fail";
            return false;
        }
    }

    return true;
}

Connection::Result ConnectionImpl::add_any_connection(const std::string &connection_url)
{
  CliArg cli_arg;
  if (!cli_arg.parse(connection_url)) {
    return Connection::Result::CONNECTION_URL_INVALID;
  }
  
  switch (cli_arg.get_protocol()) {
  case CliArg::Protocol::UDP: {
    std::string path = DronecodeSDK::DEFAULT_UDP_BIND_IP;
    int port = DronecodeSDK::DEFAULT_UDP_PORT;
    if (!cli_arg.get_path().empty()) {
      path = cli_arg.get_path();
            }
    if (cli_arg.get_port()) {
                port = cli_arg.get_port();
    }
    return add_udp_connection(path, port);
  }
    
  case CliArg::Protocol::TCP: {
    std::string path = DronecodeSDK::DEFAULT_TCP_REMOTE_IP;
    int port = DronecodeSDK::DEFAULT_TCP_REMOTE_PORT;
    if (!cli_arg.get_path().empty()) {
                path = cli_arg.get_path();
    }
    if (cli_arg.get_port()) {
      port = cli_arg.get_port();
    }
    return add_tcp_connection(path, port);
  }
    
  case CliArg::Protocol::SERIAL: {
    int baudrate = DronecodeSDK::DEFAULT_SERIAL_BAUDRATE;
    if (cli_arg.get_baudrate()) {
      baudrate = cli_arg.get_baudrate();
    }
    return add_serial_connection(cli_arg.get_path(), baudrate);
  }

  default:
    return Connection::Result::CONNECTION_ERROR;
  }
}
  
Connection::Result ConnectionImpl::add_udp_connection(const std::string &local_ip,
                                                      const int local_port)
{
  auto new_conn = std::make_shared<UdpConnection>(
						  std::bind(&ConnectionImpl::receive_message, this, std::placeholders::_1),
        local_ip,
        local_port);

    Connection::Result ret = new_conn->start();
    if (ret == Connection::Result::SUCCESS) {
        add_connection(new_conn);
    }
    return ret;
}

Connection::Result ConnectionImpl::add_tcp_connection(const std::string &remote_ip, int remote_port)
{
    auto new_conn = std::make_shared<TcpConnection>(
        std::bind(&ConnectionImpl::receive_message, this, std::placeholders::_1),
        remote_ip,
        remote_port);

    Connection::Result ret = new_conn->start();
    if (ret == Connection::Result::SUCCESS) {
        add_connection(new_conn);
    }
    return ret;
}

Connection::Result ConnectionImpl::add_serial_connection(const std::string &dev_path, int baudrate)
{
    auto new_conn = std::make_shared<SerialConnection>(
        std::bind(&ConnectionImpl::receive_message, this, std::placeholders::_1),
        dev_path,
        baudrate);

    Connection::Result ret = new_conn->start();
    if (ret == Connection::Result::SUCCESS) {
        add_connection(new_conn);
    }
    return ret;
}

void ConnectionImpl::add_connection(std::shared_ptr<Connection> new_connection)
{
    std::lock_guard<std::mutex> lock(_connections_mutex);
    _connections.push_back(new_connection);
}

}
