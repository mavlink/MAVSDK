camera_include = include_directories('include')
definition_include = include_directories('camera_definition_files/generated')

# TODO: target properties
camera = shared_library('mavsdk_camera',
  [
    'camera.cpp', 
    'camera_impl.cpp',
    'camera_definition.cpp',
    'camera_definition_files/generated/camera_definition_files.cpp',
    ],
  include_directories: [core_include, mavlink_include, camera_include, definition_include],
  #link_with: [mavsdk_lib, tinyxml2, curl],
  link_with: mavsdk_lib,
  dependencies: tinyxml2_dep,
  install: true
)

install_headers('include/plugins/camera/camera.h')
