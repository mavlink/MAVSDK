# System Information

The [Info](../api_reference/classmavsdk_1_1_info.md) class is used to get system (vehicle) information, including the UUID (MAVLink `SYS_ID` if no UUID is stored in hardware), PX4 firmware version, vendor firmware version, host OS version (e.g. for NuttX) and vendor and product ids/names.

> **Note** Not all version information will necessarily be relevant on all vehicles. Where this occurs the
hardware may return garbage values (for example, the simulator provides garbage values for the vendor
firmware semantic version).

## Preconditions

The following code assumes that you already have included `mavsdk.h` (`#include <mavsdk/mavsdk.h>`) and that there is a [connection to a system](../guide/connections.md) obtained as shown below:
```cpp
System system = mavsdk.systems()[0];
```

The code also assumes that you have defined `info`, an instance of the `Info` class associated with the system (see [Using Plugins](../guide/using_plugins.md)):
```cpp
#include <mavsdk/plugins/info/info.h>
auto info = Info{system};
```

## Query System Information

The code below shows how to query the hardware uid, version, and product, information and print it to the console:

> **Tip** The UUID/uid with type uint64_t has been replaced by uid2 called hardware_uid with type char[18]. This was a a change inherited from mavlink in order to prevent ID conflicts.

```cpp
std::cout << "hardware uid: " << info.hardware_uid() << '\n';

// Wait until version/firmware information has been populated from the vehicle
while (info.get_identification().first==Info::Result::INFORMATION_NOT_RECEIVED_YET) {
    std::cout << "Waiting for Version information to populate from system." << '\n';
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

// Get the system Version struct
const Info::Version &systemVersion =  info.get_version().second;

// Print out the vehicle version information.
std::cout << "  flight_sw_major: " << systemVersion.flight_sw_major<< '\n'
          << "  flight_sw_minor: " << systemVersion.flight_sw_minor<< '\n'
          << "  flight_sw_patch: " << systemVersion.flight_sw_patch<< '\n'
          << "  flight_sw_vendor_major: " << systemVersion.flight_sw_vendor_major<< '\n'
          << "  flight_sw_vendor_minor: " << systemVersion.flight_sw_vendor_minor<< '\n'
          << "  flight_sw_vendor_patch: " << systemVersion.flight_sw_vendor_patch<< '\n'
          << "  flight_sw_git_hash: " << systemVersion.flight_sw_git_hash<< '\n'
          << "  os_sw_major: " << systemVersion.os_sw_major<< '\n'
          << "  os_sw_minor: " << systemVersion.os_sw_minor<< '\n'
          << "  os_sw_patch: " << systemVersion.os_sw_patch<< '\n'
          << "  os_sw_git_hash: " << systemVersion.os_sw_git_hash<< '\n';

// Get the system Product struct
const Info::Product &systemProduct =  info.get_product().second;

// Print out the vehicle product information.
std::cout << "  vendor_id: " << systemProduct.vendor_id<< '\n'
          << "  vendor_name: " << systemProduct.vendor_name<< '\n'
          << "  product_id: " << systemProduct.product_id<< '\n'
          << "  product_name: " << systemProduct.product_id<< '\n';
```

> **Tip** It is possible to query for the information before all values have been retrieved.
  Note above how we use `Info::is_complete()` to check that the version information (`Info::Version` and `Info::Product`) has all been obtained from the vehicle before printing it.
