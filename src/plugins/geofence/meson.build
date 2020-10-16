geofence_include = include_directories(['include'])

# TODO: target properties
geofence = shared_library('mavsdk_geofence',
  ['geofence.cpp', 'geofence_impl.cpp'],
  include_directories: [core_include, mavlink_include, geofence_include],
  link_with: mavsdk_lib,
  install: true
)

pkg.generate(
  libraries: [geofence],
  subdirs: ['mavsdk/plugins/geofence', 'mavsdk/'],
  version: version_str,
  name: 'mavsdk_geofence',
  description: 'API and library for MAVLink compatible systems written in C++11'
)

install_headers('include/plugins/geofence/geofence.h',
  subdir: 'mavsdk/plugins/geofence')
