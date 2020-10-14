info_include = include_directories('include')

# TODO: target properties
info = shared_library('mavsdk_info',
  ['info.cpp', 'info_impl.cpp'],
  include_directories: [core_include, mavlink_include, info_include],
  link_with: mavsdk_lib,
  install: true
)

install_headers('include/plugins/info/info.h')
