calibration_include = include_directories('include')

# TODO: target properties
calibration = shared_library('mavsdk_calibration',
  ['calibration.cpp', 'calibration_impl.cpp', 'calibration_statustext_parser.cpp'],
  include_directories: [core_include, mavlink_include, calibration_include],
  link_with: mavsdk_lib,
  install: true
)

install_headers('include/plugins/calibration/calibration.h')
