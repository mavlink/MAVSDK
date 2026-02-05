# mavsdk::Telemetry::Covariance Struct Reference
`#include: telemetry.h`

----


[Covariance](structmavsdk_1_1_telemetry_1_1_covariance.md) type. 


Row-major representation of a 6x6 cross-covariance matrix upper right triangle. Set first to NaN if unknown. 


## Data Fields


std::vector< float > [covariance_matrix](#structmavsdk_1_1_telemetry_1_1_covariance_1ab740b5515e67c69fd61d21736b227da5) {} - Representation of a covariance matrix.


## Field Documentation


### covariance_matrix {#structmavsdk_1_1_telemetry_1_1_covariance_1ab740b5515e67c69fd61d21736b227da5}

```cpp
std::vector<float> mavsdk::Telemetry::Covariance::covariance_matrix {}
```


Representation of a covariance matrix.

