tune_include = include_directories('include')

# TODO: target properties
tune = shared_library('mavsdk_tune',
  ['tune.cpp', 'tune_impl.cpp'],
  include_directories: [core_include, mavlink_include, tune_include],
  link_with: mavsdk_lib,
  install: true
)

install_headers('include/plugins/tune/tune.h')
