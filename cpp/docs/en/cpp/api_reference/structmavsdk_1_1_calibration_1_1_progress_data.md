# mavsdk::Calibration::ProgressData Struct Reference
`#include: calibration.h`

----


Progress data coming from calibration. 


Can be a progress percentage, or an instruction text. 


## Data Fields


bool [has_progress](#structmavsdk_1_1_calibration_1_1_progress_data_1a1b34241aba778eb9e1e67ae9f3404249) { false} - Whether this [ProgressData](structmavsdk_1_1_calibration_1_1_progress_data.md) contains a 'progress' status or not.

float [progress](#structmavsdk_1_1_calibration_1_1_progress_data_1a573b314b1440961d593881f931aeca32) {float(NAN)} - Progress (percentage)

bool [has_status_text](#structmavsdk_1_1_calibration_1_1_progress_data_1ac6780fbc4f8e8eaea3a2ccf6290a2c78) { false} - Whether this [ProgressData](structmavsdk_1_1_calibration_1_1_progress_data.md) contains a 'status_text' or not.

std::string [status_text](#structmavsdk_1_1_calibration_1_1_progress_data_1a8fae64b0166ed65158d045b07ed9b48d) {} - Instruction text.


## Field Documentation


### has_progress {#structmavsdk_1_1_calibration_1_1_progress_data_1a1b34241aba778eb9e1e67ae9f3404249}

```cpp
bool mavsdk::Calibration::ProgressData::has_progress { false}
```


Whether this [ProgressData](structmavsdk_1_1_calibration_1_1_progress_data.md) contains a 'progress' status or not.


### progress {#structmavsdk_1_1_calibration_1_1_progress_data_1a573b314b1440961d593881f931aeca32}

```cpp
float mavsdk::Calibration::ProgressData::progress {float(NAN)}
```


Progress (percentage)


### has_status_text {#structmavsdk_1_1_calibration_1_1_progress_data_1ac6780fbc4f8e8eaea3a2ccf6290a2c78}

```cpp
bool mavsdk::Calibration::ProgressData::has_status_text { false}
```


Whether this [ProgressData](structmavsdk_1_1_calibration_1_1_progress_data.md) contains a 'status_text' or not.


### status_text {#structmavsdk_1_1_calibration_1_1_progress_data_1a8fae64b0166ed65158d045b07ed9b48d}

```cpp
std::string mavsdk::Calibration::ProgressData::status_text {}
```


Instruction text.

