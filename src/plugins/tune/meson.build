tune_include = include_directories(['include'])

# TODO: target properties
tune = shared_library('mavsdk_tune',
  ['tune.cpp', 'tune_impl.cpp'],
  include_directories: [core_include, mavlink_include, tune_include],
  link_with: mavsdk_lib,
  install: true
)

pkg.generate(
  libraries: [tune],
  subdirs: ['mavsdk/plugins/tune', 'mavsdk/'],
  version: version_str,
  name: 'mavsdk_tune',
  description: 'API and library for MAVLink compatible systems written in C++11'
)

install_headers('include/plugins/tune/tune.h',
  subdir: 'mavsdk/plugins/tune')
