gimbal_include = include_directories('include')

# TODO: target properties
gimbal = shared_library('mavsdk_gimbal',
  ['gimbal.cpp', 'gimbal_impl.cpp', 'gimbal_protocol_v1.cpp', 'gimbal_protocol_v2.cpp'],
  include_directories: [core_include, mavlink_include, gimbal_include],
  link_with: mavsdk_lib,
  install: true
)

install_headers('include/plugins/gimbal/gimbal.h')
