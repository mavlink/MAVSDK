calibration_include = include_directories(['include'])

# TODO: target properties
calibration = shared_library('mavsdk_calibration',
  ['calibration.cpp', 'calibration_impl.cpp', 'calibration_statustext_parser.cpp'],
  include_directories: [core_include, mavlink_include, calibration_include],
  link_with: mavsdk_lib,
  install: true
)

pkg.generate(
  libraries: [calibration],
  subdirs: ['mavsdk/plugins/calibration', 'mavsdk/'],
  version: version_str,
  name: 'mavsdk_calibration',
  description: 'API and library for MAVLink compatible systems written in C++11'
)

install_headers('include/plugins/calibration/calibration.h',
  subdir: 'mavsdk/plugins/calibration')
