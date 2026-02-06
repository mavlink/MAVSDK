# mavsdk::geometry::CoordinateTransformation Class Reference
`#include: geometry.h`

----


This is a utility class for coordinate transformations. 


The projections used to transform from global (lat/lon) to local (meter) coordinates are taken from: [http://mathworld.wolfram.com/AzimuthalEquidistantProjection.html](http://mathworld.wolfram.com/AzimuthalEquidistantProjection.html) and inspired by the implementations in: [https://github.com/PX4/ecl/blob/master/geo/geo.cpp](https://github.com/PX4/ecl/blob/master/geo/geo.cpp) 


## Data Structures


struct [GlobalCoordinate](structmavsdk_1_1geometry_1_1_coordinate_transformation_1_1_global_coordinate.md)

struct [LocalCoordinate](structmavsdk_1_1geometry_1_1_coordinate_transformation_1_1_local_coordinate.md)

## Public Member Functions


Type | Name | Description
---: | --- | ---
&nbsp; | [CoordinateTransformation](#classmavsdk_1_1geometry_1_1_coordinate_transformation_1a1e4d767efd8da2270bdac68e11671826) ()=delete | Default constructor not available.
&nbsp; | [CoordinateTransformation](#classmavsdk_1_1geometry_1_1_coordinate_transformation_1a33af8b4b2db7e00677e405a59d413261) ([GlobalCoordinate](structmavsdk_1_1geometry_1_1_coordinate_transformation_1_1_global_coordinate.md) reference) | Constructor to initialize projection reference.
&nbsp; | [~CoordinateTransformation](#classmavsdk_1_1geometry_1_1_coordinate_transformation_1a91a4e007c7b0e314dbd2f211b5e4e0c5) ()=default | Destructor.
[LocalCoordinate](structmavsdk_1_1geometry_1_1_coordinate_transformation_1_1_local_coordinate.md) | [local_from_global](#classmavsdk_1_1geometry_1_1_coordinate_transformation_1aedab88f8af68a2e0135d480b70ee6fbc) ([GlobalCoordinate](structmavsdk_1_1geometry_1_1_coordinate_transformation_1_1_global_coordinate.md) global_coordinate)const | Calculate local coordinates from global coordinates.
[GlobalCoordinate](structmavsdk_1_1geometry_1_1_coordinate_transformation_1_1_global_coordinate.md) | [global_from_local](#classmavsdk_1_1geometry_1_1_coordinate_transformation_1a8555f4bd4557b49d82a39b5b0273122f) ([LocalCoordinate](structmavsdk_1_1geometry_1_1_coordinate_transformation_1_1_local_coordinate.md) local_coordinate)const | Calculate global coordinates from local coordinates.


## Constructor & Destructor Documentation


### CoordinateTransformation() {#classmavsdk_1_1geometry_1_1_coordinate_transformation_1a1e4d767efd8da2270bdac68e11671826}
```cpp
mavsdk::geometry::CoordinateTransformation::CoordinateTransformation()=delete
```


Default constructor not available.


### CoordinateTransformation() {#classmavsdk_1_1geometry_1_1_coordinate_transformation_1a33af8b4b2db7e00677e405a59d413261}
```cpp
mavsdk::geometry::CoordinateTransformation::CoordinateTransformation(GlobalCoordinate reference)
```


Constructor to initialize projection reference.


**Parameters**

* [GlobalCoordinate](structmavsdk_1_1geometry_1_1_coordinate_transformation_1_1_global_coordinate.md) **reference** - Reference coordinate to project from.

### ~CoordinateTransformation() {#classmavsdk_1_1geometry_1_1_coordinate_transformation_1a91a4e007c7b0e314dbd2f211b5e4e0c5}
```cpp
mavsdk::geometry::CoordinateTransformation::~CoordinateTransformation()=default
```


Destructor.


## Member Function Documentation


### local_from_global() {#classmavsdk_1_1geometry_1_1_coordinate_transformation_1aedab88f8af68a2e0135d480b70ee6fbc}
```cpp
LocalCoordinate mavsdk::geometry::CoordinateTransformation::local_from_global(GlobalCoordinate global_coordinate) const
```


Calculate local coordinates from global coordinates.


**Parameters**

* [GlobalCoordinate](structmavsdk_1_1geometry_1_1_coordinate_transformation_1_1_global_coordinate.md) **global_coordinate** - The global coordinate to project from.

**Returns**

&emsp;[LocalCoordinate](structmavsdk_1_1geometry_1_1_coordinate_transformation_1_1_local_coordinate.md) - 

### global_from_local() {#classmavsdk_1_1geometry_1_1_coordinate_transformation_1a8555f4bd4557b49d82a39b5b0273122f}
```cpp
GlobalCoordinate mavsdk::geometry::CoordinateTransformation::global_from_local(LocalCoordinate local_coordinate) const
```


Calculate global coordinates from local coordinates.


**Parameters**

* [LocalCoordinate](structmavsdk_1_1geometry_1_1_coordinate_transformation_1_1_local_coordinate.md) **local_coordinate** - The local coordinate to project from.

**Returns**

&emsp;[GlobalCoordinate](structmavsdk_1_1geometry_1_1_coordinate_transformation_1_1_global_coordinate.md) - 