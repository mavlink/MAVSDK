# mavsdk::TelemetryServer::Covariance Struct Reference
`#include: telemetry_server.h`

----


[Covariance](structmavsdk_1_1_telemetry_server_1_1_covariance.md) type. 


Row-major representation of a 6x6 cross-covariance matrix upper right triangle. Set first to NaN if unknown. 


## Data Fields


std::vector< float > [covariance_matrix](#structmavsdk_1_1_telemetry_server_1_1_covariance_1a5be90577e6756b7583824f8dec01cdcb) {} - Representation of a covariance matrix.


## Field Documentation


### covariance_matrix {#structmavsdk_1_1_telemetry_server_1_1_covariance_1a5be90577e6756b7583824f8dec01cdcb}

```cpp
std::vector<float> mavsdk::TelemetryServer::Covariance::covariance_matrix {}
```


Representation of a covariance matrix.

