failure_include = include_directories(['include'])

# TODO: target properties
failure = shared_library('mavsdk_failure',
  ['failure.cpp', 'failure_impl.cpp'],
  include_directories: [core_include, mavlink_include, failure_include],
  link_with: mavsdk_lib,
  install: true
)

pkg.generate(
  libraries: [failure],
  subdirs: ['mavsdk/plugins/failure', 'mavsdk/'],
  version: version_str,
  name: 'mavsdk_failure',
  description: 'API and library for MAVLink compatible systems written in C++11'
)

install_headers('include/plugins/failure/failure.h',
  subdir: 'mavsdk/plugins/failure')
