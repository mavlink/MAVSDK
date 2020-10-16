gimbal_include = include_directories(['include'])

# TODO: target properties
gimbal = shared_library('mavsdk_gimbal',
  ['gimbal.cpp', 'gimbal_impl.cpp', 'gimbal_protocol_v1.cpp', 'gimbal_protocol_v2.cpp'],
  include_directories: [core_include, mavlink_include, gimbal_include],
  link_with: mavsdk_lib,
  install: true
)

pkg.generate(
  libraries: [gimbal],
  subdirs: ['mavsdk/plugins/gimbal', 'mavsdk/'],
  version: version_str,
  name: 'mavsdk_gimbal',
  description: 'API and library for MAVLink compatible systems written in C++11'
)

install_headers('include/plugins/gimbal/gimbal.h',
  subdir: 'mavsdk/plugins/gimbal')
