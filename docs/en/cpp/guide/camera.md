# Camera

The [Camera](../api_reference/classmavsdk_1_1_camera.md) plugin provides control over cameras that implement the [MAVLink Camera Protocol](https://mavlink.io/en/services/camera.html).

## Create the Plugin

```cpp
#include <mavsdk/plugins/camera/camera.h>
auto camera = Camera{system};
```

## Discover Cameras

Cameras announce themselves as MAVLink components. Subscribe to the camera list to be notified when a camera appears:

```cpp
camera.subscribe_camera_list([](Camera::CameraList list) {
    for (auto& item : list.cameras) {
        std::cout << "Camera found: component id " << item.component_id << '\n';
    }
});
```

## Set Mode and Take Photos

Switch to photo mode and capture a single image (using the `component_id` from discovery):

```cpp
camera.set_mode(component_id, Camera::Mode::Photo);
camera.take_photo(component_id);
```

Subscribe to `capture_info` to receive the result (file URL, position, timestamp):

```cpp
camera.subscribe_capture_info([](Camera::CaptureInfo info) {
    std::cout << "Photo saved to: " << info.file_url << '\n';
});
```

## Record Video

```cpp
camera.set_mode(component_id, Camera::Mode::Video);
camera.start_video(component_id);
// ... fly around ...
camera.stop_video(component_id);
```

## Zoom and Focus

```cpp
camera.zoom_range(component_id, 2.0f);   // 2× zoom
camera.focus_range(component_id, 70.0f); // 70% focus position
```

## Further Information

- [API Reference: Camera](../api_reference/classmavsdk_1_1_camera.md)
- [Example: Camera](https://github.com/mavlink/MAVSDK/tree/main/cpp/examples/camera)
- [Example: Camera settings](https://github.com/mavlink/MAVSDK/tree/main/cpp/examples/camera_settings)
- [Example: Camera zoom](https://github.com/mavlink/MAVSDK/tree/main/cpp/examples/camera_zoom)
- [Example: Camera server](https://github.com/mavlink/MAVSDK/tree/main/cpp/examples/camera_server) (implementing a camera component)
- [MAVLink Camera Protocol](https://mavlink.io/en/services/camera.html)
