# mavsdk::Param::AllParams Struct Reference
`#include: param.h`

----


Type collecting all integer, float, and custom parameters. 


## Data Fields


std::vector< [IntParam](structmavsdk_1_1_param_1_1_int_param.md) > [int_params](#structmavsdk_1_1_param_1_1_all_params_1a92aa25d021ab0d20a00c13b8bc7a4dad) {} - Collection of all parameter names and values of type int.

std::vector< [FloatParam](structmavsdk_1_1_param_1_1_float_param.md) > [float_params](#structmavsdk_1_1_param_1_1_all_params_1a198ac975671f51f5c9ac92f30dbca73a) {} - Collection of all parameter names and values of type float.

std::vector< [CustomParam](structmavsdk_1_1_param_1_1_custom_param.md) > [custom_params](#structmavsdk_1_1_param_1_1_all_params_1a7fc2ed75dd0556aa82aff9cec5cb5bbd) {} - Collection of all parameter names and values of type custom.


## Field Documentation


### int_params {#structmavsdk_1_1_param_1_1_all_params_1a92aa25d021ab0d20a00c13b8bc7a4dad}

```cpp
std::vector<IntParam> mavsdk::Param::AllParams::int_params {}
```


Collection of all parameter names and values of type int.


### float_params {#structmavsdk_1_1_param_1_1_all_params_1a198ac975671f51f5c9ac92f30dbca73a}

```cpp
std::vector<FloatParam> mavsdk::Param::AllParams::float_params {}
```


Collection of all parameter names and values of type float.


### custom_params {#structmavsdk_1_1_param_1_1_all_params_1a7fc2ed75dd0556aa82aff9cec5cb5bbd}

```cpp
std::vector<CustomParam> mavsdk::Param::AllParams::custom_params {}
```


Collection of all parameter names and values of type custom.

