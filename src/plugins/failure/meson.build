failure_include = include_directories('include')

# TODO: target properties
failure = shared_library('mavsdk_failure',
  ['failure.cpp', 'failure_impl.cpp'],
  include_directories: [core_include, mavlink_include, failure_include],
  link_with: mavsdk_lib,
  install: true
)

install_headers('include/plugins/failure/failure.h')
