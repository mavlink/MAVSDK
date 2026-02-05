# mavsdk::Mocap::Covariance Struct Reference
`#include: mocap.h`

----


[Covariance](structmavsdk_1_1_mocap_1_1_covariance.md) type. Row-major representation of a 6x6 cross-covariance matrix upper right triangle. Needs to be 21 entries or 1 entry with NaN if unknown. 


## Data Fields


std::vector< float > [covariance_matrix](#structmavsdk_1_1_mocap_1_1_covariance_1ab2b28fd722cefb7f3783e4060baada2f) {} - The covariance matrix.


## Field Documentation


### covariance_matrix {#structmavsdk_1_1_mocap_1_1_covariance_1ab2b28fd722cefb7f3783e4060baada2f}

```cpp
std::vector<float> mavsdk::Mocap::Covariance::covariance_matrix {}
```


The covariance matrix.

