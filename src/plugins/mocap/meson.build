mocap_include = include_directories('include')

# TODO: target properties
mocap = shared_library('mavsdk_mocap',
  ['mocap.cpp', 'mocap_impl.cpp'],
  include_directories: [core_include, mavlink_include, mocap_include],
  link_with: mavsdk_lib,
  install: true
)

install_headers('include/plugins/mocap/mocap.h')
