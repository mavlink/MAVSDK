# mavsdk::ParamServer::AllParams Struct Reference
`#include: param_server.h`

----


Type collecting all integer, float, and custom parameters. 


## Data Fields


std::vector< [IntParam](structmavsdk_1_1_param_server_1_1_int_param.md) > [int_params](#structmavsdk_1_1_param_server_1_1_all_params_1a282fadb0b1c0cf138f95958a3adcd1b5) {} - Collection of all parameter names and values of type int.

std::vector< [FloatParam](structmavsdk_1_1_param_server_1_1_float_param.md) > [float_params](#structmavsdk_1_1_param_server_1_1_all_params_1a829345b4413b71e705aa9e792b28f0e1) {} - Collection of all parameter names and values of type float.

std::vector< [CustomParam](structmavsdk_1_1_param_server_1_1_custom_param.md) > [custom_params](#structmavsdk_1_1_param_server_1_1_all_params_1a5397de1b3979efb2958af7547ebd1b68) {} - Collection of all parameter names and values of type custom.


## Field Documentation


### int_params {#structmavsdk_1_1_param_server_1_1_all_params_1a282fadb0b1c0cf138f95958a3adcd1b5}

```cpp
std::vector<IntParam> mavsdk::ParamServer::AllParams::int_params {}
```


Collection of all parameter names and values of type int.


### float_params {#structmavsdk_1_1_param_server_1_1_all_params_1a829345b4413b71e705aa9e792b28f0e1}

```cpp
std::vector<FloatParam> mavsdk::ParamServer::AllParams::float_params {}
```


Collection of all parameter names and values of type float.


### custom_params {#structmavsdk_1_1_param_server_1_1_all_params_1a5397de1b3979efb2958af7547ebd1b68}

```cpp
std::vector<CustomParam> mavsdk::ParamServer::AllParams::custom_params {}
```


Collection of all parameter names and values of type custom.

