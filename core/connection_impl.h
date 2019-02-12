#pragma once


#include <mutex>
#include <vector>

#include "connection.h"
#include "mavlink_include.h"


namespace dronecode_sdk {

class ConnectionImpl {
public:
  ConnnectionImpl();
  ~ConnnectionImpl();
  
  void receive_message(const mavlink_message_t &message);
  bool send_message(const mavlink_message_t &message);
  
  ConnectionResult add_any_connection(const std::string &connection_url);
  ConnectionResult
  add_link_connection(const std::string &protocol, const std::string &ip, int port);
  ConnectionResult add_udp_connection(const std::string &local_ip, int local_port_number);
  ConnectionResult add_tcp_connection(const std::string &remote_ip, int remote_port);
  ConnectionResult add_serial_connection(const std::string &dev_path, int baudrate);
  
  bool is_connected() const;
  bool is_connected(uint64_t uuid) const;
  
  
private:
  void add_connection(std::shared_ptr<Connection>);


};
  
}

