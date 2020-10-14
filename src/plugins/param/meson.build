param_include = include_directories('include')

# TODO: target properties
param = shared_library('mavsdk_param',
  ['param.cpp', 'param_impl.cpp'],
  include_directories: [core_include, mavlink_include, param_include],
  link_with: mavsdk_lib,
  install: true
)

install_headers('include/plugins/param/param.h')
