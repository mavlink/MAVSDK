telemetry_include = include_directories(['include'])

# TODO: target properties
telemetry = shared_library('mavsdk_telemetry',
  ['telemetry.cpp', 'telemetry_impl.cpp', 'math_conversions.cpp'],
  include_directories: [core_include, mavlink_include, telemetry_include],
  link_with: mavsdk_lib,
  install: true
)

pkg.generate(
  libraries: telemetry,
  subdirs: ['include'],
  version: version_str,
  name: 'mavsdk_telemetry',
  description: 'API and library for MAVLink compatible systems written in C++11'
)

install_headers('include/plugins/telemetry/telemetry.h',
  subdir: 'mavsdk/plugins/telemetry')
