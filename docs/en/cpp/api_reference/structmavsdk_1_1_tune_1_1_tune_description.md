# mavsdk::Tune::TuneDescription Struct Reference
`#include: tune.h`

----


[Tune](classmavsdk_1_1_tune.md) description, containing song elements and tempo. 


## Data Fields


std::vector< [SongElement](classmavsdk_1_1_tune.md#classmavsdk_1_1_tune_1a97d85b5d578c74edc82a4337fc7bed57) > [song_elements](#structmavsdk_1_1_tune_1_1_tune_description_1a30b69d9f32b509fd31091c722d7e4512) {} - The list of song elements (notes, pauses, ...) to be played.

int32_t [tempo](#structmavsdk_1_1_tune_1_1_tune_description_1ad31ec50a46db211d8d31d2622692cce8) {} - The tempo of the song (range: 32 - 255)


## Field Documentation


### song_elements {#structmavsdk_1_1_tune_1_1_tune_description_1a30b69d9f32b509fd31091c722d7e4512}

```cpp
std::vector<SongElement> mavsdk::Tune::TuneDescription::song_elements {}
```


The list of song elements (notes, pauses, ...) to be played.


### tempo {#structmavsdk_1_1_tune_1_1_tune_description_1ad31ec50a46db211d8d31d2622692cce8}

```cpp
int32_t mavsdk::Tune::TuneDescription::tempo {}
```


The tempo of the song (range: 32 - 255)

