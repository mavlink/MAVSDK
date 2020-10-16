camera_include = include_directories(['include', 'camera_definition_files/generated'])

# TODO: target properties
camera = shared_library('mavsdk_camera',
  [
    'camera.cpp',
    'camera_impl.cpp',
    'camera_definition.cpp',
    'camera_definition_files/generated/camera_definition_files.cpp'
  ],
  include_directories: [core_include, mavlink_include, camera_include],
  link_with: mavsdk_lib,
  dependencies: [tinyxml2_dep, curl_dep],
  install: true
)

pkg.generate(
  libraries: [camera],
  subdirs: ['mavsdk/plugins/camera', 'mavsdk/'],
  version: version_str,
  name: 'mavsdk_camera',
  description: 'API and library for MAVLink compatible systems written in C++11'
)

install_headers('include/plugins/camera/camera.h',
  subdir: 'mavsdk/plugins/camera')
