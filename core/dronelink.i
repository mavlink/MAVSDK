%module dronelink
%{
#include "dronelink.h"
#include "dronelink_impl.h"
#include "global_include.h"
#include "connection.h"
#include "udp_connection.h"
%}

namespace dronelink {
  class DroneLink {
  public:
    DroneLink();
    ~DroneLink();
    /*enum class ConnectionResult{};*/
    /*static const char *connection_result_str(ConnectionResult);*/
    /*ConnectionResult add_udp_connection();*/
    /*ConnectionResult add_udp_connection(int local_port_number);*/
    /*const std::vector<uint64_t> &device_uuids() const;*/
    /*typedef std::function<void(uint64_t uuid)> event_callback_t;*/
    /*void register_on_discover(event_callback_t callback);*/
    /*void register_on_timeout(event_callback_t callback);*/
  };
} // namespace dronelink
